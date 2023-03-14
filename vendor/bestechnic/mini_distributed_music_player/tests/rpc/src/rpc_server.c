/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "cmsis_os2.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "ohos_init.h"
#include "rpc_mini_samgr.h"
#include "ipc_skeleton.h"
#include "serializer.h"
#include "rpc_log.h"
#include "rpc_errno.h"

#define SAID 16
#define DEFAULT_THREAD_STACK_SIZE 10240
#define TEST_DELAY_MILLISECONDS 20000
#define TEST_SERVER_DELAY_MILLISECONDS 4000
#define IPC_LENGTH 64
#define IPC_LENGTH_LONG 128
#define LWIP_NSC_IPSTATUS_CHANGE          0xf0

enum {
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MULTI = 3,
};

static int32_t RemoteRequestOne(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option)
{
    int32_t result = ERR_NONE;
    RPC_LOG_INFO("server OnRemoteRequestOne called....");
    sleep(1);
    switch (code) {
        case OP_ADD: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            RPC_LOG_INFO("RemoteRequestOne add called a = %d, b = %d", a, b);
            WriteInt32(reply, a + b);
            break;
        }
        case OP_SUB: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            RPC_LOG_INFO("RemoteRequestOne sub called a = %d, b = %d", a, b);
            WriteInt32(reply, a - b);
            break;
        }
        case OP_MULTI: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            RPC_LOG_INFO("RemoteRequestOne mulit called a = %d, b = %d", a, b);
            WriteInt32(reply, a * b);
            break;
        }
        default:
            RPC_LOG_ERROR("unknown code %d", code);
            break;
    }
    return result;
}

static void RpcServerMain(void)
{
    osDelay(TEST_SERVER_DELAY_MILLISECONDS);
    RPC_LOG_INFO("RpcServerMain start");

    IpcObjectStub *objectStubOne = (IpcObjectStub *) calloc(1, sizeof(IpcObjectStub));
    if (objectStubOne == NULL) {
        RPC_LOG_ERROR("objectStubOne calloc failed");
        return;
    }
    objectStubOne->func = RemoteRequestOne;
    objectStubOne->isRemote = true;

    printf("RpcServerMain func %x\n", objectStubOne->func);
    sleep(1);

    IpcIo data;
    uint8_t tmpData[IPC_LENGTH_LONG];
    IpcIoInit(&data, tmpData, IPC_LENGTH_LONG, 0);
    SvcIdentity svcOne = {
            .handle = -1,
            .token  = (uintptr_t) objectStubOne,
            .cookie = (uintptr_t) objectStubOne
    };
    WriteUint32(&data, SAID);
    WriteRemoteObject(&data, &svcOne);
    data.bufferCur = data.bufferBase;
    data.offsetsCur = data.offsetsBase;

    if (AddRemoteSystemAbility(&data) != ERR_NONE) {
        RPC_LOG_INFO("AddRemoteSystemAbility failed");
        return;
    }
    RPC_LOG_INFO("JoinWorkThread start");

    return;
}

static void RpcServerTest(void)
{
    pthread_t threadId2;
    pthread_attr_t threadAttr2;
    int ret = pthread_attr_init(&threadAttr2);
    if (ret != 0) {
        RPC_LOG_ERROR("pthread_attr_init failed %d", ret);
        return ERR_FAILED;
    }

    if (pthread_attr_setstacksize(&threadAttr2, DEFAULT_THREAD_STACK_SIZE) != 0) {
        RPC_LOG_ERROR("pthread_attr_setstacksize failed");
        return ERR_FAILED;
    }

    ret = pthread_create(&threadId2, &threadAttr2, RpcStartSamgr, NULL);
    if (ret != 0) {
        RPC_LOG_ERROR("pthread_create failed %d", ret);
        return ERR_FAILED;
    }
    pthread_detach(threadId2);
}

static void RpcServerWifiDHCPSucCB(struct netif *netif, netif_nsc_reason_t reason,
                                   const netif_ext_callback_args_t *args) {
    (void) args;
    if (netif == NULL) {
        printf("%s %d, error: input netif is NULL!\n", __FUNCTION__, __LINE__);
        return;
    }
    if (reason == LWIP_NSC_IPSTATUS_CHANGE) {
        if (netif_is_up(netif) && !ip_addr_isany(&netif->ip_addr)) {
            printf("%s %d, start rpc server!\n", __FUNCTION__, __LINE__);
            RpcServerTest();
        }
    }
}

static void WifiDHCPRpcServerCB(void)
{
    printf("%s %d\n", __FUNCTION__, __LINE__);
    NETIF_DECLARE_EXT_CALLBACK(WifiReadyRpcServerCallback);
    netif_add_ext_callback(&WifiReadyRpcServerCallback, RpcServerWifiDHCPSucCB);
}

APP_FEATURE_INIT(WifiDHCPRpcServerCB);