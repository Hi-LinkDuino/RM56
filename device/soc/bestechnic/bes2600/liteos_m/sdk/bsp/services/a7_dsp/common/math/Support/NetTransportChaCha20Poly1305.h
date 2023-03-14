/*
	Copyright (C) 2014 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __NetTransportChaCha20Poly1305_h__
#define __NetTransportChaCha20Poly1305_h__

#include "LogUtils.h"
#include "NetUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NetTransportChaCha20Poly1305Configure
	@abstract	Sets up an NetTransportDelegate for ChaCha20-Poly1305 use.
	@discussion
	
	This acquires resources and needs to run through the normal transport finalize process. So it's expected that if this
	function returns noErr, it will be passed to HTTPClientSetTransportDelegate or HTTPConnectionSetTransportDelegate.
	If that ends up not being possible, you can finalize it manually by calling the delegate's finalize_f function.
*/
OSStatus
NetTransportChaCha20Poly1305Configure(
    NetTransportDelegate* ioDelegate,
    LogCategory* inCategory,
    const uint8_t inReadKey[32],
    const uint8_t inReadNonce[8],
    const uint8_t inWriteKey[32],
    const uint8_t inWriteNonce[8]);

#ifdef __cplusplus
}
#endif

#endif // __NetTransportChaCha20Poly1305_h__
