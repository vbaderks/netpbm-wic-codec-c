// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "module.h"

static LONG g_lockCount = 0;

LONG ModuleAddRef(void)
{
    return InterlockedIncrement(&g_lockCount);
}

LONG ModuleRelease(void)
{
    return InterlockedDecrement(&g_lockCount);
}

bool ModuleIsLocked(void)
{
    return g_lockCount > 0;
}
