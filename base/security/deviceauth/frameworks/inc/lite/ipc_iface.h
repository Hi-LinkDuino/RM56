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

#ifndef IPC_IFACE_H
#define IPC_IFACE_H

#include "iproxy_server.h"
#include "samgr_lite.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DevAuthOpType {
    DEV_AUTH_CALL_REQUEST = 1,
    DEV_AUTH_CALLBACK_REQUEST = 2,
};

typedef struct {
    INHERIT_SERVER_IPROXY;
} LiteIpcProxyApi;

typedef struct {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(LiteIpcProxyApi);
    Identity identity;
} LiteIpcCallService;

#ifdef __cplusplus
}
#endif
#endif