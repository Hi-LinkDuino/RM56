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

#ifndef BIND_SESSION_COMMON_UTIL_H
#define BIND_SESSION_COMMON_UTIL_H

#include "bind_session_common_defines.h"

BindSession *CreateBaseBindSession(int32_t sessionType, int32_t opCode, const CJson *params,
    const DeviceAuthCallback *callback, ProcessSessionFunc func);
void DestroyBindSession(Session *session);
void InitClientChannel(const DeviceAuthCallback *callback, const CJson *params, BindSession *session);
void InitServerChannel(const CJson *params, BindSession *session);

bool IsAcceptRequest(const CJson *params);
int32_t CheckPeerStatus(const CJson *params, bool *isNeedInform);
int32_t SendBindSessionData(const BindSession *session, const CJson *sendData);
int32_t CreateAndProcessModule(BindSession *session, const CJson *in, CJson *out);
int32_t ProcessModule(const BindSession *session, const CJson *in, CJson *out, int32_t *status);
void InformPeerGroupErrorIfNeed(bool isNeedInform, int32_t errorCode, const BindSession *session);
void InformPeerModuleError(CJson *out, const BindSession *session);
#endif
