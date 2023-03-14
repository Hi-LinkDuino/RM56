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

#include "module_usage_record.h"

#include "string_ex.h"

#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {
bool ModuleUsageRecord::ReadFromParcel(Parcel &parcel)
{
    name = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    abilityName = Str16ToStr8(parcel.ReadString16());
    appLabelId = static_cast<uint32_t>(parcel.ReadInt32());
    labelId = static_cast<uint32_t>(parcel.ReadInt32());
    descriptionId = static_cast<uint32_t>(parcel.ReadInt32());
    abilityLabelId = static_cast<uint32_t>(parcel.ReadInt32());
    abilityDescriptionId = static_cast<uint32_t>(parcel.ReadInt32());
    abilityIconId = static_cast<uint32_t>(parcel.ReadInt32());
    launchedCount = static_cast<uint32_t>(parcel.ReadInt32());
    lastLaunchTime = parcel.ReadInt64();
    removed = parcel.ReadBool();
    installationFreeSupported = parcel.ReadBool();
    return true;
}

ModuleUsageRecord *ModuleUsageRecord::Unmarshalling(Parcel &parcel)
{
    ModuleUsageRecord *record = new ModuleUsageRecord();
    if (record && !record->ReadFromParcel(parcel)) {
        APP_LOGW("read ModuleUsageRecord from parcel failed");
        delete record;
        record = nullptr;
    }
    return record;
}

bool ModuleUsageRecord::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(name));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(abilityName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, appLabelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, abilityLabelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, abilityDescriptionId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, abilityIconId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, launchedCount);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, lastLaunchTime);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, removed);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, installationFreeSupported);
    return true;
}

std::string ModuleUsageRecord::ToString() const
{
    nlohmann::json json;
    json[UsageRecordKey::LAUNCHED_COUNT] = launchedCount;
    json[UsageRecordKey::LAST_LAUNCH_TIME] = lastLaunchTime;
    json[UsageRecordKey::IS_REMOVED] = removed;
    json[UsageRecordKey::ABILITY_NAME] = abilityName;
    return json.dump();
}

bool ModuleUsageRecord::FromJsonString(const std::string &jsonString)
{
    nlohmann::json jsonObject;

    jsonObject = nlohmann::json::parse(jsonString);
    if (jsonObject.is_discarded()) {
        APP_LOGE("failed to parse module usage record: %{public}s.", jsonString.c_str());
        return false;
    }

    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        UsageRecordKey::LAUNCHED_COUNT,
        launchedCount,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<int64_t>(jsonObject,
        jsonObjectEnd,
        UsageRecordKey::LAST_LAUNCH_TIME,
        lastLaunchTime,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<bool>(jsonObject,
        jsonObjectEnd,
        UsageRecordKey::IS_REMOVED,
        removed,
        JsonType::BOOLEAN,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        UsageRecordKey::ABILITY_NAME,
        abilityName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    return parseResult == ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS