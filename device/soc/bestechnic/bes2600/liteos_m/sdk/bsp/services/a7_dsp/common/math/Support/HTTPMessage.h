/*
	Copyright (C) 2011-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __HTTPMessage_h__
#define __HTTPMessage_h__

#include "CommonServices.h"
#include "DebugServices.h"
#include "HTTPUtils.h"
#include "NetUtils.h"

#include CF_RUNTIME_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		HTTPMessage
	@abstract	Encapsulates an HTTP message.
*/
typedef struct HTTPMessagePrivate* HTTPMessageRef;

CFTypeID HTTPMessageGetTypeID(void);
OSStatus HTTPMessageCreate(HTTPMessageRef* outMessage);
void HTTPMessageReset(HTTPMessageRef inMsg);

OSStatus HTTPMessageReadMessage(HTTPMessageRef inMsg, NetTransportRead_f inRead_f, void* inRead_ctx);
OSStatus HTTPMessageWriteMessage(HTTPMessageRef inMsg, NetTransportWriteV_f inWriteV_f, void* inWriteV_ctx);
OSStatus HTTPMessageSetBody(HTTPMessageRef inMsg, const char* inContentType, const void* inData, size_t inLen);
OSStatus HTTPMessageSetBodyPtr(HTTPMessageRef inMsg, const char* inContentType, const void* inData, size_t inLen);
OSStatus HTTPMessageSetBodyLength(HTTPMessageRef inMsg, size_t inLen);

OSStatus
HTTPMessageGetOrCopyFormVariable(
    HTTPMessageRef inMsg,
    const char* inName,
    const char** outValuePtr,
    size_t* outValueLen,
    char** outValueStorage);

//===========================================================================================================================
//	Internals
//===========================================================================================================================

#define kHTTPDefaultMaxBodyLen 16000000
#define kHTTPNoTimeout UINT64_C(0xFFFFFFFFFFFFFFFF)

typedef uint32_t HTTPMessageFlags;
#define kHTTPMessageFlags_None 0 // No flags.
#define kHTTPMessageFlag_NoCopy (1 << 0) // Don't copy data. Caller must ensure data remains valid.

typedef void (*HTTPMessageUser_f)(void* inArg);
typedef void (*HTTPMessageCompletionFunc)(HTTPMessageRef inMsg);

struct HTTPMessagePrivate {
    CFRuntimeBase base; // CF type info. Must be first.
    HTTPMessageRef next; // Next message in the list.
    HTTPHeader header; // Header of the message read or written.
    Boolean headerRead; // True if the header has been read.
    Boolean closeAfterRequest; // True if connection should be shutdown after sending request.
    uint8_t* bodyPtr; // Pointer to the body buffer.
    size_t bodyLen; // Total body length.
    size_t maxBodyLen; // Max allowed body length.
    size_t bodyOffset; // Offset into the body that we've read so far.
    uint8_t smallBodyBuf[1024]; // Fixed buffer used for small messages to avoid allocations.
    uint8_t* bigBodyBuf; // malloc'd buffer for large bodies.
    HTTPHeader* requestHeader; // Copy of request header when using HTTP auth.
    uint8_t* requestBodyPtr; // Copy of request body when using HTTP auth.
    size_t requestBodyLen; // Number of bytes in requestBodyPtr.
    iovec_t iov[2]; // Used for gathered I/O to avoid non-MTU packets when possible.
    iovec_t* iop; // Ptr to the current iovec being sent.
    int ion; // Number of iovecs remaining to be sent.
    uint64_t timeoutNanos; // DEPRECATED: Nanoseconds until timing out on connects.
    int connectTimeoutSecs; // Seconds until timing out on connects if > 0.
    int dataTimeoutSecs; // Seconds until timing out on reads/writes if > 0.
    OSStatus status; // Status of the message.
    void* httpContext1; // Context pointer for use by HTTP library code. Don't touch this.
    void* httpContext2; // Context pointer for use by HTTP library code. Don't touch this.
    void* userContext1; // Context pointer for use by user code.
    void* userContext2; // Context pointer for use by user code.
    void* userContext3; // Context pointer for use by user code.
    void* userContext4; // Context pointer for use by user code.
    HTTPMessageCompletionFunc completion; // Function to call when a message completes.
};

#ifdef __cplusplus
}
#endif

#endif // __HTTPMessage_h__
