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

#include <fstream>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "ability_info.h"
#include "bundle_constants.h"
#include "bundle_info.h"
#include "inner_bundle_info.h"
#include "json_constants.h"
#include "json_serializer.h"
#include "nlohmann/json.hpp"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::JsonConstants;

namespace {
const std::string NORMAL_BUNDLE_NAME{"com.example.test"};
}  // namespace

class BmsBundleDataStorageDatabaseTest : public testing::Test {
public:
    BmsBundleDataStorageDatabaseTest();
    ~BmsBundleDataStorageDatabaseTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    enum class InfoType {
        BUNDLE_INFO,
        APPLICATION_INFO,
        ABILITY_INFO,
    };
    void CheckInvalidPropDeserialize(const nlohmann::json infoJson, const InfoType infoType) const;

protected:
    nlohmann::json innerBundleInfoJson_ = R"(
        {
            "allowedAcls": [],
            "appFeature": "ohos_system_app",
            "appType": 0,
            "baseAbilityInfos": {
                "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility": {
                    "applicationInfo": {
                        "accessTokenId": 0,
                        "accessible": false,
                        "apiCompatibleVersion": 0,
                        "apiReleaseType": "",
                        "apiTargetVersion": 0,
                        "appPrivilegeLevel": "normal",
                        "bundleName": "",
                        "cacheDir": "",
                        "codePath": "",
                        "cpuAbi": "",
                        "dataBaseDir": "",
                        "dataDir": "",
                        "debug": false,
                        "description": "",
                        "descriptionId": 0,
                        "deviceId": "",
                        "distributedNotificationEnabled": true,
                        "enabled": false,
                        "entityType": "",
                        "entryDir": "",
                        "entryModuleName": "",
                        "flags": 0,
                        "icon": "",
                        "iconId": 0,
                        "iconPath": "",
                        "isCloned": false,
                        "isCompressNativeLibs": true,
                        "isLauncherApp": false,
                        "isSystemApp": false,
                        "keepAlive": false,
                        "label": "",
                        "labelId": 0,
                        "metaData": {},
                        "metadata": {},
                        "minCompatibleVersionCode": 0,
                        "moduleInfos": [],
                        "moduleSourceDirs": [],
                        "name": "",
                        "nativeLibraryPath": "",
                        "permissions": [],
                        "targetBundleList": [
                        ],
                        "isFreeInstallApp": false,
                        "process": "",
                        "removable": true,
                        "signatureKey": "",
                        "singleton": false,
                        "supportedModes": 0,
                        "uid": -1,
                        "userDataClearable": true,
                        "vendor": "",
                        "versionCode": 0,
                        "versionName": ""
                    },
                    "extensionAbilityType": 9,
                    "priority": 0,
                    "startWindowIcon":"",
                    "startWindowIconId":0,
                    "startWindowBackground":"",
                    "startWindowBackgroundId":0,
                    "applicationName": "com.ohos.launcher",
                    "backgroundModes": 0,
                    "bundleName": "com.ohos.launcher",
                    "codePath": "",
                    "configChanges": [
                    ],
                    "continuable": false,
                    "defaultFormHeight": 0,
                    "defaultFormWidth": 0,
                    "description": "$string:mainability_description",
                    "descriptionId": 218103837,
                    "deviceCapabilities": [
                    ],
                    "deviceTypes": [
                        "phone"
                    ],
                    "enabled": true,
                    "formEnabled": false,
                    "formEntity": 0,
                    "iconId": 218103847,
                    "iconPath": "$media:icon",
                    "isLauncherAbility": true,
                    "isModuleJson": false,
                    "isNativeAbility": false,
                    "isStageBasedModel": true,
                    "kind": "page",
                    "label": "$string:entry_MainAbility",
                    "labelId": 218103828,
                    "launchMode": 0,
                    "metaData": {
                        "customizeData": [
                            {
                                "extra": "",
                                "name": "",
                                "value": ""
                            }
                        ]
                    },
                    "metadata": [
                    ],
                    "minFormHeight": 0,
                    "minFormWidth": 0,
                    "moduleName": "phone",
                    "name": "com.ohos.launcher.MainAbility",
                    "orientation": 0,
                    "package": "com.ohos.launcher",
                    "permissions": [
                    ],
                    "process": "",
                    "readPermission": "",
                    "resourcePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher/assets/phone/resources.index",
                    "srcEntrance": "",
                    "srcLanguage": "ets",
                    "srcPath": "MainAbility",
                    "supportPipMode": false,
                    "targetAbility": "",
                    "theme": "",
                    "type": 1,
                    "uri": "",
                    "visible": true,
                    "writePermission": ""
                },
                "com.ohos.launcher.com.ohos.launcher.recents.com.ohos.launcher.recents.MainAbility": {
                    "applicationName": "com.ohos.launcher",
                    "backgroundModes": 0,
                    "bundleName": "com.ohos.launcher",
                    "codePath": "",
                    "configChanges": [
                    ],
                    "continuable": false,
                    "defaultFormHeight": 0,
                    "defaultFormWidth": 0,
                    "description": "$string: mainability_description",
                    "descriptionId": 251658241,
                    "deviceCapabilities": [
                    ],
                    "deviceId": "",
                    "deviceTypes": [
                        "phone"
                    ],
                    "enabled": true,
                    "formEnabled": false,
                    "formEntity": 0,
                    "iconId": 251658246,
                    "iconPath": "$media: icon",
                    "isLauncherAbility": false,
                    "isModuleJson": false,
                    "isNativeAbility": false,
                    "isStageBasedModel": true,
                    "kind": "page",
                    "label": "$string: recents_MainAbility",
                    "labelId": 251658242,
                    "launchMode": 0,
                    "libPath": "",
                    "metaData": {
                        "customizeData": [
                            {
                                "extra": "",
                                "name": "",
                                "value": ""
                            }
                        ]
                    },
                    "metadata": [
                    ],
                    "minFormHeight": 0,
                    "minFormWidth": 0,
                    "moduleName": "recents",
                    "name": "com.ohos.launcher.recents.MainAbility",
                    "orientation": 0,
                    "package": "com.ohos.launcher.recents",
                    "permissions": [
                    ],
                    "process": "",
                    "readPermission": "",
                    "resourcePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher.recents/assets/recents/resources.index",
                    "srcEntrance": "",
                    "srcLanguage": "ets",
                    "srcPath": "MainAbility",
                    "supportPipMode": false,
                    "targetAbility": "",
                    "theme": "",
                    "type": 1,
                    "uri": "",
                    "visible": true,
                    "writePermission": ""
                },
                "com.ohos.launcher.com.ohos.launcher.settings.com.ohos.launcher.settings.MainAbility": {
                    "applicationName": "com.ohos.launcher",
                    "backgroundModes": 0,
                    "bundleName": "com.ohos.launcher",
                    "codePath": "",
                    "configChanges": [
                    ],
                    "continuable": false,
                    "defaultFormHeight": 0,
                    "defaultFormWidth": 0,
                    "description": "$string: mainability_description",
                    "descriptionId": 285212677,
                    "deviceCapabilities": [
                    ],
                    "deviceId": "",
                    "deviceTypes": [
                        "phone"
                    ],
                    "enabled": true,
                    "formEnabled": false,
                    "formEntity": 0,
                    "iconId": 285212682,
                    "iconPath": "$media: icon",
                    "isLauncherAbility": false,
                    "isModuleJson": false,
                    "isNativeAbility": false,
                    "isStageBasedModel": true,
                    "kind": "page",
                    "label": "$string: settings_MainAbility",
                    "labelId": 285212679,
                    "launchMode": 0,
                    "libPath": "",
                    "metaData": {
                        "customizeData": [
                        ]
                    },
                    "metadata": [
                    ],
                    "minFormHeight": 0,
                    "minFormWidth": 0,
                    "moduleName": "settings",
                    "name": "com.ohos.launcher.settings.MainAbility",
                    "orientation": 0,
                    "package": "com.ohos.launcher.settings",
                    "permissions": [
                    ],
                    "process": "",
                    "readPermission": "",
                    "resourcePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher.settings/assets/settings/resources.index",
                    "srcEntrance": "",
                    "srcLanguage": "ets",
                    "srcPath": "MainAbility",
                    "supportPipMode": false,
                    "targetAbility": "",
                    "theme": "",
                    "type": 1,
                    "uri": "",
                    "visible": true,
                    "writePermission": ""
                }
            },
            "baseApplicationInfo": {
                "accessTokenId": 0,
                "accessible": false,
                "apiCompatibleVersion": 8,
                "apiReleaseType": "Beta1",
                "apiTargetVersion": 8,
                "appPrivilegeLevel": "normal",
                "bundleName": "com.ohos.launcher",
                "cacheDir": "/data/app/el2/100/base/com.ohos.launcher/cache",
                "codePath": "/data/app/el1/bundle/public/com.ohos.launcher",
                "cpuAbi": "armeabi-v7a",
                "dataBaseDir": "/data/app/el2/100/database/com.ohos.launcher",
                "dataDir": "/data/app/el2/100/base/com.ohos.launcher",
                "debug": false,
                "description": "$string: mainability_description",
                "descriptionId": 218103837,
                "deviceId": "PHONE-001",
                "distributedNotificationEnabled": true,
                "enabled": true,
                "entityType": "unspecified",
                "entryDir": "",
                "entryModuleName": "",
                "flags": 0,
                "icon": "",
                "iconId": 218103847,
                "iconPath": "$media: icon",
                "isCloned": false,
                "isCompressNativeLibs": true,
                "isLauncherApp": true,
                "isSystemApp": true,
                "keepAlive": false,
                "label": "$string: entry_MainAbility",
                "labelId": 218103828,
                "metaData": {
                },
                "metadata": {
                },
                "minCompatibleVersionCode": 1000000,
                "moduleInfos": [
                ],
                "moduleSourceDirs": [
                ],
                "name": "com.ohos.launcher",
                "nativeLibraryPath": "",
                "permissions": [
                ],
                "targetBundleList": [
                ],
                "isFreeInstallApp": false,
                "process": "",
                "removable": false,
                "signatureKey": "",
                "singleton": false,
                "supportedModes": 0,
                "uid": -1,
                "userDataClearable": true,
                "vendor": "ohos",
                "versionCode": 1000000,
                "versionName": "1.0.0"
            },
            "baseBundleInfo": {
                "abilityInfos": [
                ],
                "appId": "com.ohos.launcher_BNtg4JBClbl92Rgc3jm/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=",
                "applicationInfo": {
                    "accessTokenId": 0,
                    "accessible": false,
                    "apiCompatibleVersion": 0,
                    "apiReleaseType": "",
                    "apiTargetVersion": 0,
                    "appPrivilegeLevel": "normal",
                    "bundleName": "",
                    "cacheDir": "",
                    "codePath": "",
                    "cpuAbi": "",
                    "dataBaseDir": "",
                    "dataDir": "",
                    "debug": false,
                    "description": "",
                    "descriptionId": 0,
                    "deviceId": "",
                    "distributedNotificationEnabled": true,
                    "enabled": false,
                    "entityType": "",
                    "entryDir": "",
                    "entryModuleName": "",
                    "flags": 0,
                    "icon": "",
                    "iconId": 0,
                    "iconPath": "",
                    "isCloned": false,
                    "isCompressNativeLibs": true,
                    "isLauncherApp": false,
                    "isSystemApp": false,
                    "keepAlive": false,
                    "label": "",
                    "labelId": 0,
                    "metaData": {
                    },
                    "metadata": {
                    },
                    "minCompatibleVersionCode": 0,
                    "moduleInfos": [
                    ],
                    "moduleSourceDirs": [
                    ],
                    "name": "",
                    "nativeLibraryPath": "",
                    "permissions": [
                    ],
                    "targetBundleList": [
                    ],
                    "isFreeInstallApp": false,
                    "process": "",
                    "removable": true,
                    "signatureKey": "",
                    "singleton": false,
                    "supportedModes": 0,
                    "uid": -1,
                    "userDataClearable": true,
                    "vendor": "",
                    "versionCode": 0,
                    "versionName": ""
                },
                "compatibleVersion": 8,
                "cpuAbi": "",
                "defPermissions": [
                ],
                "description": "",
                "entryInstallationFree": true,
                "entryModuleName": "phone",
                "extensionAbilityInfo": [
                ],
                "gid": -1,
                "hapModuleInfos": [
                ],
                "hapModuleNames": [
                ],
                "installTime": 0,
                "isDifferentName": false,
                "isKeepAlive": false,
                "isNativeApp": false,
                "isPreInstallApp": true,
                "jointUserId": "",
                "label": "",
                "mainEntry": "com.ohos.launcher",
                "maxSdkVersion": -1,
                "minCompatibleVersionCode": 1000000,
                "minSdkVersion": -1,
                "moduleDirs": [
                ],
                "moduleNames": [
                ],
                "modulePublicDirs": [
                ],
                "moduleResPaths": [
                ],
                "name": "com.ohos.launcher",
                "releaseType": "Beta1",
                "reqPermissionDetails": [
                ],
                "reqPermissionStates": [
                ],
                "reqPermissions": [
                ],
                "seInfo": "",
                "singleton": false,
                "targetVersion": 8,
                "uid": -1,
                "updateTime": 0,
                "vendor": "ohos",
                "versionCode": 1000000,
                "versionName": "1.0.0"
            },
            "baseDataDir": "/data/app/el2/100/base/com.ohos.launcher",
            "baseExtensionInfos_": {
            },
            "bundleStatus": 1,
            "commonEvents": {
            },
            "extensionSkillInfos_": {
            },
            "formInfos": {
                "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility": [
                ],
                "com.ohos.launcher.com.ohos.launcher.recents.com.ohos.launcher.recents.MainAbility": [
                ],
                "com.ohos.launcher.com.ohos.launcher.settings.com.ohos.launcher.settings.MainAbility": [
                ]
            },
            "gid": -1,
            "hasEntry": true,
            "innerBundleUserInfos": {
                "com.ohos.launcher_100": {
                    "accessTokenId": 537510556,
                    "bundleName": "com.ohos.launcher",
                    "bundleUserInfo": {
                        "abilities": [
                        ],
                        "disabledAbilities": [
                        ],
                        "enabled": true,
                        "userId": 100
                    },
                    "gids": [
                        20010012
                    ],
                    "installTime": 1503988035,
                    "uid": 20010012,
                    "updateTime": 1503988036
                }
            },
            "innerModuleInfos": {
                "com.ohos.launcher": {
                    "abilityKeys": [
                        "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility"
                    ],
                    "colorMode": -1,
                    "defPermissions": [
                    ],
                    "definePermissions": [
                    ],
                    "description": "",
                    "descriptionId": 0,
                    "deviceTypes": [
                    ],
                    "distro": {
                        "deliveryWithInstall": true,
                        "installationFree": true,
                        "moduleName": "phone",
                        "moduleType": "entry"
                    },
                    "extensionKeys": [
                    ],
                    "extensionSkillKeys": [
                    ],
                    "hapPath": "/data/app/el1/bundle/public/com.ohos.launcher/com_ohos_launcher.hap",
                    "hashValue": "",
                    "installationFree": true,
                    "isEntry": true,
                    "isModuleJson": false,
                    "isStageBasedModel": true,
                    "label": "$string: entry_MainAbility",
                    "labelId": 218103828,
                    "mainAbility": "com.ohos.launcher.MainAbility",
                    "metaData": {
                        "customizeData": [
                        ]
                    },
                    "metadata": [
                    ],
                    "moduleDataDir": "/data/app/el2/100/base/com.ohos.launcher/haps/com.ohos.launcher",
                    "moduleName": "phone",
                    "modulePackage": "com.ohos.launcher",
                    "modulePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher",
                    "moduleResPath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher/assets/phone/resources.index",
                    "pages": "",
                    "process": "",
                    "reqCapabilities": [
                    ],
                    "requestPermissions": [
                        {
                            "name": "ohos.permission.GET_BUNDLE_INFO_PRIVILEGED",
                            "reason": "",
                            "reasonId": 0,
                            "usedScene": {
                                "abilities": [
                                ],
                                "when": ""
                            }
                        },
                        {
                            "name": "ohos.permission.INSTALL_BUNDLE",
                            "reason": "",
                            "reasonId": 0,
                            "usedScene": {
                                "abilities": [
                                ],
                                "when": ""
                            }
                        },
                        {
                            "name": "ohos.permission.LISTEN_BUNDLE_CHANGE",
                            "reason": "",
                            "reasonId": 0,
                            "usedScene": {
                                "abilities": [
                                ],
                                "when": ""
                            }
                        },
                        {
                            "name": "ohos.permission.MANAGE_MISSIONS",
                            "reason": "",
                            "reasonId": 0,
                            "usedScene": {
                                "abilities": [
                                ],
                                "when": ""
                            }
                        },
                        {
                            "name": "ohos.permission.REQUIRE_FORM",
                            "reason": "",
                            "reasonId": 0,
                            "usedScene": {
                                "abilities": [
                                ],
                                "when": ""
                            }
                        }
                    ],
                    "skillKeys": [
                        "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility"
                    ],
                    "srcEntrance": "",
                    "srcPath": "",
                    "uiSyntax": "",
                    "virtualMachine": ""
                },
                "com.ohos.launcher.recents": {
                    "abilityKeys": [
                        "com.ohos.launcher.com.ohos.launcher.recents.com.ohos.launcher.recents.MainAbility"
                    ],
                    "colorMode": -1,
                    "defPermissions": [
                    ],
                    "definePermissions": [
                    ],
                    "description": "",
                    "descriptionId": 0,
                    "deviceTypes": [
                    ],
                    "distro": {
                        "deliveryWithInstall": true,
                        "installationFree": true,
                        "moduleName": "recents",
                        "moduleType": "feature"
                    },
                    "extensionKeys": [
                    ],
                    "extensionSkillKeys": [
                    ],
                    "hapPath": "/data/app/el1/bundle/public/com.ohos.launcher/com_ohos_launcher_recents.hap",
                    "hashValue": "",
                    "installationFree": true,
                    "isEntry": false,
                    "isModuleJson": false,
                    "isStageBasedModel": true,
                    "label": "",
                    "labelId": 0,
                    "mainAbility": "com.ohos.launcher.recents.MainAbility",
                    "metaData": {
                        "customizeData": [
                        ]
                    },
                    "metadata": [
                    ],
                    "moduleDataDir": "/data/app/el2/100/base/com.ohos.launcher/haps/com.ohos.launcher.recents",
                    "moduleName": "recents",
                    "modulePackage": "com.ohos.launcher.recents",
                    "modulePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher.recents",
                    "moduleResPath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher.recents/assets/recents/resources.index",
                    "pages": "",
                    "process": "",
                    "reqCapabilities": [
                    ],
                    "requestPermissions": [
                    ],
                    "skillKeys": [
                        "com.ohos.launcher.com.ohos.launcher.recents.com.ohos.launcher.recents.MainAbility"
                    ],
                    "srcEntrance": "",
                    "srcPath": "",
                    "uiSyntax": "",
                    "virtualMachine": ""
                },
                "com.ohos.launcher.settings": {
                    "abilityKeys": [
                        "com.ohos.launcher.com.ohos.launcher.settings.com.ohos.launcher.settings.MainAbility"
                    ],
                    "colorMode": -1,
                    "defPermissions": [
                    ],
                    "definePermissions": [
                    ],
                    "description": "",
                    "descriptionId": 0,
                    "deviceTypes": [
                    ],
                    "distro": {
                        "deliveryWithInstall": true,
                        "installationFree": true,
                        "moduleName": "settings",
                        "moduleType": "feature"
                    },
                    "extensionKeys": [
                    ],
                    "extensionSkillKeys": [
                    ],
                    "hapPath": "/data/app/el1/bundle/public/com.ohos.launcher/com_ohos_launcher_settings.hap",
                    "hashValue": "",
                    "installationFree": true,
                    "isEntry": false,
                    "isModuleJson": false,
                    "isStageBasedModel": true,
                    "label": "",
                    "labelId": 0,
                    "mainAbility": "com.ohos.launcher.settings.MainAbility",
                    "metaData": {
                        "customizeData": [
                        ]
                    },
                    "metadata": [
                    ],
                    "moduleDataDir": "/data/app/el2/100/base/com.ohos.launcher/haps/com.ohos.launcher.settings",
                    "moduleName": "settings",
                    "modulePackage": "com.ohos.launcher.settings",
                    "modulePath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher.settings",
                    "moduleResPath": "/data/app/el1/bundle/public/com.ohos.launcher/com.ohos.launcher.settings/assets/settings/resources.index",
                    "pages": "",
                    "process": "",
                    "reqCapabilities": [
                    ],
                    "requestPermissions": [
                    ],
                    "skillKeys": [
                        "com.ohos.launcher.com.ohos.launcher.settings.com.ohos.launcher.settings.MainAbility"
                    ],
                    "srcEntrance": "",
                    "srcPath": "",
                    "uiSyntax": "",
                    "virtualMachine": ""
                }
            },
            "installMark": {
                "installMarkBundle": "com.ohos.launcher",
                "installMarkPackage": "com.ohos.launcher.settings",
                "installMarkStatus": 2
            },
            "isNewVersion_": false,
            "isSupportBackup": false,
            "mainAbility": "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility",
            "mainAbilityName": "com.ohos.launcher.MainAbility",
            "newBundleName": "",
            "shortcutInfos": {
            },
            "skillInfos": {
                "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility": [
                    {
                        "actions": [
                            "action.system.home",
                            "com.ohos.action.main"
                        ],
                        "entities": [
                            "entity.system.home",
                            "flag.home.intent.from.system"
                        ],
                        "uris": [
                        ]
                    }
                ],
                "com.ohos.launcher.com.ohos.launcher.recents.com.ohos.launcher.recents.MainAbility": [
                ],
                "com.ohos.launcher.com.ohos.launcher.settings.com.ohos.launcher.settings.MainAbility": [
                ]
            },
            "uid": -1,
            "userId_": 100
        }
    )"_json;

    nlohmann::json moduleInfoJson_ = R"(
        {
            "moduleName": "entry",
            "moduleSourceDir": ""
        }
    )"_json;
    const std::string BASE_ABILITY_INFO = "baseAbilityInfos";
    // need modify with innerBundleInfoJson_
    const std::string abilityName = "com.ohos.launcher.com.ohos.launcher.com.ohos.launcher.MainAbility";
    const std::string BASE_BUNDLE_INFO = "baseBundleInfo";
    const std::string BASE_APPLICATION_INFO = "baseApplicationInfo";
};

