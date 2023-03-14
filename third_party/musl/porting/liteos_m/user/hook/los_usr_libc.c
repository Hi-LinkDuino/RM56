/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "usr_lib_ops.h"
#include "pthread.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

const static UsrLibcOps *g_usrLibc = NULL;

int pthread_attr_init(pthread_attr_t *attr)
{
    return g_usrLibc->pthread_attr_init(attr);
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    return g_usrLibc->pthread_attr_destroy(attr);
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachState)
{
    return g_usrLibc->pthread_attr_setdetachstate(attr, detachState);
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachState)
{
    return g_usrLibc->pthread_attr_getdetachstate(attr, detachState);
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    return g_usrLibc->pthread_attr_setscope(attr, scope);
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *scope)
{
    return g_usrLibc->pthread_attr_getscope(attr, scope);
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit)
{
    return g_usrLibc->pthread_attr_setinheritsched(attr, inherit);
}

int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit)
{
    return g_usrLibc->pthread_attr_getinheritsched(attr, inherit);
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    return g_usrLibc->pthread_attr_setschedpolicy(attr, policy);
}

int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
    return g_usrLibc->pthread_attr_getschedpolicy(attr, policy);
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    return g_usrLibc->pthread_attr_setschedparam(attr, param);
}

int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
    return g_usrLibc->pthread_attr_getschedparam(attr, param);
}

int pthread_attr_setstack(pthread_attr_t *a, void *addr, size_t size)
{
    return g_usrLibc->pthread_attr_setstack(a, addr, size);
}

int pthread_attr_getstack(const pthread_attr_t *restrict a, void **restrict addr, size_t *restrict size)
{
    return g_usrLibc->pthread_attr_getstack(a, addr, size);
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stackSize)
{
    return g_usrLibc->pthread_attr_setstacksize(attr, stackSize);
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stackSize)
{
    return g_usrLibc->pthread_attr_getstacksize(attr, stackSize);
}

int sched_get_priority_min(int policy)
{
    return g_usrLibc->sched_get_priority_min(policy);
}

int sched_get_priority_max(int policy)
{
    return g_usrLibc->sched_get_priority_max(policy);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*startRoutine)(void *), void *arg)
{
    return g_usrLibc->pthread_create(thread, attr, startRoutine, arg);
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    return g_usrLibc->pthread_setschedparam(thread, policy, param);
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    return g_usrLibc->pthread_getschedparam(thread, policy, param);
}

pthread_t pthread_self(void)
{
    return g_usrLibc->pthread_self();
}

int pthread_cancel(pthread_t thread)
{
    return g_usrLibc->pthread_cancel(thread);
}

int pthread_join(pthread_t thread, void **retval)
{
    return g_usrLibc->pthread_join(thread, retval);
}

int pthread_detach(pthread_t thread)
{
    return g_usrLibc->pthread_detach(thread);
}

void pthread_exit(void *retVal)
{
    g_usrLibc->pthread_exit(retVal);
}

int pthread_setname_np(pthread_t thread, const char *name)
{
    return g_usrLibc->pthread_setname_np(thread, name);
}

int pthread_getname_np(pthread_t thread, char *buf, size_t buflen)
{
    return g_usrLibc->pthread_getname_np(thread, buf, buflen);
}

int pthread_mutexattr_init(pthread_mutexattr_t *mutexAttr)
{
    return g_usrLibc->pthread_mutexattr_init(mutexAttr);
}

int pthread_mutexattr_settype(pthread_mutexattr_t *mutexAttr, int type)
{
    return g_usrLibc->pthread_mutexattr_settype(mutexAttr, type);
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *mutexAttr)
{
    return g_usrLibc->pthread_mutexattr_destroy(mutexAttr);
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexAttr)
{
    return g_usrLibc->pthread_mutex_init(mutex, mutexAttr);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    return g_usrLibc->pthread_mutex_destroy(mutex);
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *absTimeout)
{
    return g_usrLibc->pthread_mutex_timedlock(mutex, absTimeout);
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    return g_usrLibc->pthread_mutex_lock(mutex);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    return g_usrLibc->pthread_mutex_trylock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    return g_usrLibc->pthread_mutex_unlock(mutex);
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    return g_usrLibc->pthread_condattr_init(attr);
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    return g_usrLibc->pthread_condattr_destroy(attr);
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clk)
{
    return g_usrLibc->pthread_condattr_setclock(attr, clk);
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    return g_usrLibc->pthread_cond_init(cond, attr);
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    return g_usrLibc->pthread_cond_destroy(cond);
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    return g_usrLibc->pthread_cond_broadcast(cond);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    return g_usrLibc->pthread_cond_signal(cond);
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *ts)
{
    return g_usrLibc->pthread_cond_timedwait(cond, mutex, ts);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    return g_usrLibc->pthread_cond_wait(cond, mutex);
}

