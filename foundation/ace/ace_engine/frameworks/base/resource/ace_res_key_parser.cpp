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

#include "base/resource/ace_res_key_parser.h"

#ifdef WINDOWS_PLATFORM
#include <cstdio>
#else
#include "securec.h"
#endif
#include <regex>
#include <unordered_map>

#include "base/log/log.h"
#include "base/utils/string_utils.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t MCC_MNC_KEYWORD_LEN = 3;
constexpr int32_t MCC_MNC_VALUE_LEN = 3;
constexpr int32_t MAX_MCC_MNC_LEN = 8;

const std::unordered_map<std::string, ResolutionType> RES_RESOLUTION = {
    { "ldpi", ResolutionType::RESOLUTION_LDPI },
    { "mdpi", ResolutionType::RESOLUTION_MDPI },
    { "hdpi", ResolutionType::RESOLUTION_HDPI },
    { "xhdpi", ResolutionType::RESOLUTION_XHDPI },
    { "xxhdpi", ResolutionType::RESOLUTION_XXHDPI },
    { "xxxhdpi", ResolutionType::RESOLUTION_XXXHDPI },
};

const std::unordered_map<std::string, DeviceOrientation> RES_ORIENTATION = {
    { "horizontal", DeviceOrientation::LANDSCAPE },
    { "vertical", DeviceOrientation::PORTRAIT }
};

const std::unordered_map<std::string, ColorMode> RES_COLOR_MODE = {
    { "dark", ColorMode::DARK },
};

const std::unordered_map<std::string, ColorMode> DECLARATIVE_RES_COLOR_MODE = {
    { "dark", ColorMode::DARK },
    { "light", ColorMode::LIGHT },
};

const std::unordered_map<std::string, DeviceType> RES_DEVICE_TYPE = {
    { "phone", DeviceType::PHONE },
    { "tablet", DeviceType::TABLET },
    { "tv", DeviceType::TV },
    { "wearable", DeviceType::WATCH },
};

const std::unordered_map<std::string, ScreenShape> RES_SCREEN_SHAPE = {
    { "round", ScreenShape::ROUND },
    { "notround", ScreenShape::NOT_ROUND },
};

const std::unordered_map<std::string, LongScreenType> RES_LONG_SCREEN = {
    { "long", LongScreenType::LONG },
    { "notlong", LongScreenType::NOT_LONG },
};

}

AceResKeyParser::AceResKeyParser() = default;
AceResKeyParser::~AceResKeyParser() = default;

bool AceResKeyParser::Parse(const std::string& deviceResConfigTag, std::vector<KeyParam> &keyParams, bool styleRes)
{
    std::vector<std::string> keyParts;
    StringUtils::StringSpliter(deviceResConfigTag, '-', keyParts);

    static parse_key_function functions[] = {
        ParseMcc,
        ParseMnc,
        ParseOrientation,
        ParseColorMode,
        ParseDeviceType,
        ParseResolution,
        ParseDefaults,
    };
    const int32_t FUNC_LEN = static_cast<int32_t>(sizeof(functions) / sizeof(functions[0]));
    if (!keyParts.empty()) {
        if (!ParseMatch(keyParts, keyParams, functions, FUNC_LEN, styleRes)) {
            LOGE("Invalid config:%{public}s", deviceResConfigTag.c_str());
            return false;
        }
        return true;
    }
    return false;
}

