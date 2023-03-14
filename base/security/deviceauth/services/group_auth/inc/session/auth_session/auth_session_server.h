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

#ifndef AUTH_SESSION_SERVER_H
#define AUTH_SESSION_SERVER_H

#include "auth_session_common_defines.h"
#include "device_auth.h"
#include "json_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
Session *CreateServerAuthSession(CJson *param, const DeviceAuthCallback *callback);
#ifdef __cplusplus
}
#endif
#endif
