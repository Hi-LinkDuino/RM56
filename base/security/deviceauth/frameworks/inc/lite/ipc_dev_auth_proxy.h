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

#ifndef IPC_DEV_AUTH_PROXY_H
#define IPC_DEV_AUTH_PROXY_H

#include <stdbool.h>
#include <stdint.h>
#include "ipc_iface.h"
#include "iproxy_client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    IpcIo *reply;
    IpcIo *data;
    IpcIo *tmpData;
    int32_t paramCnt;
    SvcIdentity cbSvc;
    int32_t ioBuffOffset;
    bool withCallback;
} ProxyDevAuthData;

int32_t EncodeCallRequest(ProxyDevAuthData *dataCtx, int32_t type, const uint8_t *param, int32_t paramSz);
int32_t FinalCallRequest(ProxyDevAuthData *dataCtx, int32_t methodId);
int32_t ActCall(const IClientProxy *clientInst, ProxyDevAuthData *dataCtx);
void SetCallbackStub(ProxyDevAuthData *dataCtx, const SvcIdentity *cbSvc);
const IClientProxy *GetProxyInstance(const char *serviceName);

#ifdef __cplusplus
}
#endif
#endif
