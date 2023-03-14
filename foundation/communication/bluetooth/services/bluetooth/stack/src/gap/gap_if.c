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

#include "gap.h"
#include "gap_def.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"

#include "btm/btm_thread.h"

typedef struct {
    int result;
    char *name;
    int length;
} GapSetLocalNameInfo;

typedef struct {
    int result;
    uint32_t cod;
} GapSetClassOfDeviceInfo;

typedef struct {
    int result;
    uint8_t *eir;
} GapSetExtendedInquiryResponseInfo;

typedef struct {
    int result;
    GapDiscoverModeInfo discoverInfo;
    GapConnectableModeInfo connectableInfo;
    GapSetScanModeResultCallback callback;
    void *context;
} GapSetScanModeInfo;

typedef struct {
    int result;
    uint8_t isBondable;
} GapSetBondableModeInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GapServiceSecurityInfo serviceInfo;
    uint16_t securityMode;
} GapRegisterServiceSecurityInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GapServiceSecurityInfo serviceInfo;
} GapDeregisterServiceSecurityInfo;

typedef struct {
    int result;
    BtAddr addr;
    GapRequestSecurityParam param;
} GapRequestSecurityInfo;

typedef struct {
    int result;
    GAP_SecurityMode mode;
} GapSetSecurityModeInfo;

typedef struct {
    int result;
    BtAddr *addr;
    GAP_Service service;
    uint8_t accept;
} GapAuthorizeResInfo;

typedef struct {
    int result;
    BtAddr *addr;
    bool *isLocal;
} GapPairIsFromLocalInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint8_t ioCapability;
    uint8_t oobDataPresent;
    uint8_t authReq;
} GapIOCapabilityRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
} GapUserConfirmRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint32_t number;
} GapUserPasskeyRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    GapOOBData *data;
} GapRemoteOobRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    GapOOBData *data192;
    GapOOBData *data256;
} GapRemoteExtandedOobRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint8_t *pinCode;
    uint8_t pinCodeLength;
} GapPinCodeRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t accept;
    uint8_t *linkKey;
    uint8_t keyType;
} GapLinkKeyRspInfo;

typedef struct {
    int result;
    BtAddr *addr;
    uint8_t status;
    GapEncryptionChangeCallback callback;
    void *context;
} GapSetEncryptionInfo;

typedef struct {
    int result;
    GapOOBData *data192;
    GapOOBData *data256;
} GapGetLocalExtendedOOBDataInfo;

typedef struct {
    int result;
    uint8_t mode;
    uint8_t inquiryLength;
} GapInquiryInfo;

typedef struct {
    int result;
    bool retry;
} GapSetKeyMissingRetryInfo;

#ifdef GAP_BREDR_SUPPORT

static void GapSetLocalNameTask(void *ctx)
{
    GapSetLocalNameInfo *info = ctx;
    info->result = GAP_SetLocalName(info->name, info->length);
}

