// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#include "pch.h"
#include "midi2.BluetoothMidiAbstraction.h"

using namespace wil;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#define MAX_DEVICE_ID_LEN 200 // size in chars

GUID AbstractionLayerGUID = ABSTRACTION_LAYER_GUID;



_Use_decl_annotations_
HRESULT
CMidi2BluetoothMidiEndpointManager::Initialize(
    IUnknown* MidiDeviceManager, 
    IUnknown* /*midiEndpointProtocolManager*/
)
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );

    RETURN_HR_IF(E_INVALIDARG, nullptr == MidiDeviceManager);

    RETURN_IF_FAILED(MidiDeviceManager->QueryInterface(__uuidof(IMidiDeviceManagerInterface), (void**)&m_MidiDeviceManager));

    m_TransportAbstractionId = AbstractionLayerGUID;   // this is needed so MidiSrv can instantiate the correct transport
    m_ContainerId = m_TransportAbstractionId;                           // we use the transport ID as the container ID for convenience


    RETURN_IF_FAILED(StartAdvertisementWatcher());
    //RETURN_IF_FAILED(StartDeviceWatcher());

    return S_OK;
}

HRESULT
CMidi2BluetoothMidiEndpointManager::CreateParentDevice()
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );

    // the parent device parameters are set by the transport (this)
    std::wstring parentDeviceName{ TRANSPORT_PARENT_DEVICE_NAME };
    std::wstring parentDeviceId{ internal::NormalizeDeviceInstanceIdWStringCopy(TRANSPORT_PARENT_ID) };

    SW_DEVICE_CREATE_INFO createInfo = {};
    createInfo.cbSize = sizeof(createInfo);
    createInfo.pszInstanceId = parentDeviceId.c_str();
    createInfo.CapabilityFlags = SWDeviceCapabilitiesNone;
    createInfo.pszDeviceDescription = parentDeviceName.c_str();
    createInfo.pContainerId = &m_ContainerId;

    const ULONG deviceIdMaxSize = 255;
    wchar_t newDeviceId[deviceIdMaxSize]{ 0 };

    RETURN_IF_FAILED(m_MidiDeviceManager->ActivateVirtualParentDevice(
        0,
        nullptr,
        &createInfo,
        (PWSTR)newDeviceId,
        deviceIdMaxSize
    ));

    m_parentDeviceId = internal::NormalizeDeviceInstanceIdWStringCopy(newDeviceId);


    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(newDeviceId, "New parent device instance id")
    );

    return S_OK;
}


_Use_decl_annotations_
HRESULT 
CMidi2BluetoothMidiEndpointManager::CreateEndpoint(
    MidiBluetoothDeviceDefinition& definition
)
{
    //RETURN_HR_IF_MSG(E_INVALIDARG, definition->EndpointName.empty(), "Empty endpoint name");
    //RETURN_HR_IF_MSG(E_INVALIDARG, definition->InstanceIdPrefix.empty(), "Empty endpoint prefix");
    //RETURN_HR_IF_MSG(E_INVALIDARG, definition->EndpointUniqueIdentifier.empty(), "Empty endpoint unique id");


    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingUInt64(definition.BluetoothAddress, "bluetooth address")
    );

    std::wstring mnemonic(TRANSPORT_MNEMONIC);

  
    // TODO: Need to fold in user data here
    std::wstring endpointName = definition.TransportSuppliedName.c_str();
    std::wstring endpointDescription = definition.TransportSuppliedDescription.c_str();

    std::vector<DEVPROPERTY> interfaceDeviceProperties{};

    // no user or in-protocol data in this case
    std::wstring friendlyName = internal::CalculateEndpointDevicePrimaryName(endpointName, L"", L"");


    bool requiresMetadataHandler = false;
    bool multiClient = true;
    bool generateIncomingTimestamps = true;

    std::wstring uniqueIdentifier = std::to_wstring(definition.BluetoothAddress);

    // Device properties


    SW_DEVICE_CREATE_INFO createInfo = {};
    createInfo.cbSize = sizeof(createInfo);

    // build the instance id, which becomes the middle of the SWD id
    std::wstring instanceId = internal::NormalizeDeviceInstanceIdWStringCopy(
        MIDI_BLE_INSTANCE_ID_PREFIX +
        uniqueIdentifier);

    createInfo.pszInstanceId = instanceId.c_str();
    createInfo.CapabilityFlags = SWDeviceCapabilitiesNone;
    createInfo.pszDeviceDescription = friendlyName.c_str();


    const ULONG deviceInterfaceIdMaxSize = 255;
    wchar_t newDeviceInterfaceId[deviceInterfaceIdMaxSize]{ 0 };

    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(instanceId.c_str(), "instance id"),
        TraceLoggingWideString(L"Activating endpoint", "message")
    );

    MIDIENDPOINTCOMMONPROPERTIES commonProperties{};
    commonProperties.AbstractionLayerGuid = m_TransportAbstractionId;
    commonProperties.EndpointPurpose = MidiEndpointDevicePurposePropertyValue::NormalMessageEndpoint;
    commonProperties.FriendlyName = friendlyName.c_str();
    commonProperties.TransportMnemonic = mnemonic.c_str();
    commonProperties.TransportSuppliedEndpointName = endpointName.c_str();
    commonProperties.TransportSuppliedEndpointDescription = endpointDescription.c_str();
    commonProperties.UserSuppliedEndpointName = nullptr;
    commonProperties.UserSuppliedEndpointDescription = nullptr;
    commonProperties.UniqueIdentifier = uniqueIdentifier.c_str();
    commonProperties.SupportedDataFormats = MidiDataFormat::MidiDataFormat_ByteStream;
    commonProperties.NativeDataFormat = MIDI_PROP_NATIVEDATAFORMAT_BYTESTREAM;
    commonProperties.SupportsMultiClient = multiClient;
    commonProperties.RequiresMetadataHandler = requiresMetadataHandler;
    commonProperties.GenerateIncomingTimestamps = generateIncomingTimestamps;
    commonProperties.SupportsMidi1ProtocolDefaultValue = true;
    commonProperties.SupportsMidi2ProtocolDefaultValue = false;

    if (definition.ManufacturerName.empty())
    {
        commonProperties.ManufacturerName = nullptr;
    }
    else
    {
        commonProperties.ManufacturerName = definition.ManufacturerName.c_str();
    }

    RETURN_IF_FAILED(m_MidiDeviceManager->ActivateEndpoint(
        (PCWSTR)m_parentDeviceId.c_str(),                       // parent instance Id
        true,                                                   // UMP-only
        MidiFlow::MidiFlowBidirectional,                        // MIDI Flow is bidirectional for a BLE MIDI 1.0 device
        &commonProperties,
        (ULONG)interfaceDeviceProperties.size(),
        (ULONG)0,
        (PVOID)interfaceDeviceProperties.data(),
        (PVOID)nullptr,
        (PVOID)&createInfo,
        (LPWSTR)&newDeviceInterfaceId,
        deviceInterfaceIdMaxSize));

    // we need this for removal later
    definition.CreatedMidiDeviceInstanceId = instanceId;
    definition.CreatedEndpointInterfaceId = internal::NormalizeEndpointInterfaceIdWStringCopy(newDeviceInterfaceId);

    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingWideString(newDeviceInterfaceId, "new device interface id"),
        TraceLoggingWideString(instanceId.c_str(), "instance id"),
        TraceLoggingWideString(uniqueIdentifier.c_str(), "unique identifier"),
        TraceLoggingWideString(L"Endpoint activated", "message")
    );

    return S_OK;
}




