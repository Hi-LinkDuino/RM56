/*
	Copyright (C) 2015-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef CryptoHashUtils_h
#define CryptoHashUtils_h

#include "BLAKEUtils.h"
#include "CommonServices.h"
#include "SHAUtils.h"

#include MD5_HEADER
#include SHA_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#pragma mark == Hash ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defgroup	CryptoHashDescriptor
	@brief		Descriptor for a particular hash function.
*/
typedef const struct CryptoHashDescriptorPrivate* CryptoHashDescriptorRef;

extern CryptoHashDescriptorRef kCryptoHashDescriptor_BLAKE2b;
extern CryptoHashDescriptorRef kCryptoHashDescriptor_MD5;
extern CryptoHashDescriptorRef kCryptoHashDescriptor_SHA1;
extern CryptoHashDescriptorRef kCryptoHashDescriptor_SHA512;
extern CryptoHashDescriptorRef kCryptoHashDescriptor_SHA3;

#define kCryptoHashDigestLength_BLAKE2b 64
#define kCryptoHashDigestLength_MD5 16
#define kCryptoHashDigestLength_SHA1 20
#define kCryptoHashDigestLength_SHA512 64
#define kCryptoHashDigestLength_SHA3 64

#define kCryptoHashMaxBlockLen 128
#define kCryptoHashMaxDigestLen 64

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashDescriptorGetBlockSize
	@brief		Returns the number of bytes in a block of the hash function (e.g. 72 bytes for SHA-3).
*/
size_t CryptoHashDescriptorGetBlockSize(CryptoHashDescriptorRef inDesc);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashDescriptorGetDigestSize
	@brief		Returns the number of bytes in the digest of the hash function (e.g. 64 bytes for SHA-3).
*/
size_t CryptoHashDescriptorGetDigestSize(CryptoHashDescriptorRef inDesc);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		CryptoHashContext
	@brief		Context/state for a hash instance.
*/
typedef struct
{
    CryptoHashDescriptorRef desc;
    union {
        BLAKE2bContext blake2b;
        MD5_CTX md5;
        SHA_CTX sha1;
        SHA512_CTX sha512;
        SHA3_CTX sha3;

    } state;

} CryptoHashContext;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashInit
	@brief		Initializes the context for the hash.
*/
void CryptoHashInit(CryptoHashContext* ctx, CryptoHashDescriptorRef inDesc);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashUpdate
	@brief		Updates the hash state with the specified data.
*/
void CryptoHashUpdate(CryptoHashContext* ctx, const void* inData, size_t inLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashFinal
	@brief		Finalizes the hash state and produces the digest.
*/
void CryptoHashFinal(CryptoHashContext* ctx, uint8_t* inDigestBuffer);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashOneShot
	@brief		Performs CryptoHashInit, CryptoHashUpdate, and CryptoHashFinal in one function call.
*/
void CryptoHashOneShot(CryptoHashDescriptorRef inDesc, const void* inData, size_t inLen, uint8_t* inDigestBuffer);

#if 0
#pragma mark -
#pragma mark == HMAC ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		CryptoHMACContext
	@brief		Context/state for an HMAC instance.
*/
typedef struct
{
    CryptoHashContext hashCtx;
    uint8_t opad[kCryptoHashMaxBlockLen];

} CryptoHMACContext;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHMACInit
	@brief		Initializes the context for HMAC.
*/
void CryptoHMACInit(CryptoHMACContext* ctx, CryptoHashDescriptorRef inDesc, const void* inKeyPtr, size_t inKeyLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHMACUpdate
	@brief		Updates the HMAC state with the specified data.
*/
void CryptoHMACUpdate(CryptoHMACContext* ctx, const void* inPtr, size_t inLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashFinal
	@brief		Finalizes the hash state and produces the digest.
*/
void CryptoHMACFinal(CryptoHMACContext* ctx, uint8_t* outDigest);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHMACOneShot
	@brief		Performs CryptoHMACInit, CryptoHMACUpdate, and CryptoHMACFinal in one function call.
*/
void CryptoHMACOneShot(
    CryptoHashDescriptorRef inDesc,
    const void* inKeyPtr,
    size_t inKeyLen,
    const void* inMsgPtr,
    size_t inMsgLen,
    uint8_t* outDigest);

#if 0
#pragma mark -
#pragma mark == HKDF ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHKDF
	@brief		Derives a key according to HKDF. See <https://tools.ietf.org/html/rfc5869>.
*/
void CryptoHKDF(
    CryptoHashDescriptorRef inDesc,
    const void* inInputKeyPtr, size_t inInputKeyLen,
    const void* inSaltPtr, size_t inSaltLen,
    const void* inInfoPtr, size_t inInfoLen,
    size_t inOutputLen, void* outKey);

#if 0
#pragma mark -
#pragma mark == Testing ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			CryptoHashUtilsTest
	@brief		Unit test.
*/
void CryptoHashUtilsTest(void);

#ifdef __cplusplus
}
#endif

#endif // CryptoHashUtils_h
