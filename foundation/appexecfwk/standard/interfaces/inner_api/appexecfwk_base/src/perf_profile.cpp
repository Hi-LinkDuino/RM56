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

#include "perf_profile.h"
#include <cinttypes>

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
PerfProfile::PerfProfile()
{
    APP_LOGD("create");
}

PerfProfile::~PerfProfile()
{
    APP_LOGD("destroy");
}

int64_t PerfProfile::GetBmsLoadStartTime() const
{
    return bmsLoadStart_;
}

void PerfProfile::SetBmsLoadStartTime(int64_t time)
{
    bmsLoadStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetBmsLoadEndTime() const
{
    return bmsLoadEnd_;
}

void PerfProfile::SetBmsLoadEndTime(int64_t time)
{
    bmsLoadEnd_ = (time > 0 && time > bmsLoadStart_) ? time : bmsLoadStart_;
}

int64_t PerfProfile::GetBundleScanStartTime() const
{
    return bundleScanStart_;
}

void PerfProfile::SetBundleScanStartTime(int64_t time)
{
    bundleScanStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetBundleScanEndTime() const
{
    return bundleScanEnd_;
}

void PerfProfile::SetBundleScanEndTime(int64_t time)
{
    bundleScanEnd_ = (time > 0 && time > bundleScanStart_) ? time : bundleScanStart_;
}

int64_t PerfProfile::GetBundleDownloadStartTime() const
{
    return bundleDownloadStart_;
}

void PerfProfile::SetBundleDownloadStartTime(int64_t time)
{
    bundleDownloadStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetBundleDownloadEndTime() const
{
    return bundleDownloadEnd_;
}

void PerfProfile::SetBundleDownloadEndTime(int64_t time)
{
    bundleDownloadEnd_ = (time > 0 && time > bundleDownloadStart_) ? time : bundleDownloadStart_;
}

int64_t PerfProfile::GetBundleInstallStartTime() const
{
    return bundleInstallStart_;
}

void PerfProfile::SetBundleInstallStartTime(int64_t time)
{
    bundleInstallStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetBundleInstallEndTime() const
{
    return bundleInstallEnd_;
}

void PerfProfile::SetBundleInstallEndTime(int64_t time)
{
    if (time >= 0 && time > bundleInstallStart_) {
        bundleInstallEnd_ = time;
        bundleInstallTime_ += (bundleInstallEnd_ - bundleInstallStart_);
    } else {
        bundleInstallEnd_ = bundleInstallStart_;
    }
}

int64_t PerfProfile::GetBundleTotalInstallTime() const
{
    return bundleInstallTime_;
}

int64_t PerfProfile::GetBundleUninstallStartTime() const
{
    return bundleUninstallStart_;
}

void PerfProfile::SetBundleUninstallStartTime(int64_t time)
{
    bundleUninstallStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetBundleUninstallEndTime() const
{
    return bundleUninstallEnd_;
}

void PerfProfile::SetBundleUninstallEndTime(int64_t time)
{
    bundleUninstallEnd_ = (time > 0 && time > bundleUninstallStart_) ? time : bundleUninstallStart_;
}

int64_t PerfProfile::GetBundleParseStartTime() const
{
    return bundleParseStart_;
}

void PerfProfile::SetBundleParseStartTime(int64_t time)
{
    bundleParseStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetBundleParseEndTime() const
{
    return bundleParseEnd_;
}

void PerfProfile::SetBundleParseEndTime(int64_t time)
{
    bundleParseEnd_ = (time > 0 && time > bundleParseStart_) ? time : bundleParseStart_;
}

int64_t PerfProfile::GetAmsLoadStartTime() const
{
    return amsLoadStart_;
}

void PerfProfile::SetAmsLoadStartTime(int64_t time)
{
    amsLoadStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetAmsLoadEndTime() const
{
    return amsLoadEnd_;
}

void PerfProfile::SetAmsLoadEndTime(int64_t time)
{
    amsLoadEnd_ = (time > 0 && time > amsLoadStart_) ? time : amsLoadStart_;
}

int64_t PerfProfile::GetAbilityLoadStartTime() const
{
    return abilityLoadStart_;
}

void PerfProfile::SetAbilityLoadStartTime(int64_t time)
{
    abilityLoadStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetAbilityLoadEndTime() const
{
    return abilityLoadEnd_;
}

void PerfProfile::SetAbilityLoadEndTime(int64_t time)
{
    abilityLoadEnd_ = (time > 0 && time > abilityLoadStart_) ? time : abilityLoadStart_;
}

int64_t PerfProfile::GetAppForkStartTime() const
{
    return appForkStart_;
}

void PerfProfile::SetAppForkStartTime(int64_t time)
{
    appForkStart_ = (time > 0) ? time : 0;
}

int64_t PerfProfile::GetAppForkEndTime() const
{
    return appForkEnd_;
}

void PerfProfile::SetAppForkEndTime(int64_t time)
{
    appForkEnd_ = (time > 0 && time > appForkStart_) ? time : appForkStart_;
}

bool PerfProfile::GetPerfProfileEnabled() const
{
    return enableProfile_;
}

void PerfProfile::SetPerfProfileEnabled(bool enable)
{
    enableProfile_ = enable;
}

void PerfProfile::Reset()
{
    bmsLoadStart_ = 0;
    bmsLoadEnd_ = 0;

    bundleScanStart_ = 0;
    bundleScanEnd_ = 0;

    bundleDownloadStart_ = 0;
    bundleDownloadEnd_ = 0;

    bundleInstallStart_ = 0;
    bundleInstallEnd_ = 0;
    bundleInstallTime_ = 0;

    bundleUninstallStart_ = 0;
    bundleUninstallEnd_ = 0;

    bundleParseStart_ = 0;
    bundleParseEnd_ = 0;

    amsLoadStart_ = 0;
    amsLoadEnd_ = 0;

    abilityLoadStart_ = 0;
    abilityLoadEnd_ = 0;
    appForkStart_ = 0;
    appForkEnd_ = 0;

    enableProfile_ = true;
}

void PerfProfile::Dump() const
{
    if (!enableProfile_) {
        APP_LOGI("profile disabled!\n");
        return;
    }

    // only dump the valid perf time
    if ((bundleScanEnd_ > bundleScanStart_) || (bundleInstallTime_ > 0) ||
        (bundleUninstallEnd_ > bundleUninstallStart_) || (bundleParseEnd_ > bundleParseStart_) ||
        (abilityLoadEnd_ > abilityLoadStart_) || (bmsLoadEnd_ > bmsLoadStart_) || (amsLoadEnd_ > amsLoadStart_)) {
        APP_LOGI("start\n");

        APP_LOGI("*************************\n");

        if (bmsLoadEnd_ > bmsLoadStart_) {
            APP_LOGI("BmsLoadTime: %{public}" PRId64 "(ms) \n", (bmsLoadEnd_ - bmsLoadStart_));
        }
        if (bundleScanEnd_ > bundleScanStart_) {
            APP_LOGI("BundleScanTime: %{public}" PRId64 "(ms) \n", (bundleScanEnd_ - bundleScanStart_));
        }
        if (bundleInstallTime_ > 0) {
            APP_LOGI("BundleInstallTime: %{public}" PRId64 "(ms) \n", bundleInstallTime_);
        }
        if (bundleUninstallEnd_ > bundleUninstallStart_) {
            APP_LOGI("BundleUninstallTime: %{public}" PRId64 "(ms) \n", (bundleUninstallEnd_ - bundleUninstallStart_));
        }
        if (bundleParseEnd_ > bundleParseStart_) {
            APP_LOGI("BundleParseTime: %{public}" PRId64 "(ms) \n", (bundleParseEnd_ - bundleParseStart_));
        }
        if (amsLoadEnd_ > amsLoadStart_) {
            APP_LOGI("AmsLoadTime: %{public}" PRId64 "(ms) \n", (amsLoadEnd_ - amsLoadStart_));
        }
        if (abilityLoadEnd_ > abilityLoadStart_) {
            APP_LOGI("AbilityLoadTime: %{public}" PRId64 "(ms) \n", (abilityLoadEnd_ - abilityLoadStart_));
            APP_LOGI("    AppForkTime: %{public}" PRId64 "(ms) \n", (appForkEnd_ - appForkStart_));
        }

        APP_LOGI("*************************\n");

        APP_LOGI("end\n");
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
