// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include <Windows.h>

HRESULT CreatePropertyStoreClassFactory(_In_ REFCLSID interfaceId, _Outptr_ void** result);
