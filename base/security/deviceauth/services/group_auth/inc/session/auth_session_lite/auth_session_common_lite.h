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

#ifndef AUTH_SESSION_COMMON_LITE_H
#define AUTH_SESSION_COMMON_LITE_H

#include "base_session.h"
#include "common_defs.h"
#include "json_utils.h"
#include "device_auth.h"

typedef struct AuthSessionLiteT {
    Session base;
    int32_t curTaskId;
    CJson *authParams;
} AuthSessionLite;

#ifdef __cplusplus
extern "C" {
#endif
int32_t ReturnTransmitDataLite(const AuthSessionLite *session, CJson *out);
void InformLocalAuthErrorLite(const CJson *authParam, const DeviceAuthCallback *callback);
void InformPeerAuthErrorLite(const CJson *in, const DeviceAuthCallback *callback);
void InformAuthErrorLite(const CJson *in, const DeviceAuthCallback *callback, const CJson *out, int errorCode);
int32_t CreateAndProcessLiteTask(AuthSessionLite *session, CJson *out, int32_t *status);
int32_t ProcessLiteTaskStatusForAuth(const AuthSessionLite *session, CJson *out, int32_t status);
void DestroyAuthSessionLite(Session *session);
AuthSessionLite *InitAuthSessionLite(const CJson *in, const DeviceAuthCallback *callback);
#ifdef __cplusplus
}
#endif
#endif
