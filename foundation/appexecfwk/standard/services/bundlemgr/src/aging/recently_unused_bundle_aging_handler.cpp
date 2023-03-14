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

#include <cinttypes>

#include "ability_manager.h"
#include "account_helper.h"
#include "aging/aging_handler.h"
#include "app_log_wrapper.h"
#include "bundle_data_mgr.h"
#include "bundle_mgr_service.h"
#include "install_param.h"
#include "running_process_info.h"

namespace OHOS {
namespace AppExecFwk {
bool RecentlyUnuseBundleAgingHandler::Process(AgingRequest &request) const
{
    bool needContinue = true;
    APP_LOGD("aging handler start: %{public}s, currentTotalDataBytes: %{pubic}" PRId64, GetName().c_str(),
        request.GetTotalDataBytes());
    std::vector<AgingBundleInfo> &agingBundles = const_cast<std::vector<AgingBundleInfo> &>(request.GetAgingBundles());
    APP_LOGD("aging handler start: agingBundles size :%{public}zu / %{public}" PRId64, agingBundles.size(),
        request.GetTotalDataBytes());
    auto iter = agingBundles.begin();
    while (iter != agingBundles.end()) {
        if (!CheckBundle(*iter)) {
            break;
        }
        APP_LOGD("found matching bundle: %{public}s.", iter->GetBundleName().c_str());
        bool isBundlerunning = IsRunning(iter->GetBundleName(), iter->GetBundleUid());
        if (!isBundlerunning) {
            bool isBundleUnistalled = UnInstallBundle(iter->GetBundleName());
            if (isBundleUnistalled) {
                request.UpdateTotalDataBytesAfterUninstalled(iter->GetDataBytes());
            }
        }
        iter = agingBundles.erase(iter);
        if (GetName() == AgingConstants::UNUSED_FOR_10_DAYS_BUNDLE_AGING_HANDLER
            || GetName() == AgingConstants::BUNDLE_DATA_SIZE_AGING_HANDLER) {
            if (!NeedContinue(request)) {
                APP_LOGD("there is no need to continue now.");
                needContinue = false;
                return needContinue;
            }
        }
    }
    if (!NeedContinue(request)) {
        APP_LOGD("there is no need to continue now.");
        needContinue = false;
    }
    APP_LOGD("aging handle done: %{public}s, currentTotalDataBytes: %{public}" PRId64, GetName().c_str(),
        request.GetTotalDataBytes());
    return needContinue;
}

bool RecentlyUnuseBundleAgingHandler::NeedContinue(const AgingRequest &request) const
{
    return !request.IsReachEndAgingThreshold();
}

bool RecentlyUnuseBundleAgingHandler::UnInstallBundle(const std::string &bundleName) const
{
    auto bms = DelayedSingleton<BundleMgrService>::GetInstance();
    auto bundleInstaller = bms->GetBundleInstaller();
    auto bundleDataMgr = bms->GetDataMgr();
    if (!bundleInstaller) {
        APP_LOGE("bundleInstaller is null.");
        return false;
    }

    sptr<AgingUninstallReceiveImpl> userReceiverImpl(new (std::nothrow) AgingUninstallReceiveImpl());
    InstallParam installParam;
    installParam.userId = AccountHelper::GetCurrentActiveUserId();
    installParam.installFlag = InstallFlag::FREE_INSTALL;
    bundleInstaller->Uninstall(bundleName, installParam, userReceiverImpl);
    return true;
}

bool RecentlyUnuseBundleAgingHandler::IsRunning(const std::string bundleName, const int bundleUid) const
{
    if (bundleUid < 0) {
        APP_LOGE("bundleUid is error.");
        return false;
    }
    std::vector<RunningProcessInfo> runningList = AbilityManager::GetInstance().GetAllRunningProcesses();
    if (runningList.size() == 0) {
        APP_LOGD("app runningList size = 0.");
        return false;
    }
    for (RunningProcessInfo info : runningList) {
        if (info.uid_ == bundleUid) {
            APP_LOGD("bundleName: %{public}s is running.", bundleName.c_str());
            return true;
        }
    }
    APP_LOGD("nothing app running.");
    return false;
}
}  //  namespace AppExecFwk
}  //  namespace OHOS
