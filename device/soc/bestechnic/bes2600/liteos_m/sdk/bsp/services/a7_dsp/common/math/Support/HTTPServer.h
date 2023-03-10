/*
	Copyright (C) 2011-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __HTTPServer_h__
#define __HTTPServer_h__

#include "CFUtils.h"
#include "CommonServices.h"
#include "DebugServices.h"
#include "HTTPMessage.h"
#include "NetUtils.h"

#include CF_RUNTIME_HEADER
#include LIBDISPATCH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#pragma mark == Delegate ==
#endif

//===========================================================================================================================
/*!	@group		HTTPServerDelegate
	@abstract	Customization of the HTTP server.
	@discussion	The server delegate is used to override functionality, such as starting, stopping, handling messages, etc.
	
	The delegate structure can be allocated on the stack, initialized, and then filled out before creating the server.
	For example:
	
	HTTPServerRef			server;
	HTTPServerDelegate		delegate;
	
	HTTPServerDelegateInit( &delegate );
	delegate.handleMessage_f = MyServerHandleMessage;
	
	err = HTTPServerCreate( &server, &delegate );
	...
*/
typedef struct HTTPServerPrivate* HTTPServerRef;
typedef struct HTTPConnectionPrivate* HTTPConnectionRef;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerInitialize_f
	@abstract	Called when the server is initialized, but before the server is started.
	@discussion	The delegate can use this to initialize anything it needs for the lifetime of the server.
				The server has not started yet so it's normally better to defer initializations to the start handler.
				If an error is returned from this function, the server will fail and return the error to the user.
