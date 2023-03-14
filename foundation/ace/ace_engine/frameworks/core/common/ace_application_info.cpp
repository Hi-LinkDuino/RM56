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

#include "core/common/ace_application_info.h"

#include <thread>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "core/common/ace_page.h"

namespace OHOS::Ace {

void AceApplicationInfo::SetPackageName(const std::string& packageName)
{
    packageName_ = packageName;
}

const std::string& AceApplicationInfo::GetPackageName() const
{
    return packageName_;
}

void AceApplicationInfo::SetUid(int32_t uid)
{
    uid_ = uid;
}

int32_t AceApplicationInfo::GetUid() const
{
    return uid_;
}

void AceApplicationInfo::SetProcessName(const std::string& processName)
{
    processName_ = processName;
}

const std::string& AceApplicationInfo::GetProcessName() const
{
    return processName_;
}

std::string AceApplicationInfo::GetUnicodeSetting() const
{
    std::vector<std::string> keyValuePairs;
    StringUtils::StringSpliter(keywordsAndValues_, ';', keyValuePairs);
    auto keyValuePairsJson = JsonUtil::Create(true);
    for (const auto& pair : keyValuePairs) {
        // [pair] is like "nu=arab" or "nu=" for most occasions, but may be "=" under extreme scenarios
        std::vector<std::string> res;
        StringUtils::StringSpliter(pair, '=', res);
        if (res.size() == 0) {
            continue;
        }
        auto value = (res.size() == 2) ? res[1] : "";
        keyValuePairsJson->Put(res[0].c_str(), value.c_str());
    }
    return keyValuePairsJson->ToString(); // Return a string in json format
}

} // namespace OHOS::Ace
