/*
	Copyright (C) 2009 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef	__curve25519_donna_h__
#define	__curve25519_donna_h__

#ifdef __cplusplus
extern "C" {
#endif

void curve25519_donna( unsigned char *outKey, const unsigned char *inSecret, const unsigned char *inBasePoint );

#ifdef __cplusplus
}
#endif

#endif // __curve25519_donna_h__
