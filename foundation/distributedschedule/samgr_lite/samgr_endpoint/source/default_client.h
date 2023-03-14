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
#ifndef LITE_DEFAULT_CLIENT_H
#define LITE_DEFAULT_CLIENT_H

#ifndef MINI_SAMGR_LITE_RPC
#include <liteipc_adapter.h>
#endif

#include "iunknown.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef struct SaName SaName;
struct SaName {
    const char *service;
    const char *feature;
};

#ifndef MINI_SAMGR_LITE_RPC
IUnknown *SAMGR_CreateIProxy(const IpcContext *context, const char *service, const char *feature);
#else
IUnknown *SAMGR_CreateIProxy(const char *service, const char *feature);
IUnknown *SAMGR_CreateIRemoteProxy(const char *deviceId, const char *service, const char *feature);
#endif

SaName *SAMGR_GetSAName(const IUnknown *proxy);
int SAMGR_CompareSAName(const SaName *key1, const SaName *key2);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // LITE_DEFAULT_CLIENT_H
