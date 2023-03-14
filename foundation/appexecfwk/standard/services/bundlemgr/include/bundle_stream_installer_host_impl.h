/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_STREAM_INSTALLER_HOST_IMPL_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_STREAM_INSTALLER_HOST_IMPL_H

#include<vector>

#include "bundle_stream_installer_host.h"
#include "install_param.h"

namespace OHOS {
namespace AppExecFwk {
class BundleStreamInstallerHostImpl : public BundleStreamInstallerHost {
public:
    BundleStreamInstallerHostImpl(uint32_t installerId, int32_t installedUid);
    virtual ~BundleStreamInstallerHostImpl() override;

    bool Init(const InstallParam &installParam);
    virtual void UnInit() override;

    virtual int CreateStream(const std::string &bundleName, long offset) override;
    virtual bool Install(const sptr<IStatusReceiver>& receiver) override;

    virtual uint32_t GetInstallerId() const override;
    virtual void SetInstallerId(uint32_t installerId) override;

private:
    std::string tempDir_;
    uint32_t installerId_ = -1;
    std::vector<int32_t> streamFdVec_;
    InstallParam installParam_;
    int32_t installedUid_;
    bool isInstallStarted_ = false;
};
} // AppExecFwk
} // OHOS


#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_STREAM_INSTALLER_HOST_IMPL_H