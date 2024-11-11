// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "com_factory.h"

static HMODULE codec_library = NULL;

void ConstructComFactory(void)
{
    codec_library = LoadLibrary(L"netpbm-wic-codec-c.dll");
}

void DestructComFactory(void)
{
    if (codec_library != NULL)
    {
        FreeLibrary(codec_library);
    }
}

typedef HRESULT(WINAPI *DllGetClassObjectPtr)(const CLSID *rclsid, const IID *riid, void **ppv);


void *GetClassObject(const CLSID *rclsid, const IID *riid)
{
    DllGetClassObjectPtr pDllGetClassObject = (DllGetClassObjectPtr)GetProcAddress(codec_library, "DllGetClassObject");
    if (!pDllGetClassObject)
        return NULL;

    void *classObject = NULL;
    const HRESULT hr = pDllGetClassObject(rclsid, riid, &classObject);
    if (SUCCEEDED(hr))
        return classObject;

    return NULL;
}

typedef HRESULT(WINAPI *DllCanUnloadNowPtr)(void);
HRESULT CallDllCanUnloadNow(void)
{
    DllCanUnloadNowPtr pDllCanUnloadNow = (DllCanUnloadNowPtr)GetProcAddress(codec_library, "DllCanUnloadNow");
    if (!pDllCanUnloadNow)
        return E_FAIL;

    return pDllCanUnloadNow();
}
