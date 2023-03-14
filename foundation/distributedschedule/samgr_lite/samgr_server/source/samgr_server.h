/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#ifndef LITE_IPC_MANAGER_H
#define LITE_IPC_MANAGER_H

#include <ipc_auth_interface.h>
#include "service.h"
#include "iproxy_server.h"
#include "endpoint.h"
#include "sa_store.h"
#ifdef MINI_SAMGR_LITE_RPC
#include "utils_list.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define SAMGR_SERVICE "samgr"
#define MAX_SYSCAP_NAME_LEN 64
typedef struct SamgrServer SamgrServer;
typedef struct SamgrProxy SamgrProxy;
typedef struct SysCapImpl SysCapImpl;
typedef enum MsgId {
    MSG_CLEAN,
} MsgId;
struct SamgrProxy {
    INHERIT_SERVER_IPROXY;
};

struct SamgrServer {
    INHERIT_SERVICE;
    INHERIT_IPROXY_ENTRY(SamgrProxy);
    Identity identity;
    Endpoint *samgr;
    MutexId mtx;
    SAStore store;
    IpcAuthInterface *ipcAuth;
    MutexId sysCapMtx;
    Vector sysCapabilitys;
};

struct SysCapImpl {
    char name[MAX_SYSCAP_NAME_LEN + 1];
    BOOL isRegister;
};

#ifdef MINI_SAMGR_LITE_RPC
typedef struct {
    UTILS_DL_LIST list;
    SaName saName;
    uintptr_t saId;
    int token;
} SaNode;
SaNode *GetSaNodeBySaName(const char *service, const char *feature);
SaNode *GetSaNodeBySaId(uintptr_t saId);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // LITE_IPC_MANAGER_H
