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

#include <signal.h>
#include "server.h"
#include "wifi_log.h"
#include "wifi_hal_adapter.h"
#include "wifi_hal_ap_interface.h"
#include "wifi_hal_crpc_server.h"
#include "wifi_hal_sta_interface.h"
#include "wifi_hal_p2p_interface.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalService"

static void SignalExit(int sig)
{
    LOGI("Caught signal %{public}d", sig);
    RpcServer *server = GetRpcServer();
    if (server != NULL) {
        StopEventLoop(server->loop);
    }
    return;
}

int main(void)
{
    char rpcSockPath[] = "/data/misc/wifi/unix_sock.sock";

    if (access(rpcSockPath, 0) == 0) {
        unlink(rpcSockPath);
    }
    if (InitRpcFunc() < 0) {
        LOGE("Init Rpc Function failed!");
        return -1;
    }
    RpcServer *server = CreateRpcServer(rpcSockPath);
    if (server == NULL) {
        LOGE("Create RPC Server by %{public}s failed!", rpcSockPath);
        return -1;
    }
    SetRpcServerInited(server);
    setvbuf(stdout, NULL, _IOLBF, 0);
    signal(SIGINT, SignalExit);
    signal(SIGTERM, SignalExit);
    signal(SIGPIPE, SIG_IGN);

    RunRpcLoop(server);
    /* stop wpa_supplicant, hostapd, and other resources */
    ForceStop();
    StopSoftAp();
    P2pForceStop();
    ReleaseWifiHalVendorInterface();
    /* clear RPC Server */
    SetRpcServerInited(NULL);
    ReleaseRpcServer(server);
    ReleaseRpcFunc();
    LOGI("hal service exists!");
    return 0;
}
