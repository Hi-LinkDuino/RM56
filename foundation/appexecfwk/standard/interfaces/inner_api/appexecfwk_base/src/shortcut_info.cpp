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
#include "shortcut_info.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "bundle_constants.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_BUNDLE_ID = "id";
const std::string JSON_KEY_BUNDLE_NAME = "bundleName";
const std::string JSON_KEY_BUNDLE_HOST_ABILITY = "hostAbility";
const std::string JSON_KEY_BUNDLE_ICON = "icon";
const std::string JSON_KEY_BUNDLE_LABEL = "label";
const std::string JSON_KEY_BUNDLE_DISABLE_MESSAGE = "disableMessage";
const std::string JSON_KEY_BUNDLE_IS_STATIC = "isStatic";
const std::string JSON_KEY_BUNDLE_IS_HOME_SHORTCUT = "isHomeShortcut";
const std::string JSON_KEY_BUNDLE_IS_ENABLES = "isEnables";
const std::string JSON_KEY_BUNDLE_INTENTS = "intents";
const std::string JSON_KEY_BUNDLE_TARGET_BUNDLE = "targetBundle";
const std::string JSON_KEY_BUNDLE_TARGET_CLASS = "targetClass";
const std::string JSON_KEY_ICON_ID = "iconId";
const std::string JSON_KEY_LABEL_ID = "labelId";
const std::string SHORTCUTS = "shortcuts";
const std::string SHORTCUT_ID = "shortcutId";
const std::string SHORTCUT_WANTS = "wants";
const std::string WANT_BUNDLE_NAME = "bundleName";
const std::string WANT_ABILITY_NAME = "abilityName";
const std::string ICON = "icon";
const std::string ICON_ID = "iconId";
const std::string LABEL = "label";
const std::string LABEL_ID = "labelId";
}  // namespace

bool ShortcutInfo::ReadFromParcel(Parcel &parcel)
{
    id = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    hostAbility = Str16ToStr8(parcel.ReadString16());
    icon = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    iconId = parcel.ReadInt32();
    labelId = parcel.ReadInt32();
    disableMessage = Str16ToStr8(parcel.ReadString16());
    isStatic = parcel.ReadBool();
    isHomeShortcut = parcel.ReadBool();
    isEnables = parcel.ReadBool();
    int32_t intentsSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, intentsSize);
    for (auto i = 0; i < intentsSize; i++) {
        ShortcutIntent shortcutIntent;
        std::string targetBundleName = Str16ToStr8(parcel.ReadString16());
        std::string targetClassName = Str16ToStr8(parcel.ReadString16());
        shortcutIntent.targetBundle = targetBundleName;
        shortcutIntent.targetClass = targetClassName;
        intents.emplace_back(shortcutIntent);
    }
    return true;
}

ShortcutInfo *ShortcutInfo::Unmarshalling(Parcel &parcel)
{
    ShortcutInfo *info = new ShortcutInfo();
    if (!info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool ShortcutInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(id));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(hostAbility));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(icon));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, iconId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(disableMessage));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isStatic);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isHomeShortcut);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isEnables);

    const auto intentsSize = static_cast<int32_t>(intents.size());
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, intentsSize);
    for (auto i = 0; i < intentsSize; i++) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(intents[i].targetBundle));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(intents[i].targetClass));
    }
    return true;
}

void to_json(nlohmann::json &jsonObject, const ShortcutIntent &shortcutIntent)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_BUNDLE_TARGET_BUNDLE, shortcutIntent.targetBundle},
        {JSON_KEY_BUNDLE_TARGET_CLASS, shortcutIntent.targetClass}};
}

void to_json(nlohmann::json &jsonObject, const ShortcutInfo &shortcutInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_BUNDLE_ID, shortcutInfo.id},
        {JSON_KEY_BUNDLE_NAME, shortcutInfo.bundleName},
        {JSON_KEY_BUNDLE_HOST_ABILITY, shortcutInfo.hostAbility},
        {JSON_KEY_BUNDLE_ICON, shortcutInfo.icon},
        {JSON_KEY_BUNDLE_LABEL, shortcutInfo.label},
        {JSON_KEY_BUNDLE_DISABLE_MESSAGE, shortcutInfo.disableMessage},
        {JSON_KEY_BUNDLE_IS_STATIC, shortcutInfo.isStatic},
        {JSON_KEY_BUNDLE_IS_HOME_SHORTCUT, shortcutInfo.isHomeShortcut},
        {JSON_KEY_BUNDLE_IS_ENABLES, shortcutInfo.isEnables},
        {JSON_KEY_BUNDLE_INTENTS, shortcutInfo.intents},
        {JSON_KEY_ICON_ID, shortcutInfo.iconId},
        {JSON_KEY_LABEL_ID, shortcutInfo.labelId},
  };
}

void from_json(const nlohmann::json &jsonObject, ShortcutIntent &shortcutIntent)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_TARGET_BUNDLE,
        shortcutIntent.targetBundle,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_TARGET_CLASS,
        shortcutIntent.targetClass,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, ShortcutInfo &shortcutInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_ID,
        shortcutInfo.id,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_NAME,
        shortcutInfo.bundleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_HOST_ABILITY,
        shortcutInfo.hostAbility,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_ICON,
        shortcutInfo.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_LABEL,
        shortcutInfo.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_DISABLE_MESSAGE,
        shortcutInfo.disableMessage,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_IS_STATIC,
        shortcutInfo.isStatic,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_IS_HOME_SHORTCUT,
        shortcutInfo.isHomeShortcut,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_IS_ENABLES,
        shortcutInfo.isEnables,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<ShortcutIntent>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLE_INTENTS,
        shortcutInfo.intents,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ICON_ID,
        shortcutInfo.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_LABEL_ID,
        shortcutInfo.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

void from_json(const nlohmann::json &jsonObject, ShortcutWant &shortcutWant)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        WANT_BUNDLE_NAME,
        shortcutWant.bundleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        WANT_ABILITY_NAME,
        shortcutWant.abilityName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read shortcutWant from module.json error, error code : %{public}d", parseResult);
    }
}

void from_json(const nlohmann::json &jsonObject, Shortcut &shortcut)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        SHORTCUT_ID,
        shortcut.shortcutId,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        ICON,
        shortcut.icon,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        ICON_ID,
        shortcut.iconId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        LABEL,
        shortcut.label,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int32_t>(jsonObject,
        jsonObjectEnd,
        LABEL_ID,
        shortcut.labelId,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<ShortcutWant>>(jsonObject,
        jsonObjectEnd,
        SHORTCUT_WANTS,
        shortcut.wants,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read Shortcut from module.json error, error code : %{public}d", parseResult);
    }
}

void from_json(const nlohmann::json &jsonObject, ShortcutJson &shortcutJson)
{
    APP_LOGD("read shortcuts tag from module.json");
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::vector<Shortcut>>(jsonObject,
        jsonObjectEnd,
        SHORTCUTS,
        shortcutJson.shortcuts,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
    if (parseResult != ERR_OK) {
        APP_LOGE("read ShortcutJson from module.json error, error code : %{public}d", parseResult);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
