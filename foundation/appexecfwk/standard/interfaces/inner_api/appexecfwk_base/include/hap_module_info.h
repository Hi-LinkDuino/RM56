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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_HAP_MODULE_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_HAP_MODULE_INFO_H

#include <string>

#include "ability_info.h"
#include "extension_ability_info.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
enum class ModuleColorMode {
    AUTO = -1,
    DARK,
    LIGHT,
};

enum class ModuleType {
    UNKNOWN = 0,
    ENTRY = 1,
    FEATURE = 2,
    HAR = 3
};

// configuration information about an module
struct HapModuleInfo : public Parcelable {
    std::string name;        // module.package in config.json
    std::string moduleName;  // module.name in config.json
    std::string description;
    int32_t descriptionId = 0;
    std::string iconPath;
    std::string label;
    int32_t labelId = 0;
    std::string backgroundImg;
    std::string mainAbility;
    std::string srcPath;
    std::string hashValue;
    std::string hapPath;
    int supportedModes = 0;

    std::vector<std::string> reqCapabilities;
    std::vector<std::string> deviceTypes;
    std::vector<std::string> dependencies;
    std::vector<AbilityInfo> abilityInfos;
    ModuleColorMode colorMode = ModuleColorMode::AUTO;
    // new version fields
    std::string bundleName;
    std::string mainElementName;
    std::string pages;
    std::string process;
    std::string resourcePath;
    std::string srcEntrance;
    std::string uiSyntax;
    std::string virtualMachine;
    bool deliveryWithInstall = false;
    bool installationFree = false;
    bool isModuleJson = false;
    bool isStageBasedModel = false;
    std::map<std::string, bool> isRemovable;
    ModuleType moduleType = ModuleType::UNKNOWN;
    std::vector<ExtensionAbilityInfo> extensionInfos;
    std::vector<Metadata> metadata;
    int32_t upgradeFlag = 0;
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static HapModuleInfo *Unmarshalling(Parcel &parcel);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_HAP_MODULE_INFO_H