int GAPIF_SetLocalName(const char *name, int length)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapSetLocalNameInfo *ctx = MEM_MALLOC.alloc(sizeof(GapSetLocalNameInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapSetLocalNameInfo), 0x00, sizeof(GapSetLocalNameInfo));

    ctx->name = (char *)name;
    ctx->length = length;

    int ret = GapRunTaskBlockProcess(GapSetLocalNameTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapSetClassOfDeviceTask(void *ctx)
{
    GapSetClassOfDeviceInfo *info = ctx;
    info->result = GAP_SetClassOfDevice(info->cod);
}

int GAPIF_SetClassOfDevice(uint32_t cod)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapSetClassOfDeviceInfo *ctx = MEM_MALLOC.alloc(sizeof(GapSetClassOfDeviceInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapSetClassOfDeviceInfo), 0x00, sizeof(GapSetClassOfDeviceInfo));

    ctx->cod = cod;

    int ret = GapRunTaskBlockProcess(GapSetClassOfDeviceTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapSetExtendedInquiryResponseTask(void *ctx)
{
    GapSetExtendedInquiryResponseInfo *info = ctx;
    info->result = GAP_SetExtendedInquiryResponse(info->eir);
}

int GAPIF_SetExtendedInquiryResponse(const uint8_t eir[GAP_EIR_SIZE_MAX])
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapSetExtendedInquiryResponseInfo *ctx = MEM_MALLOC.alloc(sizeof(GapSetExtendedInquiryResponseInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapSetExtendedInquiryResponseInfo), 0x00, sizeof(GapSetExtendedInquiryResponseInfo));

    ctx->eir = (uint8_t *)eir;

    int ret = GapRunTaskBlockProcess(GapSetExtendedInquiryResponseTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapSetScanModeTask(void *ctx)
{
    GapSetScanModeInfo *info = ctx;
    info->result = GAP_SetScanMode(&info->discoverInfo, &info->connectableInfo, info->callback, info->context);
}

int GAPIF_SetScanMode(const GapDiscoverModeInfo *discoverInfo, const GapConnectableModeInfo *connectableInfo,
    GapSetScanModeResultCallback callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapSetScanModeInfo *ctx = MEM_MALLOC.alloc(sizeof(GapSetScanModeInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapSetScanModeInfo), 0x00, sizeof(GapSetScanModeInfo));

    ctx->discoverInfo = *discoverInfo;
    ctx->connectableInfo = *connectableInfo;
    ctx->callback = callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapSetScanModeTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapSetBondableModeTask(void *ctx)
{
    GapSetBondableModeInfo *info = ctx;
    info->result = GAP_SetBondableMode(info->isBondable);
}

int GAPIF_SetBondableMode(uint8_t isBondable)
{
    LOG_INFO("%{public}s: Bondable:%hhu", __FUNCTION__, isBondable);
    GapSetBondableModeInfo *ctx = MEM_MALLOC.alloc(sizeof(GapSetBondableModeInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapSetBondableModeInfo), 0x00, sizeof(GapSetBondableModeInfo));

    ctx->isBondable = isBondable;

    int ret = GapRunTaskBlockProcess(GapSetBondableModeTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterServiceSecurityTask(void *ctx)
{
    GapRegisterServiceSecurityInfo *info = ctx;
    info->result = GAP_RegisterServiceSecurity(info->addr, &info->serviceInfo, info->securityMode);
}

int GAPIF_RegisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo, uint16_t securityMode)
{
    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s-0x%04x mode:%04x",
        __FUNCTION__,
        serviceInfo->direction ? "IN" : "OUT",
        serviceInfo->serviceId,
        serviceInfo->protocolId ? "RFCOMM" : "L2CAP",
        serviceInfo->protocolId ? serviceInfo->channelId.rfcommChannel : serviceInfo->channelId.l2capPsm,
        securityMode);
    GapRegisterServiceSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapRegisterServiceSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapRegisterServiceSecurityInfo), 0x00, sizeof(GapRegisterServiceSecurityInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->serviceInfo = *serviceInfo;
    ctx->securityMode = securityMode;

    int ret = GapRunTaskBlockProcess(GapRegisterServiceSecurityTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapFreeRegisterServiceSecurity(void *ctx)
{
    GapRegisterServiceSecurityInfo *info = ctx;

    MEM_MALLOC.free(info->addr);
}

int GAPIF_RegisterServiceSecurityAsync(
    const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo, uint16_t securityMode)
{
    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s-0x%04x mode:%04x",
        __FUNCTION__,
        serviceInfo->direction ? "IN" : "OUT",
        serviceInfo->serviceId,
        serviceInfo->protocolId ? "RFCOMM" : "L2CAP",
        serviceInfo->protocolId ? serviceInfo->channelId.rfcommChannel : serviceInfo->channelId.l2capPsm,
        securityMode);
    GapRegisterServiceSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapRegisterServiceSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapRegisterServiceSecurityInfo), 0x00, sizeof(GapRegisterServiceSecurityInfo));

    ctx->addr = MEM_MALLOC.alloc(sizeof(BtAddr));
    if (ctx->addr == NULL) {
        MEM_MALLOC.free(ctx);
        return BT_NO_MEMORY;
    }

    if (addr != NULL) {
        ctx->addr = (BtAddr *)addr;
    } else {
        (void)memset_s(ctx->addr, sizeof(BtAddr), 0x00, sizeof(BtAddr));
    }
    ctx->serviceInfo = *serviceInfo;
    ctx->securityMode = securityMode;

    int ret = GapRunTaskUnBlockProcess(GapRegisterServiceSecurityTask, ctx, GapFreeRegisterServiceSecurity);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    return ret;
}

static void GapDeregisterServiceSecurityTask(void *ctx)
{
    GapDeregisterServiceSecurityInfo *info = ctx;
    info->result = GAP_DeregisterServiceSecurity(info->addr, &info->serviceInfo);
}

int GAPIF_DeregisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo)
{
    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s-0x%04x",
        __FUNCTION__,
        serviceInfo->direction ? "IN" : "OUT",
        serviceInfo->serviceId,
        serviceInfo->protocolId ? "RFCOMM" : "L2CAP",
        serviceInfo->protocolId ? serviceInfo->channelId.rfcommChannel : serviceInfo->channelId.l2capPsm);
    GapDeregisterServiceSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapDeregisterServiceSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapDeregisterServiceSecurityInfo), 0x00, sizeof(GapDeregisterServiceSecurityInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->serviceInfo = *serviceInfo;

    int ret = GapRunTaskBlockProcess(GapDeregisterServiceSecurityTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapFreeDeregisterServiceSecurity(void *ctx)
{
    GapDeregisterServiceSecurityInfo *info = ctx;

    MEM_MALLOC.free(info->addr);
}

int GAPIF_DeregisterServiceSecurityAsync(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo)
{
    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s-0x%04x",
        __FUNCTION__,
        serviceInfo->direction ? "IN" : "OUT",
        serviceInfo->serviceId,
        serviceInfo->protocolId ? "RFCOMM" : "L2CAP",
        serviceInfo->protocolId ? serviceInfo->channelId.rfcommChannel : serviceInfo->channelId.l2capPsm);
    GapDeregisterServiceSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapDeregisterServiceSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapDeregisterServiceSecurityInfo), 0x00, sizeof(GapDeregisterServiceSecurityInfo));

    ctx->addr = MEM_MALLOC.alloc(sizeof(BtAddr));
    if (ctx->addr == NULL) {
        MEM_MALLOC.free(ctx);
        return BT_NO_MEMORY;
    }

    if (addr != NULL) {
        ctx->addr = (BtAddr *)addr;
    } else {
        (void)memset_s(ctx->addr, sizeof(BtAddr), 0x00, sizeof(BtAddr));
    }
    ctx->serviceInfo = *serviceInfo;

    int ret = GapRunTaskUnBlockProcess(GapDeregisterServiceSecurityTask, ctx, GapFreeDeregisterServiceSecurity);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    return ret;
}

