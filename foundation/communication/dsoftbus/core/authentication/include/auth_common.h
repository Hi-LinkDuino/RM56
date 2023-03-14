/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef AUTH_COMMON_H
#define AUTH_COMMON_H

#include <stdint.h>
#include <string.h>

#include "auth_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

int64_t GetSeq(AuthSideFlag flag);
uint16_t AuthGetNextConnectionId(void);
void UniqueIdInit(void);
void AuthGetAbility(void);
bool AuthIsSupportServerSide(void);
AuthSideFlag AuthGetSideByRemoteSeq(int64_t seq);
int32_t AuthGetDeviceKey(char *key, uint32_t size, uint32_t *len, const ConnectOption *option);
int32_t AuthConvertConnInfo(ConnectOption *option, const ConnectionInfo *connInfo);

int32_t ConvertAuthConnInfoToOption(const AuthConnInfo *info, ConnectOption *option);
int32_t ConvertOptionToAuthConnInfo(const ConnectOption *option, bool isAuthP2p, AuthConnInfo *info);
bool CompareConnectOption(const ConnectOption *option1, const ConnectOption *option2);

#ifdef __cplusplus
}
#endif
#endif /* AUTH_COMMON_H */
