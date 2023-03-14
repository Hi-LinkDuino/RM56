/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/theme/theme_utils.h"

#include <cmath>
#include <regex>
#include <set>

#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "base/utils/system_properties.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t THEME_ID_MIN_SIZE = 5; // Input should contain "@id"
constexpr uint32_t THEME_ID_MATCH_SIZE = 2;
const std::regex THEME_ID_REGEX(R"(^\"@id([0-9]+)\"$)", std::regex::icase); // regex for "@id001"
constexpr uint32_t THEME_ATTR_MIN_SIZE = 7;
const std::regex THEME_ATTR_REGEX(R"(\?theme:([a-zA-Z0-9_]+))"); // regex for "?theme:attr_color_emphasis"
constexpr uint32_t CUSTOM_STYLE_STRING_MAX_SIZE = 128;
constexpr uint32_t OHOS_ID_MIN_SIZE = 7; // Input should contain "@ohos_id"
constexpr uint32_t SYSTEM_RES_ID_START = 0x7000000;
const std::regex OHOS_ID_REGEX(R"(^@ohos_id_([0-9]+)$)", std::regex::icase); // regex for "@ohos_id_001"
// regex for "@sys.type.xxx", xxx represents system resource id.
const std::regex SYS_TYPE_RES_ID_REGEX(R"(^@sys.(\w+).([0-9]+)$)", std::regex::icase);
constexpr uint32_t SYS_TYPE_RES_ID_MIN_SIZE = 7;
// regex for "@app.type.xxx", xxx represents application name.
const std::regex APP_TYPE_RES_NAME_REGEX(R"(^@app.(\w+).(\w+)$)", std::regex::icase);
constexpr uint32_t APP_TYPE_RES_NAME_MIN_SIZE = 7;
constexpr uint32_t TYPE_RESOURCE_MATCH_SIZE = 3;
// regex for "@sys.media.xxx", xxx represents system resource id.
const std::regex SYS_MEDIA_RES_ID_REGEX(R"(^@sys\.media\.([0-9]+)$)", std::regex::icase);
// regex for "@app.media.xxx", xxx represents resource name.
const std::regex APP_MEDIA_RES_NAME_REGEX(R"(^@app\.media\.(\w+)$)", std::regex::icase);
constexpr uint32_t MEDIA_RESOURCE_MATCH_SIZE = 2;

const std::set<uint32_t> FONT_WEIGHT_STYLE_ID = {
    THEME_BUTTON_TEXT_FONTWEIGHT,
    THEME_DIALOG_TITLE_TEXT_FONTWEIGHT,
    THEME_TOAST_TEXT_TEXT_FONTWEIGHT,
    THEME_TEXTFIELD_FONT_WEIGHT,
    THEME_SEARCH_FONT_WEIGHT
};

} // namespace

IdParseResult ThemeUtils::ParseThemeIdReference(const std::string& str, const RefPtr<ThemeConstants>& themeConstants)
{
    std::smatch matches;
    IdParseResult result { .parseSuccess = false, .isIdRef = false, .id = 0, .refAttr = "" };
    if (str.size() > THEME_ID_MIN_SIZE && std::regex_match(str, matches, THEME_ID_REGEX) &&
        matches.size() == THEME_ID_MATCH_SIZE) {
        // Platform style id is no more than 32 bit.
        result.id = static_cast<uint32_t>(std::stoul(matches[1].str()));
        result.parseSuccess = true;
        result.isIdRef = true;
        return result;
    }
    if (str.size() > THEME_ATTR_MIN_SIZE && std::regex_match(str, matches, THEME_ATTR_REGEX) &&
        matches.size() == THEME_ID_MATCH_SIZE) {
        result.refAttr = matches[1].str();
        result.parseSuccess = true;
        result.isIdRef = false;
        return result;
    }
    if (str.size() > OHOS_ID_MIN_SIZE && std::regex_match(str, matches, OHOS_ID_REGEX) &&
        matches.size() == THEME_ID_MATCH_SIZE) {
        // Platform style id is no more than 32 bit.
        result.id = static_cast<uint32_t>(std::stoul(matches[1].str())) + SYSTEM_RES_ID_START;
        result.parseSuccess = true;
        result.isIdRef = true;
        return result;
    }
    if (str.size() > SYS_TYPE_RES_ID_MIN_SIZE && std::regex_match(str, matches, SYS_TYPE_RES_ID_REGEX) &&
        matches.size() == TYPE_RESOURCE_MATCH_SIZE) {
        result.id = static_cast<uint32_t>(std::stoul(matches[2].str())) + SYSTEM_RES_ID_START;
        result.parseSuccess = true;
        result.isIdRef = true;
        return result;
    }
    if (str.size() > APP_TYPE_RES_NAME_MIN_SIZE && std::regex_match(str, matches, APP_TYPE_RES_NAME_REGEX) &&
        matches.size() == TYPE_RESOURCE_MATCH_SIZE) {
        uint32_t resId = 0;
        if (themeConstants && themeConstants->GetResourceIdByName(matches[2].str(), matches[1].str(), resId)) {
            result.id = resId;
            result.parseSuccess = true;
            result.isIdRef = true;
            return result;
        }
    }
    // Not reference format, ignore.
    return result;
}

