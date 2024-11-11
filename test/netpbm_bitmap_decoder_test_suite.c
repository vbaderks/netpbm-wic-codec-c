// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "com_factory.h"
#include <unknwn.h>

#include "../src/guids.h"

#define CLOVE_SUITE_NAME netpbm_bitmap_decoder_test_suite
#include <wincodec.h>
#include <clove-unit/clove-unit.h>

IWICBitmapDecoder *CreateDecoder()
{
    IClassFactory *classFactory = GetClassObject(&CLSID_WICBitmapDecoder, &IID_IClassFactory);

    IWICBitmapDecoder *wicBitmapDecoder = NULL;
    const HRESULT hr = classFactory->lpVtbl->CreateInstance(classFactory, NULL, &IID_IWICBitmapDecoder, (void **)&wicBitmapDecoder);
    classFactory->lpVtbl->Release(classFactory);
    if (FAILED(hr))
    {
        return NULL;
    }

    return wicBitmapDecoder;
}


CLOVE_SUITE_SETUP_ONCE()
{
    ConstructComFactory();
}

CLOVE_SUITE_TEARDOWN_ONCE()
{
    DestructComFactory();
}

CLOVE_TEST(GetClassFactory)
{
    IClassFactory *classFactory = GetClassObject(&CLSID_WICBitmapDecoder, &IID_IClassFactory);

    CLOVE_NOT_NULL(classFactory);
    classFactory->lpVtbl->Release(classFactory);

    HRESULT result = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, result);
}

CLOVE_TEST(GetClassFactoryIUnknown)
{
    IUnknown *classFactory = GetClassObject(&CLSID_WICBitmapDecoder, &IID_IUnknown);

    CLOVE_NOT_NULL(classFactory);
    classFactory->lpVtbl->Release(classFactory);

    const HRESULT hr = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, hr);
}

CLOVE_TEST(CreateNetpbmBitmapDecoderGetIUnknown)
{
    IClassFactory *classFactory = GetClassObject(&CLSID_WICBitmapDecoder, &IID_IClassFactory);

    IUnknown *unknown = NULL;
    HRESULT hr = classFactory->lpVtbl->CreateInstance(classFactory, NULL, &IID_IWICBitmapDecoder, (void **)&unknown);
    CLOVE_UINT_EQ(S_OK, hr);

    CLOVE_NOT_NULL(unknown);

    classFactory->lpVtbl->Release(classFactory);
    unknown->lpVtbl->Release(unknown);

    hr = CallDllCanUnloadNow();
    CLOVE_UINT_EQ(S_OK, hr);
}

CLOVE_TEST(GetContainerFormat)
{
    IWICBitmapDecoder *wicBitmapDecoder = CreateDecoder();

    GUID containerFormat;
    const HRESULT hr = wicBitmapDecoder->lpVtbl->GetContainerFormat(wicBitmapDecoder, &containerFormat);

    CLOVE_UINT_EQ(S_OK, hr);
    CLOVE_IS_TRUE(IsEqualGUID(&CLSID_ContainerFormatNetpbm, &containerFormat));
}
