// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include <Windows.h>

void ConstructComFactory(void);
void DestructComFactory(void);
void *GetClassObject(const CLSID *rclsid, const IID *riid);
HRESULT CallDllCanUnloadNow(void);
