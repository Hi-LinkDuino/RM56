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
#include "bundle_manager.h"
#include "bundlems_slite_client.h"
#include "want.h"

extern "C" {
bool Install(const char *hapPath, const InstallParam *installParam, InstallerCallback installerCallback)
{
    return OHOS::BundleMsClient::GetInstance().Install(hapPath, installParam, installerCallback);
}

bool Uninstall(const char *bundleName, const InstallParam *installParam, InstallerCallback installerCallback)
{
    return OHOS::BundleMsClient::GetInstance().Uninstall(bundleName, installParam, installerCallback);
}

uint8_t QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo)
{
    return OHOS::BundleMsClient::GetInstance().QueryAbilityInfo(want, abilityInfo);
}

uint8_t GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo)
{
    return OHOS::BundleMsClient::GetInstance().GetBundleInfo(bundleName, flags, bundleInfo);
}

uint8_t GetBundleInfos(const int flags, BundleInfo **bundleInfos, int32_t *len)
{
    return OHOS::BundleMsClient::GetInstance().GetBundleInfos(flags, bundleInfos, len);
}
}
