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

#ifndef __USR_LIBC_OPS_H
#define __USR_LIBC_OPS_H

#define _GNU_SOURCE
#include "pthread.h"
#include "string.h"
#include "semaphore.h"

typedef struct {
    int (*pthread_attr_init)(pthread_attr_t *attr);
    int (*pthread_attr_destroy)(pthread_attr_t *attr);
    int (*pthread_attr_setdetachstate)(pthread_attr_t *attr, int detachState);
    int (*pthread_attr_getdetachstate)(const pthread_attr_t *attr, int *detachState);
    int (*pthread_attr_setscope)(pthread_attr_t *attr, int scope);
    int (*pthread_attr_getscope)(const pthread_attr_t *attr, int *scope);
    int (*pthread_attr_setinheritsched)(pthread_attr_t *attr, int inherit);
    int (*pthread_attr_getinheritsched)(const pthread_attr_t *attr, int *inherit);
    int (*pthread_attr_setschedpolicy)(pthread_attr_t *attr, int policy);
    int (*pthread_attr_getschedpolicy)(const pthread_attr_t *attr, int *policy);
    int (*pthread_attr_setschedparam)(pthread_attr_t *attr, const struct sched_param *param);
    int (*pthread_attr_getschedparam)(const pthread_attr_t *attr, struct sched_param *param);
    int (*pthread_attr_setstack)(pthread_attr_t *a, void *addr, size_t size);
    int (*pthread_attr_getstack)(const pthread_attr_t *restrict a, void **restrict addr, size_t *restrict size);
    int (*pthread_attr_setstacksize)(pthread_attr_t *attr, size_t stackSize);
    int (*pthread_attr_getstacksize)(const pthread_attr_t *attr, size_t *stackSize);
    int (*sched_get_priority_min)(int policy);
    int (*sched_get_priority_max)(int policy);
    int (*pthread_create)(pthread_t *thread, const pthread_attr_t *attr, void *(*startRoutine)(void *), void *arg);
    int (*pthread_setschedparam)(pthread_t thread, int policy, const struct sched_param *param);
    int (*pthread_getschedparam)(pthread_t thread, int *policy, struct sched_param *param);
    pthread_t (*pthread_self)(void);
    int (*pthread_cancel)(pthread_t thread);
    int (*pthread_join)(pthread_t thread, void **retval);
    int (*pthread_detach)(pthread_t thread);
    void (*pthread_exit)(void *retVal);
    int (*pthread_setname_np)(pthread_t thread, const char *name);
    int (*pthread_getname_np)(pthread_t thread, char *buf, size_t buflen);
    int (*pthread_mutexattr_init)(pthread_mutexattr_t *mutexAttr);
    int (*pthread_mutexattr_settype)(pthread_mutexattr_t *mutexAttr, int type);
    int (*pthread_mutexattr_destroy)(pthread_mutexattr_t *mutexAttr);
    int (*pthread_mutex_init)(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexAttr);
    int (*pthread_mutex_destroy)(pthread_mutex_t *mutex);
    int (*pthread_mutex_timedlock)(pthread_mutex_t *mutex, const struct timespec *absTimeout);
    int (*pthread_mutex_lock)(pthread_mutex_t *mutex);
    int (*pthread_mutex_trylock)(pthread_mutex_t *mutex);
    int (*pthread_mutex_unlock)(pthread_mutex_t *mutex);
    int (*pthread_condattr_init)(pthread_condattr_t *attr);
    int (*pthread_condattr_destroy)(pthread_condattr_t *attr);
    int (*pthread_condattr_setclock)(pthread_condattr_t *attr, clockid_t clk);
    int (*pthread_cond_init)(pthread_cond_t *cond, const pthread_condattr_t *attr);
    int (*pthread_cond_destroy)(pthread_cond_t *cond);
    int (*pthread_cond_broadcast)(pthread_cond_t *cond);
    int (*pthread_cond_signal)(pthread_cond_t *cond);
    int (*pthread_cond_timedwait)(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *ts);
    int (*pthread_cond_wait)(pthread_cond_t *cond, pthread_mutex_t *mutex);
    int (*sem_init)(sem_t *sem, int shared, unsigned int value);
    int (*sem_destroy)(sem_t *sem);
    int (*sem_wait)(sem_t *sem);
    int (*sem_post)(sem_t *sem);
    int (*sem_timedwait)(sem_t *sem, const struct timespec *timeout);
    int (*sem_getvalue)(sem_t *sem, int *currVal);
    int (*nanosleep)(const struct timespec *rqtp, struct timespec *rmtp);
    int (*timer_create)(clockid_t clockID, struct sigevent *restrict evp, timer_t *restrict timerID);
    int (*timer_delete)(timer_t timerID);
    int (*timer_settime)(timer_t timerID, int flags, const struct itimerspec *restrict value, struct itimerspec *restrict oldValue);
    int (*timer_gettime)(timer_t timerID, struct itimerspec *value);
    int (*timer_getoverrun)(timer_t timerID);
    int (*clock_settime)(clockid_t clockID, const struct timespec *tp);
    int (*clock_gettime)(clockid_t clockID, struct timespec *tp);
    int (*clock_getres)(clockid_t clockID, struct timespec *tp);
    int (*clock_nanosleep)(clockid_t clk, int flags, const struct timespec *req, struct timespec *rem);
    clock_t (*clock)(void);
    time_t (*time)(time_t *timer);
    struct tm *(*gmtime_r)(const time_t *timep, struct tm *result);
    struct tm *(*localtime_r)(const time_t *timep, struct tm *result);
    time_t (*mktime)(struct tm *tmptr);
    int (*gettimeofday)(struct timeval *tv, void *ptz);
    int (*settimeofday)(const struct timeval *tv, const struct timezone *tz);
    void (*free)(void *ptr);
    void *(*malloc)(size_t size);
    void *(*memcpy)(void *restrict dest, const void *restrict src, size_t n);
    int (*memcmp)(const void *vl, const void *vr, size_t n);
    void *(*memset)(void *dest, int c, size_t n);
    size_t (*strlen)(const char *s);
    int (*strcmp)(const char *l, const char *r);
} UsrLibcOps;

#endif
