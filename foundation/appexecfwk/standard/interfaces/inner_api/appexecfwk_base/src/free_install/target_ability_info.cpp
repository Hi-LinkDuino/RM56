/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "target_ability_info.h"

#include "app_log_wrapper.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_VERSION = "version";
const std::string JSON_KEY_TARGETINFO = "targetInfo";
const std::string JSON_KEY_TARGETEXTSETTING = "targetExtSetting";
const std::string JSON_KEY_EXTINFO = "extInfo";
const std::string JSON_KEY_TRANSACTID = "transactId";
const std::string JSON_KEY_BUNDLENAME = "bundleName";
const std::string JSON_KEY_MODULENAME = "moduleName";
const std::string JSON_KEY_ABILITYNAME = "abilityName";
const std::string JSON_KEY_FLAGS = "flags";
const std::string JSON_KEY_CALLINGUID = "callingUid";
const std::string JSON_KEY_CALLINGAPPTYPE = "callingAppType";
const std::string JSON_KEY_CALLINGBUNDLENAMES = "callingBundleNames";
const std::string JSON_KEY_CALLINGAPPIDS = "callingAppIds";
}  // namespace

void to_json(nlohmann::json &jsonObject, const TargetInfo &targetInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_TRANSACTID, targetInfo.transactId},
        {JSON_KEY_BUNDLENAME, targetInfo.bundleName},
        {JSON_KEY_MODULENAME, targetInfo.moduleName},
        {JSON_KEY_ABILITYNAME, targetInfo.abilityName},
        {JSON_KEY_FLAGS, targetInfo.flags},
        {JSON_KEY_CALLINGUID, targetInfo.callingUid},
        {JSON_KEY_CALLINGAPPTYPE, targetInfo.callingAppType},
        {JSON_KEY_CALLINGBUNDLENAMES, targetInfo.callingBundleNames},
        {JSON_KEY_CALLINGAPPIDS, targetInfo.callingAppIds}
    };
}

void from_json(const nlohmann::json &jsonObject, TargetInfo &targetInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TRANSACTID,
        targetInfo.transactId,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_BUNDLENAME,
        targetInfo.bundleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_MODULENAME,
        targetInfo.moduleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ABILITYNAME,
        targetInfo.abilityName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_FLAGS,
        targetInfo.flags,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGUID,
        targetInfo.callingUid,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGAPPTYPE,
        targetInfo.callingAppType,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGBUNDLENAMES,
        targetInfo.callingBundleNames,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGAPPIDS,
        targetInfo.callingAppIds,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
}

bool TargetInfo::ReadFromParcel(Parcel &parcel)
{
    transactId = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    moduleName = Str16ToStr8(parcel.ReadString16());
    abilityName = Str16ToStr8(parcel.ReadString16());
    flags = parcel.ReadInt32();
    callingUid = parcel.ReadInt32();
    callingAppType = parcel.ReadInt32();
    if (!parcel.ReadStringVector(&callingBundleNames)) {
        APP_LOGE("read callingBundleNames from parcel failed");
        return false;
    }

    if (!parcel.ReadStringVector(&callingAppIds)) {
        APP_LOGE("read callingAppIds from parcel failed");
        return false;
    }
    return true;
}

bool TargetInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(transactId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(abilityName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, flags);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingUid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingAppType);
    for (auto &callingBundleName : callingBundleNames) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(callingBundleName));
    }
    for (auto &callingAppId : callingAppIds) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(callingAppId));
    }
    return true;
}

TargetInfo *TargetInfo::Unmarshalling(Parcel &parcel)
{
    TargetInfo *targetInfo = new (std::nothrow) TargetInfo();
    if (targetInfo && !targetInfo->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete targetInfo;
        targetInfo = nullptr;
    }
    return targetInfo;
}

void to_json(nlohmann::json &jsonObject, const TargetAbilityInfo &targetAbilityInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_VERSION, targetAbilityInfo.version},
        {JSON_KEY_TARGETINFO, targetAbilityInfo.targetInfo},
        {JSON_KEY_TARGETEXTSETTING, targetAbilityInfo.targetExtSetting},
    };
}

void from_json(const nlohmann::json &jsonObject, TargetAbilityInfo &targetAbilityInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_VERSION,
        targetAbilityInfo.version,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<TargetInfo>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TARGETINFO,
        targetAbilityInfo.targetInfo,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TARGETEXTSETTING,
        targetAbilityInfo.targetExtSetting,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

bool TargetAbilityInfo::ReadFromParcel(Parcel &parcel)
{
    version = Str16ToStr8(parcel.ReadString16());
    auto params = parcel.ReadParcelable<TargetInfo>();
    if (params != nullptr) {
        targetInfo = *params;
        delete params;
        params = nullptr;
    } else {
        return false;
    }
    targetExtSetting = Str16ToStr8(parcel.ReadString16());
    return true;
}

bool TargetAbilityInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(version));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &targetInfo);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(targetExtSetting));
    return true;
}

TargetAbilityInfo *TargetAbilityInfo::Unmarshalling(Parcel &parcel)
{
    TargetAbilityInfo *targetAbilityInfo = new (std::nothrow) TargetAbilityInfo();
    if (targetAbilityInfo && !targetAbilityInfo->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete targetAbilityInfo;
        targetAbilityInfo = nullptr;
    }
    return targetAbilityInfo;
}
}  //  namespace AppExecFwk
}  //  namespace OHOS