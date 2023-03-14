/*
	Copyright (C) 2009-2021 Apple Inc. All Rights Reserved.
*/
/*!
	@header			MFi-SAP
	@discussion		APIs and platform interfaces for the Made for iPod (MFi) Security Association Protocol (SAP).
*/

#ifndef __MFiSAP_h__
#define __MFiSAP_h__

#if (defined(MFiSAP_PLATFORM_HEADER))
#include MFiSAP_PLATFORM_HEADER
#endif

#include "CommonServices.h"
#include "DebugServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
//	Configuration
//===========================================================================================================================

// MFI_SAP_SERVER_AUTHV3: 1=Use AuthV3 server code. 0=Use pre-AuthV3 server code.

#if (!defined(MFI_SAP_SERVER_AUTHV3))
#define MFI_SAP_SERVER_AUTHV3 0
#endif

// MFI_SAP_ENABLE_CLIENT: 1=Enable code for the client side of MFi-SAP. 0=Strip out client code.

#if (!defined(MFI_SAP_ENABLE_CLIENT))
#if (TARGET_OS_DARWIN || TARGET_OS_WINDOWS)
#define MFI_SAP_ENABLE_CLIENT 1
#else
#define MFI_SAP_ENABLE_CLIENT 0
#endif
#endif

// MFI_SAP_ENABLE_SERVER: 1=Enable code for the server side of MFi-SAP. 0=Strip out server code.

#if (!defined(MFI_SAP_ENABLE_SERVER))
#if (TARGET_OS_DARWIN || TARGET_OS_LINUX || TARGET_OS_QNX || TARGET_PLATFORM_WICED)
#define MFI_SAP_ENABLE_SERVER 1
#else
#define MFI_SAP_ENABLE_SERVER 0
#endif
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Flags for controlling MFi auth behavior.
*/
typedef uint32_t MFiAuthFlags;
#define kMFiAuthFlagsNone 0 // No flags.
#define kMFiAuthFlagsAlreadyHashed (1U << 0) // Data is already hashed so don't hash it again.

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		Creation and deletion.
	@abstract	APIs for performing the Made for iPod (MFi) Security Association Protocol (SAP).
*/
#define kMFiSAPVersion1 1 // Curve25519 ECDH key exchange, RSA or ECDSA signing, AES-128 CTR encryption.

typedef struct MFiSAP* MFiSAPRef;

