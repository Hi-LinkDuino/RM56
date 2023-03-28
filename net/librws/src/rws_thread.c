/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "../librws.h"
#include "rws_thread.h"
#include "rws_memory.h"
#include "rws_common.h"

#include <assert.h>

#if defined(RWS_OS_WINDOWS)
#include <windows.h>
#elif defined(MBED_BUILD)
//bes mbed rtos
#include "rtos_redefine.h"
#include "cmsis_os.h"
//#include "rtos.h"
#elif defined(RWS_OS_CMSIS)
#include "cmsis_os.h"
#else
#include <posix/pthread.h>
#include <unistd.h>
#endif
#include "hal_trace.h"

typedef struct _rws_thread_struct
{
	rws_thread_funct thread_function;
	void * user_object;
#if defined(RWS_OS_WINDOWS)
	HANDLE thread;
#elif defined(MBED_BUILD)
	struct cg_thread *thread;
#elif defined(RWS_OS_CMSIS)
    osThreadId    thread;
    osThreadDef_t thread_def;
#else
	pthread_t thread;
#endif
} _rws_thread;

typedef struct _rws_threads_joiner_struct
{
	_rws_thread * thread;
	rws_mutex mutex;
} _rws_threads_joiner;

static _rws_threads_joiner * _threads_joiner = NULL;
static void rws_threads_joiner_clean(void) // private
{
	_rws_thread * t = _threads_joiner->thread;
#if defined(RWS_OS_WINDOWS)
	DWORD dwExitCode = 0;
#endif

	if (!t) return;
	_threads_joiner->thread = NULL;

#if defined(RWS_OS_WINDOWS)
	do {
		if (GetExitCodeThread(t->thread, &dwExitCode) == 0) break; // fail
	} while (dwExitCode == STILL_ACTIVE);
	if (dwExitCode == STILL_ACTIVE) TerminateThread(t->thread, 0);
	if (CloseHandle(t->thread)) t->thread = NULL;
#elif defined(RWS_OS_CMSIS)
	//osThreadTerminate(t->thread);
#else
	void * r = NULL;
	pthread_join(t->thread, &r);
	assert(r == NULL);
#endif
	rws_free(t);
	t = NULL;
}

static void rws_threads_joiner_add(_rws_thread * thread) // public
{
	rws_mutex_lock(_threads_joiner->mutex);
	rws_threads_joiner_clean();
	_threads_joiner->thread = thread;
	rws_mutex_unlock(_threads_joiner->mutex);
}

static void rws_threads_joiner_create_ifneed(void)
{
	if (_threads_joiner) return;
	_threads_joiner = (_rws_threads_joiner *)rws_malloc_zero(sizeof(_rws_threads_joiner));
	_threads_joiner->mutex = rws_mutex_create_recursive();
}

#if defined(RWS_OS_WINDOWS)
static DWORD WINAPI rws_thread_func_priv(LPVOID some_pointer)
#elif defined(MBED_BUILD)
static void rws_thread_func_priv(void *some_pointer)
#elif defined(RWS_OS_CMSIS)
static void rws_thread_func_priv(void *some_pointer)
#else
static void * rws_thread_func_priv(void * some_pointer)
#endif
{
	_rws_thread * t = (_rws_thread *)some_pointer;
	t->thread_function(t->user_object);
	rws_threads_joiner_add(t);

#if  defined(RWS_OS_WINDOWS)
	return NULL;
#elif defined(MBED_BUILD)
	return NULL;
#elif defined(RWS_OS_CMSIS)
    osThreadExit();
	return NULL;
#else
	return NULL;
#endif
}

rws_thread rws_thread_create(rws_thread_funct thread_function, void * user_object)
{
	_rws_thread * t = NULL;
#if defined(RWS_OS_WINDOWS)
#elif defined(MBED_BUILD)
#elif defined(RWS_OS_CMSIS)
#else
	pthread_attr_t attr;
#endif

	if (!thread_function) return NULL;
	rws_threads_joiner_create_ifneed();
	t = (_rws_thread *)rws_malloc_zero(sizeof(_rws_thread));
	t->user_object = user_object;
	t->thread_function = thread_function;
#if defined(RWS_OS_WINDOWS)
	t->thread = CreateThread(NULL, 0, &rws_thread_func_priv, (LPVOID)t, 0, NULL);
	assert(t->thread);
#elif defined(MBED_BUILD)
	int res = -1;
	res = mbed_pthread_create(&t->thread, osPriorityAboveNormal, RWS_STACK_SIZE, "rws_thread", rws_thread_func_priv, (void *)t);
	assert(res == 0);
#elif defined(RWS_OS_CMSIS)
    t->thread_def.pthread = (os_pthread)rws_thread_func_priv;
    t->thread_def.attr.name = "librws";
    t->thread_def.attr.stack_size = 0x4000;
    t->thread_def.attr.priority = osPriorityAboveNormal;
    t->thread = osThreadCreate(&t->thread_def, t);
    TRACE_IMM(1,"thread %p: %s stack 0x%X\n", t->thread, t->thread_def.attr.name, t->thread_def.attr.stack_size);
    assert(t->thread != NULL);
#else
	int res = -1;
	if (pthread_attr_init(&attr) == 0)
	{
		if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0)
		{
			if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0)
			{
				pthread_attr_setstacksize(&attr, 0x4000);
				res = pthread_create(&t->thread, &attr, &rws_thread_func_priv, (void *)t);
			}
		}
		pthread_attr_destroy(&attr);
	}
	assert(res == 0);
