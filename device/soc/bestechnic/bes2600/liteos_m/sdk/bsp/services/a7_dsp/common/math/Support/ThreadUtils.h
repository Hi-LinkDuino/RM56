/*
	Copyright (C) 2009-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __ThreadUtils_h__
#define __ThreadUtils_h__

#include "CommonServices.h"
#include "DebugServices.h"
#include "posix_porting.h"
#if (TARGET_OS_FREEBSD)
#include <pthread_np.h>
#endif

#if (TARGET_OS_LINUX)
#include <sys/prctl.h>
#endif

#if (TARGET_OS_NETBSD)
#include <time.h>
#endif

#if (TARGET_OS_POSIX)
#include <pthread.h>
#endif

#if (TARGET_OS_THREADX)
#include "ThreadUtilsThreadX.h"
#endif

#if (TARGET_OS_WINDOWS)
#include "ThreadUtilsWindows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (TARGET_OS_NETBSD)
//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	COND_TIMEDWAIT_relative_np
	@abstract	Relative timed wait.
*/
int COND_TIMEDWAIT_relative_np(
    COND_HANDLE_T* inCondition,
    mutex_handle_t* inMutex,
    const struct timespec* inTimeout);
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	pthread_setname_np_compat
	@abstract	Sets the name of the current thread.
*/
#if (TARGET_OS_DARWIN)
#define pthread_setname_np_compat(NAME) pthread_setname_np(NAME)
#elif (TARGET_OS_FREEBSD)
#define pthread_setname_np_compat(NAME) pthread_set_name_np(pthread_self(), (NAME))
#elif (TARGET_OS_LINUX)
// #define pthread_setname_np_compat(NAME) prctl(PR_SET_NAME, (NAME), 0, 0, 0)
#define pthread_setname_np_compat(NAME) 
#elif (TARGET_OS_QNX)
#define pthread_setname_np_compat(NAME) pthread_setname_np(pthread_self(), (NAME))
#elif (TARGET_OS_THREADX)
// Prototype already provided by ThreadUtilsThreadX.h.
#elif (TARGET_OS_WINDOWS)
#define pthread_setname_np_compat(NAME) pthread_setname_np((NAME))
#elif (TARGET_OS_NETBSD)
#define pthread_setname_np_compat(NAME) pthread_setname_np(pthread_self(), "%s", ((void*)(NAME)))
#else
#define pthread_setname_np_compat(NAME) \
    do {                                \
    } while (0)
#endif

#if (DEBUG)
#define debug_pthread_setname_np(NAME) pthread_setname_np_compat(NAME)
#else
#define debug_pthread_setname_np(NAME) \
    do {                               \
    } while (0)
#endif

#define mach_policy_to_name(POLICY) ( \
    ((POLICY) == POLICY_TIMESHARE) ? "POLICY_TIMESHARE" : ((POLICY) == POLICY_FIFO) ? "POLICY_FIFO" : ((POLICY) == POLICY_RR) ? "POLICY_RR" : "<<UNKNOWN>>")

#define pthread_policy_to_name(POLICY) ( \
    ((POLICY) == SCHED_OTHER) ? "SCHED_OTHER" : ((POLICY) == SCHED_FIFO) ? "SCHED_FIFO" : ((POLICY) == SCHED_RR) ? "SCHED_RR" : "<<UNKNOWN>>")
//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	SetThreadName
	@abstract	Sets the name of the current thread.
*/
#if (TARGET_OS_WINDOWS)
OSStatus SetThreadName(const char* inName);
#else
#define SetThreadName(NAME) pthread_setname_np_compat(NAME)
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	GetThreadPriority / SetCurrentThreadPriority
	@abstract	Gets/sets the priority of the current thread.
*/
#define kThreadPriority_TimeConstraint INT_MAX

OSStatus SetCurrentThreadPriority(int inPriority);

#ifdef __cplusplus
}
#endif

#endif // __ThreadUtils_h__
