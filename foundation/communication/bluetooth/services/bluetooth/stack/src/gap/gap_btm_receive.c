/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "gap_internal.h"
#include "gap_task_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"

#include "btm.h"
#include "btm/btm_acl.h"
#include "btm/btm_thread.h"

#ifdef GAP_BREDR_SUPPORT

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    BtAddr addr;
    uint8_t classOfDevice[BT_COD_SIZE];
    bool encyptionEnabled;
    void *context;
} GapAclConnectionCompleteParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t reason;
    void *context;
} GapAclDisconnectionCompleteParam;

typedef struct {
    BtAddr addr;
    bool support;
} GapRemoteDeviceSupportCallbackParam;

static void GapAclConnectionCompleteTask(void *ctx)
{
    GapAclConnectionCompleteParam *param = ctx;
    BtmAclConnectCompleteParam btmParam = {.addr = &param->addr};
    btmParam.status = param->status;
    btmParam.connectionHandle = param->connectionHandle;
    (void)memcpy_s(
        btmParam.classOfDevice, sizeof(btmParam.classOfDevice), param->classOfDevice, sizeof(param->classOfDevice));
    btmParam.encyptionEnabled = param->encyptionEnabled;
    GapAclConnectionComplete(&btmParam, param->context);
}

static void GapRecvAclConnectionComplete(const BtmAclConnectCompleteParam *param, void *context)
{
    LOG_INFO("%{public}s: status:0x%{public}02x handle:0x%{public}04x " BT_ADDR_FMT,
        __FUNCTION__,
        param->status,
        param->connectionHandle,
        BT_ADDR_FMT_OUTPUT(param->addr->addr));
    GapAclConnectionCompleteParam *btmParam = MEM_MALLOC.alloc(sizeof(GapAclConnectionCompleteParam));
    if (btmParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    btmParam->status = param->status;
    btmParam->connectionHandle = param->connectionHandle;
    btmParam->addr = *param->addr;
    (void)memcpy_s(
        btmParam->classOfDevice, sizeof(btmParam->classOfDevice), param->classOfDevice, sizeof(param->classOfDevice));
    btmParam->encyptionEnabled = param->encyptionEnabled;
    btmParam->context = context;

    int ret = GapRunTaskUnBlockProcess(GapAclConnectionCompleteTask, btmParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapAclDisconnectionCompleteTask(void *ctx)
{
    GapAclDisconnectionCompleteParam *param = ctx;
    GapAclDisconnectionComplete(param->status, param->connectionHandle, param->reason, param->context);
}

static void GapRecvAclDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    LOG_INFO("%{public}s: status:0x%{public}02x handle:0x%{public}04x reason:0x%{public}02x", __FUNCTION__,
        status, connectionHandle, reason);
    GapAclDisconnectionCompleteParam *btmParam = MEM_MALLOC.alloc(sizeof(GapAclDisconnectionCompleteParam));
    if (btmParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    btmParam->status = status;
    btmParam->connectionHandle = connectionHandle;
    btmParam->reason = reason;
    btmParam->context = context;

    int ret = GapRunTaskUnBlockProcess(GapAclDisconnectionCompleteTask, btmParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

#endif

#ifdef GAP_LE_SUPPORT

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    BtAddr addr;
    uint8_t role;
    void *context;
} GapLeConnectionCompleteParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t reason;
    void *context;
} GapLeDisconnectionCompleteParam;

static void GapLeConnectionCompleteTask(void *ctx)
{
    GapLeConnectionCompleteParam *param = ctx;
    GapLeConnectionComplete(param->status, param->connectionHandle, &param->addr, param->role, param->context);
}

static void GapRecvLeConnectionComplete(
    uint8_t status, uint16_t connectionHandle, const BtAddr *addr, uint8_t role, void *context)
{
    LOG_INFO("%{public}s: status:0x%{public}02x role:%{public}hhu handle:0x%{public}04x " BT_ADDR_FMT,
        __FUNCTION__,
        status,
        role,
        connectionHandle,
        BT_ADDR_FMT_OUTPUT(addr->addr));
    GapLeConnectionCompleteParam *btmParam = MEM_MALLOC.alloc(sizeof(GapLeConnectionCompleteParam));
    if (btmParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    btmParam->status = status;
    btmParam->connectionHandle = connectionHandle;
    btmParam->addr = *addr;
    btmParam->role = role;
    btmParam->context = context;

    int ret = GapRunTaskUnBlockProcess(GapLeConnectionCompleteTask, btmParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapLeDisconnectionCompleteTask(void *ctx)
{
    GapLeDisconnectionCompleteParam *param = ctx;
    GapLeDisconnectionComplete(param->status, param->connectionHandle, param->reason, param->context);
}

static void GapRecvLeDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context)
{
    LOG_INFO("%{public}s: status:0x%{public}02x handle:0x%{public}04x reason:0x%{public}02x", __FUNCTION__,
        status, connectionHandle, reason);
    GapLeDisconnectionCompleteParam *btmParam = MEM_MALLOC.alloc(sizeof(GapLeDisconnectionCompleteParam));
    if (btmParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    btmParam->status = status;
    btmParam->connectionHandle = connectionHandle;
    btmParam->reason = reason;
    btmParam->context = context;

    int ret = GapRunTaskUnBlockProcess(GapLeDisconnectionCompleteTask, btmParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

#endif

static BtmAclCallbacks g_aclCallback = {0};

void GapRegisterBtmAclCallbacks(void)
{
#ifdef GAP_BREDR_SUPPORT
    g_aclCallback.connectionComplete = GapRecvAclConnectionComplete;
    g_aclCallback.disconnectionComplete = GapRecvAclDisconnectionComplete;
#endif
#ifdef GAP_LE_SUPPORT
    g_aclCallback.leConnectionComplete = GapRecvLeConnectionComplete;
    g_aclCallback.leDisconnectionComplete = GapRecvLeDisconnectionComplete;
#endif
    BTM_RegisterAclCallbacks(&g_aclCallback, NULL);
}

void GapDeregisterBtmAclCallbacks(void)
{
    BTM_DeregisterAclCallbacks(&g_aclCallback);
}

static void GapRemoteDeviceSupportHostSecureSimplePairingCallbackTask(void *ctx)
{
    GapRemoteDeviceSupportCallbackParam *param = ctx;
    GapRemoteDeviceSupportHostSecureSimplePairingCallback(&param->addr, param->support);
}

static void GapRecvRemoteDeviceSupportHostSecureSimplePairingCallback(const BtAddr *addr, bool support)
{
    LOG_INFO("%{public}s: " BT_ADDR_FMT " support:%{public}d", __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr->addr), support);
    GapRemoteDeviceSupportCallbackParam *supportParam = MEM_MALLOC.alloc(sizeof(GapRemoteDeviceSupportCallbackParam));
    if (supportParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    supportParam->addr = *addr;
    supportParam->support = support;

    int ret = GapRunTaskUnBlockProcess(GapRemoteDeviceSupportHostSecureSimplePairingCallbackTask, supportParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

void GapIsRemoteDeviceSupportHostSecureSimplePairingAsync(const BtAddr *addr)
{
    BTM_IsRemoteDeviceSupportHostSecureSimplePairing(addr, GapRecvRemoteDeviceSupportHostSecureSimplePairingCallback);
}
