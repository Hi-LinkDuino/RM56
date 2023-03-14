/*
    Copyright (C) 2007-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/
#ifndef __SDPUtils_h__
#define __SDPUtils_h__

#include "CommonServices.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------------------------------------------------------------------------
/*!    @group        SDPParsing
    @abstract    Functions for parsing Session Description Protocol (SDP) records.
*/
OSStatus
SDPFindAttribute(
    const char* inSrc,
    const char* inEnd,
    const char* inAttribute,
    const char** outValuePtr,
    size_t* outValueLen,
    const char** outNextPtr);
OSStatus
SDPFindMediaSection(
    const char* inSrc,
    const char* inEnd,
    const char** outMediaSectionPtr,
    const char** outMediaSectionEnd,
    const char** outMediaValuePtr,
    size_t* outMediaValueLen,
    const char** outNextPtr);
OSStatus
SDPFindMediaSectionEx(
    const char* inSrc,
    const char* inEnd,
    const char** outMediaTypePtr,
    size_t* outMediaTypeLen,
    const char** outPortPtr,
    size_t* outPortLen,
    const char** outProtocolPtr,
    size_t* outProtocolLen,
    const char** outFormatPtr,
    size_t* outFormatLen,
    const char** outAttributesPtr,
    size_t* outAttributesLen,
    const char** outSrc);
OSStatus
SDPFindParameter(
    const char* inSrc,
    const char* inEnd,
    const char* inName,
    const char** outValuePtr,
    size_t* outValueLen,
    const char** outNextPtr);
OSStatus
SDPFindSessionSection(
    const char* inSrc,
    const char* inEnd,
    const char** outSectionPtr,
    const char** outSectionEnd,
    const char** outNextPtr);
OSStatus
SDPFindType(
    const char* inSrc,
    const char* inEnd,
    char inType,
    const char** outValuePtr,
    size_t* outValueLen,
    const char** outNextPtr);
OSStatus
SDPGetNext(
    const char* inSrc,
    const char* inEnd,
    char* outType,
    const char** outValuePtr,
    size_t* outValueLen,
    const char** outNextPtr);
OSStatus
SDPGetNextAttribute(
    const char* inSrc,
    const char* inEnd,
    const char** outNamePtr,
    size_t* outNameLen,
    const char** outValuePtr,
    size_t* outValueLen,
    const char** outNextPtr);
OSStatus
SDPGetNextParameter(
    const char* inSrc,
    const char* inEnd,
    const char** outNamePtr,
    size_t* outNameLen,
    const char** outValuePtr,
    size_t* outValueLen,
    const char** outNextPtr);
int SDPScanFAttribute(
    const char* inSrc,
    const char* inEnd,
    const char** outSrc,
    const char* inAttribute,
    const char* inFormat,
    ...);
int SDPScanFAttributeV(
    const char* inSrc,
    const char* inEnd,
    const char** outSrc,
    const char* inAttribute,
    const char* inFormat,
    va_list inArgs);

#if (DEBUG && !EXCLUDE_UNIT_TESTS)
OSStatus SDPUtils_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __SDPUtils_h__
