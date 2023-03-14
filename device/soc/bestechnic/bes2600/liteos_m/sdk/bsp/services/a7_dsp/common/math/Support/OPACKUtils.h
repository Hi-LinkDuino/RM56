/*
	Copyright (C) 2012-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/
/*!	@file
	@brief		Encoder/Decoder for the OPACK object serialization format.
	@details
	
	This format supports the following object types:
	
	Array
	Boolean
	Dictionary
	Data
	Date
	Null
	Number (integer and floating point)
	String
	UUID
*/

#ifndef OPACK_h
#define OPACK_h

#include "CommonServices.h"

#include CF_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	OPACKFlags
	@brief		Flags for encoding and decoding.
*/
typedef uint32_t OPACKFlags;
#define kOPACKFlag_None 0 //! No flags.
#define kOPACKFlag_Dups (1U << 1) //! Allow duplicates instead of using back-references to de-dup.
#define kOPACKFlag_NoCopy (1U << 2) //! Try to point to input data instead of making a copy.
#define kOPACKFlag_ObjC (1U << 3) //! Objective-C caller for cases when CF vs NS matters.

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	OPACKWrite_f
	@brief		Callback to write OPACK-encoded bytes.
*/
typedef OSStatus (*OPACKWrite_f)(const uint8_t* _Nonnull inData, size_t inLen, OPACKFlags inFlags, void* _Nullable inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	OPACKEncoderCreateData / OPACKEncoderCreateDataMutable
	@abstract	Serializes an object to OPACK format and returns it as a CFData.
*/
CF_RETURNS_RETAINED
CFDataRef _Nullable OPACKEncoderCreateData(CFTypeRef _Nonnull inObj, OPACKFlags inFlags, OSStatus* _Nullable outErr);
CF_RETURNS_RETAINED
CFMutableDataRef _Nullable OPACKEncoderCreateDataMutable(CFTypeRef _Nonnull inObj, OPACKFlags inFlags, OSStatus* _Nullable outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			OPACKEncodeObject
	@brief		Serializes an object to OPACK format and calls a callback to write bytes.
*/
OSStatus
OPACKEncodeObject(
    CFTypeRef _Nonnull inObj,
    OPACKFlags inFlags,
    OPACKWrite_f _Nonnull inCallback,
    void* _Nullable inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			OPACKDecodeBytes
	@brief		Deserializes raw bytes in OPACK format to an object.
*/
CF_RETURNS_RETAINED
CFTypeRef _Nullable OPACKDecodeBytes(const void* _Nonnull inPtr, size_t inLen, OPACKFlags inFlags, OSStatus* _Nullable outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			OPACKDecodeData
	@brief		Deserializes data in OPACK format to an object.
*/
CF_RETURNS_RETAINED
CFTypeRef _Nullable OPACKDecodeData(CFDataRef _Nonnull inData, OPACKFlags inFlags, OSStatus* _Nullable outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@fn			OPACKUtilsTest
	@brief		Unit test.
*/
void OPACKUtilsTest(void);

#ifdef __cplusplus
}
#endif

#endif // OPACK_h
