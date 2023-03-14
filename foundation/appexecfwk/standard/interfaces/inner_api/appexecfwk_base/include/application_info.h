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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APPLICATION_INFO_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APPLICATION_INFO_H

#include <map>
#include <string>
#include <vector>

#include "module_info.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    const std::string AVAILABLELEVEL_NORMAL = "normal";
}
enum ApplicationFlag {
    GET_BASIC_APPLICATION_INFO = 0x00000000,
    GET_APPLICATION_INFO_WITH_PERMISSION = 0x00000008,
    GET_APPLICATION_INFO_WITH_METADATA = 0x00000040,
    GET_APPLICATION_INFO_WITH_DISABLE = 0x00000200,
    GET_ALL_APPLICATION_INFO = 0xFFFF0000,
};

struct Metadata : public Parcelable {
    std::string name;
    std::string value;
    std::string resource;
    Metadata() = default;
    Metadata(const std::string &paramName, const std::string &paramValue, const std::string &paramResource);
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static Metadata *Unmarshalling(Parcel &parcel);
};

struct CustomizeData : public Parcelable {
    std::string name;
    std::string value;
    std::string extra;
    CustomizeData() = default;
    CustomizeData(std::string paramName, std::string paramValue, std::string paramExtra);
    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static CustomizeData *Unmarshalling(Parcel &parcel);
};

struct MetaData {
    std::vector<CustomizeData> customizeData;
};

struct ApplicationInfo;

struct CompatibleApplicationInfo : public Parcelable {
    // items set when installing.
    std::string name; // application name.
    std::string icon; // application icon resource index.
    std::string label; // application name displayed to the user.
    std::string description; // description of application.
    std::string cpuAbi; // current device cpu abi.
    std::string process;
    bool isCompressNativeLibs = true;

    int32_t iconId = 0;
    int32_t labelId = 0;
    int32_t descriptionId = 0;

    bool systemApp = false;

    std::vector<std::string> permissions;
    std::vector<ModuleInfo> moduleInfos;

    int32_t supportedModes = 0; // supported modes.
    bool enabled = true;
    bool debug = false;

    bool ReadFromParcel(Parcel& parcel);
    virtual bool Marshalling(Parcel& parcel) const override;
    static CompatibleApplicationInfo* Unmarshalling(Parcel& parcel);
    void ConvertToApplicationInfo(ApplicationInfo& applicationInfo) const;
};

// configuration information about an application
struct ApplicationInfo : public Parcelable {
    std::string name;  // application name is same to bundleName
    std::string bundleName;

    uint32_t versionCode = 0;
    std::string versionName;
    int32_t minCompatibleVersionCode = 0;

    uint32_t apiCompatibleVersion = 0;
    int32_t apiTargetVersion = 0;

    std::string iconPath;
    int32_t iconId = 0;
    std::string label;
    int32_t labelId = 0;
    std::string description;
    int32_t descriptionId = 0;

    bool keepAlive = false;
    bool removable = true;
    bool singleton = false;
    bool userDataClearable = true;
    bool accessible = false;

    bool isSystemApp = false;
    bool isLauncherApp = false;
    bool isFreeInstallApp = false;

    std::string codePath;
    std::string dataDir;
    std::string dataBaseDir;
    std::string cacheDir;
    std::string entryDir;

    std::string apiReleaseType;
    bool debug = false;
    std::string deviceId;
    bool distributedNotificationEnabled = true;
    std::string entityType;
    std::string process;
    int32_t supportedModes = 0;  // returns 0 if the application does not support the driving mode
    std::string vendor;

    // apl
    std::string appPrivilegeLevel = AVAILABLELEVEL_NORMAL;

    // user related fields, assign when calling the get interface
    uint32_t accessTokenId = 0;
    bool enabled = false;
    int32_t uid = -1;

    // native so
    std::string nativeLibraryPath;
    std::string cpuAbi;

    // assign when calling the get interface
    std::vector<std::string> permissions;
    std::vector<std::string> moduleSourceDirs;
    std::vector<ModuleInfo> moduleInfos;
    std::map<std::string, std::vector<CustomizeData>> metaData;
    std::map<std::string, std::vector<Metadata>> metadata;
    // Installation-free
    std::vector<std::string> targetBundleList;

    bool isCloned = false;

    // unused
    std::string icon;
    int32_t flags = 0;
    std::string entryModuleName;
    bool isCompressNativeLibs = true;
    std::string signatureKey;

    bool ReadFromParcel(Parcel &parcel);
    bool ReadMetaDataFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static ApplicationInfo *Unmarshalling(Parcel &parcel);
    void Dump(std::string prefix, int fd);
    void ConvertToCompatibleApplicationInfo(CompatibleApplicationInfo& compatibleApplicationInfo) const;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_INCLUDE_APPLICATION_INFO_H
