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

#ifndef OHOS_BUNDLEMS_SLITE_CLIENT_H
#define OHOS_BUNDLEMS_SLITE_CLIENT_H

#include "iproxy_client.h"
#include "nocopyable.h"
#include "want.h"
#include "bundle_install_msg.h"
#include "bundle_service_interface.h"

namespace OHOS {
class BundleMsClient {
public:
    static BundleMsClient &GetInstance()
    {
        static BundleMsClient instance;
        return instance;
    }

    ~BundleMsClient() = default;

    bool Initialize() const;

    bool Install(const char *hapPath, const InstallParam *installParam, InstallerCallback installerCallback) const;

    bool Uninstall(const char *bundleName, const InstallParam *installParam, InstallerCallback installerCallback) const;

    uint8_t QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo) const;

    uint8_t GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo) const;

    uint8_t GetBundleInfos(int32_t flags, BundleInfo **bundleInfos, int32_t *len) const;

    bool GetInstallState(const char *bundleName, InstallState *installState, uint8_t *installProcess) const;

    uint32_t GetBundleSize(const char *bundleName) const;

    bool RegisterInstallerCallback(InstallerCallback installerCallback) const;

    void UpdateBundleInfoList() const;

    uint8_t GetBundleInfosNoReplication(const int flags, BundleInfo **bundleInfos, int32_t *len) const;

private:
    BundleMsClient() = default;

    BmsSliteInterface *bmsProxy_ { nullptr };

    DISALLOW_COPY_AND_MOVE(BundleMsClient);
};
} // namespace OHOS

#endif // OHOS_BUNDLEMS_SLITE_CLIENT_H