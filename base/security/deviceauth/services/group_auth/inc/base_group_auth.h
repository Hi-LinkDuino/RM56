/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BASE_GROUP_AUTH_H
#define BASE_GROUP_AUTH_H

#include <stdint.h>
#include "auth_session_common_defines.h"
#include "data_manager.h"
#include "device_auth.h"
#include "json_utils.h"

typedef struct {
    void (*onFinish)(int64_t requestId, const CJson *authParam, const CJson *out, const DeviceAuthCallback *callback);
    void (*onError)(int64_t requestId, const AuthSession *session, int errorCode);
    int32_t (*fillDeviceAuthInfo)(int32_t osAccountId, const TrustedGroupEntry *entry,
        const TrustedDeviceEntry *localAuthInfo, CJson *paramsData);
    int32_t (*getAuthParamForServer)(const CJson *dataFromClient, ParamsVec *authParamsVec);
    int32_t (*getReqParams)(const CJson *receiveData, CJson *reqParam);
    int32_t (*combineServerConfirmParams)(const CJson *confirmationJson, CJson *dataFromClient);
    int32_t authType;
} BaseGroupAuth;

#endif
