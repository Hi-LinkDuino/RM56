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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_MODULE_USAGE_RECORD_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_MODULE_USAGE_RECORD_H

#include <string>

#include "ability_info.h"
#include "parcel.h"
#include "refbase.h"

namespace OHOS {
namespace AppExecFwk {
namespace UsageRecordKey {
    const std::string LAUNCHED_COUNT = "launchedCount";
    const std::string LAST_LAUNCH_TIME = "lastLaunchTime";
    const std::string IS_REMOVED = "isRemoved";
    const std::string ABILITY_NAME = "abilityName";
    const std::string SCHEMA_LAST_LAUNCH_TIME = "$.lastLaunchTime";
} // UsageRecordKey

struct ModuleUsageRecord : public Parcelable {
    std::string bundleName;
    uint32_t appLabelId = 0;
    std::string name;
    uint32_t labelId = 0;
    uint32_t descriptionId = 0;
    std::string abilityName;
    uint32_t abilityLabelId = 0;
    uint32_t abilityDescriptionId = 0;
    uint32_t abilityIconId = 0;
    uint32_t launchedCount = 0;
    int64_t lastLaunchTime = 0;
    bool removed = false;
    bool installationFreeSupported = true;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static ModuleUsageRecord *Unmarshalling(Parcel &parcel);
    std::string ToString() const;
    bool FromJsonString(const std::string &jsonString);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_MODULE_USAGE_RECORD_H