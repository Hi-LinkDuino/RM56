/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pin_auth.h"

#include <memory>
#include <string>

#include "dm_constants.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace DistributedHardware {
const int32_t MAX_VERIFY_TIMES = 3;
PinAuth::PinAuth()
{
    pinAuthUi_ = std::make_shared<PinAuthUi>();
    LOGI("PinAuth constructor");
}

PinAuth::~PinAuth()
{
}

int32_t PinAuth::ShowAuthInfo(std::string &authToken, std::shared_ptr<DmAuthManager> authManager)
{
    nlohmann::json jsonObject = nlohmann::json::parse(authToken, nullptr, false);
    if (jsonObject.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    if (!jsonObject.contains(PIN_CODE_KEY)) {
        LOGE("err json string, first time");
        return DM_FAILED;
    }
    return pinAuthUi_->ShowPinDialog(jsonObject[PIN_CODE_KEY], authManager);
}

int32_t PinAuth::StartAuth(std::string &authToken, std::shared_ptr<DmAuthManager> authManager)
{
    nlohmann::json jsonObject = nlohmann::json::parse(authToken, nullptr, false);
    if (jsonObject.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    if (!jsonObject.contains(PIN_CODE_KEY)) {
        LOGE("err json string, first time");
        return DM_FAILED;
    }
    return pinAuthUi_->InputPinDialog(jsonObject[PIN_CODE_KEY], authManager);
}

int32_t PinAuth::VerifyAuthentication(std::string &authToken, const std::string &authParam)
{
    times_ += 1;
    if (authParam.length() == 1) {
        if (authParam == EVENT_CONFIRM_CODE) {
            return DM_OK;
        }
        LOGE("Peer rejection");
        return DM_FAILED;
    }
    nlohmann::json authParamJson = nlohmann::json::parse(authParam, nullptr, false);
    if (authParamJson.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    nlohmann::json authTokenJson = nlohmann::json::parse(authToken, nullptr, false);
    if (authTokenJson.is_discarded()) {
        LOGE("DecodeRequestAuth jsonStr error");
        return DM_FAILED;
    }
    if (!authParamJson.contains(PIN_CODE_KEY) || !authParamJson.contains(PIN_TOKEN)
        || !authTokenJson.contains(PIN_CODE_KEY) || !authTokenJson.contains(PIN_TOKEN)) {
        LOGE("err json string, first time");
        return DM_FAILED;
    }
    int32_t code = authTokenJson[PIN_CODE_KEY];
    std::string pinToken = authTokenJson[PIN_TOKEN];
    int32_t inputPinCode = authParamJson[PIN_CODE_KEY];
    std::string inputPinToken = authParamJson[PIN_TOKEN];
    if (code == inputPinCode && pinToken == inputPinToken) {
        return DM_OK;
    } else if (code != inputPinCode && times_ < MAX_VERIFY_TIMES) {
        return DM_AUTH_INPUT_FAILED;
    } else {
        return DM_FAILED;
    }
}

int32_t PinAuth::CloseAuthInfo(const int32_t &pageId, std::shared_ptr<DmAuthManager> authManager)
{
    return pinAuthUi_->ClosePage(pageId, authManager);
}

extern "C" IAuthentication *CreatePinAuthObject(void)
{
    return new PinAuth;
}
} // namespace DistributedHardware
} // namespace OHOS
