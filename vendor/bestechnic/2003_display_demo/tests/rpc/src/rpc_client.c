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
#include "ohos_init.h"
#include "rpc_mini_samgr.h"
#include "ipc_skeleton.h"
#include "serializer.h"
#include "rpc_log.h"
#include "rpc_errno.h"

#define SAID 16
#define DEVICEID "0.0.0.0"
#define DEFAULT_THREAD_STACK_SIZE 10240
#define TEST_DELAY_MILLISECONDS 20000
#define TEST_CLIENT_DELAY_MILLISECONDS 10000
#define NUMBER_A 12
#define NUMBER_B 17

#define IPC_LENGTH 64
#define IPC_LENGTH_LONG 128

enum {
    OP_ADD = 1,
    OP_SUB = 2,
    OP_MULTI = 3,
};

static void RpcClientMain(void)
{
    osDelay(TEST_CLIENT_DELAY_MILLISECONDS);
    RPC_LOG_INFO("RpcClientMain start");

    IpcIo reply;
    uint8_t tmpData[IPC_LENGTH_LONG];
    IpcIoInit(&reply, tmpData, IPC_LENGTH_LONG, 0);
    if (GetRemoteSystemAbility(SAID, DEVICEID, &reply) != ERR_NONE) {
        RPC_LOG_INFO("GetRemoteSystemAbility failed");
        return;
    }

    SvcIdentity svc;
    ReadRemoteObject(&reply, &svc);

    IpcIo data2;
    uint8_t tmpData2[IPC_LENGTH];
    IpcIoInit(&data2, tmpData2, IPC_LENGTH, 0);
    WriteInt32(&data2, NUMBER_A);
    WriteInt32(&data2, NUMBER_B);
    RPC_LOG_INFO("RpcClientMain serializer");

    IpcIo reply2;
    uintptr_t ptr2 = 0;
    MessageOption option;
    MessageOptionInit(&option);
    int32_t ret = SendRequest(svc, OP_ADD, &data2, &reply2, option, &ptr2);
    if (ret != ERR_NONE) {
        RPC_LOG_INFO("OP_ADD failed");
        FreeBuffer((void *)ptr2);
        return;
    }
    RPC_LOG_INFO("RpcClientMain SendRequest");

    int32_t sum;
    ReadInt32(&reply2, &sum);
    RPC_LOG_INFO("%d + %d = %d", NUMBER_A, NUMBER_B, sum);
    FreeBuffer((void *)ptr2);

    return;
}

static void RpcClientTest(void)
{
    osDelay(TEST_DELAY_MILLISECONDS);
    printf("[%s:%d]: %s\n", __FILE__, __LINE__, __func__);
    printf("RpcClientTest\n");

    pthread_t threadId;
    pthread_attr_t threadAttr;
    int ret = pthread_attr_init(&threadAttr);
    if (ret != 0) {
        RPC_LOG_ERROR("pthread_attr_init failed %d", ret);
        return ERR_FAILED;
    }

    if (pthread_attr_setstacksize(&threadAttr, DEFAULT_THREAD_STACK_SIZE) != 0) {
        RPC_LOG_ERROR("pthread_attr_setstacksize failed");
        return ERR_FAILED;
    }

    ret = pthread_create(&threadId, &threadAttr, RpcStartSamgr, NULL);
    if (ret != 0) {
        RPC_LOG_ERROR("pthread_create failed %d", ret);
        return ERR_FAILED;
    }
    pthread_detach(threadId);

    pthread_t threadId2;
    pthread_attr_t threadAttr2;
    ret = pthread_attr_init(&threadAttr2);
    if (ret != 0) {
        RPC_LOG_ERROR("pthread_attr_init failed %d", ret);
        return ERR_FAILED;
    }

    if (pthread_attr_setstacksize(&threadAttr2, DEFAULT_THREAD_STACK_SIZE) != 0) {
        RPC_LOG_ERROR("pthread_attr_setstacksize failed");
        return ERR_FAILED;
    }

    ret = pthread_create(&threadId2, &threadAttr2, RpcClientMain, NULL);
    if (ret != 0) {
        RPC_LOG_ERROR("pthread_create failed %d", ret);
        return ERR_FAILED;
    }
    pthread_detach(threadId2);
}

APP_FEATURE_INIT(RpcClientTest);