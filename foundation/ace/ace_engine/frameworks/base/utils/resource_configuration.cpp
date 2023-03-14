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

#include "base/utils/resource_configuration.h"

#include "base/log/log.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {

const std::string CONFIGURATION_COLOR_MODE = "colorMode";
const std::string COLOR_MODE_LIGHT = "light";
const std::string COLOR_MODE_DARK = "dark";
const std::string CONFIGURATION_FONT_RATIO = "fontScale";

void AddFlag(uint32_t& flags, uint32_t flag)
{
    flags |= flag;
}

} // namespace

bool ResourceConfiguration::ParseJsonColorMode(const std::unique_ptr<JsonValue>& jsonConfig, uint32_t& updateFlags)
{
    if (!jsonConfig->Contains(CONFIGURATION_COLOR_MODE)) {
        return false;
    }
    auto jsonColorMode = jsonConfig->GetValue(CONFIGURATION_COLOR_MODE);
    if (!jsonColorMode->IsString()) {
        return false;
    }
    auto strColorMode = jsonColorMode->GetString();
    if (strColorMode != COLOR_MODE_LIGHT && strColorMode != COLOR_MODE_DARK) {
        return false;
    }
    ColorMode colorMode = ColorMode::LIGHT;
    if (strColorMode == COLOR_MODE_DARK) {
        colorMode = ColorMode::DARK;
    }
    if (colorMode != colorMode_) {
        colorMode_ = colorMode;
        AddFlag(updateFlags, ResourceConfiguration::COLOR_MODE_UPDATED_FLAG);
    }
    return true;
}

bool ResourceConfiguration::ParseJsonFontRatio(const std::unique_ptr<JsonValue>& jsonConfig, uint32_t& updateFlags)
{
    if (!jsonConfig->Contains(CONFIGURATION_FONT_RATIO)) {
        return false;
    }
    auto jsonFontRatio = jsonConfig->GetValue(CONFIGURATION_FONT_RATIO);
    if (!jsonFontRatio->IsNumber()) {
        return false;
    }
    double fontRatio = jsonFontRatio->GetDouble();
    if (!NearEqual(fontRatio, fontRatio_)) {
        fontRatio_ = fontRatio;
        AddFlag(updateFlags, ResourceConfiguration::FONT_RATIO_UPDATED_FLAG);
    }
    return true;
}

bool ResourceConfiguration::UpdateFromJsonString(const std::string jsonStr, uint32_t& updateFlags)
{
    updateFlags = 0;
    std::unique_ptr<JsonValue> jsonConfig = JsonUtil::ParseJsonString(jsonStr);
    if (!jsonConfig) {
        LOGW("Parse Json String failed");
        return false;
    }
    ParseJsonColorMode(jsonConfig, updateFlags);
    ParseJsonFontRatio(jsonConfig, updateFlags);
    return true;
}

} // namespace OHOS::Ace
