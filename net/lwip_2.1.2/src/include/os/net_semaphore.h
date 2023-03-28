/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Distributed under the terms of the GNU GPL, version 2
 *
 * Please see kernel/semaphore.c for documentation of these functions
 */
#ifndef __LINUX_SEMAPHORE_H
#define __LINUX_SEMAPHORE_H
#include "net_defs.h"
#include "cmsis_os.h"
#include "debug.h"
#include "net_debug.h"
/* Please don't access any members of this structure directly */
struct semaphore {
    osSemaphoreId _osSemaphoreId;
    osSemaphoreDef_t _osSemaphoreDef;
#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
    uint32_t _semaphore_data[2];
#endif
};

static inline void sema_init(struct semaphore *sem, int val)
{
    if (sem->_osSemaphoreId != NULL)
        osSemaphoreDelete(sem->_osSemaphoreId);
    memset(&sem->_osSemaphoreDef, 0, sizeof(sem->_osSemaphoreDef));

#ifdef CMSIS_OS_RTX
    memset(sem, 0, sizeof(struct semaphore));
#if (osCMSIS < 0x20000U)
    sem->_osSemaphoreDef.semaphore = sem->_semaphore_data;
#else
    memset(&sem->_osSemaphoreDef, 0, sizeof(osSemaphoreDef_t));
#endif
#endif
    sem->_osSemaphoreId = osSemaphoreCreate(&sem->_osSemaphoreDef, val);
    NET_ASSERT(sem->_osSemaphoreId, "seme_init fail\n");
}
static inline void sema_deinit(struct semaphore *sem)
{
    osSemaphoreDelete(sem->_osSemaphoreId);
    sem->_osSemaphoreId = NULL;
}

extern void down(struct semaphore *sem);
extern int __must_check down_trylock(struct semaphore *sem);
extern void up(struct semaphore *sem);

#endif /* __LINUX_SEMAPHORE_H */
