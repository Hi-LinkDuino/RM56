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

#include "adapter/preview/osal/resource_adapter_impl.h"

#include <set>

#include "base/i18n/localization.h"
#include "base/utils/system_properties.h"
#include "core/common/ace_application_info.h"
#include "core/common/container.h"
#include "core/components/common/layout/constants.h"
#include "core/components/theme/theme_attributes.h"

namespace OHOS::Ace {
namespace {

constexpr char COLOR_VALUE_PREFIX[] = "$color:";
constexpr char PATTERN_NAME_KEY_WORD[] = "$pattern:";
constexpr char STATE_VALUE_KEY_WORD[] = ".sxml";
constexpr char REF_ATTR_VALUE_KEY_WORD[] = "?theme:";
constexpr char STATE_CONTAINER[] = "state-container";
constexpr char STATE_ELEMENT[] = "element";
constexpr uint32_t STATE_MAX = 128;
constexpr double DPI_BASE = 160.0;
constexpr uint32_t THEME_ID_LIGHT = 117440515;
constexpr uint32_t THEME_ID_DARK = 117440516;

void CheckThemeId(int32_t& themeId)
{
    if (themeId >= 0) {
        return;
    }
    auto deviceType = SystemProperties::GetDeviceType();
    themeId = (deviceType == DeviceType::PHONE || deviceType == DeviceType::UNKNOWN || deviceType == DeviceType::CAR)
                  ? THEME_ID_LIGHT
                  : THEME_ID_DARK;
}

DimensionUnit ParseDimensionUnit(const std::string& unit)
{
    if (unit == "fp" || unit == "sp") {
        return DimensionUnit::FP;
    }
    return DimensionUnit::VP;
};

Global::Resource::ORIENTATION ConvertOrientation(DeviceOrientation orientation)
{
    return orientation == DeviceOrientation::PORTRAIT ? Global::Resource::ORIENTATION::ORIENTATION_PORTRAIT
                                                      : Global::Resource::ORIENTATION::ORIENTATION_LANDSCAPE;
}

Global::Resource::RESOLUTION ConvertResolution(double density)
{
    static const std::vector<std::pair<double, Global::Resource::RESOLUTION>> resolutions = {
        { 120.0, Global::Resource::RESOLUTION::RESOLUTION_LOW },
        { 160.0, Global::Resource::RESOLUTION::RESOLUTION_MEDIUM },
        { 240.0, Global::Resource::RESOLUTION::RESOLUTION_HIGH },
        { 320.0, Global::Resource::RESOLUTION::RESOLUTION_XHIGH },
        { 480.0, Global::Resource::RESOLUTION::RESOLUTION_XXHIGH },
        { 640.0, Global::Resource::RESOLUTION::RESOLUTION_XXXHIGH },
    };
    double deviceDpi = density * DPI_BASE;
    auto resolution = Global::Resource::RESOLUTION::RESOLUTION_LOW;
    for (const auto& [dpi, value] : resolutions) {
        resolution = value;
        if (LessOrEqual(deviceDpi, dpi)) {
            break;
        }
    }
    return resolution;
}

Global::Resource::DEVICE_TYPE ConvertDeviceType(DeviceType type)
{
    switch (type) {
        case DeviceType::PHONE:
            return Global::Resource::DEVICE_TYPE::DEVICE_TYPE_PHONE;
        case DeviceType::TV:
            return Global::Resource::DEVICE_TYPE::DEVICE_TYPE_TV;
        case DeviceType::WATCH:
            return Global::Resource::DEVICE_TYPE::DEVICE_TYPE_WATCH;
        case DeviceType::CAR:
            return Global::Resource::DEVICE_TYPE::DEVICE_TYPE_CAR;
        default:
            return Global::Resource::DEVICE_TYPE::DEVICE_TYPE_UNDEFINED;
    }
}

Global::Resource::COLOR_MODE ConvertColorMode(ColorMode colorMode)
{
    return colorMode == ColorMode::LIGHT ? Global::Resource::COLOR_MODE::COLOR_MODE_LIGHT
                                         : Global::Resource::COLOR_MODE::COLOR_MODE_DARK;
}

Global::Resource::Configuration ConvertConfig(const ResourceConfiguration& config)
{
    Global::Resource::Configuration::Locale locale(AceApplicationInfo::GetInstance().GetLanguage(),
        AceApplicationInfo::GetInstance().GetCountryOrRegion(), AceApplicationInfo::GetInstance().GetScript());
    Global::Resource::Configuration globalConfig = {
        .locales_ = { locale },
        .orientation_ = ConvertOrientation(config.GetOrientation()),
        .resolution_ = ConvertResolution(config.GetDensity()),
        .deviceType_ = ConvertDeviceType(config.GetDeviceType()),
        .fontRatio_ = config.GetFontRatio(),
        .colorMode_ = ConvertColorMode(config.GetColorMode()),
    };
    return globalConfig;
}

RefPtr<StateResource> ParseStateResource(const std::string& styleName, const std::string& attrName,
    std::unique_ptr<Global::Resource::SolidXmlWrapper> xmlWrapper)
{
    auto rootNode = xmlWrapper->GetRoot();
    if (!rootNode) {
        LOGE("Parse %{public}s state resource %{public}s error! No root!", styleName.c_str(), attrName.c_str());
        return nullptr;
    }
    if (rootNode->GetNodeName() != STATE_CONTAINER) {
        return nullptr;
    }
    auto stateResource = AceType::MakeRefPtr<StateResource>();
    auto node = rootNode->GetChild();
    uint32_t stateCount = 0;
    while (node && ++stateCount < STATE_MAX) {
        // Parse each item
        auto nodeAttrs = node->GetAttributes();
        auto valueFindIter = nodeAttrs.find(STATE_ELEMENT);
        if (valueFindIter == nodeAttrs.end()) {
            continue;
        }
        auto stateColor = Color(valueFindIter->second.GetColorValue());
        uint32_t state = STATE_NORMAL;
        static const std::unordered_map<std::string, uint32_t> stateMap = {
            { "state_pressed", STATE_PRESSED },
            { "state_focus", STATE_FOCUS },
            { "state_checked", STATE_CHECKED },
            { "state_disabled", STATE_DISABLED },
            { "state_waiting", STATE_WAITING },
            { "state_hovered", STATE_HOVERED },
        };
        for (auto& [stateKey, stateValue] : nodeAttrs) {
            auto stateFindIter = stateMap.find(stateKey);
            if (stateFindIter == stateMap.end()) {
                continue;
            }
            if (stateValue.GetStringValue() != "true") {
                continue;
            }
            state |= stateFindIter->second;
        }
        stateResource->SetStateValue(state, { .type = ThemeConstantsType::COLOR, .value = stateColor });
        node = node->GetSibling();
    }
    return stateResource;
}
} // namespace

class RawThemeStyle : public ThemeStyle {
    DECLARE_ACE_TYPE(RawThemeStyle, ThemeStyle);

public:
    friend class ResourceAdapterImpl;
    using RawAttrMap = std::unordered_map<std::string, std::unique_ptr<Global::Resource::TypeAttribute>>;

