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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_PACK_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_PACK_INFO_H

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {

enum BundlePackFlag {
    GET_PACK_INFO_ALL = 0x00000000,
    GET_PACKAGES = 0x00000001,
    GET_BUNDLE_SUMMARY = 0x00000002,
    GET_MODULE_SUMMARY = 0x00000004,
};

struct Version {
    uint32_t code = 0;
    std::string name;
    uint32_t minCompatibleVersionCode = 0;
};

struct PackageApp {
    std::string bundleName;
    Version version;
};

struct AbilityFormInfo {
    std::string name;
    std::string type;
    bool updateEnabled = false;
    std::string scheduledUpdateTime;
    uint32_t updateDuration = 0;
    std::vector<std::string> supportDimensions;
    std::string defaultDimension;
};

struct ModuleAbilityInfo {
    std::string name;
    std::string label;
    bool visible = false;
    std::vector<AbilityFormInfo> forms;
};

struct ModuleDistro {
    std::string moduleType;
    std::string moduleName;
    bool installationFree = false;
    bool deliveryWithInstall = false;
};

struct ApiVersion {
    uint32_t compatible = 0;
    std::string releaseType;
    uint32_t target = 0;
};

struct PackageModule {
    std::string mainAbility;
    std::vector<std::string> deviceType;
    std::vector<ModuleAbilityInfo> abilities;
    ModuleDistro distro;
    ApiVersion apiVersion;
};

struct Summary {
    PackageApp app;
    std::vector<PackageModule> modules;
};

struct Packages {
    std::vector<std::string> deviceType;
    std::string moduleType;
    bool deliveryWithInstall = false;
    std::string name;
};

struct BundlePackInfo : public Parcelable {
    Summary summary;
    std::vector<Packages> packages;

    bool GetValid() const
    {
        return isValid_;
    }
    void SetValid(bool isValid)
    {
        isValid_ = isValid;
    }
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static BundlePackInfo *Unmarshalling(Parcel &parcel);
private:
    bool isValid_ = false;
};
} // AppExecFwk
} // OHOS
#endif // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_BUNDLE_PACK_INFO_H
