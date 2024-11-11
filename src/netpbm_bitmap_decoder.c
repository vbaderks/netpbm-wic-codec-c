// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "netpbm_bitmap_decoder.h"

#include "class_factory.h"
#include "guids.h"
#include "macros.h"
#include "module.h"
#include "pnm_header.h"


typedef struct NetpbmBitmapDecoder
{
    IWICBitmapDecoder wicBitmapDecoder;
    volatile bool initialized;
    LONG refCount;
} NetpbmBitmapDecoder;


static ULONG __stdcall AddRef(_In_ IWICBitmapDecoder *this)
{
    NetpbmBitmapDecoder *netpbmBitmapDecoder = (NetpbmBitmapDecoder *)this;
    return InterlockedIncrement(&netpbmBitmapDecoder->refCount);
}

static ULONG __stdcall Release(_In_ IWICBitmapDecoder *this)
{
    NetpbmBitmapDecoder *netpbmBitmapDecoder = (NetpbmBitmapDecoder *)this;
    const ULONG refCount = InterlockedDecrement(&netpbmBitmapDecoder->refCount);
    if (refCount == 0)
    {
        free(netpbmBitmapDecoder);
        ModuleRelease();
    }

    return refCount;
}

static HRESULT __stdcall QueryInterface(_In_ IWICBitmapDecoder *this, _In_ REFIID riid, _COM_Outptr_ void **ppv)
{
    static const QITAB qiTable[] = {QITABENT(NetpbmBitmapDecoder, IWICBitmapDecoder), {NULL, 0}};

    return QISearch(this, qiTable, riid, ppv);
}


static HRESULT __stdcall QueryCapability([[maybe_unused]] IWICBitmapDecoder *this, [[maybe_unused]] IStream *stream,
                                         [[maybe_unused]] DWORD *capability)
{
    TRACE("netpbm_bitmap_decoder-c::QueryCapability.1");

    if (!stream || !capability)
        return E_INVALIDARG;

    *capability = 0;

    // Custom decoder implementations should save the current position of the specified IStream,
    // read whatever information is necessary in order to determine which capabilities
    // it can provide for the supplied stream, and restore the stream position.
    LARGE_INTEGER move = {};
    ULARGE_INTEGER original_position;
    HRESULT result = stream->lpVtbl->Seek(stream, move, STREAM_SEEK_CUR, &original_position);
    if (FAILED(result))
        return result;

    if (IsPnmFile(stream))
    {
        *capability = WICBitmapDecoderCapabilityCanDecodeAllImages;
    }

    result = stream->lpVtbl->Seek(stream, *(LARGE_INTEGER *)&original_position, STREAM_SEEK_CUR, NULL);
    if (FAILED(result))
        return result;

    return S_OK;
}

static HRESULT __stdcall Initialize([[maybe_unused]] IWICBitmapDecoder *this, [[maybe_unused]] IStream *pIStream,
                                    [[maybe_unused]] WICDecodeOptions cacheOptions)
{
    return E_NOTIMPL;
}

static HRESULT __stdcall GetContainerFormat([[maybe_unused]] IWICBitmapDecoder *this, GUID *guidContainerFormat)
{
    TRACE("netpbm_bitmap_decoder-c::GetContainerFormat\n");

    if (!guidContainerFormat)
        return E_POINTER;

    memcpy(guidContainerFormat, &CLSID_ContainerFormatNetpbm, sizeof(GUID));
    return S_OK;
}

static HRESULT __stdcall GetDecoderInfo([[maybe_unused]] IWICBitmapDecoder *this,
                                        [[maybe_unused]] IWICBitmapDecoderInfo **ppIDecoderInfo)
{
    return E_NOTIMPL;
}

static HRESULT __stdcall CopyPalette([[maybe_unused]] IWICBitmapDecoder *this, [[maybe_unused]] IWICPalette *pIPalette)
{
    TRACE("netpbm_bitmap_decoder-c::CopyPalette\n");

    // NetPbm images don't have palettes.
    return WINCODEC_ERR_PALETTEUNAVAILABLE;
}

static HRESULT __stdcall GetMetadataQueryReader([[maybe_unused]] IWICBitmapDecoder *this,
                                                [[maybe_unused]] IWICMetadataQueryReader **ppIMetadataQueryReader)
{
    TRACE("netpbm_bitmap_decoder-c::GetMetadataQueryReader (not supported)\n");

    // Keep the initial design simple: no support for container-level metadata.
    // Note: Conceptual, comments from the NetPbm file could converted into metadata.
    return WINCODEC_ERR_UNSUPPORTEDOPERATION;
}

