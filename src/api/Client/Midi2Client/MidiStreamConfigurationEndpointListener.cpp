// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#include "pch.h"
#include "MidiStreamConfigurationEndpointListener.h"
#include "MidiStreamConfigurationEndpointListener.g.cpp"


namespace winrt::Windows::Devices::Midi2::implementation
{
    void MidiStreamConfigurationEndpointListener::Initialize()
    {
        throw hresult_not_implemented();
    }

    void MidiStreamConfigurationEndpointListener::OnEndpointConnectionOpened()
    {
        throw hresult_not_implemented();
    }

    void MidiStreamConfigurationEndpointListener::Cleanup()
    {
        throw hresult_not_implemented();
    }

    _Use_decl_annotations_
    void MidiStreamConfigurationEndpointListener::ProcessIncomingMessage(
        midi2::MidiMessageReceivedEventArgs const& /*args*/,
        bool& skipFurtherListeners,
        bool& skipMainMessageReceivedEvent)
    {
        skipFurtherListeners = false;
        skipMainMessageReceivedEvent = false;

        throw hresult_not_implemented();
    }

}
