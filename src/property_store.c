// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "property_store.h"

#include "class_factory.h"
#include "macros.h"
#include "module.h"


inline HRESULT InitPropVariantFromInt32(_In_ LONG lVal, _Out_ PROPVARIANT *ppropvar)
{
    V_VT(ppropvar) = VT_I4;
    V_UNION(ppropvar, lVal) = lVal;
    return S_OK;
}


typedef struct PropertyStore
{
    IInitializeWithStream initialize_with_stream;
    IPropertyStore property_store;
    IPropertyStoreCapabilities propertyStoreCapabilities;
    volatile bool initialized;
    LONG refCount;
    PROPVARIANT values[5]; // Sample data (could be dynamically allocated for flexibility)
    PROPERTYKEY keys[5];   // Property keys
} PropertyStore;


static ULONG STDMETHODCALLTYPE AddRef(_In_ PropertyStore *this)
{
    return InterlockedIncrement(&this->refCount);
}

static ULONG STDMETHODCALLTYPE Release(_In_ PropertyStore *this)
{
    const ULONG refCount = InterlockedDecrement(&this->refCount);
    if (refCount == 0)
    {
        for (int i = 0; i < 5; ++i)
        {
            VERIFY(SUCCEEDED(PropVariantClear(&this->values[i])));
        }
        free(this);
        ModuleRelease();
    }

    return refCount;
}

static HRESULT STDMETHODCALLTYPE QueryInterface(_In_ PropertyStore *this, _In_ REFIID riid, _COM_Outptr_ void **ppv)
{
    static const QITAB qiTable[] = {QITABENT(PropertyStore, IInitializeWithStream),
                                    QITABENT(PropertyStore, IPropertyStore),
                                    QITABENT(PropertyStore, IPropertyStoreCapabilities),
                                    {NULL, 0}};

    return QISearch(this, qiTable, riid, ppv);
}

static ULONG STDMETHODCALLTYPE IInitializeWithStream_AddRef(_In_ IInitializeWithStream *this)
{
    return AddRef((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)));
}

static ULONG STDMETHODCALLTYPE IInitializeWithStream_Release(_In_ IInitializeWithStream *this)
{
    return Release((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)));
}

static HRESULT STDMETHODCALLTYPE IInitializeWithStream_QueryInterface(_In_ IInitializeWithStream *this, _In_ REFIID riid,
                                                               _COM_Outptr_ void **ppv)
{
    return QueryInterface((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)), riid, ppv);
}

static HRESULT STDMETHODCALLTYPE IInitializeWithStream_Initialize([[maybe_unused]] _In_ IInitializeWithStream *this,
                                                           [[maybe_unused]] _In_ IStream *pstream,
                                                           [[maybe_unused]] _In_ DWORD grfMode)
{
    return S_OK;
}

static ULONG STDMETHODCALLTYPE IPropertyStore_AddRef(_In_ IPropertyStore *this)
{
    return AddRef((PropertyStore *)((char *)this - offsetof(PropertyStore, property_store)));
}

static ULONG STDMETHODCALLTYPE IPropertyStore_Release(_In_ IPropertyStore *this)
{
    return Release((PropertyStore *)((char *)this - offsetof(PropertyStore, property_store)));
}

static HRESULT STDMETHODCALLTYPE IPropertyStore_QueryInterface(_In_ IPropertyStore *this, _In_ REFIID riid,
                                                        _COM_Outptr_ void **ppvObject)
{
    return QueryInterface((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)), riid,
                          ppvObject);
}

