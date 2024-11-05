// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include <stdio.h>




// {6F2DF117-AB14-4728-861E-80A36AC56BC4}
static const GUID property_store =
{0x6f2df117, 0xab14, 0x4728, { 0x86, 0x1e, 0x80, 0xa3, 0x6a, 0xc5, 0x6b, 0xc4 }};



inline  HRESULT InitPropVariantFromInt32(_In_ LONG lVal, _Out_ PROPVARIANT* ppropvar)
{
    V_VT(ppropvar) = VT_I4;
    V_UNION(ppropvar, lVal) = lVal;
    return S_OK;
}



// Define a custom structure for the PropertyStore
typedef struct PropertyStore {
    IInitializeWithStream initialize_with_stream;
    IPropertyStore property_store; // The interface pointer
    volatile bool initialized;
    LONG refCount;        // COM reference count
    PROPVARIANT values[5]; // Sample data (could be dynamically allocated for flexibility)
    PROPERTYKEY keys[5];   // Property keys
} PropertyStore;

// Forward declarations of the IUnknown methods

// Forward declarations of the IInitializeWithStream methods
HRESULT STDMETHODCALLTYPE IInitializeWithStream_QueryInterface(IInitializeWithStream* this, REFIID riid, void** ppvObject);
ULONG STDMETHODCALLTYPE IInitializeWithStream_AddRef(IInitializeWithStream* this);
ULONG STDMETHODCALLTYPE IInitializeWithStream_Release(IInitializeWithStream* this);
HRESULT STDMETHODCALLTYPE IInitializeWithStream_Initialize(IInitializeWithStream* this, _In_  IStream* pstream, _In_  DWORD grfMode);

// Forward declarations of the IPropertyStore methods
HRESULT STDMETHODCALLTYPE QueryInterface(IPropertyStore* this, REFIID riid, void** ppvObject);
ULONG STDMETHODCALLTYPE AddRef(IPropertyStore* this);
ULONG STDMETHODCALLTYPE Release(IPropertyStore* this);
HRESULT STDMETHODCALLTYPE GetCount(IPropertyStore *this, DWORD *cProps);
HRESULT STDMETHODCALLTYPE GetAt(IPropertyStore *this, DWORD iProp, PROPERTYKEY *pkey);
HRESULT STDMETHODCALLTYPE GetValue(IPropertyStore *this, REFPROPERTYKEY key, PROPVARIANT *pv);
HRESULT STDMETHODCALLTYPE SetValue(IPropertyStore *this, REFPROPERTYKEY key, REFPROPVARIANT propvar);
HRESULT STDMETHODCALLTYPE Commit(IPropertyStore *this);

// Define the vtable for the IInitializeWithStream
IInitializeWithStreamVtbl g_InitializeWithStream_Vtbl = {
    IInitializeWithStream_QueryInterface,
    IInitializeWithStream_AddRef,
    IInitializeWithStream_Release,
    IInitializeWithStream_Initialize
};

HRESULT STDMETHODCALLTYPE IInitializeWithStream_QueryInterface(IInitializeWithStream* this, REFIID riid, void** ppvObject)
{
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IInitializeWithStream)) {
        *ppvObject = this;
        IInitializeWithStream_AddRef(this);
        return S_OK;
    }
    else {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }

}

ULONG STDMETHODCALLTYPE IInitializeWithStream_AddRef(IInitializeWithStream* this) {
    PropertyStore* ps = (PropertyStore*)this;
    return InterlockedIncrement(&ps->refCount);
}

ULONG STDMETHODCALLTYPE IInitializeWithStream_Release(IInitializeWithStream* this) {
    PropertyStore* ps = (PropertyStore*)this;
    ULONG ref_count = InterlockedDecrement(&ps->refCount);
    if (ref_count == 0) {
        for (int i = 0; i < 5; ++i) {
            PropVariantClear(&ps->values[i]);
        }
        free(ps);
    }
    return ref_count;
}

