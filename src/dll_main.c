// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "macros.h"
#include "guids.h"
#include "property_store.h"

BOOL __stdcall DllMain(const HMODULE module, const DWORD reason_for_call, void * reserved)
{
    UNREFERENCED_PARAMETER(reserved);

    switch (reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        TRACE("netpbm-wic-codec::DllMain DLL_PROCESS_ATTACH \n");
        VERIFY(DisableThreadLibraryCalls(module));
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        TRACE("netpbm-wic-codec::DllMain DLL_PROCESS_DETACH \n");
        break;

    default:
        TRACE("netpbm-wic-codec::DllMain bad reason_for call\n");
        return false;
    }

    return true;
}

_Check_return_ HRESULT __stdcall DllGetClassObject(_In_ REFCLSID class_id, _In_ REFCLSID interface_id,
    _Outptr_ void** result)
{
    //if (class_id == id::netpbm_decoder)
    //{
    //    create_netpbm_bitmap_decoder_factory(interface_id, result);
    //    return S_OK;
    //}

    if (IsEqualGUID(class_id, &CLSID_PropertyStore))
    {
        return create_property_store_class_factory(interface_id, result);
    }

    TRACE("netpbm-wic-codec::DllGetClassObject error class not available\n");
    return CLASS_E_CLASSNOTAVAILABLE;
}

_Check_return_ HRESULT __stdcall DllCanUnloadNow(void)
{
    // Check if the DLL can be unloaded
    return S_OK;
    //if (/* condition to check if DLL can be unloaded */)
    //{
    //    return S_OK;
    //}
    //else
    //{
    //    return S_FALSE;
    //}
}
