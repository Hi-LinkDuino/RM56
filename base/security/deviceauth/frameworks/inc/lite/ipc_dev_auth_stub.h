/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IPC_DEV_AUTH_STUB_H
#define IPC_DEV_AUTH_STUB_H

#include "ipc_iface.h"
#include "ipc_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEV_AUTH_SERVICE_NAME "devauth_svc"

typedef struct {
    IpcServiceCall method;
    int32_t methodId;
} IpcServiceCallMap;
#define MAX_CALLMAP_SIZE 64
#define MAX_CBSTUB_SIZE 64

typedef LiteIpcCallService DevAuthService;

void InitCbStubTable();
void ResetCallMap(void);
int32_t SetCallMap(IpcServiceCall method, int32_t methodId);
int32_t SetRemoteObject(const SvcIdentity *object);
void AddCbDeathRecipient(int32_t cbStubIdx, int32_t cbDataIdx);
void ResetRemoteObject(int32_t idx);
void ActCallback(int32_t objIdx, int32_t callbackId, uintptr_t cbHook, IpcIo *dataParcel, IpcIo *reply);
int32_t OnRemoteRequest(IServerProxy *iProxy, int32_t reqId, void *origin, IpcIo *req, IpcIo *reply);

#ifdef __cplusplus
}
#endif
#endif
