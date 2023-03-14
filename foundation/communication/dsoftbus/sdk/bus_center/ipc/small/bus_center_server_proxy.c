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

#include "bus_center_server_proxy.h"

#include "securec.h"

#include "iproxy_client.h"
#include "samgr_lite.h"
#include "serializer.h"
#include "softbus_adapter_file.h"
#include "softbus_adapter_mem.h"
#include "softbus_adapter_timer.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_ipc_def.h"
#include "softbus_log.h"

#define WAIT_SERVER_READY_INTERVAL_COUNT 50

typedef enum {
    GET_ALL_ONLINE_NODE_INFO = 0,
    GET_LOCAL_DEVICE_INFO,
    GET_NODE_KEY_INFO,
    ACTIVE_META_NODE,
    DEACTIVE_META_NODE,
    GET_ALL_META_NODE,
} FunID;

typedef struct {
    FunID id;
    int32_t arg1;
    int32_t retCode;
    void* data;
} Reply;

static IClientProxy *g_serverProxy = NULL;

static int32_t ClientBusCenterResultCb(IOwner owner, int code, IpcIo *reply)
{
    Reply *info = (Reply *)owner;
    uint32_t infoSize;
    switch (info->id) {
        case GET_ALL_ONLINE_NODE_INFO:
            info->arg1 = IpcIoPopInt32(reply);
            if (info->arg1 > 0) {
                info->data = (void *)IpcIoPopFlatObj(reply, &infoSize);
            }
            break;
        case GET_LOCAL_DEVICE_INFO:
        case GET_NODE_KEY_INFO:
            info->data = (void *)IpcIoPopFlatObj(reply, &infoSize);
            break;
        case ACTIVE_META_NODE:
            info->retCode = IpcIoPopInt32(reply);
            if (info->retCode == SOFTBUS_OK) {
                info->data = (void *)IpcIoPopString(reply, &infoSize);
                if (infoSize != (NETWORK_ID_BUF_LEN - 1)) {
                    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "invalid meta node id length: %d", infoSize);
                    return SOFTBUS_ERR;
                }
            }
            break;
        case DEACTIVE_META_NODE:
            info->retCode = IpcIoPopInt32(reply);
            break;
        case GET_ALL_META_NODE:
            info->retCode = IpcIoPopInt32(reply);
            if (info->retCode == SOFTBUS_OK) {
                info->arg1 = IpcIoPopInt32(reply);
                if (info->arg1 > 0) {
                    info->data = (void *)IpcIoPopFlatObj(reply, &infoSize);
                }
            }
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "unknown funid");
            return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t BusCenterServerProxyInit(void)
{
    if (g_serverProxy != NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "server proxy has initialized.");
        return SOFTBUS_OK;
    }

    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "bus center start get server proxy");
    int32_t proxyInitCount = 0;
    while (g_serverProxy == NULL) {
        proxyInitCount++;
        if (proxyInitCount == WAIT_SERVER_READY_INTERVAL_COUNT) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "bus center get server proxy error");
            return SOFTBUS_ERR;
        }

        IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(SOFTBUS_SERVICE);
        if (iUnknown == NULL) {
            SoftBusSleepMs(WAIT_SERVER_READY_INTERVAL);
            continue;
        }

        int32_t ret = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&g_serverProxy);
        if (ret != EC_SUCCESS || g_serverProxy == NULL) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "QueryInterface failed [%d]", ret);
            SoftBusSleepMs(WAIT_SERVER_READY_INTERVAL);
            continue;
        }
    }
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "bus center get server proxy ok");
    return SOFTBUS_OK;
}

void BusCenterServerProxyDeInit(void)
{
    g_serverProxy = NULL;
}

