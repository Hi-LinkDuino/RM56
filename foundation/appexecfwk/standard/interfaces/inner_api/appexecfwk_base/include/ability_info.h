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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ABILITY_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ABILITY_INFO_H

#include <string>

#include "parcel.h"
#include "application_info.h"
#include "extension_ability_info.h"

namespace OHOS {
namespace AppExecFwk {
enum AbilityInfoFlag {
    GET_ABILITY_INFO_DEFAULT = 0x00000000,
    GET_ABILITY_INFO_WITH_PERMISSION = 0x00000002,
    GET_ABILITY_INFO_WITH_APPLICATION = 0x00000004,
    GET_ABILITY_INFO_WITH_METADATA = 0x00000020,
    GET_ABILITY_INFO_SYSTEMAPP_ONLY = 0x00000080,
    GET_ABILITY_INFO_WITH_DISABLE = 0x00000100,
};

enum class AbilityType {
    UNKNOWN = 0,
    PAGE,
    SERVICE,
    DATA,
    FORM,
    EXTENSION,
};

enum class BackgroundMode {
    DEFAULT = 0,
    DATA_TRANSFER = 1 << 0,
    AUDIO_PLAYBACK = 1 << 1,
    AUDIO_RECORDING = 1 << 2,
    LOCATION = 1 << 3,
    BLUETOOTH_INTERACTION = 1 << 4,
    MULTI_DEVICE_CONNECTION = 1 << 5,
    WIFI_INTERACTION = 1 << 6,
    VOIP = 1 << 7,
    TASK_KEEPING = 1 << 8,
};

enum class AbilitySubType {
    UNSPECIFIED = 0,
    CA,
};

enum class DisplayOrientation {
    UNSPECIFIED = 0,
    LANDSCAPE,
    PORTRAIT,
    FOLLOWRECENT,
};

enum class LaunchMode {
    SINGLETON = 0,
    STANDARD,  // support more than one instance
    SINGLETOP,
    SPECIFIED,
};

struct AbilityInfo;

/*
* According to Ability profile 1.0
*/
struct CompatibleAbilityInfo : public Parcelable {
    // deprecated: ability code class simple name, use 'className' instead.
    std::string package;
    std::string name;
    std::string label; // display name on screen.
    std::string description;
    std::string iconPath; // used as icon data (base64) for WEB Ability.
    std::string uri; // uri of ability.
    std::string moduleName; // indicates the name of the .hap package to which the capability belongs.
    std::string process;
    std::string targetAbility;
    std::string appName;
    std::string privacyUrl;
    std::string privacyName;
    std::string downloadUrl;
    std::string versionName;
    uint32_t backgroundModes = 0;
    uint32_t packageSize = 0; // The size of the package that AbilityInfo.uri points to.
    bool visible = false;
    bool formEnabled = false;
    bool multiUserShared = false;
    // deprecated: remove this field in new package format.
    AbilityType type = AbilityType::UNKNOWN;
    AbilitySubType subType = AbilitySubType::UNSPECIFIED;
    DisplayOrientation orientation = DisplayOrientation::UNSPECIFIED;
    LaunchMode launchMode = LaunchMode::SINGLETON;
    std::vector<std::string> permissions;
    std::vector<std::string> deviceTypes;
    std::vector<std::string> deviceCapabilities;
    bool supportPipMode = false;
    bool grantPermission = false;
    std::string readPermission;
    std::string writePermission;
    std::string uriPermissionMode;
    std::string uriPermissionPath;
    bool directLaunch = true;

    // set when install
    std::string bundleName; // bundle name which has this ability.
    std::string className;  // the ability full class name.
    std::string originalClassName; // the original ability full class name
    std::string deviceId; // device UDID information.
    CompatibleApplicationInfo applicationInfo;

    // form widget info
    uint32_t formEntity = 1; // where form can be displayed
    int32_t minFormHeight = 0; // minimum height of ability.
    int32_t defaultFormHeight = 0; // default height of ability.
    int32_t minFormWidth = 0; // minimum width of ability.
    int32_t defaultFormWidth = 0; // default width of ability.

    int32_t iconId = 0;
    int32_t labelId = 0;
    int32_t descriptionId = 0;
    bool enabled = true;

    bool ReadFromParcel(Parcel& parcel);
    virtual bool Marshalling(Parcel& parcel) const override;
    static CompatibleAbilityInfo* Unmarshalling(Parcel& parcel);

    void CopyToDest(CompatibleAbilityInfo& dest) const;
    void ConvertToAbilityInfo(AbilityInfo& abilityInfo) const;
};

// configuration information about an ability
struct AbilityInfo : public Parcelable {
    std::string name;  // ability name, only the main class name
    std::string label;
    std::string description;
    std::string iconPath;
    int32_t labelId;
    int32_t descriptionId;
    int32_t iconId;
    std::string theme;
    bool visible = false;
    std::string kind;  // ability category
    AbilityType type = AbilityType::UNKNOWN;
    ExtensionAbilityType extensionAbilityType = ExtensionAbilityType::UNSPECIFIED;
    DisplayOrientation orientation = DisplayOrientation::UNSPECIFIED;
    LaunchMode launchMode = LaunchMode::SINGLETON;
    std::string srcPath;
    std::string srcLanguage = "js";
    std::vector<std::string> permissions;

    std::string process;
    std::vector<std::string> deviceTypes;
    std::vector<std::string> deviceCapabilities;
    std::string uri;
    std::string targetAbility;
    ApplicationInfo applicationInfo;
    bool isLauncherAbility = false;
    bool isNativeAbility = false;
    bool enabled = false;
    bool supportPipMode = false;
    bool formEnabled = false;
    std::string readPermission;
    std::string writePermission;
    std::vector<std::string> configChanges;
    uint32_t formEntity = 0;
    int32_t minFormHeight = 0;
    int32_t defaultFormHeight = 0;
    int32_t minFormWidth = 0;
    int32_t defaultFormWidth = 0;
    MetaData metaData;
    uint32_t backgroundModes = 0;

    // set when install
    std::string package;  // the "module.package" in config.json
    std::string bundleName;
    std::string moduleName;       // the "module.name" in config.json
    std::string applicationName;  // the "bundlename" in config.json

    std::string codePath;         // ability main code path with name
    std::string resourcePath;     // resource path for resource init

    std::string srcEntrance;
    std::vector<Metadata> metadata;
    bool isModuleJson = false;
    bool isStageBasedModel = false;
    bool continuable = false;
    int32_t priority = 0;

    // configuration fields on startup page
    std::string startWindowIcon;
    int32_t startWindowIconId;
    std::string startWindowBackground;
    int32_t startWindowBackgroundId;

    // unused
    std::string originalBundleName;
    std::string appName;
    std::string privacyUrl;
    std::string privacyName;
    std::string downloadUrl;
    std::string versionName;
    std::string className;
    std::string originalClassName;
    std::string uriPermissionMode;
    std::string uriPermissionPath;
    uint32_t packageSize = 0;
    bool multiUserShared = false;
    bool grantPermission = false;
    bool directLaunch = true;
    AbilitySubType subType = AbilitySubType::UNSPECIFIED;
    std::string libPath;
    std::string deviceId;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static AbilityInfo *Unmarshalling(Parcel &parcel);
    void Dump(std::string prefix, int fd);
    void ConvertToCompatiableAbilityInfo(CompatibleAbilityInfo& compatibleAbilityInfo) const;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_ABILITY_INFO_H
