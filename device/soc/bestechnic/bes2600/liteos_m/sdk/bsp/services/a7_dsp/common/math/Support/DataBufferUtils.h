/*
	Copyright (C) 2008-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __DataBufferUtils_h__
#define __DataBufferUtils_h__

#include "CommonServices.h"
#include "DebugServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		DataBuffer
	@abstract	Dynamically sized buffer of data.
	@discussion
	
	You use a DataBuffer by calling DataBuffer_Init, zero or more DataBuffer_Append*, etc. functions to modify the
	DataBuffer, then use DataBuffer_Commit to finalize it for use via DataBuffer_GetPtr() and DataBuffer_GetLen().
*/
#define kDataBufferDefaultMaxSize 0x7FFFFFFF

typedef struct
{
    // WARNING: All fields are private. Use the API instead of touching them directly.

    uint8_t* staticBufferPtr; //! Optional static buffer to use if the content will fit. May be NULL.
    size_t staticBufferLen; //! Number of bytes the static buffer can hold.
    size_t maxGrowLen; //! Max number of bytes to allow it to grow to.
    uint8_t* bufferPtr; //! May be malloc'd or a static buffer.
    size_t bufferLen; //! Number of bytes in use.
    size_t bufferMaxLen; //! Number of bytes the current buffer can hold.
    uint8_t malloced; //! Non-zero if buffer was malloc'd. 0 if static or NULL.
    OSStatus firstErr; //! First error that occurred or kNoErr.

} DataBuffer;

#define DataBuffer_GetError(DB) (DB)->firstErr
#define DataBuffer_GetPtr(DB) (DB)->bufferPtr
#define DataBuffer_GetEnd(DB) (DataBuffer_GetPtr(DB) + DataBuffer_GetLen(DB))
#define DataBuffer_GetLen(DB) (DB)->bufferLen

void DataBuffer_Init(DataBuffer* inDB, void* inStaticBufferPtr, size_t inStaticBufferLen, size_t inMaxGrowLen);
void DataBuffer_Free(DataBuffer* inDB);
uint8_t* DataBuffer_Disown(DataBuffer* inDB);
OSStatus DataBuffer_Commit(DataBuffer* inDB, uint8_t** outPtr, size_t* outLen);
OSStatus DataBuffer_Detach(DataBuffer* inDB, uint8_t** outPtr, size_t* outLen);
OSStatus DataBuffer_DetachCString(DataBuffer* inDB, char** outStr);
#define DataBuffer_Reset(DB) \
    do {                     \
        (DB)->bufferLen = 0; \
        (DB)->firstErr = 0;  \
    } while (0)

#define DataBuffer_Insert(DB, OFFSET, PTR, LEN) DataBuffer_Replace(DB, OFFSET, 0, PTR, LEN)
#define DataBuffer_Remove(DB, OFFSET, LEN) DataBuffer_Replace(DB, OFFSET, LEN, NULL, 0)
OSStatus DataBuffer_Replace(DataBuffer* inDB, size_t inOffset, size_t inOldLen, const void* inNewData, size_t inNewLen);

OSStatus DataBuffer_Resize(DataBuffer* inDB, size_t inNewLen, void* outPtr);
#define DataBuffer_Grow(DB, AMOUNT, OUT_PTR) DataBuffer_Resize(DB, DataBuffer_GetLen(DB) + (AMOUNT), OUT_PTR)
OSStatus DataBuffer_Shrink(DataBuffer* inDB, size_t inAmount);

OSStatus DataBuffer_Append(DataBuffer* inDB, const void* inData, size_t inLen);
OSStatus DataBuffer_AppendF(DataBuffer* inDB, const char* inFormat, ...) PRINTF_STYLE_FUNCTION(2, 3);

#if (TARGET_OS_POSIX)
OSStatus DataBuffer_RunProcessAndAppendOutput(DataBuffer* inBuffer, const char* inCmdLine);
#endif
#if (TARGET_HAS_STD_C_LIB)
OSStatus DataBuffer_AppendANSIFile(DataBuffer* inBuffer, FILE* inFile);
OSStatus DataBuffer_AppendFile(DataBuffer* inBuffer, const char* inPath);
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DataBufferUtils_Test
	@abstract	Unit test.
*/
OSStatus DataBufferUtils_Test(void);

#ifdef __cplusplus
}
#endif

#endif // __DataBufferUtils_h__
