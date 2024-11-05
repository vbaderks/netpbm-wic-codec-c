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

static const GUID property_store =
{0x6f2df117, 0xab14, 0x4728, { 0x86, 0x1e, 0x80, 0xa3, 0x6a, 0xc5, 0x6b, 0xc4 }};

static HMODULE codec_library = NULL;

static IClassFactory* get_class_factory(void) {
    DllGetClassObjectPtr pDllGetClassObject = (DllGetClassObjectPtr)GetProcAddress(codec_library, "DllGetClassObject");
    if (!pDllGetClassObject)
        return NULL;

    IID iid = IID_IClassFactory;
    IClassFactory* pClassFactory = NULL;
    HRESULT hr = pDllGetClassObject(&property_store, &iid, (void**)&pClassFactory);
    if (SUCCEEDED(hr))
        return pClassFactory;

    return NULL;
}


CLOVE_SUITE_SETUP_ONCE() {
    codec_library = LoadLibrary(L"netpbm-wic-codec-c.dll");
}

CLOVE_SUITE_TEARDOWN_ONCE() {
    if (codec_library != NULL) {
        FreeLibrary(codec_library);
    }
}

CLOVE_TEST(create_test) {
    IClassFactory* classFactory = get_class_factory();

    // TODO: create instance and check if create was ok.
    CLOVE_IS_TRUE(classFactory);
}
