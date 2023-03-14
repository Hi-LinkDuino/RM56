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

#include "form_info.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "json_serializer.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_COLOR_MODE = "colorMode";
const std::string JSON_KEY_PACKAGE = "package";
const std::string JSON_KEY_SUPPORT_DIMENSIONS = "supportDimensions";
const std::string JSON_KEY_DEFAULT_DIMENSION = "defaultDimension";
const std::string JSON_KEY_UPDATE_ENABLED = "updateEnabled";
const std::string JSON_KEY_SCHEDULED_UPDATE_TIME = "scheduledUpdateTime";
const std::string JSON_KEY_UPDATE_DURATION = "updateDuration";
const std::string JSON_KEY_DEEP_LINK = "deepLink";
const std::string JSON_KEY_JS_COMPONENT_NAME = "jsComponentName";
const std::string JSON_KEY_VALUE = "value";
const std::string JSON_KEY_NAME = "name";
const std::string JSON_KEY_ABILITY_NAME = "abilityName";
const std::string JSON_KEY_BUNDLE_NAME = "bundleName";
const std::string JSON_KEY_MODULE_NAME = "moduleName";
const std::string JSON_KEY_ORIGINAL_BUNDLE_NAME = "originalBundleName";
const std::string JSON_KEY_CUSTOMIZE_DATA = "customizeData";
const std::string JSON_KEY_DESCRIPTION = "description";
const std::string JSON_KEY_DESCRIPTION_ID = "descriptionId";
const std::string JSON_KEY_TYPE = "type";
const std::string JSON_KEY_LANDSCAPE_LAYOUTS = "landscapeLayouts";
const std::string JSON_KEY_FORMCONFIG_ABILITY = "formConfigAbility";
const std::string JSON_KEY_FORM_VISIBLE_NOTIFY = "formVisibleNotify";
const std::string JSON_KEY_RELATED_BUNDLE_NAME = "relatedBundleName";
const std::string JSON_KEY_DEFAULT_FLAG = "defaultFlag";
const std::string JSON_KEY_PORTRAIT_LAYOUTS = "portraitLayouts";
const std::string JSON_KEY_SRC = "src";
const std::string JSON_KEY_WINDOW = "window";
const std::string JSON_KEY_DESIGN_WIDTH = "designWidth";
const std::string JSON_KEY_AUTO_DESIGN_WIDTH = "autoDesignWidth";
}  // namespace

FormInfo::FormInfo(const ExtensionAbilityInfo &abilityInfo, const ExtensionFormInfo &formInfo)
{
    package = abilityInfo.bundleName + abilityInfo.moduleName;
    bundleName = abilityInfo.bundleName;
    originalBundleName = abilityInfo.bundleName;
    relatedBundleName = abilityInfo.bundleName;
    moduleName = abilityInfo.moduleName;
    abilityName = abilityInfo.name;
    name = formInfo.name;
    description = formInfo.description;
    jsComponentName = "";
    deepLink = "";
    formConfigAbility = formInfo.formConfigAbility;
    scheduledUpdateTime = formInfo.scheduledUpdateTime;
    src = formInfo.src;
    window.designWidth = formInfo.window.designWidth;
    window.autoDesignWidth = formInfo.window.autoDesignWidth;
    descriptionId = 0;
    updateDuration = formInfo.updateDuration;
    defaultDimension = formInfo.defaultDimension;
    defaultFlag = formInfo.isDefault;
    formVisibleNotify = formInfo.formVisibleNotify;
    updateEnabled = formInfo.updateEnabled;
    type = FormType::JS;
    colorMode = formInfo.colorMode;
    for (const auto &dimension : formInfo.supportDimensions) {
        supportDimensions.push_back(dimension);
    }
    for (const auto &metadata : formInfo.metadata) {
        customizeDatas.push_back(metadata);
    }
}

bool FormInfo::ReadCustomizeData(Parcel &parcel)
{
    int32_t customizeDataSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, customizeDataSize);
    for (auto i = 0; i < customizeDataSize; ++i) {
        FormCustomizeData customizeData;
        std::string customizeName = Str16ToStr8(parcel.ReadString16());
        std::string customizeValue = Str16ToStr8(parcel.ReadString16());
        customizeData.name = customizeName;
        customizeData.value = customizeValue;
        customizeDatas.emplace_back(customizeData);
    }
    return true;
}

