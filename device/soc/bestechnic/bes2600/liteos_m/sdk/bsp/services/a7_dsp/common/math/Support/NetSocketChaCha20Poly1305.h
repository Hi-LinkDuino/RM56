/*
	Copyright (C) 2015-2021 Apple Inc. All Rights Reserved.
*/

#ifndef NetSocketChaCha20Poly1305_h
#define NetSocketChaCha20Poly1305_h

#include "CommonServices.h"
#include "NetUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================

OSStatus
NetSocketChaCha20Poly1305Configure(
    NetSocketRef inSock,
    const uint8_t inReadKey[32],
    const uint8_t inReadNonce[8],
    const uint8_t inWriteKey[32],
    const uint8_t inWriteNonce[8]);

OSStatus NetSocketChaCha20Poly1305Test(void);

#ifdef __cplusplus
}
#endif

#endif // NetSocketChaCha20Poly1305_h
