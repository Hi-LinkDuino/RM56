/*
	Copyright (C) 2011-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __DebugIPCUtils_h__
#define __DebugIPCUtils_h__

#include "CommonServices.h"
#include "DebugServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================================================================
//	API
//===========================================================================================================================

typedef struct IPCAgentPrivate* IPCAgentRef;

typedef void (*IPCMessageHandlerFunc)(CFDictionaryRef inMsg, void* inContext);

OSStatus IPCAgent_Create(IPCAgentRef* outAgent);
void IPCAgent_DeleteAsync(IPCAgentRef inAgent);
void IPCAgent_DeleteSync(IPCAgentRef inAgent);
void IPCAgent_SetMessageHandler(IPCAgentRef inAgent, IPCMessageHandlerFunc inHandler, void* inContext);
void IPCAgent_Start(IPCAgentRef inAgent);
OSStatus IPCAgent_SendMessage(IPCAgentRef inAgent, CFDictionaryRef inMessage);
OSStatus IPCAgent_Perform(CFDictionaryRef inMessage, IPCMessageHandlerFunc inResponseHandler, void* inResponseContext);

//===========================================================================================================================
//	Debugging Support
//===========================================================================================================================

#define kDebugIPCOpCode_Control CFSTR("control")
#define kDebugIPCOpCode_Logging CFSTR("logging")
#define kDebugIPCOpCode_Show CFSTR("show")

#define kDebugIPCKey_Command CFSTR("command")
#define kDebugIPCKey_ResponseType CFSTR("responseType")
#define kDebugIPCKey_Value CFSTR("value")

typedef OSStatus (*DebugIPCHandlerFunc)(CFDictionaryRef inRequest, CFDictionaryRef* outResponse, void* inContext);

OSStatus DebugIPC_EnsureInitialized(DebugIPCHandlerFunc inHandler, void* inContext);
void DebugIPC_EnsureFinalized(void);
OSStatus DebugIPC_LogControl(const char* inNewConfig);
OSStatus DebugIPC_Perform(CFDictionaryRef inRequest, IPCMessageHandlerFunc inHandler, void* inContext);
OSStatus DebugIPC_PerformF(IPCMessageHandlerFunc inHandler, void* inContext, const char* inFormat, ...);
OSStatus DebugIPC_PerformV(IPCMessageHandlerFunc inHandler, void* inContext, const char* inFormat, va_list inArgs);

#if (!EXCLUDE_UNIT_TESTS)
OSStatus DebugIPCUtils_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __DebugIPCUtils_h__
