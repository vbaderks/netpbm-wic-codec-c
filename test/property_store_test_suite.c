// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#define CLOVE_SUITE_NAME property_store_test_suite
#include <clove-unit/clove-unit.h>

#include <Windows.h>
#include <unknwn.h>
#include <propsys.h>

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

    CLOVE_NOT_NULL(classFactory);
    classFactory->lpVtbl->Release(classFactory);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}

CLOVE_TEST(GetClassFactoryIUnknown) {
    IUnknown* classFactory = GetClassObject(&IID_IUnknown);

    CLOVE_NOT_NULL(classFactory);
    classFactory->lpVtbl->Release(classFactory);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}

CLOVE_TEST(CreatePropertyStoreGetIUnknown)
{
    IClassFactory *classFactory = GetClassObject(&IID_IClassFactory);

    IUnknown *unknown = NULL;
    classFactory->lpVtbl->CreateInstance(classFactory, NULL, &IID_IInitializeWithStream, (void **)&unknown);

    CLOVE_NOT_NULL(unknown);

    classFactory->lpVtbl->Release(classFactory);
    unknown->lpVtbl->Release(unknown);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}

CLOVE_TEST(CreatePropertyStoreGetIInitializeWithStream) {
    IClassFactory* classFactory = GetClassObject(&IID_IClassFactory);

    IInitializeWithStream* initializeWithStream = NULL;
    classFactory->lpVtbl->CreateInstance(classFactory, NULL, &IID_IInitializeWithStream, (void**)&initializeWithStream);

    CLOVE_NOT_NULL(initializeWithStream);

    classFactory->lpVtbl->Release(classFactory);
    initializeWithStream->lpVtbl->Release(initializeWithStream);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}

CLOVE_TEST(CreatePropertyStoreGetIPropertyStore) {
    IClassFactory* classFactory = GetClassObject(&IID_IClassFactory);

    IPropertyStore* propertyStore = NULL;
    classFactory->lpVtbl->CreateInstance(classFactory, NULL, &IID_IPropertyStore, (void**)&propertyStore);

    CLOVE_NOT_NULL(propertyStore);
    if (propertyStore)
    {
        propertyStore->lpVtbl->Release(propertyStore);
    }

    classFactory->lpVtbl->Release(classFactory);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}



