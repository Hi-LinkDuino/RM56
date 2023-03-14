/*
	BLAKE2 Cryptographic Hash and Message Authentication Code (MAC)
	Based on RFC 7693.
	
	Copyright (C) 2015-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef BLAKEUtils_h
#define BLAKEUtils_h

#include "CommonServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
//	BLAKE2b
//===========================================================================================================================

#define kBLAKE2bBlockSize 128
#define kBLAKE2bDigestSize 64

typedef struct
{
    uint8_t b[128]; // Input buffer.
    uint64_t h[8]; // Chained state.
    uint64_t t[2]; // Total number of bytes.
    size_t c; // Offset into b.
    size_t digestLen; // Number of bytes in digest.

} BLAKE2bContext;

void BLAKE2bInit(BLAKE2bContext* tx, const void* inKeyPtr, size_t inKeyLen, size_t inDigestLen);
void BLAKE2bUpdate(BLAKE2bContext* tx, const void* inDataPtr, size_t inDataLen);
void BLAKE2bFinal(BLAKE2bContext* tx, void* inDigestBuffer);
void BLAKE2bOneShot(
    const void* inKeyPtr,
    size_t inKeyLen,
    const void* inDataPtr,
    size_t inDataLen,
    size_t inDigestLen,
    void* inDigestBuffer);

#ifdef __cplusplus
}
#endif

#endif // BLAKEUtils_h
