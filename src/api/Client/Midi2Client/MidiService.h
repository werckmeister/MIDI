// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include "MidiService.g.h"

namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiService : MidiServiceT<MidiService>
    {
        MidiService() = default;

        static midi2::MidiServicePingResponseSummary PingService(
            _In_ uint8_t const pingCount
            ) noexcept;

        static midi2::MidiServicePingResponseSummary PingService(
            _In_ uint8_t const pingCount,
            _In_ uint32_t const timeoutMilliseconds
            ) noexcept;

        static foundation::Collections::IVectorView<midi2::MidiTransportInformation> GetInstalledTransports();

        // This will always return false for the first revs of Windows MIDI Services
        // but software developers want to use a property to identify when the feature
        // is enabled, without having to code new logic later.
        static bool IsMessageSchedulingEnabled() noexcept { return false; }

    private:



    };
}
namespace winrt::Windows::Devices::Midi2::factory_implementation
{
    struct MidiService : MidiServiceT<MidiService, implementation::MidiService, winrt::static_lifetime>
    {
    };
}