HRESULT STDMETHODCALLTYPE IInitializeWithStream_Initialize(IInitializeWithStream* this, _In_  IStream* pstream, _In_  DWORD grfMode)
{
    UNREFERENCED_PARAMETER(this);
    UNREFERENCED_PARAMETER(pstream);
    UNREFERENCED_PARAMETER(grfMode);
    return S_OK;
}


// Define the vtable for the IPropertyStore
IPropertyStoreVtbl g_PropertyStore_Vtbl = {
    QueryInterface,
    AddRef,
    Release,
    GetCount,
    GetAt,
    GetValue,
    SetValue,
    Commit
};

// QueryInterface implementation
HRESULT STDMETHODCALLTYPE QueryInterface(IPropertyStore *this, REFIID riid, void **ppvObject) {
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IPropertyStore)) {
        *ppvObject = this;
        AddRef(this);

        return S_OK;
    } else {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

// AddRef implementation
ULONG STDMETHODCALLTYPE AddRef(IPropertyStore *this) {
    PropertyStore *ps = (PropertyStore *)this;
    return InterlockedIncrement(&ps->refCount);
}

// Release implementation
ULONG STDMETHODCALLTYPE Release(IPropertyStore *this) {
    PropertyStore *ps = (PropertyStore *)this;
    ULONG ref_count = InterlockedDecrement(&ps->refCount);
    if (ref_count == 0) {
        for (int i = 0; i < 5; ++i) {
            PropVariantClear(&ps->values[i]);
        }
        free(ps);
    }
    return ref_count;
}

// GetCount implementation
HRESULT STDMETHODCALLTYPE GetCount(IPropertyStore *this, DWORD *cProps) {
    UNREFERENCED_PARAMETER(this);

    if (!cProps) return E_POINTER;
    *cProps = 5; // We have 5 properties in this example
    return S_OK;
}

// GetAt implementation
HRESULT STDMETHODCALLTYPE GetAt(IPropertyStore *this, DWORD iProp, PROPERTYKEY *pkey) {
    PropertyStore *ps = (PropertyStore *)this;
    if (iProp >= 5) return E_INVALIDARG; // Invalid index
    *pkey = ps->keys[iProp];
    return S_OK;
}


#define IsEqualPropertyKey2(a, b)   (((a)->pid == (b)->pid) && IsEqualIID(&((a)->fmtid), &((b)->fmtid)))


// GetValue implementation
HRESULT STDMETHODCALLTYPE GetValue(IPropertyStore *this, REFPROPERTYKEY key, PROPVARIANT *pv) {
    PropertyStore *ps = (PropertyStore *)this;
    for (int i = 0; i < 5; ++i) {
        if (IsEqualPropertyKey2(key, &ps->keys[i])) {
            //return PropVariantCopy(pv, &ps->values[i]);
        }
    }

    PropVariantInit(pv);
    return E_INVALIDARG; // Key not found
}

// SetValue implementation
HRESULT STDMETHODCALLTYPE SetValue(IPropertyStore *this, REFPROPERTYKEY key, REFPROPVARIANT propvar) {
    PropertyStore *ps = (PropertyStore *)this;
    for (int i = 0; i < 5; ++i) {
        if (IsEqualPropertyKey2(key, &ps->keys[i])) {
            return PropVariantCopy(&ps->values[i], propvar);
        }
    }
    return E_INVALIDARG; // Key not found
}

// Commit implementation (does nothing here)
HRESULT STDMETHODCALLTYPE Commit(IPropertyStore *this) {
    UNREFERENCED_PARAMETER(this);
    return S_OK;
}

// Factory function to create the PropertyStore
HRESULT CreatePropertyStore(PropertyStore **ppStore) {
    PropertyStore *ps = (PropertyStore *)malloc(sizeof(PropertyStore));
    if (!ps) return E_OUTOFMEMORY;
    
    // Initialize vtable and reference count
    ps->property_store.lpVtbl = &g_PropertyStore_Vtbl;
    ps->refCount = 1;

    // Initialize the property keys and values (example data)
    for (int i = 0; i < 5; ++i) {
        InitPropVariantFromInt32(i, &ps->values[i]);
        ps->keys[i] = PKEY_Title; // Example, you can set specific PROPERTYKEY values here
    }

    *ppStore = ps;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE classQueryInterface(IClassFactory* this,
    REFIID factoryGuid, void** ppv)
{
    // Check if the GUID matches an IClassFactory or IUnknown GUID.

    if (!IsEqualIID(factoryGuid, &IID_IUnknown) &&
        !IsEqualIID(factoryGuid, &IID_IClassFactory))
    {
        // It doesn't. Clear his handle, and return E_NOINTERFACE.

        *ppv = 0;
        return(E_NOINTERFACE);
    }

    // It's a match!

    // First, we fill in his handle with the same object pointer he passed us.
    // That's our IClassFactory (MyIClassFactoryObj) he obtained from us.

    *ppv = this;

    // Call our IClassFactory's AddRef, passing the IClassFactory. 
    this->lpVtbl->AddRef(this);

    // Let him know he indeed has an IClassFactory. 

    return(NOERROR);
}

HRESULT STDMETHODCALLTYPE classCreateInstance(IClassFactory* this,
    IUnknown* punkOuter, REFIID vTableGuid, void** ppv)
{
    UNREFERENCED_PARAMETER(this);
    UNREFERENCED_PARAMETER(vTableGuid);

    HRESULT          hr = S_OK;
    //struct IExample* thisobj;

    // Assume an error by clearing caller's handle.

    *ppv = 0;

    // We don't support aggregation in IExample.

    if (punkOuter)
        hr = CLASS_E_NOAGGREGATION;
    else
    {
        // Create our IExample object, and initialize it.

        //if (!(thisobj = GlobalAlloc(GMEM_FIXED,
        //    sizeof(struct IExample))))
        //    hr = E_OUTOFMEMORY;
        //else
        //{
        //    // Store IExample's VTable. We declared it

        //    // as a static variable IExample_Vtbl.

        //    thisobj->lpVtbl = &IExample_Vtbl;

        //    // Increment reference count so we

        //    // can call Release() below and it will

        //    // deallocate only if there

        //    // is an error with QueryInterface().

        //    thisobj->count = 1;

        //    // Fill in the caller's handle

        //    // with a pointer to the IExample we just

        //    // allocated above. We'll let IExample's

        //    // QueryInterface do that, because

        //    // it also checks the GUID the caller

        //    // passed, and also increments the

        //    // reference count (to 2) if all goes well.

        //    hr = IExample_Vtbl.QueryInterface(thisobj, vTableGuid, ppv);

        //    // Decrement reference count.

        //    // NOTE: If there was an error in QueryInterface()

        //    // then Release() will be decrementing

        //    // the count back to 0 and will free the

        //    // IExample for us. One error that may

        //    // occur is that the caller is asking for

        //    // some sort of object that we don't

        //    // support (ie, it's a GUID we don't recognize).

        //    IExample_Vtbl.Release(thisobj);
        //}
    }

    return(hr);
}

ULONG STDMETHODCALLTYPE classAddRef(IClassFactory* this)
{
    UNREFERENCED_PARAMETER(this);
    return(1);
}

ULONG STDMETHODCALLTYPE classRelease(IClassFactory* this)
{
    UNREFERENCED_PARAMETER(this);
    return(1);
}

HRESULT STDMETHODCALLTYPE classLockServer(IClassFactory* this, BOOL flock)
{
    UNREFERENCED_PARAMETER(this);
    UNREFERENCED_PARAMETER(flock);
    return(NOERROR);
}


// TODO: define CONST_VTABLE

static /*const*/ IClassFactoryVtbl IClassFactory_Vtbl = {classQueryInterface,
classAddRef,
classRelease,
classCreateInstance,
classLockServer};

static IClassFactory MyIClassFactoryObj = {&IClassFactory_Vtbl};

HRESULT create_property_store_class_factory(_In_ REFCLSID interface_id, _Outptr_ void** result)
{
    UNREFERENCED_PARAMETER(interface_id);
    UNREFERENCED_PARAMETER(result);

    return classQueryInterface(&MyIClassFactoryObj, interface_id, result);
}
