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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_DISTRIBUTED_BUNDLE_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_DISTRIBUTED_BUNDLE_INFO_H

#include <string>

#include "distributed_module_info.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
struct DistributedBundleInfo : public Parcelable {
    uint32_t version = 1;
    std::string bundleName;
    uint32_t versionCode = 0;
    std::string versionName;
    uint32_t minCompatibleVersion = 0;
    uint32_t targetVersionCode = 0;
    uint32_t compatibleVersionCode = 0;
    std::string appId;
    std::vector<DistributedModuleInfo> moduleInfos;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static DistributedBundleInfo *Unmarshalling(Parcel &parcel);
    std::string ToString() const;
    bool FromJsonString(const std::string &jsonString);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_DISTRIBUTED_BUNDLE_INFO_H
