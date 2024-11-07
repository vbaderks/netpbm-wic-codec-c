// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

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
    volatile bool initialized;
    LONG refCount;
    PROPVARIANT values[5]; // Sample data (could be dynamically allocated for flexibility)
    PROPERTYKEY keys[5];   // Property keys
} PropertyStore;


ULONG STDMETHODCALLTYPE AddRef(PropertyStore *this)
{
    return InterlockedIncrement(&this->refCount);
}

ULONG STDMETHODCALLTYPE Release(PropertyStore *this)
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

HRESULT STDMETHODCALLTYPE QueryInterface(PropertyStore *this, REFIID riid, void **ppv)
{
    static const QITAB qit[] = {
        QITABENT(PropertyStore, IInitializeWithStream), QITABENT(PropertyStore, IPropertyStore), {NULL, 0}};

    return QISearch(this, qit, riid, ppv);
}

ULONG STDMETHODCALLTYPE IInitializeWithStream_AddRef(IInitializeWithStream *this)
{
    return AddRef((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)));
}

ULONG STDMETHODCALLTYPE IInitializeWithStream_Release(IInitializeWithStream *this)
{
    return Release((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)));
}

HRESULT STDMETHODCALLTYPE IInitializeWithStream_QueryInterface(IInitializeWithStream *this, REFIID riid, void **ppv)
{
    return QueryInterface((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)), riid, ppv);
}

HRESULT STDMETHODCALLTYPE IInitializeWithStream_Initialize([[maybe_unused]] IInitializeWithStream *this,
                                                           [[maybe_unused]] _In_ IStream *pstream,
                                                           [[maybe_unused]] _In_ DWORD grfMode)
{
    return S_OK;
}

ULONG STDMETHODCALLTYPE IPropertyStore_AddRef(IPropertyStore *this)
{
    return AddRef((PropertyStore *)((char *)this - offsetof(PropertyStore, property_store)));
}

ULONG STDMETHODCALLTYPE IPropertyStore_Release(IPropertyStore *this)
{
    return Release((PropertyStore *)((char *)this - offsetof(PropertyStore, property_store)));
}

HRESULT STDMETHODCALLTYPE IPropertyStore_QueryInterface(IPropertyStore *this, REFIID riid, void **ppvObject)
{
    return QueryInterface((PropertyStore *)((char *)this - offsetof(PropertyStore, initialize_with_stream)), riid,
                          ppvObject);
}

HRESULT STDMETHODCALLTYPE IPropertyStore_GetCount(IPropertyStore *this, DWORD *cProps)
{
    UNREFERENCED_PARAMETER(this);

    if (!cProps)
        return E_POINTER;
    *cProps = 5; // We have 5 properties in this example
    return S_OK;
}

HRESULT STDMETHODCALLTYPE IPropertyStore_GetAt(IPropertyStore *this, DWORD iProp, PROPERTYKEY *pkey)
{
    PropertyStore *ps = (PropertyStore *)this;
    if (iProp >= 5)
        return E_INVALIDARG; // Invalid index
    *pkey = ps->keys[iProp];
    return S_OK;
}


#define IsEqualPropertyKey2(a, b) (((a)->pid == (b)->pid) && IsEqualIID(&((a)->fmtid), &((b)->fmtid)))


HRESULT STDMETHODCALLTYPE IPropertyStore_GetValue(IPropertyStore *this, REFPROPERTYKEY key, PROPVARIANT *pv)
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

HRESULT STDMETHODCALLTYPE IPropertyStore_SetValue(IPropertyStore *this, REFPROPERTYKEY key, REFPROPVARIANT propvar)
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

HRESULT STDMETHODCALLTYPE IPropertyStore_Commit([[maybe_unused]] IPropertyStore *this)
{
    return S_OK;
}


static IInitializeWithStreamVtbl initializeWithStreamVtbl = {IInitializeWithStream_QueryInterface,
                                                             IInitializeWithStream_AddRef, IInitializeWithStream_Release,
                                                             IInitializeWithStream_Initialize};

static IPropertyStoreVtbl propertyStoreVtbl = {IPropertyStore_QueryInterface, IPropertyStore_AddRef, IPropertyStore_Release,
                                               IPropertyStore_GetCount,       IPropertyStore_GetAt,  IPropertyStore_GetValue,
                                               IPropertyStore_SetValue,       IPropertyStore_Commit};

HRESULT STDMETHODCALLTYPE classQueryInterface(IClassFactory *this, REFIID riid, void **ppv)
{
    if (!IsEqualIID(riid, &IID_IUnknown) && !IsEqualIID(riid, &IID_IClassFactory))
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    *ppv = this;
    this->lpVtbl->AddRef(this);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE IClassFactory_CreateInstance([[maybe_unused]] IClassFactory *this, IUnknown *punkOuter,
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
    ps->initialize_with_stream.lpVtbl = &initializeWithStreamVtbl;
    ps->property_store.lpVtbl = &propertyStoreVtbl;
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


ULONG STDMETHODCALLTYPE IClassFactory_AddRef([[maybe_unused]] IClassFactory *this)
{
    return ModuleAddRef();
}

ULONG STDMETHODCALLTYPE IClassFactory_Release([[maybe_unused]] IClassFactory *this)
{
    return ModuleRelease();
}

HRESULT STDMETHODCALLTYPE IClassFactory_LockServer([[maybe_unused]] IClassFactory *this, const BOOL flock)
{
    if (flock)
    {
        ModuleAddRef();
    }
    else
    {
        ModuleRelease();
    }

    return S_OK;
}


// TODO: define CONST_VTABLE

static /*const*/ IClassFactoryVtbl classFactoryVtbl = {classQueryInterface, IClassFactory_AddRef, IClassFactory_Release,
                                                       IClassFactory_CreateInstance, IClassFactory_LockServer};

static IClassFactory MyIClassFactoryObj = {&classFactoryVtbl};

HRESULT create_property_store_class_factory(_In_ REFCLSID interface_id, _Outptr_ void **result)
{
    UNREFERENCED_PARAMETER(interface_id);
    UNREFERENCED_PARAMETER(result);

    return classQueryInterface(&MyIClassFactoryObj, interface_id, result);
}
