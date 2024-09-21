// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once


class CMidi2NetworkMidiConfigurationManager :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    IMidiAbstractionConfigurationManager>

{
public:
    STDMETHOD(Initialize(_In_ GUID AbstractionId, _In_ IUnknown* MidiDeviceManager, _In_ IUnknown* MidiServiceConfigurationManagerInterface));
    STDMETHOD(UpdateConfiguration(_In_ LPCWSTR ConfigurationJsonSection, _In_ BOOL IsFromConfigurationFile, _Out_ BSTR* Response));
    STDMETHOD(Cleanup)();

    STDMETHOD(ValidateHostDefinition(_In_ MidiNetworkUdpHostDefinition& definition, _Out_ winrt::hstring& errorMessage));
//    STDMETHOD(ValidateClientDefinition(_In_ MidiNetworkUdpClientDefinition& definition));

private:
    wil::com_ptr_nothrow<IMidiDeviceManagerInterface> m_MidiDeviceManager;

    GUID m_abstractionId;   // kept for convenience
};
