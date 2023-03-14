/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "lnn_distributed_net_ledger.h"

#include "softbus_errcode.h"
#include "softbus_log.h"

int32_t LnnInitDistributedLedger(void)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "init virtual distribute ledger");
    return SOFTBUS_OK;
}

void LnnDeinitDistributedLedger(void)
{
}

int32_t LnnGetRemoteStrInfo(const char *networkId, InfoKey key, char *info, uint32_t len)
{
    (void)networkId;
    (void)key;
    (void)info;
    (void)len;
    return SOFTBUS_NOT_IMPLEMENT;
}

int32_t LnnGetDLNumInfo(const char *networkId, InfoKey key, int32_t *info)
{
    (void)networkId;
    (void)key;
    (void)info;
    return SOFTBUS_NOT_IMPLEMENT;
}

int32_t LnnGetAllOnlineNodeInfo(NodeBasicInfo **info, int32_t *infoNum)
{
    (void)info;
    (void)infoNum;
    return SOFTBUS_NOT_IMPLEMENT;
}

NodeInfo *LnnGetNodeInfoById(const char *id, IdCategory type)
{
    (void)id;
    (void)type;
    return NULL;
}

int32_t LnnGetNetworkIdByUuid(const char *uuid, char *buf, uint32_t len)
{
    (void)uuid;
    (void)buf;
    (void)len;
    return SOFTBUS_NOT_IMPLEMENT;
}

bool LnnGetOnlineStateById(const char *id, IdCategory type)
{
    (void)id;
    (void)type;
    return true;
}
