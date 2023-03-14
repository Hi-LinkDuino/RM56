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

#ifndef AUTH_SESSION_COMMON_H
#define AUTH_SESSION_COMMON_H

#include "auth_session_common_defines.h"
#include "common_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
void InformLocalAuthError(const CJson *param, const DeviceAuthCallback *callback);
void InformPeerAuthError(const CJson *param, const DeviceAuthCallback *callback);
int32_t InformAuthError(AuthSession *session, const CJson *out, int errorCode);
int32_t GetAuthParamsList(int32_t osAccountId, const CJson *param, ParamsVec *authParamsVec);
int32_t ProcessTaskStatusForAuth(const AuthSession *session, const CJson *param, CJson *out, int32_t status);
int32_t CreateAndProcessTask(AuthSession *session, CJson *paramInSession, CJson *out, int32_t *status);
void ProcessDeviceLevel(const CJson *receiveData, CJson *authParam);
void DestroyAuthSession(Session *session);
void DeleteCachedData(CJson *paramInSession);
int32_t GetGroupAuthType(int32_t authForm);
void CreateAuthParamsVec(ParamsVec *vec);
void DestroyAuthParamsVec(ParamsVec *vec);
int32_t ReturnSessionKey(int64_t requestId, const CJson *authParam,
    const CJson *out, const DeviceAuthCallback *callback);
#ifdef __cplusplus
}
#endif
#endif
