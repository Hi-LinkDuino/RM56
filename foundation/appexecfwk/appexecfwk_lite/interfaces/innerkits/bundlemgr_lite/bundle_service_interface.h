/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_BMS_INTERFACE_H
#define OHOS_BMS_INTERFACE_H

#include "feature.h"
#include "iproxy_server.h"
#include "want.h"
#include "install_param.h"
#include "bundle_manager.h"
#include "bundle_info.h"
#include "bundle_install_msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

const char BMS_SERVICE[] = "bundlems";
const char BMS_FEATURE[] = "BmsFeature";
const char BMS_SLITE_FEATURE[] = "BmsSliteFeature";
const char BMS_INNER_FEATURE[] = "BmsInnerFeature";
const unsigned int MAX_PATH_LEN = 256;

typedef struct {
    char bundleParameter[MAX_PATH_LEN];
    InstallParam *bundleInstallParam;
    InstallerCallback bundleInstallerCallback;
} Bmsbuff;

struct BmsSliteInterface {
    INHERIT_IUNKNOWN;
    bool (*Install)(const char *hapPath, const InstallParam *installParam, InstallerCallback installerCallback);
    bool (*Uninstall)(const char *bundleName, const InstallParam *installParam, InstallerCallback installerCallback);
    uint8_t (*QueryAbilityInfo)(const Want *want, AbilityInfo *abilityInfo);
    uint8_t (*GetBundleInfo)(const char *bundleName, int32_t flags, BundleInfo *bundleInfo);
    uint8_t (*GetBundleInfos)(int32_t flags, BundleInfo **bundleInfos, int32_t *len);
    bool (*GetInstallState)(const char *bundleName, InstallState *installState, uint8_t *installProcess);
    uint32_t (*GetBundleSize)(const char *bundleName);
    bool (*RegisterInstallerCallback)(InstallerCallback installerCallback);
    void (*UpdateBundleInfoList)();
    uint8_t (*GetBundleInfosNoReplication)(const int flags, BundleInfo **bundleInfos, int32_t *len);
};
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif // OHOS_BMS_INTERFACE_H
