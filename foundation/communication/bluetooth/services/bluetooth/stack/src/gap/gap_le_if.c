/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gap_le.h"
#include "gap_def.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"

#include "btm/btm_thread.h"

typedef struct {
    int result;
    uint8_t role;
} GapLeSetRoleInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint16_t payloadTimeout;
} GapUseLePingInfo;

typedef struct {
    int result;
    uint8_t advHandle;
    uint8_t *addr;
} GapLeExAdvSetRandAddrInfo;

typedef struct {
    int result;
    uint8_t advHandle;
    uint8_t properties;
    int8_t txPower;
    GapLeExAdvParam advExParam;
} GapLeExAdvSetParamInfo;

typedef struct {
    int result;
    uint8_t advHandle;
    uint8_t operation;
    uint8_t fragmentPreference;
    uint8_t advDataLength;
    uint8_t *advData;
} GapLeExAdvSetDataInfo;

typedef struct {
    int result;
    uint8_t enable;
    uint8_t numberOfSet;
    GapExAdvSet *advSet;
} GapLeExAdvSetEnableInfo;

typedef struct {
    int result;
    uint8_t advHandle;
} GapLeExAdvRemoveHandleInfo;

typedef struct {
    int result;
    uint8_t advType;
    GapLeAdvParam advParam;
} GapLeAdvSetParamInfo;

typedef struct {
    int result;
    uint8_t advDataLength;
    uint8_t *advData;
} GapLeAdvSetDataInfo;

typedef struct {
    int result;
    uint8_t enable;
} GapLeAdvSetEnableInfo;

typedef struct {
    int result;
    GapLeScanParam param;
    uint8_t scanFilterPolity;
} GapLeScanSetParamInfo;

typedef struct {
    int result;
    uint8_t scanEnable;
    uint8_t filterDuplicates;
} GapLeScanSetEnableInfo;

typedef struct {
    int result;
    uint8_t scanFilterPolity;
    uint8_t scanPhys;
    GapLeScanParam *param;
} GapLeExScanSetParamInfo;

typedef struct {
    int result;
    uint8_t scanEnable;
    uint8_t filterDuplicates;
    uint16_t duration;
    uint16_t period;
} GapLeExScanSetEnableInfo;

typedef struct {
    int result;
    BtAddr *addr;
    const GapLeConnectionParameter *connParam;
} GapLeConnParamUpdateInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    const GapLeConnectionParameter *connParam;
} GapLeConnectionParameterRspInfo;

typedef struct {
    int result;
    uint64_t channelMap;
} GapLeSetHostChannelClassificationInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    LeEncKey encKey;
    uint8_t keyType;
} GapLeEncryptionKeyRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    GapSigningAlgorithmInfo info;
} GapRequestSigningAlgorithmInfoRspInfo;

typedef struct {
    int result;
    uint8_t mode;
} GapLeSetBondModeInfo;

typedef struct {
    int result;
    GAP_LeSecMode1Level mode1Level;
    GAP_LeSecMode2Level mode2Level;
} GapLeSetSecurityModeInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GAP_LeSecurityStatus *status;
    uint8_t *encKeySize;
} GapLeGetSecurityStatusInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GAP_LeSecurityStatus status;
    GapLeRequestSecurityResult callback;
    void *context;
} GapLeRequestSecurityInfo;

typedef struct {
    int result;
    uint8_t minSize;
} GapLeSetMinEncKeySizeInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GapLePairFeature localFrature;
} GapLePairFeatureRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint32_t number;
} GapLePairPassKeyRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint8_t *oobData;
} GapLePairOobRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint8_t *oobDataC;
    uint8_t *oobDataR;
} GapLePairScOobRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
} GapLePairScUserConfirmRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GapSignatureData dataInfo;
    GAPSignatureGenerationResult callback;
    void *context;
} GapLeDataSignatureGenerationInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GapSignatureData dataInfo;
    uint8_t signature[GAP_SIGNATURE_SIZE];
    GAPSignatureConfirmationResult callback;
    void *context;
} GapLeDataSignatureConfirmationInfo;