bool AceResKeyParser::DeclarativeParse(const std::string& deviceResConfigTag, std::vector<KeyParam>& keyParams)
{
    std::vector<std::string> reskeyParts;
    std::string resConfigTag(deviceResConfigTag);
    StringUtils::StringSpliter(resConfigTag, '-', reskeyParts);

    static parse_key_function functions[] = {
        ParseLongScreen,
        ParseScreenShape,
        ParseOrientation,
        ParseDeclarativeColorMode,
        ParseDeviceType,
        ParseResolution,
        ParseDefaults,
    };
    const int32_t FUNC_LEN = static_cast<int32_t>(sizeof(functions) / sizeof(functions[0]));
    if (!reskeyParts.empty()) {
        if (!ParseMatch(reskeyParts, keyParams, functions, FUNC_LEN, true)) {
            LOGE("Invalid config:%{public}s", resConfigTag.c_str());
            return false;
        }
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseMatch(const std::vector<std::string>& keys, std::vector<KeyParam> &keyParams,
    parse_key_function functions[], int32_t funcLen, bool styleRes) const
{
    for (auto iter = keys.begin(); iter != keys.end(); iter++) {
        if (iter == keys.begin() && !styleRes) {
            if (*iter == "res") {
                continue;
            } else {
                LOGE("Invalid first key part:%{private}s", iter->c_str());
                return false;
            }
        }
        bool parseRet = false;
        for (int32_t k = 0; k < funcLen; k++) {
            if (functions[k](*iter, keyParams)) {
                parseRet = true;
                break;
            }
        }
        if (!parseRet) {
            LOGE("Invalid key:%{private}s", iter->c_str());
            return false;
        }
    }
    return true;
}

bool AceResKeyParser::ParseMcc(const std::string& key, std::vector<KeyParam>& keyParams)
{
    std::regex mcc("mcc(\\d{3})");
    if (regex_match(key, mcc)) {
        KeyParam keyParam;
        keyParam.value = StringUtils::StringToInt(key.substr(MCC_MNC_KEYWORD_LEN));
        keyParam.keyType = KeyType::MCC;
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseMnc(const std::string& key, std::vector<KeyParam>& keyParams)
{
    std::regex mnc("mnc(\\d{2,3})");
    if (regex_match(key, mnc)) {
        KeyParam keyParam;
        keyParam.value = StringUtils::StringToInt(key.substr(MCC_MNC_KEYWORD_LEN));
        if (key.substr(MCC_MNC_KEYWORD_LEN).length() == MCC_MNC_VALUE_LEN) {
            keyParam.keyType = KeyType::MNC;
        } else {
            keyParam.keyType = KeyType::MNC_SHORT_LEN;
        }
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseScreenShape(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = RES_SCREEN_SHAPE.find(key);
    if (iter != RES_SCREEN_SHAPE.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::SCREENSHAPE;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseLongScreen(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = RES_LONG_SCREEN.find(key);
    if (iter != RES_LONG_SCREEN.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::LONGSCREEN;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseOrientation(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = RES_ORIENTATION.find(key);
    if (iter != RES_ORIENTATION.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::ORIENTATION;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseColorMode(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = RES_COLOR_MODE.find(key);
    if (iter != RES_COLOR_MODE.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::COLOR_MODE;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseDeclarativeColorMode(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = DECLARATIVE_RES_COLOR_MODE.find(key);
    if (iter != DECLARATIVE_RES_COLOR_MODE.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::DECLARATIVE_COLOR_MODE;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseDeviceType(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = RES_DEVICE_TYPE.find(key);
    if (iter != RES_DEVICE_TYPE.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::DEVICETYPE;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseResolution(const std::string& key, std::vector<KeyParam>& keyParams)
{
    auto iter = RES_RESOLUTION.find(key);
    if (iter != RES_RESOLUTION.end()) {
        KeyParam keyParam;
        keyParam.keyType = KeyType::RESOLUTION;
        keyParam.value = static_cast<int32_t>(iter->second);
        keyParams.emplace_back(keyParam);
        return true;
    }
    return false;
}

bool AceResKeyParser::ParseDefaults(const std::string& key, std::vector<KeyParam>& keyParams)
{
    return key == "defaults" || key == "default";
}

std::string AceResKeyParser::GetMccByValue(uint32_t value) const
{
    char mccStr[MAX_MCC_MNC_LEN] = {0};
    if (sprintf_s(mccStr, MAX_MCC_MNC_LEN, "mcc%03u", value) < 0) {
        LOGE("output mcc exception");
        return "";
    }
    return std::string(mccStr);
}

std::string AceResKeyParser::GetMncShortLenByValue(uint32_t value) const
{
    char mncStr[MAX_MCC_MNC_LEN] = {0};
    if (sprintf_s(mncStr, MAX_MCC_MNC_LEN, "mnc%02u", value) < 0) {
        LOGE("output mnc short len exception");
        return "";
    }
    return std::string(mncStr);
}

std::string AceResKeyParser::GetMncByValue(uint32_t value) const
{
    char mncStr[MAX_MCC_MNC_LEN] = {0};
    if (sprintf_s(mncStr, MAX_MCC_MNC_LEN, "mnc%03u", value) < 0) {
        LOGE("output mnc len exception");
        return "";
    }
    return std::string(mncStr);
}

std::string AceResKeyParser::GetOrientationByType(DeviceOrientation type) const
{
    auto item = find_if(RES_ORIENTATION.begin(), RES_ORIENTATION.end(),
                        [type](auto iter) { return iter.second == type; });
    if (item != RES_ORIENTATION.end()) {
        return item->first;
    }
    return "";
}

std::string AceResKeyParser::GetColorModeByType(ColorMode type) const
{
    auto item = find_if(RES_COLOR_MODE.begin(), RES_COLOR_MODE.end(),
                        [type](auto iter) { return iter.second == type; });
    if (item != RES_COLOR_MODE.end()) {
        return item->first;
    }
    return "";
}

std::string AceResKeyParser::GetDeclarativeColorModeByType(ColorMode type) const
{
    auto item = find_if(DECLARATIVE_RES_COLOR_MODE.begin(), DECLARATIVE_RES_COLOR_MODE.end(),
                        [type](auto iter) { return iter.second == type; });
    if (item != DECLARATIVE_RES_COLOR_MODE.end()) {
        return item->first;
    }
    return "";
}

std::string AceResKeyParser::GetDeviceByType(DeviceType type) const
{
    auto deviceTypeItem = find_if(RES_DEVICE_TYPE.begin(), RES_DEVICE_TYPE.end(),
                                  [type](auto iter) { return iter.second == type; });
    if (deviceTypeItem != RES_DEVICE_TYPE.end()) {
        return deviceTypeItem->first;
    }
    return "";
}

std::string AceResKeyParser::GetResolutionByType(ResolutionType type) const
{
    auto resolutionItem = find_if(RES_RESOLUTION.begin(), RES_RESOLUTION.end(),
                                  [type](auto iter) { return iter.second == type; });
    if (resolutionItem != RES_RESOLUTION.end()) {
        return resolutionItem->first;
    }
    return "";
}
std::string AceResKeyParser::GetScreenShapeByType(ScreenShape type) const
{
    auto shapeItem = find_if(RES_SCREEN_SHAPE.begin(), RES_SCREEN_SHAPE.end(),
        [type](auto iter) { return iter.second == type; });
    if (shapeItem != RES_SCREEN_SHAPE.end()) {
        return shapeItem->first;
    }
    return "";
}

std::string AceResKeyParser::GetScreenLongByType(LongScreenType type) const
{
    auto longItem = find_if(RES_LONG_SCREEN.begin(), RES_LONG_SCREEN.end(),
        [type](auto iter) { return iter.second == type; });
    if (longItem != RES_LONG_SCREEN.end()) {
        return longItem->first;
    }
    return "";
}

} // namespace OHOS::Ace