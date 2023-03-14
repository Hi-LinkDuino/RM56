/*
	Copyright (C) 2007-2021 Apple Inc. All Rights Reserved.
*/

#ifndef __MD5_UTILS_H__
#define __MD5_UTILS_H__

#include "CommonServices.h"

#ifdef __cplusplus
extern "C" {
#endif

// OpenSSL-compatible mappings.
// #ifndef MD5_CTX
// #define MD5_CTX MD5Context
// #endif
// #ifndef MD5_Init
// #define MD5_Init MD5Init
// #endif
// #ifndef MD5_Update
// #define MD5_Update MD5Update
// #endif
// #ifndef MD5_Final
// #define MD5_Final MD5Final
// #endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		MD5Context
	@abstract	Structure used for context between MD5 calls.
*/

typedef struct
{
    uint32_t buf[4];
    uint32_t bytes[2];
    uint32_t in[16];

} MD5Context;
#ifndef MD5_CTX
#define MD5_CTX MD5Context
#endif

int MD5_Init(MD5_CTX *c);
int MD5_Update(MD5_CTX *c, const void *data, unsigned long len);
int MD5_Final(unsigned char *md, MD5_CTX *c);


//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MD5
	@abstract	Convenience routine to generate an MD5 from a single buffer of data.
*/

typedef void (*MD5Func)(const void* inSourcePtr, size_t inSourceSize, uint8_t outKey[16]);

void MD5OneShot(const void* inSourcePtr, size_t inSourceSize, uint8_t outKey[16]);
void MD5OneShot_V1(const void* inSourcePtr, size_t inSourceSize, uint8_t outKey[16]);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MD5Init
	@abstract	Initializes the MD5 message digest.
*/

void MD5Init(MD5Context* context);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MD5Update
	@abstract	Updates the MD5 message digest with the specified data.
*/

typedef void (*MD5UpdateFunc)(MD5Context* context, void const* inBuf, size_t len);

void MD5Update(MD5Context* context, void const* inBuf, size_t len);
void MD5Update_V1(MD5Context* context, void const* inBuf, size_t len);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MD5Final
	@abstract	Finalizes and generates the resulting message digest.
*/

typedef void (*MD5FinalFunc)(uint8_t digest[16], MD5Context* context);

void MD5Final(uint8_t digest[16], MD5Context* context);
void MD5Final_V1(uint8_t digest[16], MD5Context* context);

#if 0
#pragma mark == Debugging ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MD5UtilsTest
	@abstract	Unit test.
*/

#if (DEBUG)
OSStatus MD5UtilsTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __MD5_UTILS_H__