#ifdef GAP_LE_SUPPORT

static void GapLeSetRoleTask(void *ctx)
{
    GapLeSetRoleInfo *info = ctx;
    info->result = GAP_LeSetRole(info->role);
}

int GAPIF_LeSetRole(uint8_t role)
{
    LOG_INFO("%{public}s: role:0x%02x", __FUNCTION__, role);
    GapLeSetRoleInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeSetRoleInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeSetRoleInfo), 0x00, sizeof(GapLeSetRoleInfo));

    ctx->role = role;

    int ret = GapRunTaskBlockProcess(GapLeSetRoleTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeSetStaticIdentityAddrTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_LeSetStaticIdentityAddr(info->pointer);
}

int GAPIF_LeSetStaticIdentityAddr(uint8_t addr[BT_ADDRESS_SIZE])
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = addr;

    int ret = GapRunTaskBlockProcess(GapLeSetStaticIdentityAddrTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeGenResPriAddrTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_LeGenResPriAddrAsync(info->callback, info->context);
}

int GAPIF_LeGenResPriAddr(GenResPriAddrResult callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapLeGenResPriAddrTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvGetMaxDataLenTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_LeExAdvGetMaxDataLen(info->pointer);
}

int GAPIF_LeExAdvGetMaxDataLen(uint16_t *len)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = len;

    int ret = GapRunTaskBlockProcess(GapLeExAdvGetMaxDataLenTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvGetMaxHandleNumTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_LeExAdvGetMaxHandleNum(info->pointer);
}

int GAPIF_LeExAdvGetMaxHandleNum(uint8_t *num)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = num;

    int ret = GapRunTaskBlockProcess(GapLeExAdvGetMaxHandleNumTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterExAdvCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterExAdvCallback(info->callback, info->context);
}

int GAPIF_RegisterExAdvCallback(const GapExAdvCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterExAdvCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterExAdvCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterExAdvCallback();
}

int GAPIF_DeregisterExAdvCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterExAdvCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvSetRandAddrTask(void *ctx)
{
    GapLeExAdvSetRandAddrInfo *info = ctx;
    info->result = GAP_LeExAdvSetRandAddr(info->advHandle, info->addr);
}

