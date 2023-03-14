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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_PERMISSION_DEF_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_PERMISSION_DEF_H

#include <string>

#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
// provides known system security permissions.
struct PermissionDef : public Parcelable {
    std::string permissionName;
    std::string bundleName;
    int32_t grantMode = 0;
    int32_t availableLevel = 1;
    int32_t availableScope = 0;
    bool provisionEnable = true;
    bool distributedSceneEnable = false;
    std::string label;
    int32_t labelId = 0;
    std::string description;
    int32_t descriptionId = 0;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static PermissionDef *Unmarshalling(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_PERMISSION_DEF_H
