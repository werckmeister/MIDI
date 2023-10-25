// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


#pragma once

class CMidi2VirtualPatchBayAbstractionModule : public ATL::CAtlDllModuleT< CMidi2VirtualPatchBayAbstractionModule >
{
public :
    DECLARE_LIBID(LIBID_Midi2VirtualPatchBayAbstractionLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MIDI2VIRTUALPATCHBAYABSTRACTION, "{77D3C311-3A2A-4785-8CE9-D14DA987DED3}")
};

extern class CMidi2VirtualPatchBayAbstractionModule _AtlModule;
