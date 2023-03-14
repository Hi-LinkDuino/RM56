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
#include "i_wifi_iface.h"
#include "client.h"
#include "serial.h"
#include "wifi_log.h"
#include "wifi_idl_inner_interface.h"
#include "i_wifi_public_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiIdlIface"

WifiErrorNo GetName(char *ifname, int32_t size)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetName");
    WriteInt(context, size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetName") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetName deal failed!");
    } else {
        ReadStr(context, ifname, size);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetType(int32_t *type)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetType");
    WriteEnd(context);
    if (RpcClientCall(client, "GetType") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetType deal failed!");
    } else {
        ReadInt(context, type);
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}
