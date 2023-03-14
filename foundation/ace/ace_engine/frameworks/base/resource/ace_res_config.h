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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ACE_RES_CONFIG_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ACE_RES_CONFIG_H

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "base/resource/ace_res_data_struct.h"

namespace OHOS::Ace {

class AceResConfig {
public:
    AceResConfig() = default;
    ~AceResConfig() = default;
    AceResConfig(const AceResConfig& config) = default;
    AceResConfig& operator=(const AceResConfig& config) = default;
    AceResConfig(const std::string& language, const std::string& script, const std::string& region)
        : language_(language), script_(script), region_(region)
    {}

    AceResConfig(int32_t mcc, int32_t mnc, DeviceOrientation orientation, ColorMode colorMode, DeviceType deviceType,
        ResolutionType resolution)
        : mcc_(mcc), mnc_(mnc), orientation_(orientation), colorMode_(colorMode), deviceType_(deviceType),
          resolution_(resolution)
    {}

    AceResConfig(const std::string& language, const std::string& script, const std::string& region,
        LongScreenType screenLong, ScreenShape screenShape, DeviceOrientation orientation, ColorMode colorMode,
        DeviceType deviceType, ResolutionType resolution)
        : language_(language), script_(script), region_(region), screenLong_(screenLong), screenShape_(screenShape),
          orientation_(orientation), colorMode_(colorMode), deviceType_(deviceType), resolution_(resolution)
    {}

    bool operator==(const AceResConfig& other) const;

    static std::vector<std::string> GetLocaleFallback(
        const std::string& localeTag, const std::vector<std::string>& localeList);
    static std::vector<std::string> GetResourceFallback(const std::vector<std::string>& resourceList);
    static std::vector<std::string> GetStyleResourceFallback(const std::vector<std::string>& resourceList);
    static std::vector<std::string> GetDeclarativeResourceFallback(const std::set<std::string>& resourceList);
    static std::string GetCurrentDeviceResTag();
    static std::string GetCurrentDeviceDeclarativeResTag();
    static double GetTargetMediaScaleRatio(const std::string& targetResTag);

    static void MatchAndSortI18nConfigs(const std::vector<std::string>& candidatesFiles,
        const std::string& devicesLocaleTag, std::vector<std::string>& fileList);
    static void MatchAndSortResConfigs(const std::vector<std::string>& candidateFiles,
        const std::string& deviceConfigTag, std::vector<std::string>& viableFileList, bool styleRes = false);
    static void MatchAndSortStyleResConfigs(const std::vector<std::string>& candidateFiles,
        const std::string& deviceConfigTag, std::vector<std::string>& viableFileList);
    static void MatchAndSortDeclarativeResConfigs(const std::set<std::string>& candidateFolders,
        const std::string& deviceConfigTag, std::vector<std::string>& viableFileList);
    bool ParseConfig(const std::vector<KeyParam>& keyParams);
    static ResolutionType GetResolutionType(double resolution);
    static LongScreenType GetLongScreenType(double resolution);
    static AceResConfig ConvertResTagToConfig(const std::string& deviceResConfigTag, bool styleRes);
    static AceResConfig ConvertDeclarativeResTagToConfig(const std::string& deviceResConfigTag);
    static std::string ConvertResConfigToTag(const AceResConfig& resConfig, bool styleRes);
    static std::string ConvertDeclarativeResConfigToTag(const AceResConfig& resConfig);

    std::string language_;
    std::string script_;
    std::string region_;
    int32_t mcc_ = MCC_UNDEFINED;
    int32_t mnc_ = MNC_UNDEFINED;
    bool mncShortLen_ = false;
    LongScreenType screenLong_ = LongScreenType::LONG_SCREEN_UNDEFINED;
    ScreenShape screenShape_ = ScreenShape::SCREEN_SHAPE_UNDEFINED;
    DeviceOrientation orientation_ = DeviceOrientation::ORIENTATION_UNDEFINED;
    ColorMode colorMode_ = ColorMode::COLOR_MODE_UNDEFINED;
    DeviceType deviceType_ = DeviceType::UNKNOWN;
    ResolutionType resolution_ = ResolutionType::RESOLUTION_NONE;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ACE_RES_CONFIG_H