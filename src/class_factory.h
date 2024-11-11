// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include <Unknwnbase.h>

HRESULT STDMETHODCALLTYPE IClassFactory_QueryInterface(IClassFactory *this, REFIID riid, void **ppv);
ULONG STDMETHODCALLTYPE IClassFactory_AddRef([[maybe_unused]] IClassFactory *this);
ULONG STDMETHODCALLTYPE IClassFactory_Release([[maybe_unused]] IClassFactory *this);
HRESULT STDMETHODCALLTYPE IClassFactory_LockServer([[maybe_unused]] IClassFactory *this, BOOL flock);
