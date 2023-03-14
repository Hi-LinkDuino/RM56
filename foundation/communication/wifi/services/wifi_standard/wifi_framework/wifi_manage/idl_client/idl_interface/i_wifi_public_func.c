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
#include "i_wifi_public_func.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "OHWIFI_IDLCLIENT_I_WIFI_PUBLIC_FUNC"

WifiErrorNo RpcClientCall(RpcClient *client, const char *func)
{
    if (client == NULL) {
        return WIFI_IDL_OPT_FAILED;
    }
    int ret = RemoteCall(client);
    if (ret < 0) {
        LOGE("remote call %s failed!", ((func == NULL) ? "" : func));
        UnlockRpcClient(client);
        return WIFI_IDL_OPT_FAILED;
    }
    return WIFI_IDL_OPT_OK;
}
