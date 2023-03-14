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

#include "ability_kit.h"
#include "ability_service.h"

namespace OHOS {
int32_t AbilityKit::RegisterReceiver(const char *bundleName, SuccessCallback success, FailCallback fail,
    uint32_t successCallbackFunc, uint32_t failCallbackFunc, uint32_t context)
{
    return AbilityService::GetInstance().RegisterReceiver(bundleName, success, fail, successCallbackFunc,
        failCallbackFunc, context);
}

int32_t AbilityKit::UnregisterReceiver(const char *bundleName)
{
    return AbilityService::GetInstance().UnregisterReceiver(bundleName);
}

int32_t AbilityKit::SendMsgToPeerApp(bool isLocalMsg, const char *callingPkgName,
    const char *calledPkgName, const uint8_t *msgBody, uint32_t length,
    uint32_t successCallbackFunc, uint32_t failCallbackFunc, uint32_t context)
{
    return AbilityService::GetInstance().SendMsgToPeerApp(isLocalMsg, callingPkgName, calledPkgName,
        msgBody, length, successCallbackFunc, failCallbackFunc, context);
}

int32_t AbilityKit::DetectPhoneApp(const char *callingPkgName, const char *calledPkgName, uint32_t successCallbackFunc,
    uint32_t failCallbackFunc, uint32_t context)
{
    return AbilityService::GetInstance().DetectPhoneApp(callingPkgName, calledPkgName, successCallbackFunc,
        failCallbackFunc, context);
}

int32_t AbilityKit::DetectResourceRelease()
{
    return AbilityService::GetInstance().DetectResourceRelease();
}

int32_t AbilityKit::SendMsgResourceRelease()
{
    return AbilityService::GetInstance().SendMsgResourceRelease();
}
} // namespace OHOS

#ifdef APP_PLATFORM_WATCHGT
extern "C" {
int32_t RegisterReceiver(const char *bundleName, SuccessCallback success, FailCallback fail)
{
    return OHOS::AbilityKit::RegisterReceiver(bundleName, success, fail, 0, 0, 0);
}

int32_t UnregisterReceiver(const char *bundleName)
{
    return OHOS::AbilityKit::UnregisterReceiver(bundleName);
}

int32_t SendMsgToPeerApp(bool isLocalMsg, const char *callingPkgName, const char *calledPkgName,
    const uint8_t *msgBody, uint32_t length)
{
    return OHOS::AbilityKit::SendMsgToPeerApp(isLocalMsg, callingPkgName, calledPkgName, msgBody, length, 0, 0, 0);
}
}
#endif // APP_PLATFORM_WATCHGT