ResValueWrapper ThemeUtils::ParseStyleValue(
    uint32_t styleId, const ResValueWrapper& model, const std::string& value)
{
    ResValueWrapper resultValue = { .type = model.type, .isPublic = model.isPublic };
    if (FONT_WEIGHT_STYLE_ID.count(styleId) > 0) {
        resultValue.value = static_cast<int32_t>(StringUtils::StringToFontWeight(value));
        return resultValue;
    }
    switch (model.type) {
        case ThemeConstantsType::COLOR:
            resultValue.value = Color::FromString(value, COLOR_ALPHA_MASK);
            break;
        case ThemeConstantsType::DIMENSION:
            resultValue.value = StringUtils::StringToDimension(value);
            break;
        case ThemeConstantsType::INT:
            resultValue.value = StringUtils::StringToInt(value);
            break;
        case ThemeConstantsType::DOUBLE:
            resultValue.value = StringUtils::StringToDouble(value);
            break;
        case ThemeConstantsType::STRING:
            if (value.size() < CUSTOM_STYLE_STRING_MAX_SIZE) {
                resultValue.value = value;
            } else {
                LOGE("Custom style value size over limit!");
                resultValue.type = ThemeConstantsType::ERROR;
            }
            break;
        default:
            resultValue.type = ThemeConstantsType::ERROR;
            break;
    }
    return resultValue;
}

std::string ThemeUtils::ProcessImageSource(const std::string& imageSrc, const RefPtr<ThemeConstants>& themeConstants)
{
    std::smatch matches;
    uint32_t resId = 0;
    std::string resName;
    if (std::regex_match(imageSrc, matches, APP_MEDIA_RES_NAME_REGEX) && matches.size() == MEDIA_RESOURCE_MATCH_SIZE) {
        resName = matches[1].str();
    }
    if (std::regex_match(imageSrc, matches, SYS_MEDIA_RES_ID_REGEX) && matches.size() == MEDIA_RESOURCE_MATCH_SIZE) {
        resId = static_cast<uint32_t>(std::stoul(matches[1].str())) + SYSTEM_RES_ID_START;
    }
    // not a image from global global resource manager subsystem, no need process.
    if (resId == 0 && resName.empty()) {
        return imageSrc;
    }

    if (!themeConstants) {
        LOGE("themeConstants is null, process image source failed");
        return "";
    }
    if (resId == 0 && !themeConstants->GetResourceIdByName(resName, "media", resId)) {
        LOGE("get image id failed");
        return "";
    }

    std::string imagePath = themeConstants->GetString(resId);
    auto seperatorPos = imagePath.rfind('.');
    if (seperatorPos == std::string::npos) {
        LOGE("get image suffix failed");
        return "";
    }
    // image format suffix, such as ".png",".svg" and so on.
    std::string imageSuffix = imagePath.substr(seperatorPos);

    // resource name or resource id of image in global resource manager subsystem is the same in dark or light mode.
    // image will not be reloaded if name is not changed when switching between dark and light modes.
    std::string colorMode;
    switch (SystemProperties::GetColorMode()) {
        case ColorMode::LIGHT:
            colorMode = "light";
            break;
        case ColorMode::DARK:
            colorMode = "dark";
            break;
        default:
            LOGW("color mode is undefined");
            colorMode = "undefined";
            break;
    }
    std::string result = "resource://" + colorMode + "/" + std::to_string(resId) + imageSuffix;
    return result;
}

} // namespace OHOS::Ace