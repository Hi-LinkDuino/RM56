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

#include "dispatch_info.h"

#include "app_log_wrapper.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_VERSION = "version";
}  //  namespace

void to_json(nlohmann::json &jsonObject, const DispatcherInfo &dispatcherInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_VERSION, dispatcherInfo.version}
    };
}

void from_json(const nlohmann::json &jsonObject, DispatcherInfo &dispatcherInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_VERSION,
        dispatcherInfo.version,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
}

bool DispatcherInfo::ReadFromParcel(Parcel &parcel)
{
    version = Str16ToStr8(parcel.ReadString16());
    return true;
}

bool DispatcherInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(version));
    return true;
}

DispatcherInfo *DispatcherInfo::Unmarshalling(Parcel &parcel)
{
    DispatcherInfo *dispatcherInfo = new (std::nothrow) DispatcherInfo();
    if (dispatcherInfo && !dispatcherInfo->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete dispatcherInfo;
        dispatcherInfo = nullptr;
    }
    return dispatcherInfo;
}
}  //  namespace AppExecFwk
}  //  namespace OHOS