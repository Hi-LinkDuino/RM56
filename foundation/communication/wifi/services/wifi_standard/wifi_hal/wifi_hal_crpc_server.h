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

#ifndef OHOS_WIFI_HAL_CRPC_SERVER_H
#define OHOS_WIFI_HAL_CRPC_SERVER_H

#include "server.h" /* RPC Server header file */
#include "wifi_hal_define.h"
#include "wifi_hal_struct.h"
#include "wifi_hal_p2p_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*Rpcfunc)(RpcServer *server, Context *context);

typedef struct WifiHalRpcFunc {
    char funcname[128];
    Rpcfunc func;
    struct WifiHalRpcFunc *next;
} WifiHalRpcFunc;

#define RPC_FUNC_NUM 10
#define RPC_FUNCNAME_MAX_LEN 128

/**
 * @Description Initialization the function table.
 *
 * @return int - 0 Success, -1 Failed.
 */
int InitRpcFunc(void);
/**
 * @Description Release the function table.
 *
 */
void ReleaseRpcFunc(void);
/**
 * @Description Get the Rpc Func object.
 *
 * @param func - Function name string.
 * @return Rpcfunc - Function pointer found.
 */
Rpcfunc GetRpcFunc(const char *func);

/**
 * @Description Set the Rpc Server Inited object.
 *
 * @param server - Pointer to the global structure of the communication server.
 */
void SetRpcServerInited(RpcServer *server);

/**
 * @Description Get the Rpc Server object.
 *
 * @return RpcServer*.
 */
RpcServer *GetRpcServer(void);

typedef struct WifiHalCbIFaceMsg {
    int type;
    char ifname[WIFI_IFACE_NAME_MAXLEN];
} WifiHalCbIFaceMsg;

typedef struct WifiHalConnectMsg {
    int status;
    int networkId;
    char bssid[WIFI_MAC_LENGTH + 1];
} WifiHalConnectMsg;

typedef union WifiHalCallbackMsg {
    int scanStatus;
    WifiHalConnectMsg connMsg;
    WifiHalCbIFaceMsg ifMsg;
    P2pDeviceInfo deviceInfo;
    P2pGroupInfo groupInfo;
    P2pInvitationInfo invitaInfo;
    P2pServDiscRespInfo serverInfo;
    P2pServDiscReqInfo serDiscReqInfo;
} WifiHalCallbackMsg;

typedef struct WifiHalEventCallbackMsg {
    WifiHalCallbackMsg msg;
    struct WifiHalEventCallbackMsg *pre;
    struct WifiHalEventCallbackMsg *next;
} WifiHalEventCallbackMsg;

/* Define callback message processing. */
typedef struct WifiHalEventCallback {
    WifiHalEventCallbackMsg cbmsgs[WIFI_HAL_MAX_EVENT - WIFI_FAILURE_EVENT];
    pthread_mutex_t mutex; /* Message mutex. */
} WifiHalEventCallback;

/**
 * @Description Init Tabele of the Callback Msg.
 *
 * @return int - 0 Success, -1 Failed.
 */
int InitCallbackMsg(void);
/**
 * @Description Release Table of the Callback Msg.
 *
 */
void ReleaseCallbackMsg(void);
/**
 * @Description Add an event node to the header of the corresponding event linked list.
 *
 * @param event - Event id.
 * @param msg
 * @return int - 0 Success, -1 Failed.
 */
int PushBackCallbackMsg(int event, WifiHalEventCallbackMsg *msg);
/**
 * @Description Obtain event nodes from the event list.
 *
 * @param event - Event id.
 * @return int - 0 Success, -1 Failed.
 */
int PopBackCallbackMsg(int event);
/**
 * @Description Obtain the event from the event.
 *
 * @param event - Event id.
 * @return WifiHalEventCallbackMsg*
 */
WifiHalEventCallbackMsg *FrontCallbackMsg(int event);
/**
 * @Description Add an event to the event list.
 *
 * @param event - Event id.
 * @return int - 0 Success, -1 Failed.
 */
int PopFrontCallbackMsg(int event);

#ifdef __cplusplus
}
#endif
#endif