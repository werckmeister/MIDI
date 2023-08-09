// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

// ----------------------------------------------------------------------------
// This requires the version of MidiSrv with the hard-coded loopback endpoint
// ----------------------------------------------------------------------------


#include "pch.h"

#include "catch_amalgamated.hpp"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>

#include <Windows.h>

#include <wil\resource.h>

#include "..\api-core\memory_buffer.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Midi2;


auto ump32mt = MidiUmpMessageType::Midi1ChannelVoice32;
auto ump64mt = MidiUmpMessageType::Midi2ChannelVoice64;
auto ump96mt = MidiUmpMessageType::FutureReservedB96;
auto ump128mt = MidiUmpMessageType::UmpStream128;





#define BIDI_ENDPOINT_DEVICE_ID L"foobarbaz"

TEST_CASE("Connected.Benchmark.APIWords Send / receive word array through loopback")
{
	std::cout << std::endl << "API Words benchmark" << std::endl;

	uint32_t numMessagesToSend = 1000;
	uint32_t receivedMessageCount{};

	wil::unique_event_nothrow allMessagesReceived;
	allMessagesReceived.create();

	uint64_t setupStartTimestamp = MidiClock::GetMidiTimestamp();

	auto settings = MidiSessionSettings::Default();
	auto session = MidiSession::CreateSession(L"Test Session Name", settings);

	REQUIRE((bool)(session.IsOpen()));
	REQUIRE((bool)(session.Connections().Size() == 0));

	auto conn1 = session.ConnectBidirectionalEndpoint(BIDI_ENDPOINT_DEVICE_ID, nullptr);

	REQUIRE((bool)(conn1 != nullptr));


	//auto ump32mt = MidiUmpMessageType::UtilityMessage32;
	//auto ump64mt = MidiUmpMessageType::DataMessage64;
	//auto ump96mt = MidiUmpMessageType::FutureReservedB96;
	//auto ump128mt = MidiUmpMessageType::FlexData128;

	std::vector<uint64_t> timestampDeltas;
	timestampDeltas.reserve(numMessagesToSend);


	auto ReceivedHandler = [&allMessagesReceived, &receivedMessageCount, &numMessagesToSend, &timestampDeltas](winrt::Windows::Foundation::IInspectable const& /*sender*/, MidiMessageReceivedEventArgs const& args)
		{
//			REQUIRE((bool)(args != nullptr));

			receivedMessageCount++;

			// this is to help with calculating jitter. Keep in mind that jitter will be 
			// negatively affected by our receive loop as well, but should be close enough
			// to real-world expectations
			uint64_t currentStamp = MidiClock::GetMidiTimestamp();
			uint64_t umpStamp = args.Timestamp();
			//uint64_t umpStamp = args.Timestamp();
			timestampDeltas.push_back(currentStamp - umpStamp);

			if (receivedMessageCount == numMessagesToSend)
			{
				allMessagesReceived.SetEvent();
			}

		};

	auto eventRevokeToken = conn1.MessageReceived(ReceivedHandler);


	uint32_t numBytes = 0;
	uint32_t ump32Count = 0;
	uint32_t ump64Count = 0;
	uint32_t ump96Count = 0;
	uint32_t ump128Count = 0;


	// send messages
	uint64_t sendingStartTimestamp = MidiClock::GetMidiTimestamp();

	// the distribution of messages here tries to mimic what we expect to be
	// a reasonably typical mix. In reality, almost all messages going back
	// and forth with an endpoint during a performance are going to be
	// UMP32 (MIDI 1.0 CV) or UMP64 (MIDI 2.0 CV), with the others in only at 
	// certain times. The exception is any device which relies on SysEx for
	// parameter changes on the fly.

	uint32_t words[]{ 0x40000000,0,0,0 };
	uint32_t wordCount{};

	for (int i = 0; i < numMessagesToSend; i++)
	{
		switch (i % 12)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		{
			words[0] = (uint32_t)ump32mt << 28;
			wordCount = 1;
			ump32Count++;
		}
		break;

		case 4:
		case 5:
		case 6:
		case 7:
		{
			words[0] = (uint32_t)ump64mt << 28;
			wordCount = 2;
			ump64Count++;
		}
		break;

		case 8:
		{
			words[0] = (uint32_t)ump96mt << 28;
			wordCount = 3;
			ump96Count++;
		}
		break;

		case 9:
		case 10:
		case 11:
		{
			words[0] = (uint32_t)ump128mt << 28;
			wordCount = 4;
			ump128Count++;
		}
		break;
		}

		numBytes += sizeof(uint32_t) * wordCount + sizeof(uint64_t);
		conn1.SendUmpWordArray(MidiClock::GetMidiTimestamp(), words, wordCount);
	}

	uint64_t sendingFinishTimestamp = MidiClock::GetMidiTimestamp();


	// Wait for incoming message
	if (!allMessagesReceived.wait(30000))
	{
		std::cout << "Failure waiting for messages, timed out." << std::endl;
	}

	uint64_t endingTimestamp = MidiClock::GetMidiTimestamp();

	REQUIRE(receivedMessageCount == numMessagesToSend);


	uint64_t sendOnlyDurationDelta = sendingFinishTimestamp - sendingStartTimestamp;
	uint64_t sendReceiveDurationDelta = endingTimestamp - sendingStartTimestamp;
	uint64_t setupDurationDelta = sendingStartTimestamp - setupStartTimestamp;

	uint64_t freq = MidiClock::GetMidiTimestampFrequency();

	//	std::cout << " - timeoutCounter " << std::dec << timeoutCounter << std::endl;

	std::cout << "Num Messages:                " << std::dec << numMessagesToSend << std::endl;
	std::cout << "- Count UMP32                " << std::dec << ump32Count << std::endl;
	std::cout << "- Count UMP64                " << std::dec << ump64Count << std::endl;
	std::cout << "- Count UMP96                " << std::dec << ump96Count << std::endl;
	std::cout << "- Count UMP128               " << std::dec << ump128Count << std::endl;
	std::cout << "Num Bytes (inc timestamp):   " << std::dec << numBytes << std::endl;
	std::cout << "Timestamp Frequency:         " << std::dec << freq << " hz (ticks/second)" << std::endl;
	std::cout << "-----------------------------" << std::endl;
	std::cout << "Setup Start Timestamp:       " << std::dec << setupStartTimestamp << std::endl;
	std::cout << "Setup/Connection Delta:      " << std::dec << setupDurationDelta << " ticks" << std::endl;
	std::cout << "Sending Start timestamp:     " << std::dec << sendingStartTimestamp << std::endl;
	std::cout << "Sending Stop timestamp:      " << std::dec << sendingFinishTimestamp << std::endl;
	std::cout << "Send/Rec End timestamp:      " << std::dec << endingTimestamp << std::endl;
	std::cout << "Sending/Receiving Delta:     " << std::dec << sendReceiveDurationDelta << " ticks" << std::endl;

	// calculate time to connect up, create the endpoint, etc.

	double setupSeconds = setupDurationDelta / (double)freq;
	double setupMilliseconds = setupSeconds * 1000.0;
	double setupMicroseconds = setupMilliseconds * 1000;

	// calculate send-only totals (included in send/receive totals)

	double sendOnlySeconds = sendOnlyDurationDelta / (double)freq;
	double sendOnlyMilliseconds = sendOnlySeconds * 1000.0;
	double sendOnlyMicroseconds = sendOnlyMilliseconds * 1000;
	double sendOnlyAverageMilliseconds = sendOnlyMilliseconds / (double)numMessagesToSend;
	double sendOnlyAverageMicroseconds = sendOnlyAverageMilliseconds * 1000;

	// calculate send/receive totals

	double sendReceiveSeconds = sendReceiveDurationDelta / (double)freq;
	double sendReceiveMilliseconds = sendReceiveSeconds * 1000.0;
	double sendReceiveMicroseconds = sendReceiveMilliseconds * 1000;
	double sendReceiveAverageMilliseconds = sendReceiveMilliseconds / (double)numMessagesToSend;
	double sendReceiveAverageMicroseconds = sendReceiveAverageMilliseconds * 1000;

	// jitter

	// output results

	std::cout << std::endl;
	std::cout << "Prep" << std::endl;
	std::cout << "- Setup and connect:           " << std::dec << std::fixed << setupMilliseconds << "ms (" << setupSeconds << " seconds, " << setupMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;
	std::cout << "Send loop" << std::endl;
	std::cout << "- Send only:                   " << std::dec << std::fixed << sendOnlyMilliseconds << "ms (" << sendOnlySeconds << " seconds, " << sendOnlyMicroseconds << " microseconds)." << std::endl;
	std::cout << "- Average single send          " << std::dec << std::fixed << sendOnlyAverageMilliseconds << "ms (" << sendOnlyAverageMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;
	std::cout << "Send Loop, Receive Loop, Callback" << std::endl;
	std::cout << "- Send/receive total:          " << std::dec << std::fixed << sendReceiveMilliseconds << "ms (" << sendReceiveSeconds << " seconds, " << sendReceiveMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;
	std::cout << "Single message round-trip" << std::endl;
	std::cout << "- Average single send/receive: " << std::dec << std::fixed << sendReceiveAverageMilliseconds << "ms (" << sendReceiveAverageMicroseconds << " microseconds)." << std::endl;
	//std::cout << "- Min single send/receive:     " << std::dec << std::fixed << minDeltaMilliseconds << "ms (" << minDeltaMicroseconds << " microseconds)." << std::endl;
	//std::cout << "- Max single send/receive:     " << std::dec << std::fixed << maxDeltaMilliseconds << "ms (" << maxDeltaMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;


	if (timestampDeltas.size() > 0)
	{
		auto [minDeltaTicks, maxDeltaTicks] = std::minmax_element(begin(timestampDeltas), end(timestampDeltas));

		double minDeltaMilliseconds = *minDeltaTicks / (double)freq;
		double minDeltaMicroseconds = minDeltaMilliseconds * 1000;

		double maxDeltaMilliseconds = *maxDeltaTicks / (double)freq;
		double maxDeltaMicroseconds = maxDeltaMilliseconds * 1000;

		double minToMaxDeltaMilliseconds = maxDeltaMilliseconds - minDeltaMilliseconds;
		double minToMaxDeltaMicroseconds = maxDeltaMicroseconds - minDeltaMicroseconds;

		double totalDeltaTicks = std::accumulate(begin(timestampDeltas), end(timestampDeltas), 0.0);
		double avgDeltaTicks = totalDeltaTicks / (double)numMessagesToSend;	// this should be close to the other calculated average
		double avgDeltaTicksMilliseconds = avgDeltaTicks / (double)freq;
		double avgDeltaTicksMicroseconds = avgDeltaTicksMilliseconds * 1000;

		// adapted from: https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
		double accum = 0.0;
		std::for_each(begin(timestampDeltas), end(timestampDeltas), [&](const double d) { accum += (d - avgDeltaTicks) * (d - avgDeltaTicks); });
		double stdevTicks = std::sqrt(accum / numMessagesToSend - 1);

		double stdevDeltaMilliseconds = stdevTicks / (double)freq;
		double stdevDeltaMicroseconds = stdevDeltaMilliseconds * 1000;


		std::cout << "Round-trip jitter" << std::endl;
		std::cout << "- Max - min:                   " << std::dec << std::fixed << minToMaxDeltaMilliseconds << "ms (" << minToMaxDeltaMicroseconds << " microseconds)." << std::endl;
		std::cout << "- Average:                     " << std::dec << std::fixed << avgDeltaTicksMilliseconds << "ms (" << avgDeltaTicksMicroseconds << " microseconds)." << std::endl;
		std::cout << "- Standard deviation:          " << std::dec << std::fixed << stdevDeltaMilliseconds << "ms (" << stdevDeltaMicroseconds << " microseconds)." << std::endl;
	}

	// unwire event
	conn1.MessageReceived(eventRevokeToken);

	// cleanup endpoint. Technically not required as session will do it
	session.DisconnectEndpointConnection(conn1.Id());
}


