// ------------------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the GitHub project root for license information.
// ------------------------------------------------------------------------------------------------

#define WINDOWSMIDISERVICES_EXPORTS


#include "WindowsMidiServicesClient.h"

namespace Microsoft::Windows::Midi
{
	struct MidiSession::impl
	{
		MidiObjectId _id;
		std::string _name;
		SYSTEMTIME _createdDateTime;

		std::vector<MidiDevice> _openDevices;
	};



}