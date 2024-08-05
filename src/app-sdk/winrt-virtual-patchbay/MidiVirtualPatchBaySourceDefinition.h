// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App SDK and should be used
// in your Windows application via an official binary distribution.
// Further information: https://aka.ms/midi
// ============================================================================

#pragma once
#include "MidiVirtualPatchBaySourceDefinition.g.h"

namespace winrt::Microsoft::Windows::Devices::Midi2::VirtualPatchBay::implementation
{
    struct MidiVirtualPatchBaySourceDefinition : MidiVirtualPatchBaySourceDefinitionT<MidiVirtualPatchBaySourceDefinition>
    {
        MidiVirtualPatchBaySourceDefinition() = default;

        bool IsEnabled() { return m_isEnabled; }
        void IsEnabled(_In_ bool value) { m_isEnabled = value; }

        winrt::hstring EndpointDeviceId() { return m_endpointDeviceId; }
        void EndpointDeviceId(_In_ winrt::hstring const& value) { m_endpointDeviceId = value; }


        collections::IVector<midi2::MidiMessageType> IncludedMessageTypes();
        void IncludedMessageTypes(_In_ collections::IVector<midi2::MidiMessageType> const& value);

        collections::IVector<midi2::MidiGroup> IncludedGroups();
        void IncludedGroups(_In_ collections::IVector<midi2::MidiGroup> const& value);

    private:
        bool m_isEnabled{ true };
        winrt::hstring m_endpointDeviceId{};
    };
}
namespace winrt::Microsoft::Windows::Devices::Midi2::VirtualPatchBay::factory_implementation
{
    struct MidiVirtualPatchBaySourceDefinition : MidiVirtualPatchBaySourceDefinitionT<MidiVirtualPatchBaySourceDefinition, implementation::MidiVirtualPatchBaySourceDefinition>
    {
    };
}