#if false

TEST_CASE("Connected.Benchmark.APIUmp Send / receive UMPs through loopback")
{
	std::cout << std::endl << "API UMP benchmark" << std::endl;

	uint32_t numMessagesToSend = 1000;
	uint32_t receivedMessageCount{};

	wil::unique_event_nothrow allMessagesReceived;
	allMessagesReceived.create();

	uint64_t setupStartTimestamp = MidiClock::GetMidiTimestamp();

	auto settings = MidiSessionSettings::Default();
	auto session = MidiSession::CreateSession(L"Test Session Name", settings);

	REQUIRE((bool)(session.IsOpen()));
	REQUIRE((bool)(session.Connections().Size() == 0));

	auto conn1 = session.ConnectBidirectionalEndpoint(BIDI_ENDPOINT_DEVICE_ID, nullptr);

	REQUIRE((bool)(conn1 != nullptr));


	//auto ump32mt = MidiUmpMessageType::UtilityMessage32;
	//auto ump64mt = MidiUmpMessageType::DataMessage64;
	//auto ump96mt = MidiUmpMessageType::FutureReservedB96;
	//auto ump128mt = MidiUmpMessageType::FlexData128;

	std::vector<uint64_t> timestampDeltas;
	timestampDeltas.reserve(numMessagesToSend);


	auto MessageReceivedHandler = [&allMessagesReceived, &receivedMessageCount, &numMessagesToSend, &timestampDeltas](winrt::Windows::Foundation::IInspectable const& /*sender*/, MidiMessageReceivedEventArgs const& args)
		{
			REQUIRE((bool)(args != nullptr));

			receivedMessageCount++;

			// this is to help with calculating jitter. Keep in mind that jitter will be 
			// negatively affected by our receive loop as well, but should be close enough
			// to real-world expectations
			uint64_t currentStamp = MidiClock::GetMidiTimestamp();
			uint64_t umpStamp = args.Ump().Timestamp();
			//uint64_t umpStamp = args.Timestamp();
			timestampDeltas.push_back(currentStamp - umpStamp);

			if (receivedMessageCount == numMessagesToSend)
			{
				allMessagesReceived.SetEvent();
			}

		};

	auto eventRevokeToken = conn1.MessageReceived(MessageReceivedHandler);


	uint32_t numBytes = 0;
	uint32_t ump32Count = 0;
	uint32_t ump64Count = 0;
	uint32_t ump96Count = 0;
	uint32_t ump128Count = 0;


	// send messages
	uint64_t sendingStartTimestamp = MidiClock::GetMidiTimestamp();

	// the distribution of messages here tries to mimic what we expect to be
	// a reasonably typical mix. In reality, almost all messages going back
	// and forth with an endpoint during a performance are going to be
	// UMP32 (MIDI 1.0 CV) or UMP64 (MIDI 2.0 CV), with the others in only at 
	// certain times. The exception is any device which relies on SysEx for
	// parameter changes on the fly.

	IMidiUmp ump;
	MidiUmp32 ump32{};
	MidiUmp64 ump64{};
	MidiUmp96 ump96{};
	MidiUmp128 ump128{};

	for (int i = 0; i < numMessagesToSend; i++)
	{

		switch (i % 12)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		{
			ump32.MessageType(ump32mt);
			ump = ump32.as<IMidiUmp>();
			numBytes += sizeof(uint32_t) + sizeof(uint64_t);
			ump32Count++;
		}
		break;

		case 4:
		case 5:
		case 6:
		case 7:
		{
			ump64.MessageType(ump64mt);
			ump = ump64.as<IMidiUmp>();
			numBytes += sizeof(uint32_t) * 2 + sizeof(uint64_t);
			ump64Count++;
		}
		break;

		case 8:
		{
			ump96.MessageType(ump96mt);
			ump = ump96.as<IMidiUmp>();
			numBytes += sizeof(uint32_t) * 3 + sizeof(uint64_t);
			ump96Count++;
		}
		break;

		case 9:
		case 10:
		case 11:
		{
			ump128.MessageType(ump128mt);
			ump = ump128.as<IMidiUmp>();
			numBytes += sizeof(uint32_t) * 4 + sizeof(uint64_t);
			ump128Count++;
		}
		break;
		}

		ump.Timestamp(MidiClock::GetMidiTimestamp());
		conn1.SendUmp(ump);

	}

	uint64_t sendingFinishTimestamp = MidiClock::GetMidiTimestamp();


	// Wait for incoming message

	if (!allMessagesReceived.wait(30000))
	{
		std::cout << "Failure waiting for messages, timed out." << std::endl;
	}

	uint64_t endingTimestamp = MidiClock::GetMidiTimestamp();

	REQUIRE(receivedMessageCount == numMessagesToSend);


	uint64_t sendOnlyDurationDelta = sendingFinishTimestamp - sendingStartTimestamp;	// this will contain some receives
	uint64_t sendReceiveDurationDelta = endingTimestamp - sendingStartTimestamp;
	uint64_t setupDurationDelta = sendingStartTimestamp - setupStartTimestamp;

	uint64_t freq = MidiClock::GetMidiTimestampFrequency();

	//	std::cout << " - timeoutCounter " << std::dec << timeoutCounter << std::endl;

	std::cout << "Num Messages:                " << std::dec << numMessagesToSend << std::endl;
	std::cout << "- Count UMP32                " << std::dec << ump32Count << std::endl;
	std::cout << "- Count UMP64                " << std::dec << ump64Count << std::endl;
	std::cout << "- Count UMP96                " << std::dec << ump96Count << std::endl;
	std::cout << "- Count UMP128               " << std::dec << ump128Count << std::endl;
	std::cout << "Num Bytes (inc timestamp):   " << std::dec << numBytes << std::endl;
	std::cout << "Timestamp Frequency:         " << std::dec << freq << " hz (ticks/second)" << std::endl;
	std::cout << "-----------------------------" << std::endl;
	std::cout << "Setup Start Timestamp:       " << std::dec << setupStartTimestamp << std::endl;
	std::cout << "Setup/Connection Delta:      " << std::dec << setupDurationDelta << " ticks" << std::endl;
	std::cout << "Sending Start timestamp:     " << std::dec << sendingStartTimestamp << std::endl;
	std::cout << "Sending Stop timestamp:      " << std::dec << sendingFinishTimestamp << std::endl;
	std::cout << "Send/Rec End timestamp:      " << std::dec << endingTimestamp << std::endl;
	std::cout << "Sending/Receiving Delta:     " << std::dec << sendReceiveDurationDelta << " ticks" << std::endl;

	// calculate time to connect up, create the endpoint, etc.

	double setupSeconds = setupDurationDelta / (double)freq;
	double setupMilliseconds = setupSeconds * 1000.0;
	double setupMicroseconds = setupMilliseconds * 1000;

	// calculate send-only totals (included in send/receive totals)

	double sendOnlySeconds = sendOnlyDurationDelta / (double)freq;
	double sendOnlyMilliseconds = sendOnlySeconds * 1000.0;
	double sendOnlyMicroseconds = sendOnlyMilliseconds * 1000;
	double sendOnlyAverageMilliseconds = sendOnlyMilliseconds / (double)numMessagesToSend;
	double sendOnlyAverageMicroseconds = sendOnlyAverageMilliseconds * 1000;

	// calculate send/receive totals

	double sendReceiveSeconds = sendReceiveDurationDelta / (double)freq;
	double sendReceiveMilliseconds = sendReceiveSeconds * 1000.0;
	double sendReceiveMicroseconds = sendReceiveMilliseconds * 1000;
	double sendReceiveAverageMilliseconds = sendReceiveMilliseconds / (double)numMessagesToSend;
	double sendReceiveAverageMicroseconds = sendReceiveAverageMilliseconds * 1000;

	// jitter

	// output results

	std::cout << std::endl;
	std::cout << "Prep" << std::endl;
	std::cout << "- Setup and connect:           " << std::dec << std::fixed << setupMilliseconds << "ms (" << setupSeconds << " seconds, " << setupMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;
	std::cout << "Send loop" << std::endl;
	std::cout << "- Send only:                   " << std::dec << std::fixed << sendOnlyMilliseconds << "ms (" << sendOnlySeconds << " seconds, " << sendOnlyMicroseconds << " microseconds)." << std::endl;
	std::cout << "- Average single send          " << std::dec << std::fixed << sendOnlyAverageMilliseconds << "ms (" << sendOnlyAverageMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;
	std::cout << "Send Loop, Receive Loop, Callback" << std::endl;
	std::cout << "- Send/receive total:          " << std::dec << std::fixed << sendReceiveMilliseconds << "ms (" << sendReceiveSeconds << " seconds, " << sendReceiveMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;
	std::cout << "Single message round-trip" << std::endl;
	std::cout << "- Average single send/receive: " << std::dec << std::fixed << sendReceiveAverageMilliseconds << "ms (" << sendReceiveAverageMicroseconds << " microseconds)." << std::endl;
	//std::cout << "- Min single send/receive:     " << std::dec << std::fixed << minDeltaMilliseconds << "ms (" << minDeltaMicroseconds << " microseconds)." << std::endl;
	//std::cout << "- Max single send/receive:     " << std::dec << std::fixed << maxDeltaMilliseconds << "ms (" << maxDeltaMicroseconds << " microseconds)." << std::endl;
	std::cout << std::endl;


	if (timestampDeltas.size() > 0)
	{
		auto [minDeltaTicks, maxDeltaTicks] = std::minmax_element(begin(timestampDeltas), end(timestampDeltas));

		double minDeltaMilliseconds = *minDeltaTicks / (double)freq;
		double minDeltaMicroseconds = minDeltaMilliseconds * 1000;

		double maxDeltaMilliseconds = *maxDeltaTicks / (double)freq;
		double maxDeltaMicroseconds = maxDeltaMilliseconds * 1000;

		double minToMaxDeltaMilliseconds = maxDeltaMilliseconds - minDeltaMilliseconds;
		double minToMaxDeltaMicroseconds = maxDeltaMicroseconds - minDeltaMicroseconds;

		double totalDeltaTicks = std::accumulate(begin(timestampDeltas), end(timestampDeltas), 0.0);
		double avgDeltaTicks = totalDeltaTicks / (double)numMessagesToSend;	// this should be close to the other calculated average
		double avgDeltaTicksMilliseconds = avgDeltaTicks / (double)freq;
		double avgDeltaTicksMicroseconds = avgDeltaTicksMilliseconds * 1000;

		// adapted from: https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
		double accum = 0.0;
		std::for_each(begin(timestampDeltas), end(timestampDeltas), [&](const double d) { accum += (d - avgDeltaTicks) * (d - avgDeltaTicks); });
		float stdevTicks = std::sqrtf(accum / numMessagesToSend - 1);

		double stdevDeltaMilliseconds = stdevTicks / (double)freq;
		double stdevDeltaMicroseconds = stdevDeltaMilliseconds * 1000;


		std::cout << "Round-trip jitter" << std::endl;
		std::cout << "- Max - min:                   " << std::dec << std::fixed << minToMaxDeltaMilliseconds << "ms (" << minToMaxDeltaMicroseconds << " microseconds)." << std::endl;
		std::cout << "- Average:                     " << std::dec << std::fixed << avgDeltaTicksMilliseconds << "ms (" << avgDeltaTicksMicroseconds << " microseconds)." << std::endl;
		std::cout << "- Standard deviation:          " << std::dec << std::fixed << stdevDeltaMilliseconds << "ms (" << stdevDeltaMicroseconds << " microseconds)." << std::endl;
	}

	// unwire event
	conn1.MessageReceived(eventRevokeToken);

	// cleanup endpoint. Technically not required as session will do it
	session.DisconnectEndpointConnection(conn1.Id());
}




#endif
