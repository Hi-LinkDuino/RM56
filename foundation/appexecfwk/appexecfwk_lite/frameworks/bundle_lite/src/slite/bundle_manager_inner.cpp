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

#include "bundle_manager_inner.h"

#include "bundle_install_msg.h"
#include "bundlems_slite_client.h"

extern "C" {
bool GetInstallState(const char *bundleName, InstallState *installState, uint8_t *installProcess)
{
    return OHOS::BundleMsClient::GetInstance().GetInstallState(bundleName, installState, installProcess);
}

uint32_t GetBundleSize(const char *bundleName)
{
    return OHOS::BundleMsClient::GetInstance().GetBundleSize(bundleName);
}

bool RegisterInstallerCallback(InstallerCallback installerCallback)
{
    return OHOS::BundleMsClient::GetInstance().RegisterInstallerCallback(installerCallback);
}

void UpdateBundleInfoList(void)
{
    OHOS::BundleMsClient::GetInstance().UpdateBundleInfoList();
}

uint8_t GetBundleInfosNoReplication(const int flags, BundleInfo **bundleInfos, int32_t *len)
{
    return OHOS::BundleMsClient::GetInstance().GetBundleInfosNoReplication(flags, bundleInfos, len);
}
}
