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

#ifndef OHOS_BUNDLEMANAGERSERVICE_SLITE_FEATURE_H
#define OHOS_BUNDLEMANAGERSERVICE_SLITE_FEATURE_H

#include "gt_bundle_manager_service.h"
#include "bundle_service_interface.h"
#include "feature.h"
#include "iproxy_client.h"
#include "iunknown.h"
#include "nocopyable.h"

namespace OHOS {
const unsigned int BMS_INSTALL_MSG = 100;
typedef int32 (*InvokeFunc)(const void *origin, IpcIo *req);

class BundleMgrSliteFeature : public Feature {
public:
    static bool Install(const char *hapPath, const InstallParam *installParam, InstallerCallback installerCallback);
    static bool Uninstall(const char *bundleName, const InstallParam *installParam,
        InstallerCallback installerCallback);
    static uint8_t QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo);
    static uint8_t GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo);
    static uint8_t GetBundleInfos(int32_t flags, BundleInfo **bundleInfos, int32_t *len);
    static bool GetInstallState(const char *bundleName, InstallState *installState, uint8_t *installProcess);
    static uint32_t GetBundleSize (const char *bundleName);
    static bool RegisterInstallerCallback(InstallerCallback installerCallback);
    static void UpdateBundleInfoList();
    static uint8_t GetBundleInfosNoReplication(const int flags, BundleInfo **bundleInfos, int32_t *len);

    static BundleMgrSliteFeature *GetInstance()
    {
        static BundleMgrSliteFeature instance;
        return &instance;
    }
    ~BundleMgrSliteFeature() = default;
    Identity *GetIdentity();

private:
    BundleMgrSliteFeature();
    static const char *GetFeatureName(Feature *feature);
    static void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity);
    static void OnFeatureStop(Feature *feature, Identity identity);
    static BOOL OnFeatureMessage(Feature *feature, Request *request);

private:
    Identity identity_;
    GtManagerService *gtManagerService_;
    DISALLOW_COPY_AND_MOVE(BundleMgrSliteFeature);
};

typedef struct {
    INHERIT_IUNKNOWNENTRY(BmsSliteInterface);
    BundleMgrSliteFeature *bms;
} BundleMgrSliteFeatureImpl;
} // namespace OHOS
#endif // OHOS_BUNDLEMANAGERSERVICE_SLITE_FEATURE_H