HRESULT
CMidi2BluetoothMidiEndpointManager::Cleanup()
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );


    return S_OK;
}






_Use_decl_annotations_
HRESULT 
CMidi2BluetoothMidiEndpointManager::OnDeviceAdded(enumeration::DeviceWatcher /*source*/, enumeration::DeviceInformation /*args*/)
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );

    return S_OK;
}

_Use_decl_annotations_
HRESULT 
CMidi2BluetoothMidiEndpointManager::OnDeviceRemoved(enumeration::DeviceWatcher /*source*/, enumeration::DeviceInformationUpdate /*args*/)
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );


    return S_OK;
}

_Use_decl_annotations_
HRESULT 
CMidi2BluetoothMidiEndpointManager::OnDeviceUpdated(enumeration::DeviceWatcher /*source*/, enumeration::DeviceInformationUpdate /*args*/)
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );


    return S_OK;
}


_Use_decl_annotations_
HRESULT 
CMidi2BluetoothMidiEndpointManager::OnDeviceStopped(enumeration::DeviceWatcher /*source*/, foundation::IInspectable /*args*/)
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );


    return S_OK;
}


_Use_decl_annotations_
HRESULT 
CMidi2BluetoothMidiEndpointManager::OnEnumerationCompleted(enumeration::DeviceWatcher /*source*/, foundation::IInspectable /*args*/)
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );


    return S_OK;
}




// TODO: This likely isn't the correct event signature

_Use_decl_annotations_
HRESULT
CMidi2BluetoothMidiEndpointManager::OnBleDeviceNameChanged(bt::BluetoothLEDevice device, foundation::IInspectable /*args*/)
{



    return S_OK;
}


_Use_decl_annotations_
HRESULT
CMidi2BluetoothMidiEndpointManager::OnBleDeviceConnectionStatusChanged(bt::BluetoothLEDevice device, foundation::IInspectable /*args*/)
{
    bool connected = (device.ConnectionStatus() == bt::BluetoothConnectionStatus::Connected);

    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingUInt64(device.BluetoothAddress(), "address"),
        TraceLoggingWideString(device.DeviceId().c_str(), "device id"),
        TraceLoggingWideString(device.Name().c_str(), "name"),
        TraceLoggingBool(connected, "is connected")
    );

    // TODO: When the device was first seen by the service, it may have already been connected, so 
    // we need to handle that even though we won't have a connection status changed event

    if (connected)
    {
        // Connected. check to see if we've already created the SWD. If not, create it.

        // create the device definition
        MidiBluetoothDeviceDefinition definition;

        definition.BluetoothAddress = device.BluetoothAddress();
        definition.TransportSuppliedName = device.Name();           // need to ensure we also wire up the name changed event handler for this 

        // create the endpoint

        CreateEndpoint(definition);


    }
    else
    {
        // Disconnected. Remove the SWD.

        std::wstring instanceId{};

        // find the instance id we created for this BLE device


        RETURN_IF_FAILED(m_MidiDeviceManager->DeactivateEndpoint(instanceId.c_str()));

    }

    return S_OK;
}


