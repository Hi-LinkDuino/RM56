/*
	Copyright (C) 2008-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __DispatchLite_h__
#define __DispatchLite_h__

#include "CommonServices.h"

#if 0
#pragma mark == Configuration ==
#endif

// DISPATCH_LITE_CF_ENABLED -- Controls whether to include Core Foundation support or not.

#if (!defined(DISPATCH_LITE_CF_ENABLED))
#if (DISPATCH_LITE_ENABLED)
#if (TARGET_OS_WINDOWS) // Not needed on Windows since it goes directly to the WinProc.
#define DISPATCH_LITE_CF_ENABLED 0
#else
#define DISPATCH_LITE_CF_ENABLED 1
#endif
#else
#define DISPATCH_LITE_CF_ENABLED 0
#endif
#endif

// DISPATCH_LITE_USE_SELECT -- Controls whether I/O stuff uses select() or another platform-specific API (e.g. kqueue).

#if (!defined(DISPATCH_LITE_USE_SELECT))
#if (TARGET_OS_BSD || TARGET_OS_WINDOWS)
#define DISPATCH_LITE_USE_SELECT 0
#else
#define DISPATCH_LITE_USE_SELECT 1
#endif
#endif

// DISPATCH_LITE_USE_KQUEUE -- Controls whether I/O stuff uses kqueue or API (e.g. select() or Windows).

#if (!defined(DISPATCH_LITE_USE_KQUEUE))
#if (!DISPATCH_LITE_USE_SELECT && TARGET_OS_BSD && !TARGET_OS_WINDOWS)
#define DISPATCH_LITE_USE_KQUEUE 1
#else
#define DISPATCH_LITE_USE_KQUEUE 0
#endif
#endif

#if (DISPATCH_LITE_ENABLED)

#if (TARGET_OS_POSIX)
#include <sys/time.h>
#endif

#if (!defined(CLOCK_MONOTONIC_SUPPORTED))
#if (TARGET_OS_LINUX)
#define CLOCK_MONOTONIC_SUPPORTED 1
#else
#define CLOCK_MONOTONIC_SUPPORTED 0
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#pragma mark == Base ==
#endif

//===========================================================================================================================
//	Base
//===========================================================================================================================

OSStatus LibDispatch_EnsureInitialized(void);
void LibDispatch_Finalize(void);
void LibDispatch_SetThreadPriority(int inPriority);

typedef void* dispatch_object_t;
typedef void (*dispatch_function_t)(void* inParam);

void dispatch_retain(dispatch_object_t inObj);
void dispatch_release(dispatch_object_t inObj);
void* dispatch_get_context(dispatch_object_t inObj);
void dispatch_set_context(dispatch_object_t inObj, void* inContext);
void dispatch_set_finalizer_f(dispatch_object_t inObj, dispatch_function_t inFinalizer);
void dispatch_suspend(dispatch_object_t inObj);
void dispatch_resume(dispatch_object_t inObj);

#if 0
#pragma mark == Queues ==
#endif

//===========================================================================================================================
//	Queues
//===========================================================================================================================

#define DISPATCH_CURRENT_QUEUE_LABEL NULL

#define DISPATCH_QUEUE_PRIORITY_HIGH 1
#define DISPATCH_QUEUE_PRIORITY_DEFAULT 0
#define DISPATCH_QUEUE_PRIORITY_LOW -1

typedef struct dispatch_queue_s* dispatch_queue_t;
typedef struct dispatch_queue_attr_s* dispatch_queue_attr_t;

void dispatch_async_f(dispatch_queue_t inQueue, void* inContext, dispatch_function_t inFunction);
void dispatch_sync_f(dispatch_queue_t inQueue, void* inContext, dispatch_function_t inFunction);

dispatch_queue_t dispatch_queue_create(const char* inLabel, dispatch_queue_attr_t inAttr);
const char* dispatch_queue_get_label(dispatch_queue_t inQueue);
void dispatch_set_target_queue(dispatch_object_t inObj, dispatch_queue_t inQueue);

dispatch_queue_attr_t dispatch_queue_attr_create(void);
void dispatch_queue_attr_release(dispatch_queue_attr_t inAttr);
void dispatch_queue_attr_set_priority(dispatch_queue_attr_t inAttr, int inPriority);

dispatch_queue_t dispatch_get_current_queue(void);
dispatch_queue_t dispatch_get_global_queue(long inPriority, unsigned long inFlags);

dispatch_queue_t dispatch_get_main_queue(void);
void dispatch_main(void);
void dispatch_main_drain_np(void);
extern dispatch_function_t gDispatchMainQueueScheduleHookFunc;

#if 0
#pragma mark == Time ==
#endif

//===========================================================================================================================
//	Time
//===========================================================================================================================

#ifdef NSEC_PER_SEC
#undef NSEC_PER_SEC
#endif
#define NSEC_PER_SEC UINT64_C(1000000000)

#ifdef USEC_PER_SEC
#undef USEC_PER_SEC
#endif
#define USEC_PER_SEC UINT64_C(1000000)

#ifdef NSEC_PER_USEC
#undef NSEC_PER_USEC
#endif
#define NSEC_PER_USEC UINT64_C(1000)

typedef uint64_t dispatch_time_t;

#define DISPATCH_TIME_NOW 0
#define DISPATCH_TIME_FOREVER (~UINT64_C(0))

dispatch_time_t dispatch_time(dispatch_time_t inWhen, int64_t inDelta);
dispatch_time_t dispatch_walltime(const struct timespec* ioWhen, int64_t inDelta);

#if 0
#pragma mark == Sources ==
#endif

//===========================================================================================================================
//	Sources
//===========================================================================================================================

typedef struct dispatch_source_s* dispatch_source_t;
typedef int dispatch_source_type_t;

#define DISPATCH_SOURCE_TYPE_READ 0
#define DISPATCH_SOURCE_TYPE_SIGNAL 1
#define DISPATCH_SOURCE_TYPE_TIMER 2
#define DISPATCH_SOURCE_TYPE_WRITE 3

dispatch_source_t
dispatch_source_create(
    dispatch_source_type_t inType,
    uintptr_t inHandle,
    unsigned long inMask,
    dispatch_queue_t inQueue);

void dispatch_source_set_event_handler_f(dispatch_source_t inSource, dispatch_function_t inHandler);
void dispatch_source_set_cancel_handler_f(dispatch_source_t inSource, dispatch_function_t inCancelHandler);
void dispatch_source_cancel(dispatch_source_t inSource);
long dispatch_source_testcancel(dispatch_source_t source);

uintptr_t dispatch_source_get_handle(dispatch_source_t inSource);
unsigned long dispatch_source_get_mask(dispatch_source_t inSource);
unsigned long dispatch_source_get_data(dispatch_source_t inSource);

void dispatch_source_set_timer(
    dispatch_source_t inSource,
    dispatch_time_t inStart,
    uint64_t inInterval,
    uint64_t inLeeway);

void dispatch_after_f(dispatch_time_t inWhen, dispatch_queue_t inQueue, void* inContext, dispatch_function_t inFunction);

#if 0
#pragma mark == Groups ==
#endif

//===========================================================================================================================
//	Groups
//===========================================================================================================================

typedef struct dispatch_group_s* dispatch_group_t;

dispatch_group_t dispatch_group_create(void);

void dispatch_group_async_f(
    dispatch_group_t inGroup,
    dispatch_queue_t inQueue,
    void* inContext,
    dispatch_function_t inFunction);

void dispatch_group_wait(dispatch_group_t inGroup, uint64_t inTimeout);

void dispatch_group_notify_f(
    dispatch_group_t inGroup,
    dispatch_queue_t inQueue,
    void* inContext,
    dispatch_function_t inFunction);

void dispatch_group_enter(dispatch_group_t inGroup);
void dispatch_group_leave(dispatch_group_t inGroup);

#if 0
#pragma mark == Once ==
#endif

//===========================================================================================================================
//	Once
//===========================================================================================================================

typedef int32_t dispatch_once_t;

void dispatch_once_f_slow(dispatch_once_t* inOnce, void* inContext, dispatch_function_t inFunction);

#define dispatch_once_f(ONCE, CONTEXT, FUNCTION)                 \
    do {                                                         \
        if (__builtin_expect(*(ONCE), 2) != 2) {                 \
            dispatch_once_f_slow((ONCE), (CONTEXT), (FUNCTION)); \
        }                                                        \
                                                                 \
    } while (0)

#if 0
#pragma mark == Sempahores ==
#endif

//===========================================================================================================================
//	Sempahores
//===========================================================================================================================

typedef struct dispatch_semaphore_s* dispatch_semaphore_t;

dispatch_semaphore_t dispatch_semaphore_create(long inValue);
long dispatch_semaphore_wait(dispatch_semaphore_t inSem, dispatch_time_t inTimeout);
long dispatch_semaphore_signal(dispatch_semaphore_t inSem);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DispatchLite_Test
	@abstract	Unit test.
*/

#if (!EXCLUDE_UNIT_TESTS)
OSStatus DispatchLite_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // DISPATCH_LITE_ENABLED

#endif // __DispatchLite_h__
