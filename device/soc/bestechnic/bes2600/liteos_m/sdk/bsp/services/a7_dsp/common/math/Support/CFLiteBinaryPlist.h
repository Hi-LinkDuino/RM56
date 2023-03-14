/*
	Copyright (C) 2006-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __CFLiteBinaryPlist_h__
#define __CFLiteBinaryPlist_h__

#include "CommonServices.h"
#include "DebugServices.h"

#if (TARGET_HAS_STD_C_LIB)
#include <stddef.h>
#endif

#include CF_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	CFBinaryPlistV0Create
 @abstract	Converts an object to a version 0 binary plist (i.e. compatible with Mac/iOS binary plists).
 */
const void* CFBinaryPlistV0Create(CFTypeRef inObj, size_t* outSize, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	CFBinaryPlistV0CreateData
 @abstract	Converts an object to a version 0 binary plist (i.e. compatible with Mac/iOS binary plists).
 */
CF_RETURNS_RETAINED
CFDataRef CFBinaryPlistV0CreateData(CFTypeRef inObj, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	CFBinaryPlistV0CreateWithData
	@abstract	Converts binary plist data to an object.
*/
CF_RETURNS_RETAINED
CFPropertyListRef CFBinaryPlistV0CreateWithData(const void* inPtr, size_t inLen, OSStatus* outErr);

#if 0
#pragma mark == Debugging ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	CFLiteBinaryPlistTest
	@abstract	Unit test.
*/
OSStatus CFLiteBinaryPlistTest(void);

#ifdef __cplusplus
}
#endif

#endif // __CFLiteBinaryPlist_h__