*/
typedef OSStatus (*HTTPServerInitialize_f)(HTTPServerRef inServer, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerFinalize_f
	@abstract	Called when the server is finalize. No further calls to delegate will be made after this.
	@discussion	This is the last chance for the delegate to release any resources it might still have in use (e.g. memory).
*/
typedef void (*HTTPServerFinalize_f)(HTTPServerRef inServer, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerControl_f
	@abstract	Called for control operations (e.g. starting or stopping the server, etc.).
	@discussion	The delegate must look at the command to determine whether to handle it or not.
				If the delegate chooses not handle the command, it must return kNotHandledErr.
*/
typedef OSStatus (*HTTPServerControl_f)(
    HTTPServerRef inServer,
    uint32_t inFlags,
    CFStringRef inCommand,
    CFTypeRef inQualifier,
    CFTypeRef inParams,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerCopyProperty_f
	@abstract	Called to copy a property.
	@discussion	The delegate must look at the property to determine whether to handle it or not.
				If the delegate chooses not handle the command, it must return kNotHandledErr.
*/
typedef CFTypeRef (*HTTPServerCopyProperty_f)(
    HTTPServerRef inServer,
    uint32_t inFlags,
    CFStringRef inProperty,
    CFTypeRef inQualifier,
    OSStatus* outErr,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerSetProperty_f
	@abstract	Called to set a property.
	@discussion	The delegate must look at the property to determine whether to handle it or not.
				If the delegate chooses not handle the command, it must return kNotHandledErr.
*/
typedef OSStatus (*HTTPServerSetProperty_f)(
    HTTPServerRef inServer,
    uint32_t inFlags,
    CFStringRef inProperty,
    CFTypeRef inQualifier,
    CFTypeRef inValue,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerConnectionCreated_f
	@abstract	Called after the server accepts a new connection.
	@discussion	This is called before the connection's delegate is called to give the server a chance to replace its delegate.
*/
typedef void (*HTTPServerConnectionCreated_f)(
    HTTPServerRef inServer,
    HTTPConnectionRef inCnx,
    void* inCnxExtraPtr,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionInitialize_f
	@abstract	Called to initialized any per-connection state.
	@discussion	It's called after accepting a connection, but before the connection is becomes active.
*/
typedef OSStatus (*HTTPConnectionInitialize_f)(HTTPConnectionRef inCnx, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionFinalize_f
	@abstract	Called when the connection is finalized. No further calls for the connection will be made after this.
	@discussion	This is the last chance for the delegate to release any resources it might still have in use (e.g. memory).
*/
typedef void (*HTTPConnectionFinalize_f)(HTTPConnectionRef inCnx, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionClose_f
	@abstract	Called when the connection needs to be closed.
*/
typedef void (*HTTPConnectionClose_f)(HTTPConnectionRef inCnx, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionRequiresAuth_f
	@abstract	Called to ask the delegate if message requires HTTP-style authorization.
	@discussion	This can be used to allow certain URLs to bypass authorization (e.g. URLs that don't return secret info).
*/
typedef Boolean (*HTTPConnectionRequiresAuth_f)(HTTPConnectionRef inCnx, HTTPMessageRef inMsg, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionHandleMessage_f
	@abstract	Called when a complete request message header and body has been received so the delegate can process it.
	@discussion	This is the main message handling function. Most of the work of the server happens here.
				The delegate is expected to either return an error (which will cause the connection to close) or it build
				a response message and send it using HTTPConnectionSendResponse (or one of the convenience wrappers).
*/
typedef OSStatus (*HTTPConnectionHandleMessage_f)(HTTPConnectionRef inCnx, HTTPMessageRef inMsg, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionInitResponse_f
	@abstract	Called for the delegate to do any response message preparation.
	@discussion	This can be used to add any custom headers to a message without requiring each message sender to do it.
				For example, a server may have a custom user agent or other common fields so rather than burdening the
				code that sends individual messages with that common code, it can be done in the delegate via this hook.
*/
typedef OSStatus (*HTTPConnectionInitResponse_f)(HTTPConnectionRef inCnx, HTTPMessageRef inResponseMsg, void* inContext);

// HTTPServerDelegate

typedef struct
{
    void* context;
    HTTPServerInitialize_f initializeServer_f;
    HTTPServerFinalize_f finalizeServer_f;
    HTTPServerControl_f control_f;
    HTTPServerCopyProperty_f copyProperty_f;
    HTTPServerSetProperty_f setProperty_f;
    HTTPServerConnectionCreated_f connectionCreated_f;

    size_t connectionExtraLen;
    const char* httpProtocol;
    HTTPConnectionInitialize_f initializeConnection_f;
    HTTPConnectionFinalize_f finalizeConnection_f;
    HTTPConnectionRequiresAuth_f requiresAuth_f;
    HTTPConnectionHandleMessage_f handleMessage_f;
    HTTPConnectionInitResponse_f initResponse_f;
} HTTPServerDelegate;

#define HTTPServerDelegateInit(PTR) memset((PTR), 0, sizeof(HTTPServerDelegate))

// HTTPConnectionDelegate

typedef struct
{
    size_t extraLen;
    void* context;
    const char* httpProtocol;
    HTTPConnectionInitialize_f initialize_f;
    HTTPConnectionFinalize_f finalize_f;
    HTTPConnectionClose_f close_f;
    HTTPConnectionRequiresAuth_f requiresAuth_f;
    HTTPConnectionHandleMessage_f handleMessage_f;
    HTTPConnectionInitResponse_f initResponse_f;

} HTTPConnectionDelegate;

#define HTTPConnectionDelegateInit(PTR) memset((PTR), 0, sizeof(HTTPConnectionDelegate))

#if 0
#pragma mark -
#pragma mark == Server API ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerGetTypeID
	@abstract	Gets the CF type ID of all HTTPServer objects.
*/
CFTypeID HTTPServerGetTypeID(void);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerCreate
	@abstract	Creates a new HTTP server object. Caller must CFRelease when done.
*/
OSStatus HTTPServerCreate(HTTPServerRef* outServer, const HTTPServerDelegate* inDelegate);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerForget
	@abstract	Stops and releases the server.
	@discussion	Note that stopping is asynchronous so it's not safe to assume it's stopped until the stop command is received.
*/
#define HTTPServerForget(X) ForgetCustomEx(X, HTTPServerStop, CFRelease)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerSetDispatchQueue
	@abstract	Sets the dispatch queue for all operations. Defaults to the main queue if NULL or not set.
*/
void HTTPServerSetDispatchQueue(HTTPServerRef inServer, dispatch_queue_t inQueue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerSetLogging
	@abstract	Sets the log category to use for logging.
*/
void HTTPServerSetLogging(HTTPServerRef me, LogCategory* inLogCategory);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	http_server_ulog
	@abstract	Logs a message related to the server.
*/
#define http_server_ulog(SERVER, LEVEL, ...) ulog((SERVER)->ucat, (LEVEL), __VA_ARGS__)

#if 0
#pragma mark -
#pragma mark == Connection API ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionGetTypeID
	@abstract	Gets the CF type ID of all HTTPConnection objects.
*/
CFTypeID HTTPConnectionGetTypeID(void);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionCreate
	@abstract	Creates a new, standalone connection object. Caller must CFRelease when done.
	@discussion	This is only used when the connection is used without an HTTPServer object managing it.
				For example, if you have code that connects a socket on its own and wants to use the HTTP protocol with it
				then you can manually create an HTTPConnection object with that connected socket using this function.
*/
OSStatus
HTTPConnectionCreate(
    HTTPConnectionRef* outCnx,
    const HTTPConnectionDelegate* inDelegate,
    SocketRef inSock,
    HTTPServerRef inServer);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionForget
	@abstract	Stops and releases the connection. Only for use with standalone objects created with HTTPConnectionCreate.
	@discussion	Note that stopping is asynchronous so it's not safe to assume it's stopped until the finalize function is called.
*/
#define HTTPConnectionForget(X) ForgetCustomEx(X, HTTPConnectionStop, CFRelease)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionSetDelegate
	@abstract	Sets the delegate to customize functionality.
*/
void HTTPConnectionSetDelegate(HTTPConnectionRef inCnx, const HTTPConnectionDelegate* inDelegate);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionSetDispatchQueue
	@abstract	Sets the dispatch queue for all operations. Defaults to the main queue if NULL or not set.
	@discussion	This should only be used by standalone connection objects created with HTTPConnectionCreate.
				Connections managed by an HTTPServer object will use the queue specified by HTTPServerSetDispatchQueue.
*/
void HTTPConnectionSetDispatchQueue(HTTPConnectionRef inCnx, dispatch_queue_t inQueue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionStart
	@abstract	Starts or stops a connection. Only for use with standalone objects created with HTTPConnectionCreate.
*/
OSStatus HTTPConnectionStart(HTTPConnectionRef inCnx);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionStop
	@abstract	Stops a connection. Only for use with standalone objects created with HTTPConnectionCreate.
*/
void HTTPConnectionStop(HTTPConnectionRef inCnx);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionDetach
	@abstract	Detaches a connection from its server so it can run standalone (e.g. reverse connections).
	@discussion
	
	This finishes handling any messages that may already be in progress and tears down the connection, but leaves the
	socket open. The handler is called after the connection has fully quiesced and the socket can be safely used.
	This function must only be called from the handleMessage_f callback.
*/
typedef void (*HTTPConnectionDetachHandler_f)(SocketRef inSock, void* inContext1, void* inContext2, void* inContext3);
OSStatus
HTTPConnectionDetach(
    HTTPConnectionRef inCnx,
    HTTPConnectionDetachHandler_f inHandler,
    void* inContext1,
    void* inContext2,
    void* inContext3);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionSetTransportDelegate
	@abstract	Sets a delegate for transport-specific reading and writing data.
*/
void HTTPConnectionSetTransportDelegate(HTTPConnectionRef inCnx, const NetTransportDelegate* inDelegate);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPConnectionSendResponse
	@abstract	Starts sending a response message.
*/
OSStatus HTTPConnectionSendResponse(HTTPConnectionRef inCnx);

#if 0
#pragma mark -
#pragma mark == Commands ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		Commands
	@abstract	Commands for controlling the server.
*/

// Starts or stops the server. These commands must be balanced (e.g. if you start it, you're responsible for stopping it).
#define kHTTPServerCommand_StartServer CFSTR("startServer")
#define kHTTPServerCommand_StopServer CFSTR("stopServer")
#define HTTPServerStart(X) HTTPServerControl((X), 0, kHTTPServerCommand_StartServer, NULL)
#define HTTPServerStop(X) HTTPServerControl((X), 0, kHTTPServerCommand_StopServer, NULL)

// Sync extension for HTTPServer.
#define HTTPServerStartSync(X) HTTPServerControlSync((X), 0, kHTTPServerCommand_StartServer, NULL)
#define HTTPServerStopSync(X) HTTPServerControlSync((X), 0, kHTTPServerCommand_StopServer, NULL)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerControl
	@abstract	Controls the server.
	@discussion	For details on the format-string variants, see the HeaderDoc for CFPropertyListCreateFormatted.
*/
#define HTTPServerControl(SERVER, FLAGS, COMMAND, PARAMS)                                            \
    CFObjectSetProperty((SERVER), (SERVER)->queue, _HTTPServerControl, (FLAGS) | kCFObjectFlagAsync, \
        (COMMAND), NULL, (PARAMS))

#define HTTPServerControlSync(SERVER, FLAGS, COMMAND, PARAMS)                   \
    CFObjectSetProperty((SERVER), (SERVER)->queue, _HTTPServerControl, (FLAGS), \
        (COMMAND), NULL, (PARAMS))

OSStatus
_HTTPServerControl(
    CFTypeRef inObject,
    CFObjectFlags inFlags,
    CFStringRef inCommand,
    CFTypeRef inQualifier,
    CFTypeRef inParams);

#if 0
#pragma mark -
#pragma mark == Properties ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		Properties
	@abstract	Properties to get/set on the server.
*/

// [Boolean] Allows or prevents the use of P2P interfaces. Defaults to off (prevent).
#define kHTTPServerProperty_P2P CFSTR("p2p")

// [String] Password used for HTTP authentication. Defaults to not requiring a password.
#define kHTTPServerProperty_Password CFSTR("password")

// [String] Realm used for HTTP authentication. Must be set if a password is set.
#define kHTTPServerProperty_Realm CFSTR("realm")

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerCopyProperty
	@abstract	Copies a property from the server.
*/
#define HTTPServerCopyProperty(SERVER, FLAGS, PROPERTY, QUALIFIER, OUT_ERROR) \
    CFObjectCopyProperty((SERVER), (SERVER)->queue, _HTTPServerCopyProperty, (FLAGS), (PROPERTY), (QUALIFIER), (OUT_ERROR))

#define HTTPServerGetPropertyBoolean(SERVER, FLAGS, PROPERTY, QUALIFIER, OUT_ERROR) \
    ((HTTPServerGetPropertyInt64((SERVER), (FLAGS), (PROPERTY), (QUALIFIER), (OUT_ERROR)) != 0) ? true : false)

#define HTTPServerGetPropertyCString(SERVER, FLAGS, PROPERTY, QUALIFIER, BUF, MAX_LEN, OUT_ERROR)                        \
    CFObjectGetPropertyCStringSync((SERVER), (SERVER)->queue, _HTTPServerCopyProperty, (FLAGS), (PROPERTY), (QUALIFIER), \
        (BUF), (MAX_LEN), (OUT_ERROR))

#define HTTPServerGetPropertyInt64(SERVER, FLAGS, PROPERTY, QUALIFIER, OUT_ERROR)                                      \
    CFObjectGetPropertyInt64Sync((SERVER), (SERVER)->queue, _HTTPServerCopyProperty, (FLAGS), (PROPERTY), (QUALIFIER), \
        (OUT_ERROR))

CF_RETURNS_RETAINED
CFTypeRef
_HTTPServerCopyProperty(
    CFTypeRef inObject,
    CFObjectFlags inFlags,
    CFStringRef inProperty,
    CFTypeRef inQualifier,
    OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	HTTPServerSetProperty
	@abstract	Sets a property on the server.
	@discussion	For details on the format-string variants, see the HeaderDoc for CFPropertyListCreateFormatted.
*/
#define HTTPServerSetProperty(SERVER, FLAGS, PROPERTY, QUALIFIER, VALUE)                                 \
    CFObjectSetProperty((SERVER), (SERVER)->queue, _HTTPServerSetProperty, (FLAGS) | kCFObjectFlagAsync, \
        (PROPERTY), (QUALIFIER), (VALUE))

#define HTTPServerSetPropertyF(SERVER, FLAGS, PROPERTY, QUALIFIER, ...)                                   \
    CFObjectSetPropertyF((SERVER), (SERVER)->queue, _HTTPServerSetProperty, (FLAGS) | kCFObjectFlagAsync, \
        (PROPERTY), (QUALIFIER), __VA_ARGS__)

#define HTTPServerSetPropertyV(SERVER, FLAGS, PROPERTY, QUALIFIER, FORMAT, ARGS)                          \
    CFObjectSetPropertyV((SERVER), (SERVER)->queue, _HTTPServerSetProperty, (FLAGS) | kCFObjectFlagAsync, \
        (PROPERTY), (QUALIFIER), (FORMAT), (ARGS))

OSStatus
_HTTPServerSetProperty(
    CFTypeRef inObject,
    CFObjectFlags inFlags,
    CFStringRef inProperty,
    CFTypeRef inQualifier,
    CFTypeRef inValue);

#if 0
#pragma mark -
#pragma mark == Server Internals ==
#endif

//===========================================================================================================================
//	HTTPServer
//===========================================================================================================================

typedef struct
{
    dispatch_source_t source;
    SocketRef sock;
    HTTPServerRef server;

} HTTPListenerContext;

struct HTTPServerPrivate {
    CFRuntimeBase base; // CF type info. Must be first.
    dispatch_queue_t queue; // Queue to perform all operations on.
    LogCategory* ucat; // Log category to use for logging.

    // Connections

    HTTPListenerContext* listenerV4; // Listener context for IPv4 connections.
    HTTPListenerContext* listenerV6; // Listener context for IPv6 connections.
    int listeningPort; // TCP port we're listening on (after start).
    HTTPConnectionRef connections; // Linked list of connections.
    Boolean started; // True if we're started and listening for connections.

    // HTTP Authorization

    char* password; // Password for HTTP auth or NULL/empty for no pasword.
    char* realm; // HTTP auth realm when using passwords.
    uint8_t timedNonceKey[16]; // Key for timed nonces.
    Boolean timedNonceInitialized; // True if timedNonceKey has been initialized.

    // Customization

    HTTPServerDelegate delegate; // Delegate used to configure and implement customizations.
    int allowP2P; // True if P2P interfaces should be allowed.
    int listenPort; // Port to listen on. -port means "try this port, but allow dynamic".
};

#if 0
#pragma mark -
#pragma mark == Connection Internals ==
#endif

//===========================================================================================================================
//	HTTPConnection
//===========================================================================================================================

typedef enum {
    kHTTPConnectionStateReadingRequest = 0,
    kHTTPConnectionStateProcessingRequest = 1,
    kHTTPConnectionStateWritingResponse = 2,
    kHTTPConnectionStatePreparingEvent = 3,
    kHTTPConnectionStateWritingEvent = 4

} HTTPConnectionState;

struct HTTPConnectionPrivate {
    CFRuntimeBase base; // CF type info. Must be first.
    HTTPConnectionRef next; // Next connection in the server's list.
    HTTPServerRef server; // Server this connection is a part of.
    HTTPConnectionDelegate delegate; // Customizes the connection.
    HTTPConnectionClose_f close_f; // Internal close handler.
    dispatch_queue_t queue; // Queue to perform all operations on.
    LogCategory* ucat; // Log category to use for logging.
    SocketRef sock; // Socket for this connection.
    char ifName[IF_NAMESIZE + 1]; // Name of the interface the connection was accepted on.
    NetTransportType transportType; // Transport type of the interface the connection was accepted on.
    dispatch_source_t readSource; // GCD source for readability notification.
    Boolean readSuspended; // True if GCD read source has been suspended.
    dispatch_source_t writeSource; // GCD source for writability notification.
    Boolean writeSuspended; // True if GCD write source has been suspended.
    sockaddr_ip selfAddr; // Address of our side of the connection.
    sockaddr_ip peerAddr; // Address of the peer side of the connection.
    HTTPConnectionState state; // State of the HTTP message reading/writing.
    HTTPMessageRef requestMsg; // Pre-allocated message for reading requests.
    HTTPMessageRef responseMsg; // Pre-allocated message for writing responses.
    HTTPMessageRef eventList; // Linked list of pending events to send.
    HTTPMessageRef* eventNext; // Ptr to append next event to send.
    NetTransportDelegate transportDelegate; // Delegate for transport-specific reading/writing.
    Boolean hasTransportDelegate; // True if a transport delegate has been set.
    HTTPConnectionDetachHandler_f detachHandler_f; // Function to call when a detach has completed.
    void* context1; // Generic context ptr.
    void* context2; // Generic context ptr.
    void* context3; // Generic context ptr.
};

#ifdef __cplusplus
}
#endif

#endif // __HTTPServer_h__