static HRESULT __stdcall GetPreview([[maybe_unused]] IWICBitmapDecoder *this,
                                    [[maybe_unused]] IWICBitmapSource **ppIBitmapSource)
{
    TRACE("netpbm_bitmap_decoder::GetPreview (not supported)\n");

    // The Netpbm format doesn't support storing previews in the file format.
    return WINCODEC_ERR_UNSUPPORTEDOPERATION;
}

static HRESULT __stdcall GetColorContexts([[maybe_unused]] IWICBitmapDecoder *this, [[maybe_unused]] UINT cCount,
                                          [[maybe_unused]] IWICColorContext **ppIColorContexts, UINT *pcActualCount)
{
    TRACE("netpbm_bitmap_decoder-c::GetColorContexts (always 0)\n");

    if (!pcActualCount)
        return E_POINTER;

    // The Netpbm format doesn't support storing color contexts (ICC profiles) in the file format.
    *pcActualCount = 0;
    return S_OK;
}

static HRESULT __stdcall GetThumbnail([[maybe_unused]] IWICBitmapDecoder *this,
                                      [[maybe_unused]] IWICBitmapSource **ppIThumbnail)
{
    TRACE("netpbm_bitmap_decoder-c::GetThumbnail (not supported)\n");

    // The Netpbm format doesn't support storing thumbnails in the file format.
    return WINCODEC_ERR_CODECNOTHUMBNAIL;
}

static HRESULT __stdcall GetFrameCount([[maybe_unused]] IWICBitmapDecoder *this, [[maybe_unused]] UINT *pCount)
{
    TRACE("netpbm_bitmap_decoder-c::GetFrameCount (always 1)");

    if (!pCount)
        return E_POINTER;

    // Only 1 frame is supported by this implementation (no real world samples are known that have more)
    *pCount = 1;
    return S_OK;
}

static HRESULT __stdcall GetFrame([[maybe_unused]] IWICBitmapDecoder *this, [[maybe_unused]] UINT index,
                                  [[maybe_unused]] IWICBitmapFrameDecode **ppIBitmapFrame)
{
    return E_NOTIMPL;
}

static HRESULT __stdcall IClassFactory_CreateInstance([[maybe_unused]] IClassFactory *this, IUnknown *punkOuter,
                                                      REFIID vTableGuid, void **ppv)
{
    if (punkOuter)
    {
        *ppv = NULL;
        return CLASS_E_NOAGGREGATION;
    }

    NetpbmBitmapDecoder *netpbmBitmapDecoder = malloc(sizeof(NetpbmBitmapDecoder));
    if (!netpbmBitmapDecoder)
    {
        *ppv = NULL;
        return E_OUTOFMEMORY;
    }

    // Initialize
    static const IWICBitmapDecoderVtbl wicBitmapDecoderVtbl = {QueryInterface,  AddRef,           Release,
                                                               QueryCapability, Initialize,       GetContainerFormat,
                                                               GetDecoderInfo,  CopyPalette,      GetMetadataQueryReader,
                                                               GetPreview,      GetColorContexts, GetThumbnail,
                                                               GetFrameCount,   GetFrame};

    netpbmBitmapDecoder->wicBitmapDecoder.lpVtbl = &wicBitmapDecoderVtbl;
    netpbmBitmapDecoder->refCount = 0;
    netpbmBitmapDecoder->initialized = false;

    const HRESULT hr = QueryInterface(&netpbmBitmapDecoder->wicBitmapDecoder, vTableGuid, ppv);
    if (SUCCEEDED(hr))
    {
        ModuleAddRef();
    }
    else
    {
        free(netpbmBitmapDecoder);
    }

    return hr;
}

_Use_decl_annotations_ HRESULT CreateWICBitmapDecoderClassFactory(REFCLSID interfaceId, void **result)
{
    static const IClassFactoryVtbl classFactoryVtbl = {IClassFactory_QueryInterface, IClassFactory_AddRef,
                                                       IClassFactory_Release, IClassFactory_CreateInstance,
                                                       IClassFactory_LockServer};
    static IClassFactory classFactory = {&classFactoryVtbl};

    return IClassFactory_QueryInterface(&classFactory, interfaceId, result);
}
