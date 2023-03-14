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

#include "no_interaction_auth.h"

#include <memory>

#include "dm_constants.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace DistributedHardware {
NoInteractionAuth::NoInteractionAuth()
{
    LOGI("NoInteractionAuth constructor");
}

NoInteractionAuth::~NoInteractionAuth()
{
}

int32_t NoInteractionAuth::ShowAuthInfo(std::string &authToken, std::shared_ptr<DmAuthManager> authManager)
{
    return DM_OK;
}

int32_t NoInteractionAuth::StartAuth(std::string &authToken, std::shared_ptr<DmAuthManager> authManager)
{
    return DM_OK;
}

int32_t NoInteractionAuth::VerifyAuthentication(std::string &authToken, const std::string &authParam)
{
    return DM_OK;
}

int32_t NoInteractionAuth::CloseAuthInfo(const int32_t &pageId, std::shared_ptr<DmAuthManager> authManager)
{
    return DM_OK;
}

extern "C" IAuthentication *CreatePinAuthObject(void)
{
    return new NoInteractionAuth;
}
} // namespace DistributedHardware
} // namespace OHOS
