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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_TARGET_ABILITY_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_TARGET_ABILITY_INFO_H

#include <map>
#include <string>

#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
struct TargetInfo : public Parcelable {
    std::string transactId;
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    int32_t flags;
    int32_t callingUid;
    int32_t callingAppType;
    std::vector<std::string> callingBundleNames;
    std::vector<std::string> callingAppIds;
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static TargetInfo *Unmarshalling(Parcel &parcel);
};

struct TargetAbilityInfo : public Parcelable {
    std::string version;
    TargetInfo targetInfo;
    std::string targetExtSetting;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static TargetAbilityInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_TARGET_ABILITY_INFO_H