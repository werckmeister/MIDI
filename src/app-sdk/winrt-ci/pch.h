﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://aka.ms/midi
// ============================================================================


#pragma once

//#define _VSDESIGNER_DONT_LOAD_AS_DLL


#include <unknwn.h>

#include <Windows.h>

//#include <wil/cppwinrt.h> // must be before the first C++ WinRT header
//#include <wil/result.h>

#include <wil\resource.h>
#include <wil\result_macros.h>
#include <wil\tracelogging.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>

namespace json = ::winrt::Windows::Data::Json;
namespace foundation = ::winrt::Windows::Foundation;
namespace collections = ::winrt::Windows::Foundation::Collections;


#include <winrt/Microsoft.Windows.Devices.Midi2.h>
namespace midi2 = ::winrt::Microsoft::Windows::Devices::Midi2;



#include <stdint.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cwctype>
#include <queue>
#include <mutex>
#include <format>
#include <filesystem>

// internal

#include "ump_helpers.h"
#include "hstring_util.h"
#include "wstring_util.h"
namespace internal = ::WindowsMidiServicesInternal;

//#include "MidiDefs.h"
//#include "WindowsMidiServices.h"
//#include "WindowsMidiServices_i.c"
//#include "Midi2MidiSrvAbstraction.h"

#include "json_defs.h"
#include "json_helpers.h"
//#include "swd_helpers.h"
#include "resource_util.h"

#include "SdkTraceLogging.h"

//#include "MidiXProc.h"
#include "resource.h"

namespace winrt::Microsoft::Windows::Devices::Midi2::CapabilityInquiry {};
namespace winrt::Microsoft::Windows::Devices::Midi2::CapabilityInquiry::implementation {};

namespace ci = ::winrt::Microsoft::Windows::Devices::Midi2::CapabilityInquiry;
namespace implementation = ::winrt::Microsoft::Windows::Devices::Midi2::CapabilityInquiry::implementation;

#include "MidiUniqueId.h"