_Use_decl_annotations_
HRESULT
CMidi2BluetoothMidiEndpointManager::OnBleAdvertisementReceived(
    ad::BluetoothLEAdvertisementWatcher /*source*/,
    ad::BluetoothLEAdvertisementReceivedEventArgs /*args*/)
{
    // Check to see if we already know about this device. If so, ignore


    // If we don't know about it, check to see if it's a device on our allowed list
    // If so, we can create a device entry for it. If not, ignore.
    // We may want a ble-wide policy option to allow any MIDI devices we see, vs deny by default. Default is deny all.


    return S_OK;
}



HRESULT 
CMidi2BluetoothMidiEndpointManager::StartAdvertisementWatcher()
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );

    auto adReceivedHandler = foundation::TypedEventHandler<ad::BluetoothLEAdvertisementWatcher, ad::BluetoothLEAdvertisementReceivedEventArgs>
        (this, &CMidi2BluetoothMidiEndpointManager::OnBleAdvertisementReceived);

    // wire up the event handler so we're notified when advertising messages are received. 
    // This will fire for every message received, even if we already know about the device.
    m_AdvertisementReceived = m_bleAdWatcher.Received(winrt::auto_revoke, adReceivedHandler);

    m_bleAdWatcher.AdvertisementFilter().Advertisement().ServiceUuids().Append(m_midiBleServiceUuid);

    return S_OK;
}

HRESULT 
CMidi2BluetoothMidiEndpointManager::StartDeviceWatcher()
{
    TraceLoggingWrite(
        MidiBluetoothMidiAbstractionTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );

    winrt::hstring deviceSelector = bt::BluetoothLEDevice::GetDeviceSelector();


    auto requestedProperties = winrt::single_threaded_vector<winrt::hstring>(
        {
            L"System.DeviceInterface.Bluetooth.DeviceAddress",
            L"System.DeviceInterface.Bluetooth.Flags",
            L"System.DeviceInterface.Bluetooth.LastConnectedTime",
            L"System.DeviceInterface.Bluetooth.Manufacturer",
            L"System.DeviceInterface.Bluetooth.ModelNumber",
            L"System.DeviceInterface.Bluetooth.ProductId",
            L"System.DeviceInterface.Bluetooth.ProductVersion",
            L"System.DeviceInterface.Bluetooth.ServiceGuid",
            L"System.DeviceInterface.Bluetooth.VendorId",
            L"System.DeviceInterface.Bluetooth.VendorIdSource",
            L"System.Devices.Connected",
            L"System.Devices.DeviceCapabilities",
            L"System.Devices.DeviceCharacteristics"
        }
    );


    m_Watcher = enumeration::DeviceInformation::CreateWatcher(deviceSelector, requestedProperties);

    auto deviceAddedHandler = foundation::TypedEventHandler<enumeration::DeviceWatcher, enumeration::DeviceInformation>(this, &CMidi2BluetoothMidiEndpointManager::OnDeviceAdded);
    auto deviceRemovedHandler = foundation::TypedEventHandler<enumeration::DeviceWatcher, enumeration::DeviceInformationUpdate>(this, &CMidi2BluetoothMidiEndpointManager::OnDeviceRemoved);
    auto deviceUpdatedHandler = foundation::TypedEventHandler<enumeration::DeviceWatcher, enumeration::DeviceInformationUpdate>(this, &CMidi2BluetoothMidiEndpointManager::OnDeviceUpdated);
    auto deviceStoppedHandler = foundation::TypedEventHandler<enumeration::DeviceWatcher, foundation::IInspectable>(this, &CMidi2BluetoothMidiEndpointManager::OnDeviceStopped);
    auto deviceEnumerationCompletedHandler = foundation::TypedEventHandler<enumeration::DeviceWatcher, foundation::IInspectable>(this, &CMidi2BluetoothMidiEndpointManager::OnEnumerationCompleted);

    m_DeviceAdded = m_Watcher.Added(winrt::auto_revoke, deviceAddedHandler);
    m_DeviceRemoved = m_Watcher.Removed(winrt::auto_revoke, deviceRemovedHandler);
    m_DeviceUpdated = m_Watcher.Updated(winrt::auto_revoke, deviceUpdatedHandler);
    m_DeviceStopped = m_Watcher.Stopped(winrt::auto_revoke, deviceStoppedHandler);
    m_DeviceEnumerationCompleted = m_Watcher.EnumerationCompleted(winrt::auto_revoke, deviceEnumerationCompletedHandler);


    m_Watcher.Start();

    return S_OK;
}

HRESULT 
CMidi2BluetoothMidiEndpointManager::CreateSelfPeripheralEndpoint()
{



    return S_OK;
}