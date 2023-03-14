/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "adapter/ohos/entrance/plugin_utils_impl.h"

#include "ability_manager_client.h"

namespace OHOS::Ace {
int32_t PluginUtilsImpl::StartAbility(
    const std::string& bundleName, const std::string& abilityName, const std::string& params)
{
    AAFwk::Want want;
    AppExecFwk::ElementName element("", bundleName, abilityName);
    want.SetElement(element);
    want.SetParam("params", params);
    int32_t error = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    return error;
}
} // namespace OHOS::Ace