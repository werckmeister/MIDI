// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once

#include "MidiInputEndpointConnection.g.h"
#include "MidiEndpointConnection.h"

#include "InternalMidiDeviceConnection.h"
#include "midi_service_interface.h"


namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiInputEndpointConnection : MidiInputEndpointConnectionT<MidiInputEndpointConnection, 
        Windows::Devices::Midi2::implementation::MidiEndpointConnection,
        IMidiCallback>
    {
        MidiInputEndpointConnection() = default;

        static hstring GetDeviceSelectorForInput() { return L""; /* TODO */ }

        STDMETHOD(Callback)(_In_ PVOID Data, _In_ UINT Size, _In_ LONGLONG Position) override;

        inline winrt::event_token MessageReceived(winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiMessageReceivedEventArgs> const& handler)
        {
            return _messagesReceivedEvent.add(handler);
        }

        inline void MessageReceived(winrt::event_token const& token) noexcept
        {
            _messagesReceivedEvent.remove(token);
        }




        winrt::event_token WordsReceived(winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiWordsReceivedEventArgs> const& handler)
        {
            return _wordsReceivedEvent.add(handler);
        }

        void WordsReceived(winrt::event_token const& token) noexcept
        {
            _wordsReceivedEvent.remove(token);
        }







        bool InternalStart();

    private:
        //winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Devices::Midi2::IMidiInputConnection, winrt::Windows::Devices::Midi2::MidiMessagesReceivedEventArgs>> _messagesReceivedEvent;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiMessageReceivedEventArgs>> _messagesReceivedEvent;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<IInspectable, winrt::Windows::Devices::Midi2::MidiWordsReceivedEventArgs>> _wordsReceivedEvent;




    };
}
namespace winrt::Windows::Devices::Midi2::factory_implementation
{
    struct MidiInputEndpointConnection : MidiInputEndpointConnectionT<MidiInputEndpointConnection, implementation::MidiInputEndpointConnection>
    {
    };
}
