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

#ifndef BIND_SESSION_COMMON_H
#define BIND_SESSION_COMMON_H

#include "bind_session_common_defines.h"

int32_t AddInfoToSendData(bool isNeedCompatibleInfo, const BindSession *session, CJson *data);
int32_t GenerateBasicModuleParams(bool isClient, BindSession *session, CJson *moduleParams);
int32_t GenerateBindParams(int32_t osAccountId, int isClient, const CJson *jsonParams, BindSession *session);
bool NeedCreateGroup(int isClient, int operationCode);
bool NeedForceDelete(const BindSession *session);
int32_t ForceUnbindDevice(const BindSession *session);
int32_t ProcessBindSession(Session *session, CJson *jsonParams);
#endif
