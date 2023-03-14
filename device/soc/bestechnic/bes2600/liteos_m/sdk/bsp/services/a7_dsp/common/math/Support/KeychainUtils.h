/*
	Copyright (C) 2011-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __KeychainUtils_h__
#define __KeychainUtils_h__

#include "CommonServices.h"

#include CF_HEADER
#include KEYCHAIN_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	KeychainAddFormatted
	@abstract	Adds an item to the Keychain.
*/
OSStatus KeychainAddFormatted(CFTypeRef* outAddedItems, const char* inAttributesFormat, ...);
OSStatus KeychainAddFormattedVAList(CFTypeRef* outResult, const char* inAttributesFormat, va_list inArgs);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	KeychainCopyMatching
	@abstract	Searches for and returns keys and optionally the password.
	@discussion
	
	Note: errSecItemNotFound if the item is not found.
	Note: errSecAuthFailed is returned if the wrong password is entered or the user clicks "Deny".
*/
CF_RETURNS_RETAINED
CFTypeRef KeychainCopyMatchingFormatted(OSStatus* outErr, const char* inFormat, ...);

CF_RETURNS_RETAINED
CFTypeRef KeychainCopyMatchingFormattedVAList(OSStatus* outErr, const char* inFormat, va_list inArgs);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	KeychainDeleteFormatted
	@abstract	Searches for and deletes all the Keychain items matching the query.
*/
OSStatus KeychainDeleteFormatted(const char* inFormat, ...);
OSStatus KeychainDeleteFormattedVAList(const char* inFormat, va_list inArgs);
#if (!KEYCHAIN_LITE_ENABLED)
OSStatus KeychainDeleteItemByPersistentRef(CFDataRef inRef, CFDictionaryRef inAttrs);
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	KeychainUpdate
	@abstract	Updates an existing Keychain item.
*/
OSStatus KeychainUpdateFormatted(CFTypeRef inRefOrQuery, const char* inAttributesToUpdateFormat, ...);
OSStatus KeychainUpdateFormattedVAList(CFTypeRef inRefOrQuery, const char* inAttributesToUpdateFormat, va_list inArgs);

#if (!EXCLUDE_UNIT_TESTS)
//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	KeychainUtils_Test
	@abstract	Unit test.
*/
OSStatus KeychainUtils_Test(int inPrint);
#endif

#ifdef __cplusplus
}
#endif

#endif // __KeychainUtils_h__