#endif
	return t;
}

void rws_thread_sleep(const unsigned int millisec)
{
#if defined(RWS_OS_WINDOWS)
	Sleep(millisec); // 1s = 1'000 millisec.
#elif defined(MBED_BUILD)
	osDelay(millisec);
#elif defined(RWS_OS_CMSIS)
	osDelay(millisec);
#else
	usleep(millisec * 1000); // 1s = 1'000'000 microsec.
#endif
}

#ifdef RWS_OS_CMSIS
typedef struct {
    osMutexDef_t mutex_def;
    osMutexId mutex_id;
    uint32_t  os_mutex_cb[3];
}_rws_mutex_t;
#endif
rws_mutex rws_mutex_create_recursive(void)
{
#if defined(RWS_OS_WINDOWS)
	CRITICAL_SECTION * mutex = (CRITICAL_SECTION *)rws_malloc_zero(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)mutex);
	return mutex;
#elif defined(MBED_BUILD)
	struct cg_mutex *mutex = CreateMutex();
	return mutex;
#elif defined(RWS_OS_CMSIS)
	_rws_mutex_t *mtx = rws_malloc_zero(sizeof(_rws_mutex_t));
#ifdef KERNEL_RHINO
    mtx->mutex_def.mutex = mtx->os_mutex_cb;
#elif defined (KERNEL_RTX5)
#endif
    mtx->mutex_id = osMutexCreate(&mtx->mutex_def);
    return mtx;
#else
	pthread_mutex_t * mutex = (pthread_mutex_t *)rws_malloc_zero(sizeof(pthread_mutex_t));
	int res = -1;
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr) == 0)
	{
		if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0)
			res = pthread_mutex_init(mutex, &attr);
		pthread_mutexattr_destroy(&attr);
	}
	assert(res == 0);
	return mutex;
#endif
}

void rws_mutex_lock(rws_mutex mutex)
{
#if defined(RWS_OS_WINDOWS)
	if (mutex) TryEnterCriticalSection((LPCRITICAL_SECTION)mutex);
#elif defined(MBED_BUILD)
	if (mutex) mbed_pthread_mutex_lock((struct cg_mutex*)mutex);
#elif defined(RWS_OS_CMSIS)
	_rws_mutex_t *mtx = (_rws_mutex_t *)mutex;
	if (mutex) osMutexWait(mtx->mutex_id, osWaitForever);
#else
	if (mutex) pthread_mutex_lock((pthread_mutex_t *)mutex);
#endif
}

void rws_mutex_unlock(rws_mutex mutex)
{
#if defined(RWS_OS_WINDOWS)
	if (mutex) LeaveCriticalSection((LPCRITICAL_SECTION)mutex);
#elif defined(MBED_BUILD)
	if (mutex) mbed_pthread_mutex_unlock((struct cg_mutex*)mutex);
#elif defined(RWS_OS_CMSIS)
	_rws_mutex_t *mtx = (_rws_mutex_t *)mutex;
	if (mutex) osMutexRelease(mtx->mutex_id);
#else
	if (mutex) pthread_mutex_unlock((pthread_mutex_t *)mutex);
#endif
}

void rws_mutex_delete(rws_mutex mutex)
{
	if (mutex)
	{
#if defined(RWS_OS_WINDOWS)
		DeleteCriticalSection((LPCRITICAL_SECTION)mutex);
#elif defined(MBED_BUILD)
		DestroyMutex(((struct cg_mutex**)&mutex));
#elif defined(RWS_OS_CMSIS)
		_rws_mutex_t *mtx = (_rws_mutex_t *)mutex;
		osMutexDelete(mtx->mutex_id);
#else
		pthread_mutex_destroy((pthread_mutex_t *)mutex);
#endif
		rws_free(mutex);
	}
}

#include "hal_timer.h"
int rws_time_get_ms(void)
{
	return TICKS_TO_MS(hal_sys_timer_get());
}
