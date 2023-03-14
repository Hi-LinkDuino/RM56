/*
	Copyright (C) 2012-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __NetworkChangeListener_h__
#define __NetworkChangeListener_h__

#include "CommonServices.h"

#include CF_HEADER
#include LIBDISPATCH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NetworkChangeListenerCreate
	@abstract	Creates an object to listen for network changes.
*/

typedef struct NetworkChangeListenerPrivate* NetworkChangeListenerRef;

CFTypeID NetworkChangeListenerGetTypeID(void);
OSStatus NetworkChangeListenerCreate(NetworkChangeListenerRef* outListener);
#define NetworkChangeListenerForget(X) ForgetCustomEx((X), NetworkChangeListenerStop, CFRelease)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NetworkChangeListenerSetDispatchQueue
	@abstract	Sets the dispatch queue to serialize all callbacks.
*/

void NetworkChangeListenerSetDispatchQueue(NetworkChangeListenerRef inListener, dispatch_queue_t inQueue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NetworkChangeListenerSetHandler
	@abstract	Sets a function to be called when the network changes.
*/

#define kNetworkEvent_Stopped 1 // Called after a stop has completed.
#define kNetworkEvent_Changed 2 // Called when a network change occurs.

typedef void (*NetworkChangeHandlerFunc)(uint32_t inEvent, void* inContext);

void NetworkChangeListenerSetHandler(
    NetworkChangeListenerRef inListener,
    NetworkChangeHandlerFunc inHandler,
    void* inArg);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NetworkChangeListenerStart / NetworkChangeListenerStop
	@abstract	Starts or stops listening for network changes.
*/

void NetworkChangeListenerStart(NetworkChangeListenerRef inListener);
void NetworkChangeListenerStop(NetworkChangeListenerRef inListener);

#ifdef __cplusplus
}
#endif

#endif // __NetworkChangeListener_h__
