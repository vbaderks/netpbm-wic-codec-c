// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "pnm_header.h"

_Use_decl_annotations_ bool IsPnmFile(IStream *stream)
{
    // Read the first two bytes to determine if the file is a PNM file.
    BYTE buffer[2];
    ULONG bytesRead;
    const HRESULT result = stream->lpVtbl->Read(stream, buffer, sizeof(buffer), &bytesRead);
    if (FAILED(result))
        return false;

    if (bytesRead != sizeof(buffer))
        return false;

    return buffer[0] == 'P' && (buffer[1] == '1' || buffer[1] == '2' || buffer[1] == '3' || buffer[1] == '4' ||
                                buffer[1] == '5' || buffer[1] == '6');
}
