/*
	Copyright (C) 2001-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/
/*!
	@header		Cryptographic random number API.
	@discussion	Provides APIs for getting cryptographic random numbers.
*/

#ifndef __RandomNumberUtils_h__
#define __RandomNumberUtils_h__

#if (defined(RandomNumberUtils_PLATFORM_HEADER))
#include RandomNumberUtils_PLATFORM_HEADER
#endif

#include "CommonServices.h"
#include "DebugServices.h"

#if (TARGET_HAS_STD_C_LIB)
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RandomBytes
	@abstract	Fills a buffer with cryptographically strong pseudo-random bytes.
	
	@param		inBuffer		Buffer to receive random bytes. Must be at least inByteCount bytes.
	@param		inByteCount		Number of bytes to be filled into "inBuffer".
*/
OSStatus RandomBytes(void* inBuffer, size_t inByteCount);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RandomString
	@abstract	Generates a null-terminated string of a random length between min/maxChars with random characters.
	
	@param		inCharSet		String containing the characters to use to fill the random string.
	@param		inCharSetSize	Number of characters in "inCharSet". 
	@param		inMinChars		Min number of characters to generate. Does not include the null terminator.
	@param		inMaxChars		Max number of characters to fill in. Does not include the null terminator.
	@param		outString		Buffer to receive the string. Must be at least inMaxChars + 1 bytes.
	
	@result		Ptr to beginning of textual string (same as input buffer).
*/
char* RandomString(const char* inCharSet, size_t inCharSetSize, size_t inMinChars, size_t inMaxChars, char* outString);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RandomNumberUtilsTest
	@abstract	Unit test.
*/
OSStatus RandomNumberUtilsTest(void);

#ifdef __cplusplus
}
#endif

#endif // __RandomNumberUtils_h__
