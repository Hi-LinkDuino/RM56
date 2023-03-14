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
#include "softbus_bus_center.h"
#include "rpc_mini_samgr.h"
#include "ipc_skeleton.h"
#include "serializer.h"
#include "samgr_lite.h"
#include "iproxy_client.h"

#define DEFAULT_THREAD_STACK_SIZE 10240

#define IPC_LENGTH 64


void RpcClientMain(void *args)
{
    pthread_setname_np(pthread_self(), "rpc_client");
    if (args == NULL) {
        return;
    }
    printf("%s %d\n", __FUNCTION__, __LINE__);
    static IClientProxy *miniInterface = NULL;

    NodeBasicInfo *nodeInfo[4];
    int32_t infoNum = 4;
    int32_t ret = GetAllNodeDeviceInfo("com.ohos.devicemanagerui", &nodeInfo, &infoNum);
    if (ret != 0) {
        printf("GetAllNodeDeviceInfo failed, error=%d\n", ret);
    } else {
        printf("GetAllNodeDeviceInfo infonum=%d\n", infoNum);
        for (int i = 0; i < infoNum; i++) {
            if (nodeInfo[i] == NULL) {
                printf("node %d info is null\n", i);
                break;
            }
            printf("trusted deviceid %s\n", nodeInfo[i]->networkId);
        }
    }

    if (nodeInfo[0]->networkId == NULL) {
        printf("all nodes are null, get all trust node device info failed\n");
        return;
    }

    IUnknown *miniDefApi = SAMGR_GetInstance()->GetRemoteDefaultFeatureApi(nodeInfo[0]->networkId, "mini_sa_rpc");
    if (miniDefApi == NULL) {
        printf("[%s:%d]\n", __func__, __LINE__);
        return;
    }
    printf("[%s:%d]\n", __func__, __LINE__);
    miniDefApi->QueryInterface(miniDefApi, 0, (void **) &miniInterface);
    IpcIo reply;
    uint8_t tmpData[IPC_LENGTH];
    IpcIoInit(&reply, tmpData, IPC_LENGTH, 0);
    WriteInt32(&reply, *(int32_t *)args);
    miniInterface->Invoke(miniInterface, 1, &reply, NULL, NULL);
}

void RpcClientTest(int32_t* progress)
{
    pthread_t threadId;
    pthread_attr_t threadAttr;
    int ret = pthread_attr_init(&threadAttr);
    if (ret != 0) {
        printf("pthread_attr_init failed %d\n", ret);
        return;
    }

    if (pthread_attr_setstacksize(&threadAttr, DEFAULT_THREAD_STACK_SIZE) != 0) {
        printf("pthread_attr_setstacksize failed\n");
        return;
    }
    int32_t *prog = malloc(sizeof(int32_t));
    if (prog == NULL) {
        return;
    } else {
        *prog = *progress;
    }
    ret = pthread_create(&threadId, &threadAttr, RpcClientMain, (void*)prog);
    if (ret != 0) {
        printf("pthread_create failed %d\n", ret);
        return;
    }
    pthread_detach(threadId);
}