static HRESULT STDMETHODCALLTYPE IPropertyStore_GetCount([[maybe_unused]] IPropertyStore *this, DWORD *cProps)
{
    if (!cProps)
        return E_POINTER;

    *cProps = 5; // We have 5 properties in this example
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE IPropertyStore_GetAt(IPropertyStore *this, DWORD iProp, PROPERTYKEY *pkey)
{
    PropertyStore *ps = (PropertyStore *)this;
    if (iProp >= 5)
        return E_INVALIDARG; // Invalid index
    *pkey = ps->keys[iProp];
    return S_OK;
}


#define IsEqualPropertyKey2(a, b) (((a)->pid == (b)->pid) && IsEqualIID(&((a)->fmtid), &((b)->fmtid)))


static HRESULT STDMETHODCALLTYPE IPropertyStore_GetValue(IPropertyStore *this, REFPROPERTYKEY key, PROPVARIANT *pv)
{
    PropertyStore *ps = (PropertyStore *)this;
    for (int i = 0; i < 5; ++i)
    {
        if (IsEqualPropertyKey2(key, &ps->keys[i]))
        {
            // return PropVariantCopy(pv, &ps->values[i]);
        }
    }

    PropVariantInit(pv);
    return E_INVALIDARG; // Key not found
}

static HRESULT STDMETHODCALLTYPE IPropertyStore_SetValue(IPropertyStore *this, REFPROPERTYKEY key, REFPROPVARIANT propvar)
{
    PropertyStore *ps = (PropertyStore *)this;
    for (int i = 0; i < 5; ++i)
    {
        if (IsEqualPropertyKey2(key, &ps->keys[i]))
        {
            return PropVariantCopy(&ps->values[i], propvar);
        }
    }
    return E_INVALIDARG; // Key not found
}

static HRESULT STDMETHODCALLTYPE IPropertyStore_Commit([[maybe_unused]] IPropertyStore *this)
{
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE IPropertyStoreCapabilities_QueryInterface(IPropertyStoreCapabilities *this, REFIID riid,
                                                                    void **ppv)
{
    return QueryInterface((PropertyStore *)((char *)this - offsetof(PropertyStore, propertyStoreCapabilities)), riid, ppv);
}

static ULONG STDMETHODCALLTYPE IPropertyStoreCapabilities_AddRef(IPropertyStoreCapabilities *this)
{
    return AddRef((PropertyStore *)((char *)this - offsetof(PropertyStore, propertyStoreCapabilities)));
}

static ULONG STDMETHODCALLTYPE IPropertyStoreCapabilities_Release(IPropertyStoreCapabilities *this)
{
    return Release((PropertyStore *)((char *)this - offsetof(PropertyStore, propertyStoreCapabilities)));
}

static HRESULT STDMETHODCALLTYPE IPropertyStoreCapabilities_IsPropertyWritable([[maybe_unused]] IPropertyStoreCapabilities *this,
                                                                        [[maybe_unused]] REFPROPERTYKEY key)
{
    return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE IClassFactory_CreateInstance([[maybe_unused]] IClassFactory *this, IUnknown *punkOuter,
                                                              REFIID vTableGuid, void **ppv)
{
    if (punkOuter)
    {
        *ppv = NULL;
        return CLASS_E_NOAGGREGATION;
    }

    PropertyStore *ps = malloc(sizeof(PropertyStore));
    if (!ps)
    {
        *ppv = NULL;
        return E_OUTOFMEMORY;
    }

    // Initialize
    static const IInitializeWithStreamVtbl initializeWithStreamVtbl = {
        IInitializeWithStream_QueryInterface, IInitializeWithStream_AddRef, IInitializeWithStream_Release,
        IInitializeWithStream_Initialize};

    static const IPropertyStoreVtbl propertyStoreVtbl = {
        IPropertyStore_QueryInterface, IPropertyStore_AddRef,   IPropertyStore_Release,  IPropertyStore_GetCount,
        IPropertyStore_GetAt,          IPropertyStore_GetValue, IPropertyStore_SetValue, IPropertyStore_Commit};

    static const IPropertyStoreCapabilitiesVtbl propertyStoreCapabilitiesVtbl = {
        IPropertyStoreCapabilities_QueryInterface, IPropertyStoreCapabilities_AddRef, IPropertyStoreCapabilities_Release,
        IPropertyStoreCapabilities_IsPropertyWritable};

    ps->initialize_with_stream.lpVtbl = &initializeWithStreamVtbl;
    ps->property_store.lpVtbl = &propertyStoreVtbl;
    ps->propertyStoreCapabilities.lpVtbl = &propertyStoreCapabilitiesVtbl;
    ps->refCount = 0;
    ps->initialized = false;
    memset(ps->values, 0, sizeof(ps->values));

    const HRESULT hr = QueryInterface(ps, vTableGuid, ppv);
    if (SUCCEEDED(hr))
    {
        ModuleAddRef();
    }
    else
    {
        free(ps);
    }

    return hr;
}

_Use_decl_annotations_ HRESULT CreatePropertyStoreClassFactory(REFCLSID interfaceId, void **result)
{
    static const IClassFactoryVtbl classFactoryVtbl = {IClassFactory_QueryInterface, IClassFactory_AddRef,
                                                       IClassFactory_Release, IClassFactory_CreateInstance,
                                                       IClassFactory_LockServer};

    static IClassFactory classFactory = {&classFactoryVtbl};

    return IClassFactory_QueryInterface(&classFactory, interfaceId, result);
}
