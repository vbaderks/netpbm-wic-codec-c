// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "module.h"
#include "macros.h"
#include "guids.h"
#include "property_store.h"

BOOL __stdcall DllMain(const HMODULE module, const DWORD reasonForCall, const void *reserved)
{
    UNREFERENCED_PARAMETER(reserved);

    switch (reasonForCall)
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

_Use_decl_annotations_ HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFCLSID riid, void** ppv)
{
    //if (classId == id::netpbm_decoder)
    //{
    //    create_netpbm_bitmap_decoder_factory(interface_id, result);
    //    return S_OK;
    //}

    if (IsEqualGUID(rclsid, &CLSID_PropertyStore))
    {
        return create_property_store_class_factory(riid, ppv);
    }

    TRACE("netpbm-wic-codec-c::DllGetClassObject error class not available\n");
    return CLASS_E_CLASSNOTAVAILABLE;
}

_Use_decl_annotations_ HRESULT __stdcall DllCanUnloadNow(void)
{
    TRACE("netpbm-wic-codec-c::DllCanUnloadNow error class not available\n");
    return ModuleIsLocked() ? S_FALSE : S_OK;
}
