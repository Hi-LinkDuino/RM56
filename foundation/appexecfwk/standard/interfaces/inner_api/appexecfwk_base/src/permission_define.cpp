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

#include "permission_define.h"

#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "app_log_wrapper.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {
bool PermissionDef::ReadFromParcel(Parcel &parcel)
{
    permissionName = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    label = Str16ToStr8(parcel.ReadString16());
    description = Str16ToStr8(parcel.ReadString16());
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, availableLevel);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, availableScope);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, provisionEnable);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, distributedSceneEnable);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, grantMode);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    return true;
}

PermissionDef *PermissionDef::Unmarshalling(Parcel &parcel)
{
    PermissionDef *info = new (std::nothrow) PermissionDef();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool PermissionDef::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(permissionName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(label));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(description));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, availableLevel);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, availableScope);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, provisionEnable);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, distributedSceneEnable);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, grantMode);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labelId);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, descriptionId);
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
