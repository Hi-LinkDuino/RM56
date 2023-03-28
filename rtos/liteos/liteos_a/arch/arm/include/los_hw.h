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

/**
 * @defgroup los_hw Hardware
 * @ingroup kernel
 */
#ifndef _LOS_HW_H
#define _LOS_HW_H

#include "los_typedef.h"
#include "los_hw_cpu.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define OS_SCHEDULE_IN_IRQ                                  0x0
#define OS_SCHEDULE_IN_TASK                                 0x1

#define PSR_T_ARM                                           0x00000000u
#define PSR_T_THUMB                                         0x00000020u
#define PSR_MODE_SVC                                        0x00000013u
#define PSR_MODE_SYS                                        0x0000001Fu
#define PSR_FIQ_DIS                                         0x00000040u
#define PSR_IRQ_DIS                                         0x00000080u
#define PSR_MODE_USR                                        0x00000010u

#define PSR_MODE_SVC_THUMB                                  (PSR_MODE_SVC | PSR_T_THUMB | PSR_FIQ_DIS | PSR_IRQ_DIS)
#define PSR_MODE_SVC_ARM                                    (PSR_MODE_SVC | PSR_T_ARM   | PSR_FIQ_DIS | PSR_IRQ_DIS)

#define PSR_MODE_SYS_THUMB                                  (PSR_MODE_SYS | PSR_T_THUMB)
#define PSR_MODE_SYS_ARM                                    (PSR_MODE_SYS | PSR_T_ARM)

#define PSR_MODE_USR_THUMB                                  (PSR_MODE_USR | PSR_T_THUMB)
#define PSR_MODE_USR_ARM                                    (PSR_MODE_USR | PSR_T_ARM)

#define LOS_CHECK_SCHEDULE                                  ((!OS_INT_ACTIVE) && OsPreemptable())

typedef struct {
    const UINT32 partNo;
    const CHAR *cpuName;
} CpuVendor;

extern CpuVendor g_cpuTable[];
extern UINT64 g_cpuMap[];

#define CPU_MAP_GET(cpuid)          g_cpuMap[(cpuid)]
#define CPU_MAP_SET(cpuid, hwid)    g_cpuMap[(cpuid)] = (hwid)

/**
 * @ingroup los_hw
 * @brief Invalidate instruction cache.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to invalidate the instruction cache.</li>
 * </ul>
 * @attention None.
 *
 * @param None.
 *
 * @retval #None.
 *
 * @par Dependency:
 * los_hw.h: the header file that contains the API declaration.
 * @see None.
 */
extern VOID FlushICache(VOID);

/**
 * @ingroup los_hw
 * @brief Flush data cache.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to flush the data cache to the memory.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input end address must be greater than the input start address.</li>
 * </ul>
 *
 * @param  start [IN] Type #int Flush start address.
 * @param  end [IN] Type #int Flush end address.
 *
 * @retval #None.
 *
 * @par Dependency:
 * los_hw.h: the header file that contains the API declaration.
 * @see None.
 */
extern VOID DCacheFlushRange(UINTPTR start, UINTPTR end);

/**
 * @ingroup los_hw
 * @brief Invalidate data cache.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to Invalidate the data in cache.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>The input end address must be greater than the input start address.</li>
 * </ul>
 *
 * @param  start [IN] Type #int Invalidate start address.
 * @param  end [IN] Type #int Invalidate end address .
 *
 * @retval #None.
 *
 * @par Dependency:
 * los_hw.h: the header file that contains the API declaration.
 * @see None.
 */
extern VOID DCacheInvRange(UINTPTR start, UINTPTR end);

/**
 * @ingroup los_hw
 * @brief Get cpu core name.
 *
 * @par Description:
 * <ul>
 * <li>This API is used to get cpu core name.</li>
 * </ul>
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param
 * @retval #CHAR *  cpu core name.
 *
 * @par Dependency:
 * los_hw.h: the header file that contains the API declaration.
 * @see None.
 */
STATIC INLINE const CHAR *LOS_CpuInfo(VOID)
{
    INT32 i;
    UINT32 midr = OsMainIDGet();
    /* [15:4] is the primary part number */
    UINT32 partNo = (midr & 0xFFF0) >> 0x4;

    for (i = 0; g_cpuTable[i].partNo != 0; i++) {
        if (partNo == g_cpuTable[i].partNo) {
            return g_cpuTable[i].cpuName;
        }
    }

    return "unknown";
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_HW_H */
