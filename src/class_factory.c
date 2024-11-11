// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "class_factory.h"

#include "module.h"

HRESULT STDMETHODCALLTYPE IClassFactory_QueryInterface(IClassFactory *this, REFIID riid, void **ppv)
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
