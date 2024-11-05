// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#define CLOVE_SUITE_NAME property_store_test_suite
#include <clove-unit/clove-unit.h>

#include <Windows.h>
#include <unknwn.h>

typedef HRESULT(WINAPI* DllGetClassObjectPtr)(
    const CLSID* rclsid,
    const IID* riid,
    void** ppv
    );

typedef HRESULT(WINAPI* DllCanUnloadNowPtr)(void);

static const GUID property_store =
{0x6f2df117, 0xab14, 0x4728, { 0x86, 0x1e, 0x80, 0xa3, 0x6a, 0xc5, 0x6b, 0xc4 }};

static HMODULE codec_library = NULL;

static void* GetClassObject(const IID* riid) {
    DllGetClassObjectPtr pDllGetClassObject = (DllGetClassObjectPtr)GetProcAddress(codec_library, "DllGetClassObject");
    if (!pDllGetClassObject)
        return NULL;

    void* classObject = NULL;
    HRESULT hr = pDllGetClassObject(&property_store, riid, &classObject);
    if (SUCCEEDED(hr))
        return classObject;

    return NULL;
}

static HRESULT CallDllCanUnloadNow() {
    DllCanUnloadNowPtr pDllCanUnloadNow = (DllCanUnloadNowPtr)GetProcAddress(codec_library, "DllCanUnloadNow");
    if (!pDllCanUnloadNow)
        return E_FAIL;

    return pDllCanUnloadNow();
}


CLOVE_SUITE_SETUP_ONCE() {
    codec_library = LoadLibrary(L"netpbm-wic-codec-c.dll");
}

CLOVE_SUITE_TEARDOWN_ONCE() {
    if (codec_library != NULL) {
        FreeLibrary(codec_library);
    }
}

CLOVE_TEST(GetClassFactory) {
    IClassFactory* classFactory = GetClassObject(&IID_IClassFactory);

    CLOVE_IS_TRUE(classFactory);
    classFactory->lpVtbl->Release(classFactory);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}

CLOVE_TEST(GetClassFactoryIUnknown) {
    IUnknown* classFactory = GetClassObject(&IID_IUnknown);

    CLOVE_IS_TRUE(classFactory);
    classFactory->lpVtbl->Release(classFactory);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}



