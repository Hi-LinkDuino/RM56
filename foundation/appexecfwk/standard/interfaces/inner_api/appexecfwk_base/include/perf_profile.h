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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_PERF_INCLUDE_PERF_PROFILE_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_PERF_INCLUDE_PERF_PROFILE_H

#include <cstdint>

#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
class PerfProfile : public Singleton<PerfProfile> {
    DECLARE_SINGLETON(PerfProfile);

public:
    int64_t GetBmsLoadStartTime() const;
    void SetBmsLoadStartTime(int64_t time);

    int64_t GetBmsLoadEndTime() const;
    void SetBmsLoadEndTime(int64_t time);

    int64_t GetBundleScanStartTime() const;
    void SetBundleScanStartTime(int64_t time);

    int64_t GetBundleScanEndTime() const;
    void SetBundleScanEndTime(int64_t time);

    int64_t GetBundleDownloadStartTime() const;
    void SetBundleDownloadStartTime(int64_t time);

    int64_t GetBundleDownloadEndTime() const;
    void SetBundleDownloadEndTime(int64_t time);

    int64_t GetBundleInstallStartTime() const;
    void SetBundleInstallStartTime(int64_t time);
    int64_t GetBundleTotalInstallTime() const;

    int64_t GetBundleInstallEndTime() const;
    void SetBundleInstallEndTime(int64_t time);

    int64_t GetBundleUninstallStartTime() const;
    void SetBundleUninstallStartTime(int64_t time);

    int64_t GetBundleUninstallEndTime() const;
    void SetBundleUninstallEndTime(int64_t time);

    int64_t GetBundleParseStartTime() const;
    void SetBundleParseStartTime(int64_t time);

    int64_t GetBundleParseEndTime() const;
    void SetBundleParseEndTime(int64_t time);

    int64_t GetAmsLoadStartTime() const;
    void SetAmsLoadStartTime(int64_t time);

    int64_t GetAmsLoadEndTime() const;
    void SetAmsLoadEndTime(int64_t time);

    int64_t GetAbilityLoadStartTime() const;
    void SetAbilityLoadStartTime(int64_t time);

    int64_t GetAbilityLoadEndTime() const;
    void SetAbilityLoadEndTime(int64_t time);

    int64_t GetAppForkStartTime() const;
    void SetAppForkStartTime(int64_t time);

    int64_t GetAppForkEndTime() const;
    void SetAppForkEndTime(int64_t time);

    bool GetPerfProfileEnabled() const;
    void SetPerfProfileEnabled(bool enable);

    void Reset();

    void Dump() const;

private:
    int64_t bmsLoadStart_ = 0;
    int64_t bmsLoadEnd_ = 0;

    int64_t bundleScanStart_ = 0;
    int64_t bundleScanEnd_ = 0;

    int64_t bundleDownloadStart_ = 0;
    int64_t bundleDownloadEnd_ = 0;

    int64_t bundleInstallStart_ = 0;
    int64_t bundleInstallEnd_ = 0;
    int64_t bundleInstallTime_ = 0;

    int64_t bundleUninstallStart_ = 0;
    int64_t bundleUninstallEnd_ = 0;

    int64_t bundleParseStart_ = 0;
    int64_t bundleParseEnd_ = 0;

    int64_t amsLoadStart_ = 0;
    int64_t amsLoadEnd_ = 0;

    int64_t abilityLoadStart_ = 0;
    int64_t abilityLoadEnd_ = 0;
    int64_t appForkStart_ = 0;
    int64_t appForkEnd_ = 0;

    bool enableProfile_ = true;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_BASE_PERF_INCLUDE_PERF_PROFILE_H