int ServerIpcGetAllOnlineNodeInfo(const char *pkgName, void **info, uint32_t infoTypeLen, int32_t *infoNum)
{
    if (info == NULL || infoNum == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetAllOnlineNodeInfo g_serverProxy is NULL!\n");
        return SOFTBUS_ERR;
    }
    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushUint32(&request, infoTypeLen);
    Reply reply = {0};
    reply.id = GET_ALL_ONLINE_NODE_INFO;
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_GET_ALL_ONLINE_NODE_INFO, &request, &reply,
        ClientBusCenterResultCb);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetAllOnlineNodeInfo invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    *infoNum = reply.arg1;
    int32_t infoSize = (*infoNum) * (int32_t)infoTypeLen;
    *info = NULL;
    if (infoSize > 0) {
        if (reply.data == NULL) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetAllOnlineNodeInfo read node info failed!");
            return SOFTBUS_ERR;
        }
        *info = SoftBusMalloc(infoSize);
        if (*info == NULL) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetAllOnlineNodeInfo malloc failed!");
            return SOFTBUS_ERR;
        }
        if (memcpy_s(*info, infoSize, reply.data, infoSize) != EOK) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetAllOnlineNodeInfo copy node info failed!");
            SoftBusFree(*info);
            *info = NULL;
            return SOFTBUS_ERR;
        }
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcGetLocalDeviceInfo(const char *pkgName, void *info, uint32_t infoTypeLen)
{
    if (info == NULL) {
        return SOFTBUS_ERR;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetLocalDeviceInfo g_serverProxy is nullptr!\n");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushUint32(&request, infoTypeLen);
    Reply reply = {0};
    reply.id = GET_LOCAL_DEVICE_INFO;
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_GET_LOCAL_DEVICE_INFO, &request, &reply,
        ClientBusCenterResultCb);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetLocalDeviceInfo invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    if (reply.data == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetLocalDeviceInfo read node info failed!");
        return SOFTBUS_ERR;
    }
    if (memcpy_s(info, infoTypeLen, reply.data, infoTypeLen) != EOK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetLocalDeviceInfo copy node info failed!");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcGetNodeKeyInfo(const char *pkgName, const char *networkId, int key, unsigned char *buf, uint32_t len)
{
    if (networkId == NULL || buf == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "params are nullptr!");
        return SOFTBUS_ERR;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetNodeKeyInfo g_serverProxy is nullptr!\n");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushString(&request, networkId);
    IpcIoPushInt32(&request, key);
    IpcIoPushInt32(&request, len);
    Reply reply = {0};
    reply.id = GET_NODE_KEY_INFO;
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_GET_NODE_KEY_INFO, &request, &reply,
        ClientBusCenterResultCb);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetNodeKeyInfo invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    if (reply.data == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetNodeKeyInfo read retBuf failed!");
        return SOFTBUS_ERR;
    }
    if (memcpy_s(buf, len, reply.data, len) != EOK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "GetNodeKeyInfo copy node key info failed");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int ServerIpcJoinLNN(const char *pkgName, void *addr, unsigned int addrTypeLen)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "join Lnn ipc client push.");
    if (addr == NULL || pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcJoinLNN g_serverProxy is nullptr!\n");
        return SOFTBUS_ERR;
    }
    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushUint32(&request, addrTypeLen);
    IpcIoPushFlatObj(&request, addr, addrTypeLen);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_JOIN_LNN, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "join Lnn invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int ServerIpcLeaveLNN(const char *pkgName, const char *networkId)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "leave Lnn ipc client push.");
    if (pkgName == NULL || networkId == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcLeaveLNN g_serverProxy is nullptr!\n");
        return SOFTBUS_ERR;
    }
    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushString(&request, networkId);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_LEAVE_LNN, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "leave Lnn invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcStartTimeSync(const char *pkgName, const char *targetNetworkId, int32_t accuracy, int32_t period)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "start time sync ipc client push.");
    if (targetNetworkId == NULL || pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_ERR;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcStartTimeSync g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushString(&request, targetNetworkId);
    IpcIoPushInt32(&request, accuracy);
    IpcIoPushInt32(&request, period);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_START_TIME_SYNC, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "StartTimeSync invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcStopTimeSync(const char *pkgName, const char *targetNetworkId)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "stop time sync ipc client push.");
    if (targetNetworkId == NULL || pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_ERR;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcStopTimeSync g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushString(&request, targetNetworkId);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_STOP_TIME_SYNC, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "StopTimeSync invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcPublishLNN(const char *pkgName, const void *info, uint32_t infoLen)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "publish Lnn ipc client push.");
    if (info == NULL || pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcPublishLNN g_serverProxy is nullptr!\n");
        return SOFTBUS_ERR;
    }
    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushUint32(&request, infoLen);
    IpcIoPushFlatObj(&request, info, infoLen);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_PUBLISH_LNN, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "publish Lnn invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcStopPublishLNN(const char *pkgName, int32_t publishId)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "stop publish lnn ipc client push.");
    if (pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_ERR;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcStopPublishLNN g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushInt32(&request, publishId);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_STOP_PUBLISH_LNN, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcStopPublishLNN invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcRefreshLNN(const char *pkgName, const void *info, uint32_t infoTypeLen)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "refresh Lnn ipc client push.");
    if (info == NULL || pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcRefreshLNN g_serverProxy is nullptr!\n");
        return SOFTBUS_ERR;
    }
    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushUint32(&request, infoTypeLen);
    IpcIoPushFlatObj(&request, info, infoTypeLen);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_REFRESH_LNN, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "refresh Lnn invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcStopRefreshLNN(const char *pkgName, int32_t refreshId)
{
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "stop refresh lnn ipc client push.");
    if (pkgName == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_ERR;
    }
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcStopRefreshLNN g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushInt32(&request, refreshId);
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_STOP_REFRESH_LNN, &request, NULL, NULL);
    if (ans != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcStopRefreshLNN invoke failed[%d].", ans);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcActiveMetaNode(const char *pkgName, const MetaNodeConfigInfo *info, char *metaNodeId)
{
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcActiveMetaNode g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN_EX] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN_EX, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushFlatObj(&request, info, sizeof(MetaNodeConfigInfo));
    Reply reply = {0};
    reply.id = ACTIVE_META_NODE;
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_ACTIVE_META_NODE, &request, &reply,
        ClientBusCenterResultCb);
    if (ans != SOFTBUS_OK || reply.retCode != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcActiveMetaNode invoke failed[%d, %d].",
            ans, reply.retCode);
        return SOFTBUS_ERR;
    }
    if (reply.data == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcActiveMetaNode read data failed!");
        return SOFTBUS_ERR;
    }
    if (strncpy_s(metaNodeId, NETWORK_ID_BUF_LEN, (char *)reply.data, strlen((char *)reply.data)) != EOK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcActiveMetaNode copy meta node id failed");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcDeactiveMetaNode(const char *pkgName, const char *metaNodeId)
{
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcDeactiveMetaNode g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushString(&request, metaNodeId);
    Reply reply = {0};
    reply.id = DEACTIVE_META_NODE;
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_DEACTIVE_META_NODE, &request,
        &reply, ClientBusCenterResultCb);
    if (ans != SOFTBUS_OK || reply.retCode != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcDeactiveMetaNode invoke failed[%d, %d]",
            ans, reply.retCode);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t ServerIpcGetAllMetaNodeInfo(const char *pkgName, MetaNodeInfo *infos, int32_t *infoNum)
{
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetAllMetaNodeInfo g_serverProxy is nullptr!");
        return SOFTBUS_ERR;
    }

    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};
    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 0);
    IpcIoPushString(&request, pkgName);
    IpcIoPushInt32(&request, *infoNum);
    Reply reply = {0};
    reply.id = GET_ALL_META_NODE;
    /* asynchronous invocation */
    int32_t ans = g_serverProxy->Invoke(g_serverProxy, SERVER_GET_ALL_META_NODE_INFO, &request, &reply,
        ClientBusCenterResultCb);
    if (ans != SOFTBUS_OK || reply.retCode != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetAllMetaNodeInfo invoke failed[%d, %d]",
            ans, reply.retCode);
        return SOFTBUS_ERR;
    }
    if (reply.arg1 > 0) {
        if (reply.data == NULL) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetAllMetaNodeInfo read meta node info failed!");
            return SOFTBUS_ERR;
        }
        if (memcpy_s(infos, *infoNum * sizeof(MetaNodeInfo), reply.data, reply.arg1 * sizeof(MetaNodeInfo)) != EOK) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ServerIpcGetAllMetaNodeInfo copy meta node info failed");
            return SOFTBUS_ERR;
        }
    }
    *infoNum = reply.arg1;
    return SOFTBUS_OK;
}
