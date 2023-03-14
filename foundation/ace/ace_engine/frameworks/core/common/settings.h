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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_SETTINGS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_SETTINGS_H

#include <string>
#include <unordered_map>

#include "base/utils/macros.h"

namespace OHOS::Ace {

struct ACE_FORCE_EXPORT Settings {
public:
    Settings() = default;
    ~Settings() = default;

    void SetUsingSharedRuntime(bool useSharedRuntime)
    {
        usingSharedRuntime = useSharedRuntime;
        usePlatformAsUIThread = useSharedRuntime;
    }

    void SetCustomParams(const std::string& key, const std::string& value)
    {
        customParams_[key] = value;
    }

    const std::string& GetCustomParams(const std::string& key)
    {
        static std::string empty;
        auto it = customParams_.find(key);
        if (it != customParams_.end()) {
            return it->second;
        }
        return empty;
    }

    bool usingSharedRuntime = false;
    bool usePlatformAsUIThread = false;
    bool useUIAsJSThread = false;

private:
    std::unordered_map<std::string, std::string> customParams_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_SETTINGS_H
