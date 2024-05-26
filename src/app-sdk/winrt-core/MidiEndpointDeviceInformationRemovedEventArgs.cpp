// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://aka.ms/midi
// ============================================================================

#include "pch.h"
#include "MidiEndpointDeviceInformationRemovedEventArgs.h"
#include "MidiEndpointDeviceInformationRemovedEventArgs.g.cpp"


namespace winrt::Microsoft::Windows::Devices::Midi2::implementation
{
    _Use_decl_annotations_
    void MidiEndpointDeviceInformationRemovedEventArgs::InternalInitialize(
        winrt::hstring const removedDeviceId,
        winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInformationUpdate
    ) noexcept
    {
        m_removedDeviceId = removedDeviceId;
        m_deviceInformationUpdate = deviceInformationUpdate;
    }
}