int GAPIF_LeExAdvSetRandAddr(uint8_t advHandle, const uint8_t addr[BT_ADDRESS_SIZE])
{
    LOG_INFO("%{public}s: advHandle:%hhu " BT_ADDR_FMT, __FUNCTION__, advHandle, BT_ADDR_FMT_OUTPUT(addr));
    GapLeExAdvSetRandAddrInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExAdvSetRandAddrInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExAdvSetRandAddrInfo), 0x00, sizeof(GapLeExAdvSetRandAddrInfo));

    ctx->advHandle = advHandle;
    ctx->addr = (uint8_t *)addr;

    int ret = GapRunTaskBlockProcess(GapLeExAdvSetRandAddrTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvSetParamTask(void *ctx)
{
    GapLeExAdvSetParamInfo *info = ctx;
    info->result = GAP_LeExAdvSetParam(info->advHandle, info->properties, info->txPower, info->advExParam);
}

int GAPIF_LeExAdvSetParam(uint8_t advHandle, uint8_t properties, int8_t txPower, GapLeExAdvParam advExParam)
{
    LOG_INFO("%{public}s: advHandle:%hhu properties:0x%02x", __FUNCTION__, advHandle, properties);
    GapLeExAdvSetParamInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExAdvSetParamInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExAdvSetParamInfo), 0x00, sizeof(GapLeExAdvSetParamInfo));

    ctx->advHandle = advHandle;
    ctx->properties = properties;
    ctx->txPower = txPower;
    ctx->advExParam = advExParam;

    int ret = GapRunTaskBlockProcess(GapLeExAdvSetParamTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvSetDataTask(void *ctx)
{
    GapLeExAdvSetDataInfo *info = ctx;
    info->result = GAP_LeExAdvSetData(
        info->advHandle, info->operation, info->fragmentPreference, info->advDataLength, info->advData);
}

int GAPIF_LeExAdvSetData(
    uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference, uint8_t advDataLength, const uint8_t *advData)
{
    LOG_INFO("%{public}s: advHandle:%hhu", __FUNCTION__, advHandle);
    GapLeExAdvSetDataInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExAdvSetDataInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExAdvSetDataInfo), 0x00, sizeof(GapLeExAdvSetDataInfo));

    ctx->advHandle = advHandle;
    ctx->operation = operation;
    ctx->fragmentPreference = fragmentPreference;
    ctx->advDataLength = advDataLength;
    ctx->advData = (uint8_t *)advData;

    int ret = GapRunTaskBlockProcess(GapLeExAdvSetDataTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvSetScanRspDataTask(void *ctx)
{
    GapLeExAdvSetDataInfo *info = ctx;
    info->result = GAP_LeExAdvSetScanRspData(
        info->advHandle, info->operation, info->fragmentPreference, info->advDataLength, info->advData);
}

int GAPIF_LeExAdvSetScanRspData(uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference,
    uint8_t scanResponseDataLen, const uint8_t *scanResponseData)
{
    LOG_INFO("%{public}s: advHandle:%hhu", __FUNCTION__, advHandle);
    GapLeExAdvSetDataInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExAdvSetDataInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExAdvSetDataInfo), 0x00, sizeof(GapLeExAdvSetDataInfo));

    ctx->advHandle = advHandle;
    ctx->operation = operation;
    ctx->fragmentPreference = fragmentPreference;
    ctx->advDataLength = scanResponseDataLen;
    ctx->advData = (uint8_t *)scanResponseData;

    int ret = GapRunTaskBlockProcess(GapLeExAdvSetScanRspDataTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvSetEnableTask(void *ctx)
{
    GapLeExAdvSetEnableInfo *info = ctx;
    info->result = GAP_LeExAdvSetEnable(info->enable, info->numberOfSet, info->advSet);
}

int GAPIF_LeExAdvSetEnable(uint8_t enable, uint8_t numberOfSet, const GapExAdvSet *advSet)
{
    LOG_INFO("%{public}s: enable:%hhu", __FUNCTION__, enable);
    GapLeExAdvSetEnableInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExAdvSetEnableInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExAdvSetEnableInfo), 0x00, sizeof(GapLeExAdvSetEnableInfo));

    ctx->enable = enable;
    ctx->numberOfSet = numberOfSet;
    ctx->advSet = (GapExAdvSet *)advSet;

    int ret = GapRunTaskBlockProcess(GapLeExAdvSetEnableTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExAdvClearHandleTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_LeExAdvClearHandle();
}

int GAPIF_LeExAdvClearHandle(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapLeExAdvClearHandleTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterAdvCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterAdvCallback(info->callback, info->context);
}

int GAPIF_RegisterAdvCallback(const GapAdvCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterAdvCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterAdvCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterAdvCallback();
}

int GAPIF_DeregisterAdvCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterAdvCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeAdvSetParamTask(void *ctx)
{
    GapLeAdvSetParamInfo *info = ctx;
    info->result = GAP_LeAdvSetParam(info->advType, info->advParam);
}

int GAPIF_LeAdvSetParam(uint8_t advType, GapLeAdvParam advParam)
{
    LOG_INFO("%{public}s: advType:%hhu", __FUNCTION__, advType);
    GapLeAdvSetParamInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeAdvSetParamInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeAdvSetParamInfo), 0x00, sizeof(GapLeAdvSetParamInfo));

    ctx->advType = advType;
    ctx->advParam = advParam;

    int ret = GapRunTaskBlockProcess(GapLeAdvSetParamTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeAdvReadTxPowerTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_LeAdvReadTxPower();
}

