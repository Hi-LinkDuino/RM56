/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
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

#ifndef _LOS_SPINLOCK_H
#define _LOS_SPINLOCK_H
#include "los_typedef.h"
#include "los_config.h"
#include "los_hwi.h"
#include "los_task.h"
#include "los_lockdep.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

extern VOID ArchSpinLock(size_t *lock);
extern VOID ArchSpinUnlock(size_t *lock);
extern INT32 ArchSpinTrylock(size_t *lock);

typedef struct Spinlock {
    size_t      rawLock;
#ifdef LOSCFG_KERNEL_SMP
    UINT32      cpuid;
    VOID        *owner;
    const CHAR  *name;
#endif
} SPIN_LOCK_S;

#ifdef LOSCFG_KERNEL_SMP_LOCKDEP
#define LOCKDEP_CHECK_IN(lock)  OsLockDepCheckIn(lock)
#define LOCKDEP_RECORD(lock)    OsLockDepRecord(lock)
#define LOCKDEP_CHECK_OUT(lock) OsLockDepCheckOut(lock)
#define LOCKDEP_CLEAR_LOCKS()   OsLockdepClearSpinlocks()
#else
#define LOCKDEP_CHECK_IN(lock)
#define LOCKDEP_RECORD(lock)
#define LOCKDEP_CHECK_OUT(lock)
#define LOCKDEP_CLEAR_LOCKS()
#endif

#ifdef LOSCFG_KERNEL_SMP
#define SPINLOCK_OWNER_INIT     NULL

#define SPIN_LOCK_INITIALIZER(lockName) \
{                                       \
    .rawLock    = 0U,                   \
    .cpuid      = (UINT32)(-1),         \
    .owner      = SPINLOCK_OWNER_INIT,  \
    .name       = #lockName,            \
}

/**
 * @ingroup  los_spinlock
 * @brief Lock the spinlock.
 *
 * @par Description:
 * This API is used to lock the spin lock.
 *
 * @attention None.
 *
 * @param  lock [IN] Type #SPIN_LOCK_S spinlock pointer.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinUnlock
 */
extern VOID LOS_SpinLock(SPIN_LOCK_S *lock);

/**
 * @ingroup  los_spinlock
 * @brief Trying lock the spinlock.
 *
 * @par Description:
 * This API is used to trying lock the spin lock.
 *
 * @attention None.
 *
 * @param  lock [IN] Type #SPIN_LOCK_S spinlock pointer.
 *
 * @retval LOS_OK   Got the spinlock.
 * @retval LOS_NOK  Not getting the spinlock.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock
 */
extern INT32 LOS_SpinTrylock(SPIN_LOCK_S *lock);

/**
 * @ingroup  los_spinlock
 * @brief Unlock the spinlock.
 *
 * @par Description:
 * This API is used to unlock the spin lock.
 *
 * @attention None.
 *
 * @param  lock [IN] Type #SPIN_LOCK_S spinlock pointer.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock
 */
extern VOID LOS_SpinUnlock(SPIN_LOCK_S *lock);

/**
 * @ingroup  los_spinlock
 * @brief Lock the spinlock and disable all interrupts.
 *
 * @par Description:
 * This API is used to lock the spin lock and disable all interrupts.
 *
 * @attention None.
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S spinlock pointer.
 * @param  intSave  [OUT]   Type #UINT32 Saved interrupt flag for latter restored.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinLock
 */
extern VOID LOS_SpinLockSave(SPIN_LOCK_S *lock, UINT32 *intSave);

/**
 * @ingroup  los_spinlock
 * @brief Unlock the spinlock and restore interrupt flag.
 *
 * @par Description:
 * This API is used to unlock the spin lock and restore interrupt flag.
 *
 * @attention None.
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S spinlock pointer.
 * @param  intSave  [IN]    Type #UINT32 Interrupt flag to be restored.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_SpinUnlock
 */
extern VOID LOS_SpinUnlockRestore(SPIN_LOCK_S *lock, UINT32 intSave);

/**
 * @ingroup  los_spinlock
 * @brief Check if holding the spinlock.
 *
 * @par Description:
 * This API is used to check if holding the spinlock.
 *
 * @attention None.
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S spinlock pointer.
 *
 * @retval TRUE   Holding the spinlock.
 * @retval FALSE  Not Holding the spinlock.
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 */
extern BOOL LOS_SpinHeld(const SPIN_LOCK_S *lock);

/**
 * @ingroup  los_spinlock
 * @brief Spinlock initialization.
 *
 * @par Description:
 * This API is used to initialize a spinlock.
 *
 * @attention None.
 *
 * @param  lock     [IN]    Type #SPIN_LOCK_S spinlock pointer.
 *
 * @retval None.
 *
 * @par Dependency:
 * <ul><li>los_spinlock.h: the header file that contains the API declaration.</li></ul>
 */
extern VOID LOS_SpinInit(SPIN_LOCK_S *lock);

#else
#define SPIN_LOCK_INITIALIZER(lockName) \
{                                       \
    .rawLock    = 0U,                   \
}

/*
 * For Non-SMP system, these apis does not handle with spinlocks,
 * but for unifying the code of drivers, vendors and etc.
 */
LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinLock(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE INT32 LOS_SpinTrylock(SPIN_LOCK_S *lock)
{
    (VOID)lock;
    return LOS_OK;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlock(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinLockSave(SPIN_LOCK_S *lock, UINT32 *intSave)
{
    (VOID)lock;
    *intSave = LOS_IntLock();
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinUnlockRestore(SPIN_LOCK_S *lock, UINT32 intSave)
{
    (VOID)lock;
    LOS_IntRestore(intSave);
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE BOOL LOS_SpinHeld(const SPIN_LOCK_S *lock)
{
    (VOID)lock;
    return TRUE;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE VOID LOS_SpinInit(SPIN_LOCK_S *lock)
{
    (VOID)lock;
}

#endif

#define SPIN_LOCK_INIT(lock)  SPIN_LOCK_S lock = SPIN_LOCK_INITIALIZER(lock)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
