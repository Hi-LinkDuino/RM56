/*
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

#include "osal_irq.h"
#include "los_hwi.h"
#include "hdf_log.h"

#define OSAL_LOG_TAG osal_irq

int32_t OsalRegisterIrq(uint32_t irqId, uint32_t config, OsalIRQHandle handle, const char *name, void *dev)
{
    uint32_t ret;
    HwiIrqParam irqParam;

    if (irqId >= OS_HWI_MAX_NUM) {
        HDF_LOGE("invalid irq number %d\n", irqId);
        return HDF_ERR_INVALID_PARAM;
    }

    irqParam.swIrq = (INT32)irqId;
    irqParam.pDevId = dev;
    irqParam.pName = name;
    ret = LOS_HwiCreate(irqId, 0, (HWI_MODE_T)config, (HWI_PROC_FUNC)handle, &irqParam);
    if (ret != LOS_OK) {
        HDF_LOGE("%s %u register fail 0x%x", __func__, irqId, ret);
        return HDF_FAILURE;
    }
    HalIrqUnmask(irqId);

    return HDF_SUCCESS;
}

int32_t OsalUnregisterIrq(uint32_t irqId, void *dev)
{
    uint32_t ret;
    HwiIrqParam irqParam;

    if (irqId >= OS_HWI_MAX_NUM) {
        HDF_LOGE("invalid irq number %u\n", irqId);
        return HDF_ERR_INVALID_PARAM;
    }

    irqParam.swIrq = (INT32)irqId;
    irqParam.pDevId = dev;
    ret = LOS_HwiDelete(irqId, &irqParam);
    if (ret != LOS_OK) {
        HDF_LOGE("irq %u unregister fail %u\n", irqId, ret);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t OsalEnableIrq(uint32_t irqId)
{
    if (irqId >= OS_HWI_MAX_NUM) {
        HDF_LOGE("invalid irq number %u\n", irqId);
        return HDF_ERR_INVALID_PARAM;
    }

    HalIrqUnmask(irqId);

    return HDF_SUCCESS;
}

int32_t OsalDisableIrq(uint32_t irqId)
{
    if (irqId >= OS_HWI_MAX_NUM) {
        HDF_LOGE("invalid irq number %u\n", irqId);
        return HDF_ERR_INVALID_PARAM;
    }

    HalIrqMask(irqId);

    return HDF_SUCCESS;
}