bool FormInfo::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    package = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    moduleName = Str16ToStr8(parcel.ReadString16());
    abilityName = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    formConfigAbility = Str16ToStr8(parcel.ReadString16());
    scheduledUpdateTime = Str16ToStr8(parcel.ReadString16());
    jsComponentName = Str16ToStr8(parcel.ReadString16());
    relatedBundleName = Str16ToStr8(parcel.ReadString16());
    originalBundleName = Str16ToStr8(parcel.ReadString16());
    deepLink = Str16ToStr8(parcel.ReadString16());
    src = Str16ToStr8(parcel.ReadString16());
    updateEnabled = parcel.ReadBool();
    defaultFlag = parcel.ReadBool();
    formVisibleNotify = parcel.ReadBool();
    defaultDimension = parcel.ReadInt32();
    descriptionId = parcel.ReadInt32();
    updateDuration = parcel.ReadInt32();

    int32_t typeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, typeData);
    type = static_cast<FormType>(typeData);

    int32_t colorModeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, colorModeData);
    colorMode = static_cast<FormsColorMode>(colorModeData);

    int32_t supportDimensionSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, supportDimensionSize);

    for (int32_t i = 0; i < supportDimensionSize; i++) {
        supportDimensions.emplace_back(parcel.ReadInt32());
    }

    int32_t landscapeLayoutsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, landscapeLayoutsSize);
    for (auto i = 0; i < landscapeLayoutsSize; i++) {
        landscapeLayouts.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    int32_t portraitLayoutsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, portraitLayoutsSize);
    for (auto i = 0; i < portraitLayoutsSize; i++) {
        portraitLayouts.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }

    if (!ReadCustomizeData(parcel)) {
        return false;
    }

    window.designWidth = parcel.ReadInt32();
    window.autoDesignWidth = parcel.ReadBool();
    return true;
}

FormInfo *FormInfo::Unmarshalling(Parcel &parcel)
{
    std::unique_ptr<FormInfo> info = std::make_unique<FormInfo>();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        info = nullptr;
    }
    return info.release();
}

bool FormInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(package));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(abilityName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(formConfigAbility));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(scheduledUpdateTime));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(jsComponentName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(relatedBundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(originalBundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(deepLink));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(src));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, updateEnabled);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, defaultFlag);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, formVisibleNotify);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, defaultDimension);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, updateDuration);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(type));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(colorMode));

    const auto supportDimensionSize = static_cast<int32_t>(supportDimensions.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, supportDimensionSize);
    for (auto i = 0; i < supportDimensionSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, supportDimensions[i]);
    }

    const auto landscapeLayoutsSize = static_cast<int32_t>(landscapeLayouts.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, landscapeLayoutsSize);
    for (auto i = 0; i < landscapeLayoutsSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(landscapeLayouts[i]));
    }

    const auto portraitLayoutsSize = static_cast<int32_t>(portraitLayouts.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, portraitLayoutsSize);
    for (auto i = 0; i < portraitLayoutsSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(portraitLayouts[i]));
    }

    const auto customizeDataSize = static_cast<int32_t>(customizeDatas.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, customizeDataSize);
    for (auto i = 0; i < customizeDataSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(customizeDatas[i].name));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(customizeDatas[i].value));
    }

    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, window.designWidth);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, window.autoDesignWidth);
    return true;
}

bool FormInfo::IsValid() const
{
    if (!window.autoDesignWidth && window.designWidth <= 0) {
        APP_LOGW("Invalid FormInfo, window.designWidth <= 0.");
        return false;
    }
    return true;
}

void to_json(nlohmann::json &jsonObject, const FormCustomizeData &customizeDatas)
{
    jsonObject = nlohmann::json{
        {JSON_KEY_NAME, customizeDatas.name},
        {JSON_KEY_VALUE, customizeDatas.value}
    };
}

void to_json(nlohmann::json &jsonObject, const FormWindow &formWindow)
{
    jsonObject[JSON_KEY_DESIGN_WIDTH] = formWindow.designWidth;
    jsonObject[JSON_KEY_AUTO_DESIGN_WIDTH] = formWindow.autoDesignWidth;
}

void to_json(nlohmann::json &jsonObject, const FormInfo &formInfo)
{
    jsonObject = nlohmann::json{
        {JSON_KEY_NAME, formInfo.name},
        {JSON_KEY_PACKAGE, formInfo.package},
        {JSON_KEY_BUNDLE_NAME, formInfo.bundleName},
        {JSON_KEY_MODULE_NAME, formInfo.moduleName},
        {JSON_KEY_ABILITY_NAME, formInfo.abilityName},
        {JSON_KEY_DESCRIPTION, formInfo.description},
        {JSON_KEY_RELATED_BUNDLE_NAME, formInfo.relatedBundleName},
        {JSON_KEY_JS_COMPONENT_NAME, formInfo.jsComponentName},
        {JSON_KEY_DEEP_LINK, formInfo.deepLink},
        {JSON_KEY_SRC, formInfo.src},
        {JSON_KEY_FORMCONFIG_ABILITY, formInfo.formConfigAbility},
        {JSON_KEY_SCHEDULED_UPDATE_TIME, formInfo.scheduledUpdateTime},
        {JSON_KEY_ORIGINAL_BUNDLE_NAME, formInfo.originalBundleName},
        {JSON_KEY_DESCRIPTION_ID, formInfo.descriptionId},
        {JSON_KEY_UPDATE_DURATION, formInfo.updateDuration},
        {JSON_KEY_DEFAULT_DIMENSION, formInfo.defaultDimension},
        {JSON_KEY_DEFAULT_FLAG, formInfo.defaultFlag},
        {JSON_KEY_FORM_VISIBLE_NOTIFY, formInfo.formVisibleNotify},
        {JSON_KEY_UPDATE_ENABLED, formInfo.updateEnabled},
        {JSON_KEY_TYPE, formInfo.type},
        {JSON_KEY_COLOR_MODE, formInfo.colorMode},
        {JSON_KEY_SUPPORT_DIMENSIONS, formInfo.supportDimensions},
        {JSON_KEY_CUSTOMIZE_DATA, formInfo.customizeDatas},
        {JSON_KEY_LANDSCAPE_LAYOUTS, formInfo.landscapeLayouts},
        {JSON_KEY_PORTRAIT_LAYOUTS, formInfo.portraitLayouts},
        {JSON_KEY_WINDOW, formInfo.window}
        };
}

