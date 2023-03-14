/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include<stdio.h>

#include "softbus_adapter_mem.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"

static char const *g_pkgName = "ohos.dsoftbus.tool";

static void PrintNodeProperty(const NodeBasicInfo *nodeInfo)
{
    printf("DeviceName = %s\n", nodeInfo->deviceName);
    printf("NetworkId = %s\n", nodeInfo->networkId);
    NodeDeviceInfoKey key;
    key = NODE_KEY_UDID;
    unsigned char udid[UDID_BUF_LEN] = {0};
    if (GetNodeKeyInfo(g_pkgName, nodeInfo->networkId, key,
        udid, UDID_BUF_LEN) != 0) {
        printf("GetNodeKeyInfo Fail!\n");
    } else {
        printf("Udid = %s\n", udid);
    }
    key = NODE_KEY_UUID;
    unsigned char uuid[UUID_BUF_LEN] = {0};
    if (GetNodeKeyInfo(g_pkgName, nodeInfo->networkId, key,
        uuid, UUID_BUF_LEN) != 0) {
        printf("GetNodeKeyInfo Fail!\n");
    } else {
        printf("Uuid = %s\n", uuid);
    }
}

int main(void)
{
    NodeBasicInfo localNodeinfo;
    NodeBasicInfo *remoteNodeInfo = NULL;
    int32_t infoNum = 0;
    printf("------Local Device Info------\n");
    if (GetLocalNodeDeviceInfo(g_pkgName, &localNodeinfo) != 0) {
        printf("LnnGetLocalNodeInfo Fail!\n");
        return -1;
    }
    PrintNodeProperty(&localNodeinfo);
    printf("------Remote Device Info------\n");
    if (GetAllNodeDeviceInfo(g_pkgName, &remoteNodeInfo, &infoNum) != 0) {
        printf("GetAllNodeDeviceInfo Fail!\n");
        return -1;
    }
    printf("Device Num = %d\n", infoNum);
    for (int i = 0; i < infoNum; i++) {
        printf("\n[No.%d]\n", i + 1);
        PrintNodeProperty(remoteNodeInfo + i);
    }
    FreeNodeInfo(remoteNodeInfo);
    printf("SoftBusDumpDeviceInfo complete!\n");
    return 0;
}
