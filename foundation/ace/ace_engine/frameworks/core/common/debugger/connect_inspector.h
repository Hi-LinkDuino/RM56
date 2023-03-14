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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_CONNECT_INSPECTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_CONNECT_INSPECTOR_H

#include <string>

namespace OHOS::Ace {
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

void StartServer(const std::string& componentName, const bool flagNeedDebugBreakPoint);

void StartUnixSocket(const std::string& componentName);

void StopServer(const std::string& componentName);

void SendMessage(const std::string& message);

void AddMessage(const int32_t instanceId, const std::string& message);

void RemoveMessage(const int32_t instanceId);

bool IsAttachStart();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_CONNECT_INSPECTOR_H
