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

#ifndef SOFTBUS_IPC_DEF_H
#define SOFTBUS_IPC_DEF_H

#define SOFTBUS_SERVER_SA_ID_INNER 4700

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_SOFT_BUS_IPC_LEN 512
#define MAX_SOFT_BUS_IPC_LEN_EX 2048
#define SOFTBUS_SERVICE "softbus_service"

struct CommonScvId {
    unsigned int handle;
    unsigned int token;
    unsigned int cookie;
    void *ipcCtx;
    unsigned int cbId;
};

enum SoftBusFuncId {
    MANAGE_REGISTER_SERVICE = 0,

    SERVER_PUBLISH_SERVICE = 128,
    SERVER_UNPUBLISH_SERVICE,

    SERVER_CREATE_SESSION_SERVER,
    SERVER_REMOVE_SESSION_SERVER,
    SERVER_OPEN_SESSION,
    SERVER_OPEN_AUTH_SESSION,
    SERVER_NOTIFY_AUTH_SUCCESS,
    SERVER_CLOSE_CHANNEL,
    SERVER_SESSION_SENDMSG,
    SERVER_QOS_REPORT,

    SERVER_START_DISCOVERY,
    SERVER_STOP_DISCOVERY,

    SERVER_JOIN_LNN,
    SERVER_LEAVE_LNN,
    SERVER_GET_ALL_ONLINE_NODE_INFO,
    SERVER_GET_LOCAL_DEVICE_INFO,
    SERVER_GET_NODE_KEY_INFO,
    SERVER_START_TIME_SYNC,
    SERVER_STOP_TIME_SYNC,
    SERVER_PUBLISH_LNN,
    SERVER_STOP_PUBLISH_LNN,
    SERVER_REFRESH_LNN,
    SERVER_STOP_REFRESH_LNN,
    SERVER_ACTIVE_META_NODE,
    SERVER_DEACTIVE_META_NODE,
    SERVER_GET_ALL_META_NODE_INFO,

    CLIENT_ON_CHANNEL_OPENED = 256,
    CLIENT_ON_CHANNEL_OPENFAILED,
    CLIENT_ON_CHANNEL_LINKDOWN,
    CLIENT_ON_CHANNEL_CLOSED,
    CLIENT_ON_CHANNEL_MSGRECEIVED,
    CLIENT_ON_CHANNEL_QOSEVENT,

    CLIENT_DISCOVERY_SUCC,
    CLIENT_DISCOVERY_FAIL,
    CLIENT_DISCOVERY_DEVICE_FOUND,
    CLIENT_PUBLISH_SUCC,
    CLIENT_PUBLISH_FAIL,

    CLIENT_ON_JOIN_RESULT,
    CLIENT_ON_LEAVE_RESULT,
    CLIENT_ON_NODE_ONLINE_STATE_CHANGED,
    CLIENT_ON_NODE_BASIC_INFO_CHANGED,
    CLIENT_ON_TIME_SYNC_RESULT,
    CLIENT_ON_PUBLISH_LNN_RESULT,
    CLIENT_ON_REFRESH_LNN_RESULT,
    CLIENT_ON_REFRESH_DEVICE_FOUND,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif