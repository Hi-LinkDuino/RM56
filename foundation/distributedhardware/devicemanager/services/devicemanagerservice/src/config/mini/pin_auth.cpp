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

#include "dm_constants.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace DistributedHardware {
const int32_t MAX_VERIFY_TIMES = 3;
PinAuth::PinAuth()
{
    LOGI("PinAuth constructor");
}

PinAuth::~PinAuth()
{
}

int32_t PinAuth::CloseAuthInfo(const int32_t &pageId, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("ClosePage hap start");
    if (authManager == nullptr) {
        LOGE("PinAuthUi::authManager is null");
        return DM_FAILED;
    }
    LOGI("ClosePage hap end");
    authManager->CancelDisplay();
    return DM_OK;
}

int32_t PinAuth::ShowAuthInfo(std::string &authToken, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("ShowConfigDialog end");
    return DM_OK;
}

int32_t PinAuth::StartAuth(std::string &authToken, std::shared_ptr<DmAuthManager> authManager)
{
    LOGI("StartAuth end");
    return DM_OK;
}

int32_t PinAuth::VerifyAuthentication(std::string &authToken, const std::string &authParam)
{
    times_ += 1;
    if (authParam.length() == 1) {
        if (authParam == "0") {
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
    if (!authParamJson.contains(PIN_CODE_KEY) || !authTokenJson.contains(PIN_CODE_KEY)) {
        LOGE("err json string, first time");
        return DM_FAILED;
    }
    int32_t code = authTokenJson[PIN_CODE_KEY];
    int32_t inputPinCode = authParamJson[PIN_CODE_KEY];
    if (code == inputPinCode) {
        return DM_OK;
    } else if (code != inputPinCode && times_ < MAX_VERIFY_TIMES) {
        return DM_AUTH_INPUT_FAILED;
    } else {
        return DM_FAILED;
    }
}
} // namespace DistributedHardware
} // namespace OHOS
