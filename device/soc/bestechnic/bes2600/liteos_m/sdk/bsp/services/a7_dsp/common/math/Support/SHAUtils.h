/*
	Copyright (C) 2012-2021 Apple Inc. All Rights Reserved.
*/

#ifndef __SHAUtils_h__
#define __SHAUtils_h__

#include "CommonServices.h"

#if (TARGET_HAS_MOCANA_SSL)
#include "mtypes.h"
#include "merrors.h"
#include "hw_accel.h"
#include "sha1.h"
#elif (!TARGET_HAS_COMMON_CRYPTO && !TARGET_HAS_SHA_UTILS && !TARGET_NO_OPENSSL)
#include <openssl/sha.h>
#endif
#if (TARGET_PLATFORM_WICED)
#include "wiced_security.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
//	SHA-1
//===========================================================================================================================

typedef struct
{
    uint64_t length;
    uint32_t state[5];
    size_t curlen;
    uint8_t buf[64];

} SHA_CTX_compat;

int SHA1_Init_compat(SHA_CTX_compat* ctx);
int SHA1_Update_compat(SHA_CTX_compat* ctx, const void* inData, size_t inLen);
int SHA1_Final_compat(unsigned char* outDigest, SHA_CTX_compat* ctx);
unsigned char* SHA1_compat(const void* inData, size_t inLen, unsigned char* outDigest);

OSStatus SHA1_Test(void);

//===========================================================================================================================
//	SHA-256
//===========================================================================================================================

typedef struct
{
    uint64_t length;
    uint32_t state[8];
    size_t curlen;
    uint8_t buf[64];

} SHA256_CTX_compat;

int SHA256_Init_compat(SHA256_CTX_compat* ctx);
int SHA256_Update_compat(SHA256_CTX_compat* ctx, const void* inData, size_t inLen);
int SHA256_Final_compat(unsigned char* outDigest, SHA256_CTX_compat* ctx);
unsigned char* SHA256_compat(const void* inData, size_t inLen, unsigned char* outDigest);

OSStatus SHA256_Test(void);

//===========================================================================================================================
//	SHA-512
//===========================================================================================================================

typedef struct
{
    uint64_t length;
    uint64_t state[8];
    size_t curlen;
    uint8_t buf[128];

} SHA512_CTX_compat;

int SHA512_Init_compat(SHA512_CTX_compat* ctx);
int SHA512_Update_compat(SHA512_CTX_compat* ctx, const void* inData, size_t inLen);
int SHA512_Final_compat(unsigned char* outDigest, SHA512_CTX_compat* ctx);
unsigned char* SHA512_compat(const void* inData, size_t inLen, unsigned char* outDigest);

OSStatus SHA512_Test(void);

//===========================================================================================================================
//	SHA-3 (Keccak)
//===========================================================================================================================

#define SHA3_BLOCK_SIZE 72
#define SHA3_DIGEST_LENGTH 64

typedef struct
{
    uint64_t state[25];
    size_t buffered;
    uint8_t buffer[SHA3_BLOCK_SIZE];

} SHA3_CTX_compat;

int SHA3_Init_compat(SHA3_CTX_compat* ctx);
int SHA3_Update_compat(SHA3_CTX_compat* ctx, const void* inData, size_t inLen);
int SHA3_Final_compat(unsigned char* outDigest, SHA3_CTX_compat* ctx);
uint8_t* SHA3_compat(const void* inData, size_t inLen, uint8_t outDigest[64]);

OSStatus SHA3_Test(void);
OSStatus SHA3_TestFile(const char* inPath);

//===========================================================================================================================
//	HMAC_SHA1
//===========================================================================================================================

typedef struct
{
    SHA_CTX shaCtx;
    uint8_t opad[64];

} HMAC_SHA1_CTX;

void HMAC_SHA1_Init(HMAC_SHA1_CTX* ctx, const void* inKeyPtr, size_t inKeyLen);
void HMAC_SHA1_Update(HMAC_SHA1_CTX* ctx, const void* inPtr, size_t inLen);
void HMAC_SHA1_Final(HMAC_SHA1_CTX* ctx, uint8_t* outDigest);
void HMAC_SHA1(const void* inKeyPtr, size_t inKeyLen, const void* inMsgPtr, size_t inMsgLen, uint8_t* outDigest);

OSStatus HMAC_SHA1_Test(void);

//===========================================================================================================================
//	HMAC_SHA512
//===========================================================================================================================

typedef struct
{
    SHA512_CTX shaCtx;
    uint8_t opad[128];

} HMAC_SHA512_CTX;

void HMAC_SHA512_Init(HMAC_SHA512_CTX* ctx, const void* inKeyPtr, size_t inKeyLen);
void HMAC_SHA512_Update(HMAC_SHA512_CTX* ctx, const void* inPtr, size_t inLen);
void HMAC_SHA512_Final(HMAC_SHA512_CTX* ctx, uint8_t* outDigest);
void HMAC_SHA512(const void* inKeyPtr, size_t inKeyLen, const void* inMsgPtr, size_t inMsgLen, uint8_t* outDigest);

OSStatus HMAC_SHA512_Test(void);

//===========================================================================================================================
//	HKDF_SHA512_compat
//===========================================================================================================================

void HKDF_SHA512_compat(
    const void* inInputKeyPtr, size_t inInputKeyLen,
    const void* inSaltPtr, size_t inSaltLen,
    const void* inInfoPtr, size_t inInfoLen,
    size_t inOutputKeyLen, uint8_t* outKey);

OSStatus HKDF_SHA512_Test(void);

//===========================================================================================================================
//	PBKDF2_HMAC_SHA1
//===========================================================================================================================

void PBKDF2_HMAC_SHA1(
    const void* inPasswordPtr,
    size_t inPasswordLen,
    const void* inSaltPtr,
    size_t inSaltLen,
    int inIterations,
    size_t inKeyLen,
    uint8_t* outKey);

OSStatus PBKDF2_HMAC_SHA1_Test(void);

#ifdef __cplusplus
}
#endif

#endif // __SHAUtils_h__
