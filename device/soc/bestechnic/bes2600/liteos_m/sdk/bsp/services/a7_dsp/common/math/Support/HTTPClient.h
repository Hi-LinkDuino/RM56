/*
	Copyright (C) 2011-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __HTTPClient_h__
#define __HTTPClient_h__

#include "CFUtils.h"
#include "CommonServices.h"
#include "HTTPMessage.h"
#include "NetUtils.h"

#include LIBDISPATCH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientCreate
	@abstract	Creates a new HTTP client.
*/
typedef struct HTTPClientPrivate* HTTPClientRef;

CFTypeID HTTPClientGetTypeID(void);
OSStatus HTTPClientCreate(HTTPClientRef* outClient);
OSStatus HTTPClientCreateWithSocket(HTTPClientRef* outClient, SocketRef inSock);
#define HTTPClientForget(X)             \
    do {                                \
        if (*(X)) {                     \
            HTTPClientInvalidate(*(X)); \
            CFRelease(*(X));            \
            *(X) = NULL;                \
        }                               \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientInvalidate
	@abstract	Cancels all outstanding operations.
*/
void HTTPClientInvalidate(HTTPClientRef inClient);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientGetPeerAddress
	@abstract	Gets the address of the connected peer.
	@discussion	Only valid after a connection has been established.
*/
OSStatus HTTPClientGetPeerAddress(HTTPClientRef inClient, void* inSockAddr, size_t inMaxLen, size_t* outLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetDebugDelegate
	@abstract	Sets a delegate for debug hooks.
*/
typedef void (*HTTPClientDebugDelegateSendMessage_f)(
    const void* inHeadersPtr,
    size_t inHeadersLen,
    const void* inBodyPtr,
    size_t inBodyLen,
    void* inContext);
typedef void (*HTTPClientDebugDelegateReceiveMessage_f)(
    const void* inHeadersPtr,
    size_t inHeadersLen,
    const void* inBodyPtr,
    size_t inBodyLen,
    void* inContext);

typedef struct
{
    void* context;
    HTTPClientDebugDelegateSendMessage_f sendMessage_f;
    HTTPClientDebugDelegateReceiveMessage_f receiveMessage_f;

} HTTPClientDebugDelegate;

#define HTTPClientDebugDelegateInit(PTR) memset((PTR), 0, sizeof(HTTPClientDebugDelegate));

void HTTPClientSetDebugDelegate(HTTPClientRef inClient, const HTTPClientDebugDelegate* inDelegate);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetDelegate
	@abstract	Sets a delegate to handle events, etc.
*/
typedef void (*HTTPClientHandleEvent_f)(HTTPMessageRef inEvent, void* inContext);
typedef void (*HTTPClientInvalidated_f)(OSStatus inReason, void* inContext);

typedef struct
{
    void* context;
    HTTPClientInvalidated_f invalidated_f;
    HTTPClientHandleEvent_f handleEvent_f;

} HTTPClientDelegate;

#define HTTPClientDelegateInit(PTR) memset((PTR), 0, sizeof(HTTPClientDelegate));

void* HTTPClientGetDelegateContext(HTTPClientRef me);
void HTTPClientSetDelegate(HTTPClientRef inClient, const HTTPClientDelegate* inDelegate);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetDestination
	@abstract	Sets the destination hostname, IP address, URL, etc. of the HTTP server to talk to.
	@discussion	Note: this cannot be changed once set.
*/
OSStatus HTTPClientSetDestination(HTTPClientRef inClient, const char* inDestination, int inDefaultPort);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetDispatchQueue
	@abstract	Sets the GCD queue to perform all operations on.
	@discussion	Note: this cannot be changed once operations have started.
*/
void HTTPClientSetDispatchQueue(HTTPClientRef inClient, dispatch_queue_t inQueue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetFlags
	@abstract	Enables or disables P2P connections.
*/
typedef uint32_t HTTPClientFlags;
#define kHTTPClientFlag_None 0
#define kHTTPClientFlag_P2P (1 << 0) // Enable P2P connections.
#define kHTTPClientFlag_SuppressUnusable (1 << 1) // Suppress trying to connect on seemingly unusable interfaces.
#define kHTTPClientFlag_Reachability (1 << 2) // Use the reachability APIs before trying to connect.
#define kHTTPClientFlag_BoundInterface (1 << 3) // Set bound interface before connect if interface index available.
#define kHTTPClientFlag_Events (1 << 4) // Enable support for unsolicited events from the server.
#define kHTTPClientFlag_NonCellular (1 << 5) // Don't allow connections over cellular links.
#define kHTTPClientFlag_NonExpensive (1 << 6) // Don't allow connections over expensive links (cellular, hotspot, etc.).
#define kHTTPClientFlag_NonLinkLocal (1 << 8) // Skip link-local addresses.

void HTTPClientSetFlags(HTTPClientRef inClient, HTTPClientFlags inFlags, HTTPClientFlags inMask);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetKeepAlive
	@abstract	Enables/disables TCP keep alive and configures the time between probes and the max probes before giving up.
	
	@param		inClient				Client to set keep-alive options for.
	@param		inIdleSecs				Number of idle seconds before a keep-alive probe is sent.
	@param		inMaxUnansweredProbes	Max number of unanswered probes before a connection is terminated.
*/
void HTTPClientSetKeepAlive(HTTPClientRef inClient, int inIdleSecs, int inMaxUnansweredProbes);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetLogging
	@abstract	Sets the log category to use for HTTP connection and other logging.
*/
void HTTPClientSetConnectionLogging(HTTPClientRef me, LogCategory* inLogCategory);
void HTTPClientSetLogging(HTTPClientRef inClient, LogCategory* inLogCategory);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetTimeout
	@abstract	Sets the seconds without any data before a response or event times out.
	@discussion	For responses, if a message has a timeout, it override this value.
*/
void HTTPClientSetTimeout(HTTPClientRef inClient, int inSecs);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSetTransportDelegate
	@abstract	Sets a delegate for transport-specific reading and writing data.
*/
void HTTPClientSetTransportDelegate(HTTPClientRef inClient, const NetTransportDelegate* inDelegate);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientDetach
	@abstract	Detaches a client from its socket to hand it off to other code (e.g. reverse connections).
	@discussion
	
	This finishes handling any messages that may already be in progress and tears down the client, but leaves the
	socket open. The handler is called after the client has fully quiesced and the socket can be safely used.
*/
typedef void (*HTTPClientDetachHandler_f)(SocketRef inSock, void* inContext1, void* inContext2, void* inContext3);
OSStatus
HTTPClientDetach(
    HTTPClientRef inClient,
    HTTPClientDetachHandler_f inHandler,
    void* inContext1,
    void* inContext2,
    void* inContext3);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientSendMessage
	@abstract	Sends an HTTP message.
*/
OSStatus HTTPClientSendMessage(HTTPClientRef inClient, HTTPMessageRef inMsg);
OSStatus HTTPClientSendMessageSync(HTTPClientRef inClient, HTTPMessageRef inMsg);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPClientTest
	@abstract	Unit test.
*/
OSStatus HTTPClientTest(void);

#ifdef __cplusplus
}
#endif

#endif // __HTTPClient_h__