int GAPIF_LeAdvReadTxPower(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapLeAdvReadTxPowerTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeAdvSetDataTask(void *ctx)
{
    GapLeAdvSetDataInfo *info = ctx;
    info->result = GAP_LeAdvSetData(info->advDataLength, info->advData);
}

int GAPIF_LeAdvSetData(uint8_t advDataLength, const uint8_t *advData)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapLeAdvSetDataInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeAdvSetDataInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeAdvSetDataInfo), 0x00, sizeof(GapLeAdvSetDataInfo));

    ctx->advDataLength = advDataLength;
    ctx->advData = (uint8_t *)advData;

    int ret = GapRunTaskBlockProcess(GapLeAdvSetDataTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeAdvSetScanRspDataTask(void *ctx)
{
    GapLeAdvSetDataInfo *info = ctx;
    info->result = GAP_LeAdvSetScanRspData(info->advDataLength, info->advData);
}

int GAPIF_LeAdvSetScanRspData(uint8_t advDataLength, const uint8_t *advData)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapLeAdvSetDataInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeAdvSetDataInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeAdvSetDataInfo), 0x00, sizeof(GapLeAdvSetDataInfo));

    ctx->advDataLength = advDataLength;
    ctx->advData = (uint8_t *)advData;

    int ret = GapRunTaskBlockProcess(GapLeAdvSetScanRspDataTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeAdvSetEnableTask(void *ctx)
{
    GapLeAdvSetEnableInfo *info = ctx;
    info->result = GAP_LeAdvSetEnable(info->enable);
}

int GAPIF_LeAdvSetEnable(uint8_t enable)
{
    LOG_INFO("%{public}s: enable:%hhu", __FUNCTION__, enable);
    GapLeAdvSetEnableInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeAdvSetEnableInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeAdvSetEnableInfo), 0x00, sizeof(GapLeAdvSetEnableInfo));

    ctx->enable = enable;

    int ret = GapRunTaskBlockProcess(GapLeAdvSetEnableTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterScanCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterScanCallback(info->callback, info->context);
}

int GAPIF_RegisterScanCallback(const GapScanCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterScanCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterScanCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterScanCallback();
}

int GAPIF_DeregisterScanCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterScanCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeScanSetParamTask(void *ctx)
{
    GapLeScanSetParamInfo *info = ctx;
    info->result = GAP_LeScanSetParam(info->param, info->scanFilterPolity);
}

int GAPIF_LeScanSetParam(GapLeScanParam param, uint8_t scanFilterPolity)
{
    LOG_INFO("%{public}s: scanType:%hhu", __FUNCTION__, param.scanType);
    GapLeScanSetParamInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeScanSetParamInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeScanSetParamInfo), 0x00, sizeof(GapLeScanSetParamInfo));

    ctx->param = param;
    ctx->scanFilterPolity = scanFilterPolity;

    int ret = GapRunTaskBlockProcess(GapLeScanSetParamTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeScanSetEnableTask(void *ctx)
{
    GapLeScanSetEnableInfo *info = ctx;
    info->result = GAP_LeScanSetEnable(info->scanEnable, info->filterDuplicates);
}

int GAPIF_LeScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates)
{
    LOG_INFO("%{public}s: scanEnable:%hhu", __FUNCTION__, scanEnable);
    GapLeScanSetEnableInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeScanSetEnableInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeScanSetEnableInfo), 0x00, sizeof(GapLeScanSetEnableInfo));

    ctx->scanEnable = scanEnable;
    ctx->filterDuplicates = filterDuplicates;

    int ret = GapRunTaskBlockProcess(GapLeScanSetEnableTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterExScanCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterExScanCallback(info->callback, info->context);
}

int GAPIF_RegisterExScanCallback(const GapExScanCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterExScanCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterExScanCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterExScanCallback();
}

int GAPIF_DeregisterExScanCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterExScanCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExScanSetParamTask(void *ctx)
{
    GapLeExScanSetParamInfo *info = ctx;
    info->result = GAP_LeExScanSetParam(info->scanFilterPolity, info->scanPhys, info->param);
}

