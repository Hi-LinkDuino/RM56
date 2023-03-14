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

#include "wifi_hal_crpc_common.h"
#include "wifi_hal_crpc_base.h"
#include "wifi_hal_sta_interface.h"
#include "wifi_hal_ap_interface.h"
#include "wifi_hal_p2p_interface.h"
#include "wifi_hal_define.h"

int RpcRegisterEventCallback(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int num = 0;
    if (ReadInt(context, &num) < 0) {
        return -1;
    }
    int *events = ReadIntArray(context, num);
    if (events == NULL) {
        return -1;
    }
    for (int i = 0; i < num; ++i) {
        RegisterCallback(server, events[i], context);
    }
    WriteBegin(context, 0);
    WriteInt(context, WIFI_HAL_SUCCESS);
    WriteEnd(context);
    free(events);
    events = NULL;
    return 0;
}

int RpcUnRegisterEventCallback(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int num = 0;
    if (ReadInt(context, &num) < 0) {
        return -1;
    }
    int *events = ReadIntArray(context, num);
    if (events == NULL) {
        return -1;
    }
    for (int i = 0; i < num; ++i) {
        UnRegisterCallback(server, events[i], context);
    }
    WriteBegin(context, 0);
    WriteInt(context, WIFI_HAL_SUCCESS);
    WriteEnd(context);
    free(events);
    events = NULL;
    return 0;
}

int RpcNotifyClear(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    ForceStop();
    StopSoftAp();
    P2pForceStop();
    WriteBegin(context, 0);
    WriteInt(context, 0);
    WriteEnd(context);
    return 0;
}