static void GapRequestSecurityTask(void *ctx)
{
    GapRequestSecurityInfo *info = ctx;
    info->result = GAP_RequestSecurity(&info->addr, &info->param);
}

int GAPIF_RequestSecurity(const BtAddr *addr, const GapRequestSecurityParam *param)
{
    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s-0x%04x",
        __FUNCTION__,
        param->info.direction ? "IN" : "OUT",
        param->info.serviceId,
        param->info.protocolId ? "RFCOMM" : "L2CAP",
        param->info.protocolId ? param->info.channelId.rfcommChannel : param->info.channelId.l2capPsm);
    GapRequestSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapRequestSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapRequestSecurityInfo), 0x00, sizeof(GapRequestSecurityInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    (void)memcpy_s(&ctx->param, sizeof(GapRequestSecurityParam), param, sizeof(GapRequestSecurityParam));

    int ret = GapRunTaskBlockProcess(GapRequestSecurityTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

int GAPIF_RequestSecurityAsync(const BtAddr *addr, const GapRequestSecurityParam *param)
{
    LOG_INFO("%{public}s:%{public}s-%02d-%{public}s-0x%04x",
        __FUNCTION__,
        param->info.direction ? "IN" : "OUT",
        param->info.serviceId,
        param->info.protocolId ? "RFCOMM" : "L2CAP",
        param->info.protocolId ? param->info.channelId.rfcommChannel : param->info.channelId.l2capPsm);
    GapRequestSecurityInfo *ctx = MEM_MALLOC.alloc(sizeof(GapRequestSecurityInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapRequestSecurityInfo), 0x00, sizeof(GapRequestSecurityInfo));

    (void)memcpy_s(&ctx->addr, sizeof(BtAddr), addr, sizeof(BtAddr));
    (void)memcpy_s(&ctx->param, sizeof(GapRequestSecurityParam), param, sizeof(GapRequestSecurityParam));

    int ret = GapRunTaskUnBlockProcess(GapRequestSecurityTask, ctx, NULL);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    return ret;
}

static void GapRegisterSecurityCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterSecurityCallback(info->callback, info->context);
}

int GAPIF_RegisterSecurityCallback(const GapSecurityCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterSecurityCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterSecurityCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterSecurityCallback();
}

int GAPIF_DeregisterSecurityCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterSecurityCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapSetSecurityModeTask(void *ctx)
{
    GapSetSecurityModeInfo *info = ctx;
    info->result = GAP_SetSecurityMode(info->mode);
}

int GAPIF_SetSecurityMode(GAP_SecurityMode mode)
{
    LOG_INFO("%{public}s: mode:%{public}d", __FUNCTION__, mode);
    GapSetSecurityModeInfo *ctx = MEM_MALLOC.alloc(sizeof(GapSetSecurityModeInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapSetSecurityModeInfo), 0x00, sizeof(GapSetSecurityModeInfo));

    ctx->mode = mode;

    int ret = GapRunTaskBlockProcess(GapSetSecurityModeTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapAuthorizeResTask(void *ctx)
{
    GapAuthorizeResInfo *info = ctx;
    info->result = GAP_AuthorizeRes(info->addr, info->service, info->accept);
}

int GAPIF_AuthorizeRes(const BtAddr *addr, GAP_Service service, uint8_t accept)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT "%{public}d accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), service, accept);
    GapAuthorizeResInfo *ctx = MEM_MALLOC.alloc(sizeof(GapAuthorizeResInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapAuthorizeResInfo), 0x00, sizeof(GapAuthorizeResInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->service = service;
    ctx->accept = accept;

    int ret = GapRunTaskBlockProcess(GapAuthorizeResTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterAuthenticationCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterAuthenticationCallback(info->callback, info->context);
}

int GAPIF_RegisterAuthenticationCallback(const GapAuthenticationCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterAuthenticationCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterAuthenticationCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterAuthenticationCallback();
}

int GAPIF_DeregisterAuthenticationCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterAuthenticationCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapPairIsFromLocalTask(void *ctx)
{
    GapPairIsFromLocalInfo *info = ctx;
    info->result = GAP_PairIsFromLocal(info->addr, info->isLocal);
}

int GAPIF_PairIsFromLocal(const BtAddr *addr, bool *isLocal)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapPairIsFromLocalInfo *ctx = MEM_MALLOC.alloc(sizeof(GapPairIsFromLocalInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapPairIsFromLocalInfo), 0x00, sizeof(GapPairIsFromLocalInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->isLocal = isLocal;

    int ret = GapRunTaskBlockProcess(GapPairIsFromLocalTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapAuthenticationReqTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_AuthenticationReq(info->pointer);
}

int GAPIF_AuthenticationReq(const BtAddr *addr)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapAuthenticationReqTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapCancelAuthenticationReqTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_CancelAuthenticationReq(info->pointer);
}

int GAPIF_CancelAuthenticationReq(const BtAddr *addr)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapCancelAuthenticationReqTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapIOCapabilityRspTask(void *ctx)
{
    GapIOCapabilityRspInfo *info = ctx;
    info->result =
        GAP_IOCapabilityRsp(info->addr, info->accept, info->ioCapability, info->oobDataPresent, info->authReq);
}

int GAPIF_IOCapabilityRsp(
    const BtAddr *addr, uint8_t accept, uint8_t ioCapability, uint8_t oobDataPresent, uint8_t authReq)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept);
    GapIOCapabilityRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapIOCapabilityRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapIOCapabilityRspInfo), 0x00, sizeof(GapIOCapabilityRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->ioCapability = ioCapability;
    ctx->oobDataPresent = oobDataPresent;
    ctx->authReq = authReq;

    int ret = GapRunTaskBlockProcess(GapIOCapabilityRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapUserConfirmRspTask(void *ctx)
{
    GapUserConfirmRspInfo *info = ctx;
    info->result = GAP_UserConfirmRsp(info->addr, info->accept);
}

int GAPIF_UserConfirmRsp(const BtAddr *addr, uint8_t accept)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept);
    GapUserConfirmRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapUserConfirmRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapUserConfirmRspInfo), 0x00, sizeof(GapUserConfirmRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;

    int ret = GapRunTaskBlockProcess(GapUserConfirmRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapUserPasskeyRspTask(void *ctx)
{
    GapUserPasskeyRspInfo *info = ctx;
    info->result = GAP_UserPasskeyRsp(info->addr, info->accept, info->number);
}

int GAPIF_UserPasskeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept);
    GapUserPasskeyRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapUserPasskeyRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapUserPasskeyRspInfo), 0x00, sizeof(GapUserPasskeyRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->number = number;

    int ret = GapRunTaskBlockProcess(GapUserPasskeyRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRemoteOobRspTask(void *ctx)
{
    GapRemoteOobRspInfo *info = ctx;
    info->result = GAP_RemoteOobRsp(info->addr, info->accept, info->data);
}

int GAPIF_RemoteOobRsp(const BtAddr *addr, uint8_t accept, const GapOOBData *data)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept);
    GapRemoteOobRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapRemoteOobRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapRemoteOobRspInfo), 0x00, sizeof(GapRemoteOobRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->data = (GapOOBData *)data;

    int ret = GapRunTaskBlockProcess(GapRemoteOobRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapPinCodeRspTask(void *ctx)
{
    GapPinCodeRspInfo *info = ctx;
    info->result = GAP_PinCodeRsp(info->addr, info->accept, info->pinCode, info->pinCodeLength);
}

int GAPIF_PinCodeRsp(const BtAddr *addr, uint8_t accept, const uint8_t *pinCode, uint8_t pinCodeLength)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " accept:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept);
    GapPinCodeRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapPinCodeRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapPinCodeRspInfo), 0x00, sizeof(GapPinCodeRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->pinCode = (uint8_t *)pinCode;
    ctx->pinCodeLength = pinCodeLength;

    int ret = GapRunTaskBlockProcess(GapPinCodeRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapLinkKeyRspTask(void *ctx)
{
    GapLinkKeyRspInfo *info = ctx;
    info->result = GAP_LinkKeyRsp(info->addr, info->accept, info->linkKey, info->keyType);
}

int GAPIF_LinkKeyRsp(const BtAddr *addr, uint8_t accept, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType)
{
    LOG_INFO(
        "%{public}s: " BT_ADDR_FMT " accept:%hhu keyType:%hhu", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), accept, keyType);
    GapLinkKeyRspInfo *ctx = MEM_MALLOC.alloc(sizeof(GapLinkKeyRspInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapLinkKeyRspInfo), 0x00, sizeof(GapLinkKeyRspInfo));

    ctx->addr = (BtAddr *)addr;
    ctx->accept = accept;
    ctx->linkKey = (uint8_t *)linkKey;
    ctx->keyType = keyType;

    int ret = GapRunTaskBlockProcess(GapLinkKeyRspTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapRegisterDiscoveryCallbackTask(void *ctx)
{
    GapGeneralCallbackInfo *info = ctx;
    info->result = GAP_RegisterDiscoveryCallback(info->callback, info->context);
}

int GAPIF_RegisterDiscoveryCallback(const GapDiscoveryCallback *callback, void *context)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralCallbackInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralCallbackInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralCallbackInfo), 0x00, sizeof(GapGeneralCallbackInfo));

    ctx->callback = (void *)callback;
    ctx->context = context;

    int ret = GapRunTaskBlockProcess(GapRegisterDiscoveryCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapDeregisterDiscoveryCallbackTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_DeregisterDiscoveryCallback();
}

int GAPIF_DeregisterDiscoveryCallback(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapDeregisterDiscoveryCallbackTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapInquiryTask(void *ctx)
{
    GapInquiryInfo *info = ctx;
    info->result = GAP_Inquiry(info->mode, info->inquiryLength);
}

int GAPIF_Inquiry(uint8_t mode, uint8_t inquiryLength)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapInquiryInfo *ctx = MEM_MALLOC.alloc(sizeof(GapInquiryInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapInquiryInfo), 0x00, sizeof(GapInquiryInfo));

    ctx->mode = mode;
    ctx->inquiryLength = inquiryLength;

    int ret = GapRunTaskBlockProcess(GapInquiryTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapInquiryCancelTask(void *ctx)
{
    GapGeneralVoidInfo *info = ctx;
    info->result = GAP_InquiryCancel();
}

int GAPIF_InquiryCancel(void)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralVoidInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralVoidInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralVoidInfo), 0x00, sizeof(GapGeneralVoidInfo));

    int ret = GapRunTaskBlockProcess(GapInquiryCancelTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapGetRemoteNameTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_GetRemoteName(info->pointer);
}

int GAPIF_GetRemoteName(const BtAddr *addr)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapGetRemoteNameTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapGetRemoteNameCancelTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_GetRemoteNameCancel(info->pointer);
}

int GAPIF_GetRemoteNameCancel(const BtAddr *addr)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr));
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapGetRemoteNameCancelTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

static void GapGetLocalAddrTask(void *ctx)
{
    GapGeneralPointerInfo *info = ctx;
    info->result = GAP_GetLocalAddr(info->pointer);
}

int GAPIF_GetLocalAddr(BtAddr *addr)
{
    LOG_INFO("%{public}s: ", __FUNCTION__);
    GapGeneralPointerInfo *ctx = MEM_MALLOC.alloc(sizeof(GapGeneralPointerInfo));
    if (ctx == NULL) {
        return BT_NO_MEMORY;
    }

    (void)memset_s(ctx, sizeof(GapGeneralPointerInfo), 0x00, sizeof(GapGeneralPointerInfo));

    ctx->pointer = (void *)addr;

    int ret = GapRunTaskBlockProcess(GapGetLocalAddrTask, ctx);
    if (ret == BT_NO_ERROR) {
        ret = ctx->result;
    }

    MEM_MALLOC.free(ctx);
    return ret;
}

#endif