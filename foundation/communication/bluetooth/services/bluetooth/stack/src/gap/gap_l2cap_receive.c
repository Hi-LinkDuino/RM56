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

#include "gap_internal.h"
#include "gap_task_internal.h"

#include <string.h>

#include "allocator.h"
#include "log.h"

typedef struct {
    uint16_t aclHandle;
    uint8_t id;
    L2capLeConnectionParameter param;
    void *ctx;
} GapReceiveL2capParameterUpdateReqParam;

typedef struct {
    uint16_t aclHandle;
    uint16_t result;
    void *ctx;
} GapReceiveL2capParameterUpdateRspParam;

static void GapReceiveL2capParameterUpdateReqTask(void *ctx)
{
    GapReceiveL2capParameterUpdateReqParam *param = ctx;
    GapReceiveL2capParameterUpdateReq(param->aclHandle, param->id, &param->param, param->ctx);
}

static void GapRecvL2capParameterUpdateReq(
    uint16_t aclHandle, uint8_t id, const L2capLeConnectionParameter *param, void *ctx)
{
    LOG_INFO("%{public}s: handle:0x%04x", __FUNCTION__, aclHandle);
    GapReceiveL2capParameterUpdateReqParam *l2capParam =
        MEM_MALLOC.alloc(sizeof(GapReceiveL2capParameterUpdateReqParam));
    if (l2capParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    l2capParam->aclHandle = aclHandle;
    l2capParam->id = id;
    l2capParam->param = *param;
    l2capParam->ctx = ctx;

    int ret = GapRunTaskUnBlockProcess(GapReceiveL2capParameterUpdateReqTask, l2capParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static void GapReceiveL2capParameterUpdateRspTask(void *ctx)
{
    GapReceiveL2capParameterUpdateRspParam *param = ctx;
    GapReceiveL2capParameterUpdateRsp(param->aclHandle, param->result, param->ctx);
}

static void GapRecvL2capParameterUpdateRsp(uint16_t aclHandle, uint16_t result, void *ctx)
{
    LOG_INFO("%{public}s: handle:0x%04x result:0x%04x", __FUNCTION__, aclHandle, result);
    GapReceiveL2capParameterUpdateRspParam *l2capParam =
        MEM_MALLOC.alloc(sizeof(GapReceiveL2capParameterUpdateRspParam));
    if (l2capParam == NULL) {
        LOG_ERROR("%{public}s: Alloc error.", __FUNCTION__);
        return;
    }

    l2capParam->aclHandle = aclHandle;
    l2capParam->result = result;
    l2capParam->ctx = ctx;

    int ret = GapRunTaskUnBlockProcess(GapReceiveL2capParameterUpdateRspTask, l2capParam, NULL);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: Task error:%{public}d.", __FUNCTION__, ret);
    }
}

static L2capLeConnectionParameterUpdate g_l2capParamUpdate = {
    .recvLeConnectionParameterUpdateReq = GapRecvL2capParameterUpdateReq,
    .recvLeConnectionParameterUpdateRsp = GapRecvL2capParameterUpdateRsp,
};

void GapRegisterL2capCallbacks(void)
{
    L2CIF_LeRegisterConnectionParameterUpdate(&g_l2capParamUpdate, NULL);
}

void GapDeregisterL2capCallbacks(void)
{
    L2CIF_LeDeregisterConnectionParameterUpdate();
}

void GapLeConnectionParameterUpdateReqResult(uint16_t aclHandle, int result)
{
    if (result != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: aclHandle:0x%04x result:%{public}d", __FUNCTION__, aclHandle, result);
    }
}

void GapLeConnectionParameterUpdateRspResult(uint16_t aclHandle, int result)
{
    if (result != BT_NO_ERROR) {
        LOG_ERROR("%{public}s: aclHandle:0x%04x result:%{public}d", __FUNCTION__, aclHandle, result);
    }
}

void GapLeConnectionParameterUpdateReq(uint16_t aclHandle, const L2capLeConnectionParameter *param)
{
    L2CIF_LeConnectionParameterUpdateReq(aclHandle, param, GapLeConnectionParameterUpdateReqResult);
}

void GapLeConnectionParameterUpdateRsp(uint16_t aclHandle, uint8_t id, uint16_t result)
{
    L2CIF_LeConnectionParameterUpdateRsp(aclHandle, id, result, GapLeConnectionParameterUpdateRspResult);
}