BmsBundleDataStorageDatabaseTest::BmsBundleDataStorageDatabaseTest()
{}

BmsBundleDataStorageDatabaseTest::~BmsBundleDataStorageDatabaseTest()
{}

void BmsBundleDataStorageDatabaseTest::CheckInvalidPropDeserialize(
    const nlohmann::json infoJson, const InfoType infoType) const
{
    APP_LOGI("deserialize infoJson = %{public}s", infoJson.dump().c_str());
    nlohmann::json innerBundleInfoJson;
    nlohmann::json bundleInfoJson = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);

    switch (infoType) {
        case InfoType::BUNDLE_INFO: {
            bundleInfoJson = infoJson;
            BundleInfo bundleInfo = infoJson;
            break;
        }
        case InfoType::APPLICATION_INFO: {
            bundleInfoJson["appInfo"] = infoJson;
            ApplicationInfo applicationInfo = infoJson;
            break;
        }
        case InfoType::ABILITY_INFO: {
            bundleInfoJson["abilityInfos"].push_back(infoJson);
            AbilityInfo abilityInfo = infoJson;
            break;
        }
        default:
            break;
    }

    innerBundleInfoJson["baseBundleInfo"] = bundleInfoJson;
    InnerBundleInfo fromJsonInfo;
    EXPECT_FALSE(fromJsonInfo.FromJson(innerBundleInfoJson));
}