    explicit RawThemeStyle(RefPtr<ResourceAdapter> resAdapter) : resAdapter_(resAdapter) {}
    ~RawThemeStyle() override = default;

    void ParseContent() override;

private:
    RawAttrMap rawAttrs_; // key and value read from global resource api.
    RefPtr<ResourceAdapter> resAdapter_;
};

void RawThemeStyle::ParseContent()
{
    static const std::set<std::string> stringAttrs = {
        "attr_text_font_family_regular",
        "attr_text_font_family_medium"
    };
    for (auto& [attrName, attrValue] : rawAttrs_) {
        if (!attrValue) {
            continue;
        }
        auto rawString = attrValue->GetOriginalValue();
        if (rawString.size() == 0) {
            continue;
        }
        if (rawString.front() == '#' || rawString.find(COLOR_VALUE_PREFIX) != std::string::npos) {
            // color
            attributes_[attrName] = { .type = ThemeConstantsType::COLOR, .value = Color(attrValue->GetColorValue()) };
        } else if (stringAttrs.find(attrName) != stringAttrs.end()) {
            // string
            attributes_[attrName] = { .type = ThemeConstantsType::STRING, .value = rawString };
        } else if (rawString.find(PATTERN_NAME_KEY_WORD) != std::string::npos) {
            // pattern
            auto patternStyle = AceType::MakeRefPtr<RawThemeStyle>(resAdapter_);
            patternStyle->SetName(attrName);
            patternStyle->parentStyle_ = AceType::WeakClaim(this);
            patternStyle->rawAttrs_ = attrValue->GetPattern();
            patternStyle->ParseContent();
            attributes_[attrName] = { .type = ThemeConstantsType::PATTERN,
                .value = RefPtr<ThemeStyle>(std::move(patternStyle)) };
        } else if (rawString.rfind(STATE_VALUE_KEY_WORD) != std::string::npos) {
            // state graphic value
            auto xmlWrapper = attrValue->GetLayoutValue();
            if (!xmlWrapper) {
                LOGE("Parse %{public}s state resource %{public}s error! xml is null!", name_.c_str(), attrName.c_str());
                continue;
            }
            auto stateResource = ParseStateResource(name_, attrName, std::move(xmlWrapper));
            if (!stateResource) {
                continue;
            }
            attributes_[attrName] = { .type = ThemeConstantsType::STATE_RESOURCE, .value = stateResource };
        } else if (rawString.find(REF_ATTR_VALUE_KEY_WORD) != std::string::npos) {
            attributes_[attrName] = { .type = ThemeConstantsType::REFERENCE_ATTR, .value = rawString };
        } else {
            // double & dimension
            std::string unit = "";
            auto doubleValue = static_cast<double>(attrValue->GetFloat(unit));
            if (unit.empty()) {
                attributes_[attrName] = { .type = ThemeConstantsType::DOUBLE, .value = doubleValue };
            } else {
                attributes_[attrName] = { .type = ThemeConstantsType::DIMENSION,
                    .value = Dimension(doubleValue, ParseDimensionUnit(unit)) };
            }
        }
    }
}

RefPtr<ResourceAdapter> ResourceAdapter::Create()
{
    auto deviceType = SystemProperties::GetDeviceType();
    if (deviceType == DeviceType::PHONE || deviceType == DeviceType::CAR) {
        return AceType::MakeRefPtr<ResourceAdapterImpl>();
    }
    return RefPtr<ResourceAdapter>();
}

void ResourceAdapterImpl::Init(const ResourceInfo& resourceInfo)
{
    std::vector<std::string> hapFiles;
    hapFiles.emplace_back(resourceInfo.GetPackagePath());
    auto configuration = ConvertConfig(resourceInfo.GetResourceConfiguration());
    auto handlers = resourceInfo.GetResourceHandlers();
    bool initRet = false;
    if (handlers.empty()) {
        initRet = resourceManger_.InitMock(hapFiles, resourceInfo.GetSystemPackagePath(), configuration);
    } else {
        initRet = resourceManger_.Init(hapFiles, handlers);
        resourceManger_.UpdateConfig(configuration);
    }
    LOGI("Init result=%{public}d, handle=%{public}zu, ori=%{public}d, dpi=%{public}d, device=%{public}d, "
         "font=%{public}f, color=%{public}d",
        initRet, handlers.size(), configuration.orientation_, configuration.resolution_, configuration.deviceType_,
        configuration.fontRatio_, configuration.colorMode_);
}

void ResourceAdapterImpl::UpdateConfig(const ResourceConfiguration& config)
{
    auto configuration = ConvertConfig(config);
    LOGI("UpdateConfig ori=%{public}d, dpi=%{public}d, device=%{public}d, font=%{public}f, color=%{public}d",
        configuration.orientation_, configuration.resolution_, configuration.deviceType_, configuration.fontRatio_,
        configuration.colorMode_);
    resourceManger_.UpdateConfig(configuration);
}

RefPtr<ThemeStyle> ResourceAdapterImpl::GetTheme(int32_t themeId)
{
    CheckThemeId(themeId);
    auto theme = AceType::MakeRefPtr<RawThemeStyle>(AceType::Claim(this));
    auto& attrMap = theme->rawAttrs_;
    auto ret = resourceManger_.GetTheme(themeId, attrMap);
    LOGI("GetTheme themeId=%{public}d, ret=%{public}d, attr size=%{public}zu", themeId, ret, attrMap.size());
    auto iter = attrMap.find(THEME_ATTR_BG_COLOR);
    if (iter != attrMap.end()) {
        auto& attribute = iter->second;
        if (attribute) {
            Color bgColor(attribute->GetColorValue());
            theme->SetAttr(THEME_ATTR_BG_COLOR, { .type = ThemeConstantsType::COLOR, .value = bgColor });
        }
    }
    return theme;
}

Color ResourceAdapterImpl::GetColor(uint32_t resId)
{
    uint32_t result = 0;
    auto ret = resourceManger_.GetColor(static_cast<int32_t>(resId), result);
    if (!ret) {
        LOGE("GetColor error, id=%{public}u", resId);
    }
    return Color(result);
}

Dimension ResourceAdapterImpl::GetDimension(uint32_t resId)
{
    float result = 0;
    std::string unit = "";
    auto ret = resourceManger_.GetFloat(static_cast<int32_t>(resId), result, unit);
    if (!ret) {
        LOGE("GetDimension error, id=%{public}u", resId);
    }
    return Dimension(result, ParseDimensionUnit(unit));
}

std::string ResourceAdapterImpl::GetString(uint32_t resId)
{
    std::string strResult = "";
    auto ret = resourceManger_.GetString(static_cast<int32_t>(resId), strResult);
    if (!ret) {
        LOGE("GetString error, id=%{public}u", resId);
    }
    return strResult;
}

std::string ResourceAdapterImpl::GetPluralString(uint32_t resId, int quantity)
{
    std::vector<std::string> pluralResults;
    auto ret = resourceManger_.GetStringArray(static_cast<int32_t>(resId), pluralResults);
    if (!ret) {
        LOGE("GetPluralString error, id=%{public}u", resId);
    }

    auto pluralChoice = Localization::GetInstance()->PluralRulesFormat(quantity);
    auto iter = std::find(pluralResults.begin(), pluralResults.end(), pluralChoice);
    std::string originStr;
    if (iter != pluralResults.end() && ++iter != pluralResults.end()) {
        originStr = *iter;
    }
    return originStr;
}

std::vector<std::string> ResourceAdapterImpl::GetStringArray(uint32_t resId) const
{
    std::vector<std::string> strResults;
    auto ret = resourceManger_.GetStringArray(static_cast<int32_t>(resId), strResults);
    if (!ret) {
        LOGE("GetStringArray error, id=%{public}u", resId);
    }
    return strResults;
}

double ResourceAdapterImpl::GetDouble(uint32_t resId)
{
    float result = 0.0f;
    auto ret = resourceManger_.GetFloat(static_cast<int32_t>(resId), result);
    if (!ret) {
        LOGE("GetDouble error, id=%{public}u", resId);
    }
    return static_cast<double>(result);
}

int32_t ResourceAdapterImpl::GetInt(uint32_t resId)
{
    int32_t result = 0;
    auto ret = resourceManger_.GetInt(static_cast<int32_t>(resId), result);
    if (!ret) {
        LOGE("GetInt error, id=%{public}u", resId);
    }
    return result;
}

bool ResourceAdapterImpl::GetResource(uint32_t resId, std::ostream& dest) const
{
    return resourceManger_.GetResource(static_cast<int32_t>(resId), dest);
}

bool ResourceAdapterImpl::GetResource(const std::string& path, std::ostream& dest) const
{
    return resourceManger_.GetResource(path, dest);
}

bool ResourceAdapterImpl::ConvertToGlobalResourceType(
    const std::string& resTypeName, Global::Resource::ResourceType& resType) const
{
    if (resTypeName == "color") {
        resType = Global::Resource::ResourceType::COLOR;
        return true;
    }
    if (resTypeName == "float") {
        resType = Global::Resource::ResourceType::FLOAT;
        return true;
    }
    if (resTypeName == "string") {
        resType = Global::Resource::ResourceType::STRING;
        return true;
    }
    if (resTypeName == "media") {
        resType = Global::Resource::ResourceType::MEDIA;
        return true;
    }
    LOGE("unsupported resource type(=%{public}s)", resTypeName.c_str());
    return false;
}

bool ResourceAdapterImpl::GetIdByName(const std::string& resName, const std::string& resType, uint32_t& resId) const
{
    Global::Resource::ResourceType globalResType;
    if (!ConvertToGlobalResourceType(resType, globalResType)) {
        return false;
    }
    int32_t globalResId = 0;
    if (!resourceManger_.GetIdByName("", resName, globalResType, globalResId)) {
        LOGE("get resource id failed.(name=%{public}s, type=%{public}s)", resName.c_str(), resType.c_str());
        return false;
    }
    resId = static_cast<uint32_t>(globalResId);
    return true;
}

std::string ResourceAdapterImpl::GetMediaPath(uint32_t resId)
{
    std::string mediaPath = "";
    auto ret = resourceManger_.GetString(static_cast<int32_t>(resId), mediaPath);
    if (!ret) {
        LOGE("GetMediaPath error, id=%{public}u", resId);
        return "";
    }
    return "resource://" + mediaPath.substr(0, mediaPath.find_last_of("/")) + "/" +
            std::to_string(resId) + mediaPath.substr(mediaPath.find_last_of("."));
}

std::string ResourceAdapterImpl::GetRawfile(const std::string& fileName)
{
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return "";
    }
    auto moduleName = container->GetModuleName();
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    return "resource://RAWFILE/" + moduleName + "/resources/rawfile/" + fileName;
#else
    return "resource://RAWFILE/assets/" + moduleName + "/resources/rawfile/" + fileName;
#endif
}
} // namespace OHOS::Ace