void from_json(const nlohmann::json &jsonObject, FormCustomizeData &customizeDatas)
{
    customizeDatas.name = jsonObject.at(JSON_KEY_NAME).get<std::string>();
    customizeDatas.value = jsonObject.at(JSON_KEY_VALUE).get<std::string>();
}

void from_json(const nlohmann::json &jsonObject, FormWindow &formWindow)
{
    formWindow.designWidth = jsonObject.at(JSON_KEY_DESIGN_WIDTH).get<int32_t>();
    formWindow.autoDesignWidth = jsonObject.at(JSON_KEY_AUTO_DESIGN_WIDTH).get<bool>();
}

void from_json(const nlohmann::json &jsonObject, FormInfo &formInfo)
{
    formInfo.bundleName = jsonObject.at(JSON_KEY_BUNDLE_NAME).get<std::string>();
    formInfo.package = jsonObject.at(JSON_KEY_PACKAGE).get<std::string>();
    formInfo.moduleName = jsonObject.at(JSON_KEY_MODULE_NAME).get<std::string>();
    formInfo.abilityName = jsonObject.at(JSON_KEY_ABILITY_NAME).get<std::string>();
    formInfo.name = jsonObject.at(JSON_KEY_NAME).get<std::string>();
    formInfo.description = jsonObject.at(JSON_KEY_DESCRIPTION).get<std::string>();
    formInfo.relatedBundleName = jsonObject.at(JSON_KEY_RELATED_BUNDLE_NAME).get<std::string>();
    formInfo.jsComponentName = jsonObject.at(JSON_KEY_JS_COMPONENT_NAME).get<std::string>();
    formInfo.deepLink = jsonObject.at(JSON_KEY_DEEP_LINK).get<std::string>();
    formInfo.formConfigAbility = jsonObject.at(JSON_KEY_FORMCONFIG_ABILITY).get<std::string>();
    formInfo.scheduledUpdateTime = jsonObject.at(JSON_KEY_SCHEDULED_UPDATE_TIME).get<std::string>();
    formInfo.src = jsonObject.at(JSON_KEY_SRC).get<std::string>();
    formInfo.originalBundleName = jsonObject.at(JSON_KEY_ORIGINAL_BUNDLE_NAME).get<std::string>();
    formInfo.descriptionId = jsonObject.at(JSON_KEY_DESCRIPTION_ID).get<int32_t>();
    formInfo.updateDuration = jsonObject.at(JSON_KEY_UPDATE_DURATION).get<int32_t>();
    formInfo.defaultDimension = jsonObject.at(JSON_KEY_DEFAULT_DIMENSION).get<int32_t>();
    formInfo.defaultFlag = jsonObject.at(JSON_KEY_DEFAULT_FLAG).get<bool>();
    formInfo.formVisibleNotify = jsonObject.at(JSON_KEY_FORM_VISIBLE_NOTIFY).get<bool>();
    formInfo.updateEnabled = jsonObject.at(JSON_KEY_UPDATE_ENABLED).get<bool>();
    formInfo.type = jsonObject.at(JSON_KEY_TYPE).get<FormType>();
    formInfo.colorMode = jsonObject.at(JSON_KEY_COLOR_MODE).get<FormsColorMode>();
    formInfo.supportDimensions = jsonObject.at(JSON_KEY_SUPPORT_DIMENSIONS).get<std::vector<int32_t>>();
    formInfo.customizeDatas = jsonObject.at(JSON_KEY_CUSTOMIZE_DATA).get<std::vector<FormCustomizeData>>();
    formInfo.landscapeLayouts = jsonObject.at(JSON_KEY_LANDSCAPE_LAYOUTS).get<std::vector<std::string>>();
    formInfo.portraitLayouts = jsonObject.at(JSON_KEY_PORTRAIT_LAYOUTS).get<std::vector<std::string>>();
    formInfo.window = jsonObject.at(JSON_KEY_WINDOW).get<FormWindow>();
}
}  // namespace AppExecFwk
}  // namespace OHOS