int GAPIF_LeExScanSetParam(uint8_t scanFilterPolity, uint8_t scanPhys, const GapLeScanParam param[])
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapLeExScanSetParamInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExScanSetParamInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExScanSetParamInfo), 0x00, sizeof(GapLeExScanSetParamInfo));

    ctx->scanFilterPolity = scanFilterPolity;
    ctx->scanPhys = scanPhys;
    ctx->param = (GapLeScanParam *)param;

    int ret = GapRunTaskBlockProcess(GapLeExScanSetParamTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeExScanSetEnableTask(void *ctx)
{
    GapLeExScanSetEnableInfo *info = ctx;
    info->result = GAP_LeExScanSetEnable(info->scanEnable, info->filterDuplicates, info->duration, info->period);
}

int GAPIF_LeExScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates, uint16_t duration, uint16_t period)
{
    LOG_INFO("%{public}s: scanEnable:%hhu", __FUNCTION__, scanEnable);
    GapLeExScanSetEnableInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeExScanSetEnableInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeExScanSetEnableInfo), 0x00, sizeof(GapLeExScanSetEnableInfo));

    ctx->scanEnable = scanEnable;
    ctx->filterDuplicates = filterDuplicates;
    ctx->duration = duration;
    ctx->period = period;

    int ret = GapRunTaskBlockProcess(GapLeExScanSetEnableTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterLeConnCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterLeConnCallback(info->callback, info->context);
}

int GAPIF_RegisterLeConnCallback(const GapLeConnCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterLeConnCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterLeConnCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterLeConnCallback();
}

int GAPIF_DeregisterLeConnCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterLeConnCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeConnParamUpdateTask(void *ctx)
{
    GapLeConnParamUpdateInfo *info = ctx;
    info->result = GAP_LeConnParamUpdate(info->addr, info->connParam);
}

