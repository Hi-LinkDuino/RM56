/*
	Copyright (C) 2011-2015 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
	
	Ed25519: Edwards-curve Digital Signature Algorithm.
*/

#ifndef	__ed25519_h__
#define	__ed25519_h__

// #include "CommonServices.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ED25519_PUBLIC_KEY_BYTES		32
#define ED25519_SECRET_KEY_BYTES		32
#define ED25519_SIGNATURE_BYTES			64

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	ed25519_make_key_pair_ref
	@abstract	Makes a random key pair.
	
	@param		outPK	Receives a 32-byte public key.
	@param		outSK	Receives a 32-byte secret key.
*/
void	ed25519_make_key_pair_ref( uint8_t outPK[ 32 ], uint8_t outSK[ 32 ] );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	ed25519_sign_ref
	@abstract	Signs a message using a secret key.
	
	@param		outSig		Receives the 64-byte signature.
	@param		inMsg		Data to sign.
	@param		inLen		Number of bytes to sign.
	@param		inPK		32-byte public key as generated by ed25519_make_key_pair_ref().
	@param		inSK		32-byte secret key as generated by ed25519_make_key_pair_ref().
*/
void	ed25519_sign_ref( uint8_t outSig[ 64 ], const void *inMsg, size_t inLen, const uint8_t inPK[ 32 ], const uint8_t inSK[ 32 ] );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	ed25519_verify_ref
	@abstract	Verifies a signed message using a public key.
	
	@param		inMsg		Data to verify.
	@param		inLen		Number of bytes of data to verify.
	@param		inSig		64-byte signature to verify data against.
	@param		inPK		32-byte public key. Should have been generated by the peer using ed25519_make_key_pair_ref().
	
	@result		0=Signed message is valid. Non-zero=Bad message.
*/
int	ed25519_verify_ref( const void *inMsg, size_t inLen, const uint8_t inSig[ 64 ], const uint8_t inPK[ 32 ] );


#ifdef __cplusplus
}
#endif

#endif // __ed25519_h__