void BmsBundleDataStorageDatabaseTest::SetUpTestCase()
{}

void BmsBundleDataStorageDatabaseTest::TearDownTestCase()
{}

void BmsBundleDataStorageDatabaseTest::SetUp()
{}

void BmsBundleDataStorageDatabaseTest::TearDown()
{}

/**
 * @tc.number: BundleInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in BundleInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, BundleInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);
    // deserialize BundleInfo from json
    BundleInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;
    EXPECT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: BundleInfoJsonSerializer_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.test can catch deserialize error for type error for name prop in BundleInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, BundleInfoJsonSerializer_0200, Function | SmallTest | Level1)
{
    nlohmann::json typeErrorProps;
    typeErrorProps["name"] = NOT_STRING_TYPE;
    typeErrorProps["label"] = NOT_STRING_TYPE;
    typeErrorProps["description"] = NOT_STRING_TYPE;
    typeErrorProps["vendor"] = NOT_STRING_TYPE;
    typeErrorProps["mainEntry"] = NOT_STRING_TYPE;
    typeErrorProps["versionName"] = NOT_STRING_TYPE;
    typeErrorProps["versionCode"] = NOT_NUMBER_TYPE;
    typeErrorProps["minSdkVersion"] = NOT_NUMBER_TYPE;
    typeErrorProps["minSdkVersion"] = NOT_NUMBER_TYPE;

    for (nlohmann::json::iterator iter = typeErrorProps.begin(); iter != typeErrorProps.end(); iter++) {
        for (auto valueIter = iter.value().begin(); valueIter != iter.value().end(); valueIter++) {
            nlohmann::json infoJson = innerBundleInfoJson_.at(BASE_BUNDLE_INFO);
            infoJson[iter.key()] = valueIter.value();
        }
    }
}

/**
 * @tc.number: AbilityInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in AbilityInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, AbilityInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_.at(BASE_ABILITY_INFO).at(abilityName);
    // deserialize AbilityInfo from json
    AbilityInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;
    EXPECT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: AbilityInfoJsonSerializer_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.test can catch deserialize error for type error for name prop in AbilityInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, AbilityInfoJsonSerializer_0200, Function | SmallTest | Level1)
{
    nlohmann::json typeErrorProps;
    typeErrorProps["package"] = NOT_STRING_TYPE;
    typeErrorProps["name"] = NOT_STRING_TYPE;
    typeErrorProps["bundleName"] = NOT_STRING_TYPE;
    typeErrorProps["applicationName"] = NOT_STRING_TYPE;
    typeErrorProps["label"] = NOT_STRING_TYPE;
    typeErrorProps["description"] = NOT_STRING_TYPE;
    typeErrorProps["iconPath"] = NOT_STRING_TYPE;
    typeErrorProps["visible"] = NOT_BOOL_TYPE;
    typeErrorProps["kind"] = NOT_STRING_TYPE;
    typeErrorProps["type"] = NOT_NUMBER_TYPE;
    typeErrorProps["orientation"] = NOT_NUMBER_TYPE;
    typeErrorProps["launchMode"] = NOT_NUMBER_TYPE;
    typeErrorProps["codePath"] = NOT_STRING_TYPE;
    typeErrorProps["resourcePath"] = NOT_STRING_TYPE;
    typeErrorProps["libPath"] = NOT_STRING_TYPE;

    for (nlohmann::json::iterator iter = typeErrorProps.begin(); iter != typeErrorProps.end(); iter++) {
        for (auto valueIter = iter.value().begin(); valueIter != iter.value().end(); valueIter++) {
            APP_LOGD("deserialize check prop key = %{public}s, type = %{public}s",
                iter.key().c_str(),
                valueIter.key().c_str());
            nlohmann::json infoJson = innerBundleInfoJson_.at(BASE_ABILITY_INFO).at(abilityName);
            infoJson[iter.key()] = valueIter.value();
        }
    }
}

/**
 * @tc.number: ApplicationInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in ApplicationInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, ApplicationInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = innerBundleInfoJson_.at(BASE_APPLICATION_INFO);
    // deserialize ApplicationInfo from json
    ApplicationInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;
    EXPECT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: ApplicationInfoJsonSerializer_0200
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.test can catch deserialize error for type error for name prop in ApplicationInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, ApplicationInfoJsonSerializer_0200, Function | SmallTest | Level1)
{
    nlohmann::json typeErrorProps;
    typeErrorProps["name"] = NOT_STRING_TYPE;
    typeErrorProps["bundleName"] = NOT_STRING_TYPE;
    typeErrorProps["sandboxId"] = NOT_NUMBER_TYPE;
    typeErrorProps["signatureKey"] = NOT_STRING_TYPE;

    for (nlohmann::json::iterator iter = typeErrorProps.begin(); iter != typeErrorProps.end(); iter++) {
        for (auto valueIter = iter.value().begin(); valueIter != iter.value().end(); valueIter++) {
            APP_LOGD("deserialize check prop key = %{public}s, type = %{public}s",
                iter.key().c_str(),
                valueIter.key().c_str());
            nlohmann::json infoJson = innerBundleInfoJson_.at(BASE_APPLICATION_INFO);
            infoJson[iter.key()] = valueIter.value();
        }
    }
}

/**
 * @tc.number: ModuleInfoJsonSerializer_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally
 *           2.successfully serialize and deserialize all right props in ModuleInfo
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, ModuleInfoJsonSerializer_0100, Function | SmallTest | Level1)
{
    nlohmann::json sourceInfoJson = moduleInfoJson_;
    // deserialize ModuleInfo from json
    ModuleInfo fromJsonInfo = sourceInfoJson;
    // serialize fromJsonInfo to json
    nlohmann::json toJsonObject = fromJsonInfo;

    EXPECT_TRUE(toJsonObject.dump() == sourceInfoJson.dump());
}

/**
 * @tc.number: SaveData_0100
 * @tc.name: save bundle installation information to persist storage
 * @tc.desc: 1.system running normally and no saved any bundle data
 *           2.successfully save a new bundle installation information for the first time
 * @tc.require: AR000GJUIR
 */
HWTEST_F(BmsBundleDataStorageDatabaseTest, SaveData_0100, Function | SmallTest | Level0)
{
    InnerBundleInfo innerBundleInfo;
    EXPECT_EQ(innerBundleInfo.FromJson(innerBundleInfoJson_), OHOS::ERR_OK);
}