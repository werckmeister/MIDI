// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include "MidiMessageConverter.g.h"

namespace winrt::Microsoft::Devices::Midi2::Messages::implementation
{
    struct MidiMessageConverter
    {
        MidiMessageConverter() = default;

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1Message(
            _In_ internal::MidiTimestamp timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ uint8_t const statusByte
        ) noexcept;

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1Message(
            _In_ internal::MidiTimestamp timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ uint8_t const statusByte,
            _In_ uint8_t const dataByte1
        ) noexcept;

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1Message(
            _In_ internal::MidiTimestamp timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ uint8_t const statusByte,
            _In_ uint8_t const dataByte1,
            _In_ uint8_t const dataByte2
        ) noexcept;




        // System Common and System Real-time

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1TimeCodeMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiTimeCodeMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1SongPositionPointerMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiSongPositionPointerMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1SongSelectMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiSongSelectMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1TuneRequestMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiTuneRequestMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }


        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1TimingClockMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiTimingClockMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }


        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1StartMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiStartMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1ContinueMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiContinueMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }


        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1StopMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiStopMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1ActiveSensingMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiActiveSensingMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1SystemResetMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiSystemResetMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::SystemCommon32);

            return message;
        }


        // Channel Voice

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1ChannelPressureMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiChannelPressureMessage const& originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::Midi1ChannelVoice32);

            return message;
        }


        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1NoteOffMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiNoteOffMessage const&  originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::Midi1ChannelVoice32);

            return message;
        }


        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1NoteOnMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiNoteOnMessage const&  originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::Midi1ChannelVoice32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1PitchBendChangeMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiPitchBendChangeMessage const&  originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::Midi1ChannelVoice32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1PolyphonicKeyPressureMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiPolyphonicKeyPressureMessage const&  originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::Midi1ChannelVoice32);

            return message;
        }

        _Success_(return != nullptr)
        static midi2::MidiMessage32 ConvertMidi1ProgramChangeMessage(
            _In_ internal::MidiTimestamp const timestamp,
            _In_ midi2::MidiGroup const& group,
            _In_ midi1::MidiProgramChangeMessage const&  originalMessage
        ) noexcept
        {
            midi2::MidiMessage32 message;
            message.Timestamp(timestamp);
            message.Word0(InternalConvertBytes(group.Index(), (Windows::Devices::Midi::IMidiMessage)originalMessage));

            message.MessageType(midi2::MidiMessageType::Midi1ChannelVoice32);

            return message;
        }


    private:
        static uint32_t InternalConvertBytes(
            _In_ uint8_t const groupIndex,
            _In_ midi1::IMidiMessage const& originalMessage
        ) noexcept;

    };
}
namespace winrt::Microsoft::Devices::Midi2::Messages::factory_implementation
{
    struct MidiMessageConverter : MidiMessageConverterT<MidiMessageConverter, implementation::MidiMessageConverter, winrt::static_lifetime>
    {
    };
}
