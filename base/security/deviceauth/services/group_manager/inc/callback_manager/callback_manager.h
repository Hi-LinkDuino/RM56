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

#ifndef CALLBACK_MANAGER_H
#define CALLBACK_MANAGER_H

#include "device_auth.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ProcessTransmitCallback(int64_t requestId, const uint8_t *data, uint32_t dataLen,
    const DeviceAuthCallback *callback);
char *ProcessRequestCallback(int64_t requestId, int operationCode, const char *reqParams,
    const DeviceAuthCallback *callback);
void ProcessFinishCallback(int64_t requestId, int operationCode, const char *returnData,
    const DeviceAuthCallback *callback);
void ProcessSessionKeyCallback(int64_t requestId, const uint8_t *sessionKey, uint32_t sessionKeyLen,
    const DeviceAuthCallback *callback);
void ProcessErrorCallback(int64_t requestId, int operationCode, int errorCode, const char *errorReturn,
    const DeviceAuthCallback *callback);

int32_t InitCallbackManager(void);
void DestroyCallbackManager(void);
int32_t RegGroupManagerCallback(const char *appId, const DeviceAuthCallback *callback);
int32_t UnRegGroupManagerCallback(const char *appId);
const DeviceAuthCallback *GetGMCallbackByAppId(const char *appId);

#ifdef __cplusplus
}
#endif
#endif
