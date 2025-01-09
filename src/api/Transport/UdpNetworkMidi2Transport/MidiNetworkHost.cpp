// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#include "pch.h"

#include <chrono>

_Use_decl_annotations_
HRESULT 
MidiNetworkHost::Initialize(
    MidiNetworkHostDefinition& hostDefinition
)
{
    TraceLoggingWrite(
        MidiNetworkMidiTransportTelemetryProvider::Provider(),
        MIDI_TRACE_EVENT_INFO,
        TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(L"Enter", MIDI_TRACE_EVENT_MESSAGE_FIELD)
    );

    RETURN_HR_IF(E_INVALIDARG, hostDefinition.HostName.empty());
    RETURN_HR_IF(E_INVALIDARG, hostDefinition.ServiceInstanceName.empty());

    RETURN_HR_IF(E_INVALIDARG, hostDefinition.UmpEndpointName.empty());
    RETURN_HR_IF(E_INVALIDARG, hostDefinition.UmpEndpointName.size() > MIDI_MAX_UMP_ENDPOINT_NAME_BYTE_COUNT);

    RETURN_HR_IF(E_INVALIDARG, hostDefinition.ProductInstanceId.empty());
    RETURN_HR_IF(E_INVALIDARG, hostDefinition.ProductInstanceId.size() > MIDI_MAX_UMP_PRODUCT_INSTANCE_ID_BYTE_COUNT);

    m_started = false;

    m_hostEndpointName = hostDefinition.UmpEndpointName;
    m_hostProductInstanceId = hostDefinition.ProductInstanceId;

    if (!hostDefinition.UseAutomaticPortAllocation)
    {
        RETURN_HR_IF(E_INVALIDARG, hostDefinition.Port.empty());
    }

    m_hostDefinition = hostDefinition;

    DatagramSocket socket;
    m_socket = std::move(socket);

    m_socket.Control().DontFragment(true);
    //m_socket.Control().InboundBufferSizeInBytes(10000);
    m_socket.Control().QualityOfService(SocketQualityOfService::LowLatency);


    if (m_hostDefinition.Advertise)
    {
        m_advertiser = std::make_shared<MidiNetworkAdvertiser>();
        RETURN_IF_NULL_ALLOC(m_advertiser);
        RETURN_IF_FAILED(m_advertiser->Initialize());
    }

    TraceLoggingWrite(
        MidiNetworkMidiTransportTelemetryProvider::Provider(),
        MIDI_TRACE_EVENT_INFO,
        TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(L"Exit", MIDI_TRACE_EVENT_MESSAGE_FIELD)
    );

    return S_OK;
}

HRESULT
MidiNetworkHost::Start()
{
    TraceLoggingWrite(
        MidiNetworkMidiTransportTelemetryProvider::Provider(),
        MIDI_TRACE_EVENT_INFO,
        TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(L"Enter", MIDI_TRACE_EVENT_MESSAGE_FIELD)
    );

    HostName hostName(m_hostDefinition.HostName);

    // wire up to handle incoming events
    auto messageReceivedHandler = winrt::Windows::Foundation::TypedEventHandler<DatagramSocket, DatagramSocketMessageReceivedEventArgs>(this, &MidiNetworkHost::OnMessageReceived);
    m_messageReceivedEventToken = m_socket.MessageReceived(messageReceivedHandler);

    // TODO: this should have error checking
    m_socket.BindServiceNameAsync(winrt::to_hstring(m_hostDefinition.Port)).get();

    auto boundPort = static_cast<uint16_t>(std::stoi(winrt::to_string(m_socket.Information().LocalPort())));

    // advertise
    if (m_hostDefinition.Advertise && m_advertiser != nullptr)
    {
        RETURN_IF_FAILED(m_advertiser->Advertise(
            m_hostDefinition.ServiceInstanceName,
            hostName,
            m_socket,
            boundPort,
            m_hostDefinition.UmpEndpointName,
            m_hostDefinition.ProductInstanceId
        ));
    }

    m_started = true;

    TraceLoggingWrite(
        MidiNetworkMidiTransportTelemetryProvider::Provider(),
        MIDI_TRACE_EVENT_INFO,
        TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(L"Exit", MIDI_TRACE_EVENT_MESSAGE_FIELD)
    );

    return S_OK;
}

// header sized plus a command packet header
#define MINIMUM_VALID_UDP_PACKET_SIZE (sizeof(uint32_t) * 2)

// "message" here means UDP packet message, not a MIDI message
_Use_decl_annotations_
void MidiNetworkHost::OnMessageReceived(
    DatagramSocket const& sender,
    DatagramSocketMessageReceivedEventArgs const& args)
{
    UNREFERENCED_PARAMETER(sender);

    auto reader = args.GetDataReader();

    if (reader.UnconsumedBufferLength() < MINIMUM_VALID_UDP_PACKET_SIZE)
    {
        // not a message we understand. Needs to be at least the size of the 
        // MIDI header plus a command packet header. Really it needs to be larger, but
        // just trying to weed out blips

        TraceLoggingWrite(
            MidiNetworkMidiTransportTelemetryProvider::Provider(),
            MIDI_TRACE_EVENT_WARNING,
            TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
            TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
            TraceLoggingPointer(this, "this"),
            TraceLoggingWideString(L"Undersized packet", MIDI_TRACE_EVENT_MESSAGE_FIELD)
        );

        // todo: does the reader need to be cleared?

        return;
    }

    uint32_t udpHeader = reader.ReadUInt32();

    if (udpHeader != MIDI_UDP_PAYLOAD_HEADER)
    {
        // not a message we understand

        return;
    }

    auto conn = GetOrCreateConnection(args.RemoteAddress(), args.RemotePort());

    if (conn)
    {
        conn->ProcessIncomingMessage(reader);
    }
    else
    {
        TraceLoggingWrite(
            MidiNetworkMidiTransportTelemetryProvider::Provider(),
            MIDI_TRACE_EVENT_ERROR,
            TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
            TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
            TraceLoggingPointer(this, "this"),
            TraceLoggingWideString(L"Message received from remote client, but GetConnection returned nullptr", MIDI_TRACE_EVENT_MESSAGE_FIELD)
        );
    }

}


HRESULT 
MidiNetworkHost::Shutdown()
{
    TraceLoggingWrite(
        MidiNetworkMidiTransportTelemetryProvider::Provider(),
        MIDI_TRACE_EVENT_INFO,
        TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(L"Enter", MIDI_TRACE_EVENT_MESSAGE_FIELD)
    );

    if (m_advertiser)
    {
        m_advertiser->Shutdown();
    }

    // TODO: send "bye" to all sessions, and then unbind the socket

    // TODO: Stop packet processing thread using the jthread stop token
    m_keepProcessing = false;

    if (m_socket)
    {
        m_socket.MessageReceived(m_messageReceivedEventToken);
        m_socket.Close();
        m_socket = nullptr;
    }


    TraceLoggingWrite(
        MidiNetworkMidiTransportTelemetryProvider::Provider(),
        MIDI_TRACE_EVENT_INFO,
        TraceLoggingString(__FUNCTION__, MIDI_TRACE_EVENT_LOCATION_FIELD),
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(L"Exit", MIDI_TRACE_EVENT_MESSAGE_FIELD)
    );

    return S_OK;
}