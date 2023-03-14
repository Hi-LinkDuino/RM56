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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ACE_RES_KEY_PARSER_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ACE_RES_KEY_PARSER_H

#include <functional>
#include <string>
#include <vector>

#include "base/resource/ace_res_data_struct.h"
#include "base/utils/singleton.h"

namespace OHOS::Ace {
using parse_key_function = std::function<bool(const std::string& , std::vector<KeyParam>&)>;

class AceResKeyParser final : public Singleton<AceResKeyParser> {
    DECLARE_SINGLETON(AceResKeyParser);
    ACE_DISALLOW_MOVE(AceResKeyParser);

public:
    bool Parse(const std::string& deviceResConfigTag, std::vector<KeyParam>& keyParams, bool styleRes);
    bool DeclarativeParse(const std::string& deviceResConfigTag, std::vector<KeyParam>& keyParams);
    std::string GetMccByValue(uint32_t value) const;
    std::string GetMncByValue(uint32_t value) const;
    std::string GetMncShortLenByValue(uint32_t value) const;
    std::string GetOrientationByType(DeviceOrientation type) const;
    std::string GetColorModeByType(ColorMode type) const;
    std::string GetDeviceByType(DeviceType type) const;
    std::string GetResolutionByType(ResolutionType type) const;
    std::string GetScreenShapeByType(ScreenShape type) const;
    std::string GetScreenLongByType(LongScreenType type) const;
    std::string GetDeclarativeColorModeByType(ColorMode type) const;

private:
    bool ParseMatch(const std::vector<std::string>& keys, std::vector<KeyParam>& keyParams,
        parse_key_function functions[], int32_t funcLen, bool styleRes) const;
    static bool ParseMcc(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseMnc(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseOrientation(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseColorMode(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseDeviceType(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseResolution(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseDefaults(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseScreenShape(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseLongScreen(const std::string& key, std::vector<KeyParam>& keyParams);
    static bool ParseDeclarativeColorMode(const std::string& key, std::vector<KeyParam>& keyParams);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ACE_RES_KEY_PARSER_H