int GAPIF_LeConnParamUpdate(const BtAddr *addr, const GapLeConnectionParameter *connParam)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLeConnParamUpdateInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeConnParamUpdateInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeConnParamUpdateInfo), 0x00, sizeof(GapLeConnParamUpdateInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->connParam = connParam;

    int ret = GapRunTaskBlockProcess(GapLeConnParamUpdateTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeConnectionParameterRspTask(void *ctx)
{
    GapLeConnectionParameterRspInfo *info = ctx;
    info->result = GAP_LeConnectionParameterRsp(info->addr, info->accept, info->connParam);
}

int GAPIF_LeConnectionParameterRsp(const BtAddr *addr, uint8_t accept, const GapLeConnectionParameter *connParam)
{
    LOG_INFO("%{public}s: accept:%hhu " BT_ADDR_FMT, __FUNCTION__, accept, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLeConnectionParameterRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeConnectionParameterRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeConnectionParameterRspInfo), 0x00, sizeof(GapLeConnectionParameterRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->connParam = connParam;

    int ret = GapRunTaskBlockProcess(GapLeConnectionParameterRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterLeSecurityCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterLeSecurityCallback(info->callback, info->context);
}

int GAPIF_RegisterLeSecurityCallback(const GapLeSecurityCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterLeSecurityCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterLeSecurityCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterLeSecurityCallback();
}

int GAPIF_DeregisterLeSecurityCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterLeSecurityCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeRemoteEncryptionKeyRspTask(void *ctx)
{
    GapLeEncryptionKeyRspInfo *info = ctx;
    info->result = GAP_LeRemoteEncryptionKeyRsp(info->addr, info->accept, info->encKey, info->keyType);
}

int GAPIF_LeRemoteEncryptionKeyRsp(const BtAddr *addr, uint8_t accept, LeEncKey remoteEncKey, uint8_t keyType)
{
    LOG_INFO(
        "%{public}s: " BT_ADDR_FMT " accept:%hhu keyType:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept, keyType);
    GapLeEncryptionKeyRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeEncryptionKeyRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeEncryptionKeyRspInfo), 0x00, sizeof(GapLeEncryptionKeyRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->encKey = remoteEncKey;
    ctx->keyType = keyType;

    int ret = GapRunTaskBlockProcess(GapLeRemoteEncryptionKeyRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeLocalEncryptionKeyRspTask(void *ctx)
{
    GapLeEncryptionKeyRspInfo *info = ctx;
    info->result = GAP_LeLocalEncryptionKeyRsp(info->addr, info->accept, info->encKey, info->keyType);
}

int GAPIF_LeLocalEncryptionKeyRsp(const BtAddr *addr, uint8_t accept, LeEncKey localEncKey, uint8_t keyType)
{
    LOG_INFO(
        "%{public}s: " BT_ADDR_FMT " accept:%hhu keyType:%{public}d", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept, keyType);
    GapLeEncryptionKeyRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeEncryptionKeyRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeEncryptionKeyRspInfo), 0x00, sizeof(GapLeEncryptionKeyRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->encKey = localEncKey;
    ctx->keyType = keyType;

    int ret = GapRunTaskBlockProcess(GapLeLocalEncryptionKeyRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRequestSigningAlgorithmInfoRspTask(void *ctx)
{
    GapRequestSigningAlgorithmInfoRspInfo *info = ctx;
    info->result = GAP_RequestSigningAlgorithmInfoRsp(info->addr, info->accept, info->info);
}

int GAPIF_RequestSigningAlgorithmInfoRsp(const BtAddr *addr, uint8_t accept, GapSigningAlgorithmInfo info)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " accept:%hhu local:%{public}d remote:%{public}d",
        __FUNCTION__,
        BT_ADDR_FMT_OUTPUT(addr->addr),
        accept,
        info.localKey != NULL,
        info.remoteKey != NULL);
    GapRequestSigningAlgorithmInfoRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapRequestSigningAlgorithmInfoRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(
        ctx, sizeof(GapRequestSigningAlgorithmInfoRspInfo), 0x00, sizeof(GapRequestSigningAlgorithmInfoRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->info = info;

    int ret = GapRunTaskBlockProcess(GapRequestSigningAlgorithmInfoRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeSetBondModeTask(void *ctx)
{
    GapLeSetBondModeInfo *info = ctx;
    info->result = GAP_LeSetBondMode(info->mode);
}

int GAPIF_LeSetBondMode(uint8_t mode)
{
    LOG_INFO("%{public}s: mode:%hhu", __FUNCTION__, mode);
    GapLeSetBondModeInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeSetBondModeInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeSetBondModeInfo), 0x00, sizeof(GapLeSetBondModeInfo));

    ctx->mode = mode;

    int ret = GapRunTaskBlockProcess(GapLeSetBondModeTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeSetSecurityModeTask(void *ctx)
{
    GapLeSetSecurityModeInfo *info = ctx;
    info->result = GAP_LeSetSecurityMode(info->mode1Level, info->mode2Level);
}

int GAPIF_LeSetSecurityMode(GAP_LeSecMode1Level mode1Level, GAP_LeSecMode2Level mode2Level)
{
    LOG_INFO("%{public}s: mode1Level:%{public}d mode2Level:%{public}d", __FUNCTION__, mode1Level, mode2Level);
    GapLeSetSecurityModeInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeSetSecurityModeInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeSetSecurityModeInfo), 0x00, sizeof(GapLeSetSecurityModeInfo));

    ctx->mode1Level = mode1Level;
    ctx->mode2Level = mode2Level;

    int ret = GapRunTaskBlockProcess(GapLeSetSecurityModeTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeGetSecurityStatusTask(void *ctx)
{
    GapLeGetSecurityStatusInfo *info = ctx;
    info->result = GAP_LeGetSecurityStatus(info->addr, info->status, info->encKeySize);
}

int GAPIF_LeGetSecurityStatus(const BtAddr *addr, GAP_LeSecurityStatus *status, uint8_t *encKeySize)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLeGetSecurityStatusInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeGetSecurityStatusInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeGetSecurityStatusInfo), 0x00, sizeof(GapLeGetSecurityStatusInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->status = status;
    ctx->encKeySize = encKeySize;

    int ret = GapRunTaskBlockProcess(GapLeGetSecurityStatusTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeRequestSecurityTask(void *ctx)
{
    GapLeRequestSecurityInfo *info = ctx;
    info->result = GAP_LeRequestSecurity(info->addr, info->status, info->callback, info->context);
}

int GAPIF_LeRequestSecurity(
    const BtAddr *addr, GAP_LeSecurityStatus status, GapLeRequestSecurityResult callback, void *context)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " status:%{public}d", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), status);
    GapLeRequestSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeRequestSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeRequestSecurityInfo), 0x00, sizeof(GapLeRequestSecurityInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->status = status;
    ctx->callback = callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapLeRequestSecurityTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLePairTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_LePair(info->pointer);
}

int GAPIF_LePair(const BtAddr *addr)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapLePairTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeCancelPairTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_LeCancelPair(info->pointer);
}

int GAPIF_LeCancelPair(const BtAddr *addr)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapLeCancelPairTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeSetMinEncKeySizeTask(void *ctx)
{
    GapLeSetMinEncKeySizeInfo *info = ctx;
    info->result = GAP_LeSetMinEncKeySize(info->minSize);
}

int GAPIF_LeSetMinEncKeySize(uint8_t minSize)
{
    LOG_INFO("%{public}s: minSize:%hhu", __FUNCTION__, minSize);
    GapLeSetMinEncKeySizeInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeSetMinEncKeySizeInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeSetMinEncKeySizeInfo), 0x00, sizeof(GapLeSetMinEncKeySizeInfo));

    ctx->minSize = minSize;

    int ret = GapRunTaskBlockProcess(GapLeSetMinEncKeySizeTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterLePairCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterLePairCallback(info->callback, info->context);
}

int GAPIF_RegisterLePairCallback(const GapLePairCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterLePairCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterLePairCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterLePairCallback();
}

int GAPIF_DeregisterLePairCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterLePairCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLePairFeatureRspTask(void *ctx)
{
    GapLePairFeatureRspInfo *info = ctx;
    info->result = GAP_LePairFeatureRsp(info->addr, info->localFrature);
}

int GAPIF_LePairFeatureRsp(const BtAddr *addr, GapLePairFeature localFrature)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLePairFeatureRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLePairFeatureRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLePairFeatureRspInfo), 0x00, sizeof(GapLePairFeatureRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->localFrature = localFrature;

    int ret = GapRunTaskBlockProcess(GapLePairFeatureRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLePairPassKeyRspTask(void *ctx)
{
    GapLePairPassKeyRspInfo *info = ctx;
    info->result = GAP_LePairPassKeyRsp(info->addr, info->accept, info->number);
}

int GAPIF_LePairPassKeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number)
{
    LOG_INFO("%{public}s: accept:%hhu" BT_ADDR_FMT, __FUNCTION__, accept, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLePairPassKeyRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLePairPassKeyRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLePairPassKeyRspInfo), 0x00, sizeof(GapLePairPassKeyRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->number = number;

    int ret = GapRunTaskBlockProcess(GapLePairPassKeyRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLePairOobRsptask(void *ctx)
{
    GapLePairOobRspInfo *info = ctx;
    info->result = GAP_LePairOobRsp(info->addr, info->accept, info->oobData);
}

int GAPIF_LePairOobRsp(const BtAddr *addr, uint8_t accept, uint8_t oobData[GAP_OOB_DATA_SIZE])
{
    LOG_INFO("%{public}s: accept:%hhu" BT_ADDR_FMT, __FUNCTION__, accept, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLePairOobRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLePairOobRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLePairOobRspInfo), 0x00, sizeof(GapLePairOobRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->oobData = oobData;

    int ret = GapRunTaskBlockProcess(GapLePairOobRsptask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLePairScOobRsptask(void *ctx)
{
    GapLePairScOobRspInfo *info = ctx;
    info->result = GAP_LePairScOobRsp(info->addr, info->accept, info->oobDataC, info->oobDataR);
}

int GAPIF_LePairScOobRsp(const BtAddr *addr, uint8_t accept, const uint8_t oobDataC[GAP_OOB_DATA_CONFIRM_SIZE],
    const uint8_t oobDataR[GAP_OOB_DATA_RANDOM_SIZE])
{
    LOG_INFO("%{public}s: accept:%hhu" BT_ADDR_FMT, __FUNCTION__, accept, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLePairScOobRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLePairScOobRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLePairScOobRspInfo), 0x00, sizeof(GapLePairScOobRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->oobDataC = (uint8_t *)oobDataC;
    ctx->oobDataR = (uint8_t *)oobDataR;

    int ret = GapRunTaskBlockProcess(GapLePairScOobRsptask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLePairScUserConfirmRspTask(void *ctx)
{
    GapLePairScUserConfirmRspInfo *info = ctx;
    info->result = GAP_LePairScUserConfirmRsp(info->addr, info->accept);
}

int GAPIF_LePairScUserConfirmRsp(const BtAddr *addr, uint8_t accept)
{
    LOG_INFO("%{public}s: accept:%hhu" BT_ADDR_FMT, __FUNCTION__, accept, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLePairScUserConfirmRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLePairScUserConfirmRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLePairScUserConfirmRspInfo), 0x00, sizeof(GapLePairScUserConfirmRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;

    int ret = GapRunTaskBlockProcess(GapLePairScUserConfirmRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLeDataSignatureGenerationTask(void *ctx)
{
    GapLeDataSignatureGenerationInfo *info = ctx;
    info->result = GAP_LeDataSignatureGeneration(info->addr, info->dataInfo, info->callback, info->context);
}

static void GapFreeLeDataSignatureGeneration(void *ctx)
{
    GapLeDataSignatureGenerationInfo *info = ctx;

    MEM_MALLOC.free((void *)info->dataInfo.data);
}

int GAPIF_LeDataSignatureGenerationAsync(
    const BtAddr *addr, GapSignatureData dataInfo, GAPSignatureGenerationResult callback, void *context)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLeDataSignatureGenerationInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeDataSignatureGenerationInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeDataSignatureGenerationInfo), 0x00, sizeof(GapLeDataSignatureGenerationInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->dataInfo = dataInfo;
    ctx->callback = callback;
    ctx->context = context;

    uint8_t *data = MEM_MALLOC.alloc(ctx->dataInfo.dataLen);
    if (data == NULL) {
        MEM_MALLOC.free(ctx);
        return BT_NO_MEMORY;
    }
    (void)memcpy_s(data, ctx->dataInfo.dataLen, ctx->dataInfo.data, ctx->dataInfo.dataLen);
    ctx->dataInfo.data = data;

    int ret = GapRunTaskUnBlockProcess(GapLeDataSignatureGenerationTask, ctx, GapFreeLeDataSignatureGeneration);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    return ret;
}

static void GapLeDataSignatureConfirmationTask(void *ctx)
{
    GapLeDataSignatureConfirmationInfo *info = ctx;
    info->result =
        GAP_LeDataSignatureConfirmation(info->addr, info->dataInfo, info->signature, info->callback, info->context);
}

static void GapFreeLeDataSignatureConfirmation(void *ctx)
{
    GapLeDataSignatureConfirmationInfo *info = ctx;

    MEM_MALLOC.free((void *)info->dataInfo.data);
}

int GAPIF_LeDataSignatureConfirmationAsync(const BtAddr *addr, GapSignatureData dataInfo,
    const uint8_t signature[GAP_SIGNATURE_SIZE], GAPSignatureConfirmationResult callback, void *context)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLeDataSignatureConfirmationInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLeDataSignatureConfirmationInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLeDataSignatureConfirmationInfo), 0x00, sizeof(GapLeDataSignatureConfirmationInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->dataInfo = dataInfo;
    (void)memcpy_s(ctx->signature, GAP_SIGNATURE_SIZE, signature, GAP_SIGNATURE_SIZE);
    ctx->callback = callback;
    ctx->context = context;

    uint8_t *data = MEM_MALLOC.alloc(ctx->dataInfo.dataLen);
    if (data == NULL) {
        MEM_MALLOC.free(ctx);
        return BT_NO_MEMORY;
    }
    (void)memcpy_s(data, ctx->dataInfo.dataLen, ctx->dataInfo.data, ctx->dataInfo.dataLen);
    ctx->dataInfo.data = data;

    int ret = GapRunTaskUnBlockProcess(GapLeDataSignatureConfirmationTask, ctx, GapFreeLeDataSignatureConfirmation);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    return ret;
}

#endif