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

#include "bundle_mgr_slite_feature.h"

#include "bundle_service_interface.h"
#include "bundlems_log.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "securec.h"
#include "utils.h"
#include "want_utils.h"


namespace OHOS {
BundleMgrSliteFeatureImpl g_bmsSliteImpl = {
    DEFAULT_IUNKNOWN_ENTRY_BEGIN,
    .Install = BundleMgrSliteFeature::Install,
    .Uninstall = BundleMgrSliteFeature::Uninstall,
    .QueryAbilityInfo = BundleMgrSliteFeature::QueryAbilityInfo,
    .GetBundleInfo = BundleMgrSliteFeature::GetBundleInfo,
    .GetBundleInfos = BundleMgrSliteFeature::GetBundleInfos,
    .GetInstallState = BundleMgrSliteFeature::GetInstallState,
    .GetBundleSize = BundleMgrSliteFeature::GetBundleSize,
    .RegisterInstallerCallback = BundleMgrSliteFeature::RegisterInstallerCallback,
    .UpdateBundleInfoList = BundleMgrSliteFeature::UpdateBundleInfoList,
    .GetBundleInfosNoReplication = BundleMgrSliteFeature::GetBundleInfosNoReplication,
    DEFAULT_IUNKNOWN_ENTRY_END
};

static void Init()
{
    SamgrLite *samgrLite = SAMGR_GetInstance();
    CHECK_NULLPTR_RETURN(samgrLite, "BundleMgrSliteFeature", "get samgr error");
    BOOL result = samgrLite->RegisterFeature(BMS_SERVICE, BundleMgrSliteFeature::GetInstance());
    if (result == FALSE) {
        PRINTE("BundleMgrSliteFeature", "bms register feature failure");
        return;
    }
    g_bmsSliteImpl.bms = BundleMgrSliteFeature::GetInstance();
    auto publicApi = GET_IUNKNOWN(g_bmsSliteImpl);
    CHECK_NULLPTR_RETURN(publicApi, "BundleMgrSliteFeatureLite", "publicApi is nullptr");
    BOOL apiResult = samgrLite->RegisterFeatureApi(BMS_SERVICE, BMS_SLITE_FEATURE, publicApi);
    PRINTI("BundleMgrSliteFeature", "bms feature init %{public}s", apiResult ? "success" : "failure");
}
SYSEX_FEATURE_INIT(Init);

BundleMgrSliteFeature::BundleMgrSliteFeature() : Feature(), identity_()
{
    this->Feature::GetName = BundleMgrSliteFeature::GetFeatureName;
    this->Feature::OnInitialize = BundleMgrSliteFeature::OnFeatureInitialize;
    this->Feature::OnStop = BundleMgrSliteFeature::OnFeatureStop;
    this->Feature::OnMessage = BundleMgrSliteFeature::OnFeatureMessage;
}

const char *BundleMgrSliteFeature::GetFeatureName(Feature *feature)
{
    (void) feature;
    return BMS_SLITE_FEATURE;
}

Identity *BundleMgrSliteFeature::GetIdentity()
{
    return &identity_;
}

void BundleMgrSliteFeature::OnFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    CHECK_NULLPTR_RETURN(feature, "BundleMgrSliteFeature", "initialize fail");
    (static_cast<BundleMgrSliteFeature *>(feature))->identity_ = identity;
}

void BundleMgrSliteFeature::OnFeatureStop(Feature *feature, Identity identity)
{
    (void) feature;
    (void) identity;
}

BOOL BundleMgrSliteFeature::OnFeatureMessage(Feature *feature, Request *request)
{
    if (feature == nullptr || request == nullptr) {
        return FALSE;
    }
    if (request->msgId == BMS_INSTALL_MSG) {
        OHOS::GtManagerService::GetInstance().Install(nullptr, nullptr, nullptr);
    }
    return TRUE;
}

bool BundleMgrSliteFeature::Install(const char *hapPath, const InstallParam *installParam,
    InstallerCallback installerCallback)
{
    return OHOS::GtManagerService::GetInstance().Install(hapPath, installParam, installerCallback);
}

bool BundleMgrSliteFeature::Uninstall(const char *bundleName, const InstallParam *installParam,
    InstallerCallback installerCallback)
{
    return OHOS::GtManagerService::GetInstance().Uninstall(bundleName, installParam, installerCallback);
}

uint8_t BundleMgrSliteFeature::QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo)
{
    return OHOS::GtManagerService::GetInstance().QueryAbilityInfo(want, abilityInfo);
}

uint8_t BundleMgrSliteFeature::GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo)
{
    return OHOS::GtManagerService::GetInstance().GetBundleInfo(bundleName, flags, *bundleInfo);
}

uint8_t BundleMgrSliteFeature::GetBundleInfos(const int32_t flags, BundleInfo **bundleInfos, int32_t *len)
{
    return OHOS::GtManagerService::GetInstance().GetBundleInfos(flags, bundleInfos, len);
}

bool BundleMgrSliteFeature::GetInstallState(const char *bundleName, InstallState *installState, uint8_t *installProcess)
{
    return OHOS::GtManagerService::GetInstance().GetInstallState(bundleName, installState, installProcess);
}

uint32_t BundleMgrSliteFeature::GetBundleSize(const char *bundleName)
{
    return OHOS::GtManagerService::GetInstance().GetBundleSize(bundleName);
}

bool BundleMgrSliteFeature::RegisterInstallerCallback(InstallerCallback installerCallback)
{
    return OHOS::GtManagerService::GetInstance().RegisterInstallerCallback(installerCallback);
}

void BundleMgrSliteFeature::UpdateBundleInfoList()
{
    OHOS::GtManagerService::GetInstance().UpdateBundleInfoList();
}

uint8_t BundleMgrSliteFeature::GetBundleInfosNoReplication(const int flags, BundleInfo **bundleInfos, int32_t *len)
{
    return OHOS::GtManagerService::GetInstance().GetBundleInfosNoReplication(flags, bundleInfos, len);
}
} // namespace OHOS
