/*
	Copyright (C) 2014-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __KeychainLite_h__
#define __KeychainLite_h__

#include "CommonServices.h"

#include CF_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

OSStatus SecItemAdd_compat(CFDictionaryRef inAttrs, CFTypeRef* outResult);
OSStatus SecItemCopyMatching_compat(CFDictionaryRef inQuery, CFTypeRef* outResult);
OSStatus SecItemUpdate_compat(CFDictionaryRef inQuery, CFDictionaryRef inAttrs);
OSStatus SecItemDelete_compat(CFDictionaryRef inQuery);

//===========================================================================================================================
//	Mappings
//===========================================================================================================================

#if (KEYCHAIN_LITE_ENABLED)
#define kSecAttrAccessGroup CFSTR("kSecAttrAccessGroup")
#define kSecAttrAccessible CFSTR("kSecAttrAccessible")
#define kSecAttrAccessibleAlways CFSTR("kSecAttrAccessibleAlways")
#define kSecAttrAccessibleWhenUnlocked CFSTR("kSecAttrAccessibleWhenUnlocked")
#define kSecAttrAccount CFSTR("kSecAttrAccount")
#define kSecAttrDescription CFSTR("kSecAttrDescription")
#define kSecAttrGeneric CFSTR("kSecAttrGeneric")
#define kSecAttrLabel CFSTR("kSecAttrLabel")
#define kSecAttrService CFSTR("kSecAttrService")
#define kSecAttrSynchronizable CFSTR("kSecAttrSynchronizable")
#define kSecAttrSynchronizableAny CFSTR("kSecAttrSynchronizableAny")
#define kSecAttrType CFSTR("kSecAttrType")
#define kSecClass CFSTR("kSecClass")
#define kSecClassGenericPassword CFSTR("kSecClassGenericPassword")
#define kSecMatchLimit CFSTR("kSecMatchLimit")
#define kSecMatchLimitAll CFSTR("kSecMatchLimitAll")
#define kSecReturnAttributes CFSTR("kSecReturnAttributes")
#define kSecReturnData CFSTR("kSecReturnData")
#define kSecReturnPersistentRef CFSTR("kSecReturnPersistentRef")
#define kSecReturnRef CFSTR("kSecReturnRef")
#define kSecValueData CFSTR("kSecValueData")

#define errSecAuthFailed -25293
#define errSecDuplicateItem -25299
#define errSecItemNotFound -25300

#define SecItemAdd SecItemAdd_compat
#define SecItemCopyMatching SecItemCopyMatching_compat
#define SecItemUpdate SecItemUpdate_compat
#define SecItemDelete SecItemDelete_compat
#endif

#ifdef __cplusplus
}
#endif

#endif // __KeychainLite_h__
