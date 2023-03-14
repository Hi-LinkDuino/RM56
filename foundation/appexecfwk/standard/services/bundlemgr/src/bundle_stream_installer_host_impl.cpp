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

#include "bundle_stream_installer_host_impl.h"

#include "bundle_mgr_service.h"
#include "bundle_permission_mgr.h"
#include "bundle_util.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace AppExecFwk {
BundleStreamInstallerHostImpl::BundleStreamInstallerHostImpl(uint32_t installerId, int32_t installedUid)
{
    APP_LOGD("create bundle stream installer host impl instance");
    installerId_ = installerId;
    installedUid_ = installedUid;
}

BundleStreamInstallerHostImpl::~BundleStreamInstallerHostImpl()
{
    APP_LOGD("destory bundle stream installer host impl instance");
    UnInit();
}

bool BundleStreamInstallerHostImpl::Init(const InstallParam &installParam)
{
    installParam_ = installParam;
    installParam_.streamInstallMode = true;
    std::string tempDir = BundleUtil::CreateInstallTempDir(installerId_);
    if (tempDir.empty()) {
        return false;
    }
    tempDir_ = tempDir;
    return true;
}

void BundleStreamInstallerHostImpl::UnInit()
{
    APP_LOGD("destory stream installer with installerId %{public}d and temp dir %{public}s", installerId_,
        tempDir_.c_str());
    BundleUtil::CloseFileDescriptor(streamFdVec_);
    BundleUtil::DeleteDir(tempDir_);
}

int BundleStreamInstallerHostImpl::CreateStream(const std::string &hapName, long offset)
{
    if (!BundlePermissionMgr::VerifyCallingPermission(Constants::PERMISSION_INSTALL_BUNDLE)) {
        APP_LOGE("CreateStream permission denied");
        return -1;
    }

    int32_t callingUid = IPCSkeleton::GetCallingUid();
    if (callingUid != installedUid_ || isInstallStarted_) {
        APP_LOGE("calling uid is inconsistent");
        return -1;
    }

    if (!BundleUtil::CheckFileType(hapName, Constants::INSTALL_FILE_SUFFIX)) {
        APP_LOGE("file is not hap");
        return -1;
    }
    std::string bundlePath = tempDir_ + hapName;
    int32_t fd = -1;
    if ((fd = BundleUtil::CreateFileDescriptor(bundlePath, offset)) < 0) {
        APP_LOGE("stream installer create file descriptor failed");
    }
    if (fd > 0) {
        streamFdVec_.emplace_back(fd);
    }
    return fd;
}

bool BundleStreamInstallerHostImpl::Install(const sptr<IStatusReceiver>& receiver)
{
    if (receiver == nullptr) {
        APP_LOGE("receiver is nullptr");
        return false;
    }
    receiver->SetStreamInstallId(installerId_);
    auto installer = DelayedSingleton<BundleMgrService>::GetInstance()->GetBundleInstaller();
    if (installer == nullptr) {
        APP_LOGE("get bundle installer failed");
        return false;
    }
    std::vector<std::string> pathVec;
    pathVec.emplace_back(tempDir_);
    auto res = installer->Install(pathVec, installParam_, receiver);
    if (!res) {
        APP_LOGE("install bundle failed");
        return false;
    }
    return true;
}

uint32_t BundleStreamInstallerHostImpl::GetInstallerId() const
{
    return installerId_;
}

void BundleStreamInstallerHostImpl::SetInstallerId(uint32_t installerId)
{
    installerId_ = installerId;
}
} // AppExecFwk
} // OHOS