int sem_init(sem_t *sem, int shared, unsigned int value)
{
    return g_usrLibc->sem_init(sem, shared, value);
}

int sem_destroy(sem_t *sem)
{
    return g_usrLibc->sem_destroy(sem);
}

int sem_wait(sem_t *sem)
{
    return g_usrLibc->sem_wait(sem);
}

int sem_post(sem_t *sem)
{
    return g_usrLibc->sem_post(sem);
}

int sem_timedwait(sem_t *sem, const struct timespec *timeout)
{
    return g_usrLibc->sem_timedwait(sem, timeout);
}

int sem_getvalue(sem_t *sem, int *currVal)
{
    return g_usrLibc->sem_getvalue(sem, currVal);
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    return g_usrLibc->nanosleep(rqtp, rmtp);
}

int timer_create(clockid_t clockID, struct sigevent *restrict evp, timer_t *restrict timerID)
{
    return g_usrLibc->timer_create(clockID, evp, timerID);
}

int timer_delete(timer_t timerID)
{
    return g_usrLibc->timer_delete(timerID);
}

int timer_settime(timer_t timerID, int flags, const struct itimerspec *restrict value, struct itimerspec *restrict oldValue)
{
    return g_usrLibc->timer_settime(timerID, flags, value, oldValue);
}

int timer_gettime(timer_t timerID, struct itimerspec *value)
{
    return g_usrLibc->timer_gettime(timerID, value);
}

int timer_getoverrun(timer_t timerID)
{
    return g_usrLibc->timer_getoverrun(timerID);
}

int clock_settime(clockid_t clockID, const struct timespec *tp)
{
    return g_usrLibc->clock_settime(clockID, tp);
}

int clock_gettime(clockid_t clockID, struct timespec *tp)
{
    return g_usrLibc->clock_gettime(clockID, tp);
}

int clock_getres(clockid_t clockID, struct timespec *tp)
{
    return g_usrLibc->clock_getres(clockID, tp);
}

int clock_nanosleep(clockid_t clk, int flags, const struct timespec *req, struct timespec *rem)
{
    return g_usrLibc->clock_nanosleep(clk, flags, req, rem);
}

clock_t clock(void)
{
    return g_usrLibc->clock();
}

time_t time(time_t *timer)
{
    return g_usrLibc->time(timer);
}

struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
    return g_usrLibc->gmtime_r(timep, result);
}

struct tm *gmtime(const time_t *timer)
{
    return gmtime_r(timer, NULL);
}

struct tm *localtime_r(const time_t *timep, struct tm *result)
{
    return g_usrLibc->localtime_r(timep, result);
}

struct tm *localtime(const time_t *timer)
{
    return localtime_r(timer, NULL);
}

time_t mktime(struct tm *tmptr)
{
    return g_usrLibc->mktime(tmptr);
}

int gettimeofday(struct timeval *tv, void *ptz)
{
    return g_usrLibc->gettimeofday(tv, ptz);
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    return g_usrLibc->settimeofday(tv, tz);
}

#define OS_SYS_NS_PER_US       1000
#define OS_SYS_NS_PER_SECOND   1000000000
int usleep(unsigned useconds)
{
    struct timespec specTime = { 0 };
    UINT64 nanoseconds = (UINT64)useconds * OS_SYS_NS_PER_US;

    specTime.tv_sec = (time_t)(nanoseconds / OS_SYS_NS_PER_SECOND);
    specTime.tv_nsec = (long)(nanoseconds % OS_SYS_NS_PER_SECOND);
    return nanosleep(&specTime, NULL);
}

unsigned sleep(unsigned seconds)
{
    struct timespec specTime = { 0 };
    UINT64 nanoseconds = (UINT64)seconds * OS_SYS_NS_PER_SECOND;

    specTime.tv_sec = (time_t)(nanoseconds / OS_SYS_NS_PER_SECOND);
    specTime.tv_nsec = (long)(nanoseconds % OS_SYS_NS_PER_SECOND);
    return nanosleep(&specTime, NULL);
}

void free(void *ptr)
{
    g_usrLibc->free(ptr);
}

void *malloc(size_t size)
{
    g_usrLibc->malloc(size);
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    return g_usrLibc->memcpy(dest, src, n);
}

int memcmp(const void *vl, const void *vr, size_t n)
{
    return g_usrLibc->memcmp(vl, vr, n);
}

void *memset(void *dest, int c, size_t n)
{
    return g_usrLibc->memset(dest, c, n);
}

size_t strlen(const char *s)
{
    return g_usrLibc->strlen(s);
}