OSStatus MFiSAP_Create(MFiSAPRef* outRef, uint8_t inVersion);
void MFiSAP_Delete(MFiSAPRef inRef);
#define MFiSAP_Forget(X)         \
    do {                         \
        if (*(X)) {              \
            MFiSAP_Delete(*(X)); \
            *(X) = NULL;         \
        }                        \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiSAP_CopyCertificate
	@abstract	Return's a malloc'd copy of the certificate.
	@discussion	Only valid to call after MFiSAP_Exchange has completed successfully for the client.
*/
OSStatus MFiSAP_CopyCertificate(MFiSAPRef inRef, uint8_t** outCertPtr, size_t* outCertLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiSAP_CopyCertificateSerialNumber
	@abstract	Return's a malloc'd copy of the serial number data.
	@discussion	Only valid to call after MFiSAP_Exchange has completed successfully for the client.
*/
OSStatus MFiSAP_CopyCertificateSerialNumber(MFiSAPRef inRef, uint8_t** outSNPtr, size_t* outSNLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiSAP_Exchange
	@abstract	Perform key exchange.
	@discussion
	
	This is called on both the client and server sides. Each side repeatedly cals MFiSAP_Exchange until it returns an 
	error to indicate a failure or it returns kNoErr and sets "done" to true.
*/
OSStatus
MFiSAP_Exchange(
    MFiSAPRef inRef,
    const uint8_t* inInputPtr,
    size_t inInputLen,
    uint8_t** outOutputPtr,
    size_t* outOutputLen,
    Boolean* outDone);
OSStatus MFiSAP_Encrypt(MFiSAPRef inRef, const void* inPlaintext, size_t inLen, void* inCiphertextBuf);
OSStatus MFiSAP_Decrypt(MFiSAPRef inRef, const void* inCiphertext, size_t inLen, void* inPlaintextBuf);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiPlatform_Initialize
	@abstract	Performs any platform-specific initialization needed.
	@discussion	MFi accessory implementors must provide this function.
*/
OSStatus MFiPlatform_Initialize(void);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiPlatform_Finalize
	@abstract	Performs any platform-specific cleanup needed.
	@discussion	MFi accessory implementors must provide this function.
*/
void MFiPlatform_Finalize(void);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiPlatform_CreateSignature
	@abstract	Create an MFi auth signature from the specified data using the MFi authentication coprocessor.
	
	@param		inFlags				Flags controlling the signing process.
	@param		inDataPtr			Data to hash and sign. If already hashed, used kMFiAuthFlagsAlreadyHashed.
	@param		inDataLen			Number of bytes in inDataPtr.
	@param		outSignaturePtr		Receives malloc()'d ptr to RSA signature. Caller must free() on success.
	@param		outSignatureLen		Receives number of bytes in RSA signature.
	
	@discussion	MFi accessory implementors must provide this function.
*/
OSStatus
MFiPlatform_CreateSignatureEx(
    MFiAuthFlags inFlags,
    const void* inDataPtr,
    size_t inDataLen,
    uint8_t** outSignaturePtr,
    size_t* outSignatureLen);

// Deprecated. Use MFiPlatform_CreateSignatureEx instead.
OSStatus
MFiPlatform_CreateSignature(
    const void* inDigestPtr,
    size_t inDigestLen,
    uint8_t** outSignaturePtr,
    size_t* outSignatureLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiPlatform_VerifySignature
	@abstract	Verifies the MFi auth IC certificate and that the RSA signature is valid for the specified data.
	
	@param		inDataPtr			Data that was signed.
	@param		inDataLen			Number of bytes of data that was signed.
	@param		inSignaturePtr		RSA signature of a SHA-1 hash of the data.
	@param		inSignatureLen		Number of bytes in the signature.
	@param		inCertificatePtr	DER-encoded PKCS#7 message containing the certificate for the signing entity.
	@param		inCertificateLen	Number of bytes in inCertificatePtr.
	
	@discussion	This is only needed on the client side so accessory implementors do not need to provide this function.
*/
#define kMFiSignatureLengthPreAuthV3 128 // Pre-AuthV3 RSA-1024 signature.
OSStatus
MFiPlatform_VerifySignature(
    const void* inDataPtr,
    size_t inDataLen,
    const void* inSignaturePtr,
    size_t inSignatureLen,
    const void* inCertificatePtr,
    size_t inCertificateLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiPlatform_CopyCertificate
	@abstract	Copy the certificate from the MFi authentication coprocessor. 
	
	@param		outCertificatePtr	Receives malloc()'d ptr to a DER-encoded PKCS#7 message containing the certificate.
									Caller must free() on success.
	@param		outCertificateLen	Number of bytes in the DER-encoded certificate.
	
	@discussion	MFi accessory implementors must provide this function.
*/
OSStatus MFiPlatform_CopyCertificate(uint8_t** outCertificatePtr, size_t* outCertificateLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiPlatform_VerifyCertificate
	@abstract	Verifies that a certificate is a valid MFi auth IC certificate.
	
	@param		inCertificatePtr	DER-encoded PKCS#7 message containing the certificate for the signing entity.
	@param		inCertificateLen	Number of bytes in inCertificatePtr.
	
	@discussion	This is only needed on the client side so accessory implementors do not need to provide this function.
*/
OSStatus MFiPlatform_VerifyCertificate(const void* inCertificatePtr, size_t inCertificateLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MFiSAP_Test
	@abstract	Unit test for the MFi-SAP library.
*/
OSStatus MFiSAP_Test(void);

void MFiClientTest(void);

#ifdef __cplusplus
}
#endif

#endif // __MFiSAP_h__
