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

#include "bundle_exception_handler.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "bundle_constants.h"
#include "bundle_permission_mgr.h"
#include "installd_client.h"

namespace OHOS {
namespace AppExecFwk {
BundleExceptionHandler::BundleExceptionHandler(const std::shared_ptr<BundleDataStorageDatabase> &dataStorage)
    : dataStorage_(dataStorage)
{
    APP_LOGD("create bundle excepetion handler instance");
}

BundleExceptionHandler::~BundleExceptionHandler()
{
    APP_LOGD("destroy bundle excepetion handler instance");
}


void BundleExceptionHandler::HandleInvalidBundle(InnerBundleInfo &info, bool &isBundleValid)
{
    auto mark = info.GetInstallMark();
    if (mark.status == InstallExceptionStatus::INSTALL_FINISH) {
        APP_LOGD("bundle %{public}s is under correct installation status", info.GetBundleName().c_str());
        return;
    }

    std::string appCodePath = Constants::BUNDLE_CODE_DIR + info.GetBundleName();
    auto moduleDir = appCodePath + Constants::PATH_SEPARATOR + mark.packageName;
    auto moduleDataDir = info.GetBundleName() + Constants::HAPS + mark.packageName;

    // install and update failed before service restart
    if (mark.status == InstallExceptionStatus::INSTALL_START &&
        RemoveBundleAndDataDir(appCodePath, info.GetBundleName(), info.GetUserId())) {
        DeleteBundleInfoFromStorage(info);
        isBundleValid = false;
    } else if (mark.status == InstallExceptionStatus::UPDATING_EXISTED_START) {
        if (InstalldClient::GetInstance()->RemoveDir(moduleDir + Constants::TMP_SUFFIX) == ERR_OK) {
            info.SetInstallMark(mark.bundleName, mark.packageName, InstallExceptionStatus::INSTALL_FINISH);
            info.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        }
    } else if (mark.status == InstallExceptionStatus::UPDATING_NEW_START &&
        RemoveBundleAndDataDir(moduleDir, moduleDataDir, info.GetUserId())) {
        info.SetInstallMark(mark.bundleName, mark.packageName, InstallExceptionStatus::INSTALL_FINISH);
        info.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
    } else if (mark.status == InstallExceptionStatus::UNINSTALL_BUNDLE_START &&
        RemoveBundleAndDataDir(appCodePath, info.GetBundleName(), info.GetUserId())) {  // continue to uninstall
        DeleteBundleInfoFromStorage(info);
        isBundleValid = false;
    } else if (mark.status == InstallExceptionStatus::UNINSTALL_PACKAGE_START) {
        if (info.IsOnlyModule(mark.packageName) &&
            RemoveBundleAndDataDir(appCodePath, info.GetBundleName(), info.GetUserId())) {
            DeleteBundleInfoFromStorage(info);
            isBundleValid = false;
            return;
        }
        if (RemoveBundleAndDataDir(moduleDir, moduleDataDir, info.GetUserId())) {
            info.RemoveModuleInfo(mark.packageName);
            info.SetInstallMark(mark.bundleName, mark.packageName, InstallExceptionStatus::INSTALL_FINISH);
            info.SetBundleStatus(InnerBundleInfo::BundleStatus::ENABLED);
        }
    } else if (mark.status == InstallExceptionStatus::UPDATING_FINISH) {
        if (InstalldClient::GetInstance()->RenameModuleDir(moduleDir + Constants::TMP_SUFFIX, moduleDir) == ERR_OK) {
            info.SetInstallMark(mark.bundleName, mark.packageName, InstallExceptionStatus::INSTALL_FINISH);
        }
    } else {
        APP_LOGD("bundle %{public}s is under unknown installation status", info.GetBundleName().c_str());
    }
}

bool BundleExceptionHandler::RemoveBundleAndDataDir(const std::string &bundleDir,
    const std::string &bundleOrMoudleDir, int32_t userId) const
{
    ErrCode result = InstalldClient::GetInstance()->RemoveDir(bundleDir);
    if (result != ERR_OK) {
        APP_LOGE("fail to remove bundle dir %{public}s, error is %{public}d", bundleDir.c_str(), result);
        return false;
    }

    if (bundleOrMoudleDir.find(Constants::HAPS) != std::string::npos) {
        result = InstalldClient::GetInstance()->RemoveModuleDataDir(bundleOrMoudleDir, userId);
        if (result != ERR_OK) {
            APP_LOGE("fail to remove module data dir %{public}s, error is %{public}d", bundleOrMoudleDir.c_str(),
                result);
            return false;
        }
    } else {
        result = InstalldClient::GetInstance()->RemoveBundleDataDir(bundleOrMoudleDir, userId);
        if (result != ERR_OK) {
            APP_LOGE("fail to remove bundle data dir %{public}s, error is %{public}d", bundleOrMoudleDir.c_str(),
                result);
            return false;
        }
    }
    return true;
}

void BundleExceptionHandler::DeleteBundleInfoFromStorage(const InnerBundleInfo &info)
{
    auto storage = dataStorage_.lock();
    if (storage) {
        APP_LOGD("remove bundle info of %{public}s from the storage", info.GetBundleName().c_str());
        storage->DeleteStorageBundleInfo(info);
    } else {
        APP_LOGE(" fail to remove bundle info of %{public}s from the storage", info.GetBundleName().c_str());
    }
}
}  // namespace AppExecFwkConstants
}  // namespace OHOS