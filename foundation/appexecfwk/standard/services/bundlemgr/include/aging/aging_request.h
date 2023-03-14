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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_REQUEST_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_REQUEST_H

#include <vector>

#include "aging_bundle_info.h"
#include "aging_util.h"

namespace OHOS {
namespace AppExecFwk {
class AgingRequest {
public:
    AgingRequest();
    bool IsReachStartAgingThreshold() const;
    bool IsReachEndAgingThreshold() const;
    const std::vector<AgingBundleInfo> &GetAgingBundles() const
    {
        return agingBundles;
    };
    void AddAgingBundle(AgingBundleInfo &bundleInfo);
    size_t SortAgingBundles()
    {
        AgingUtil::SortAgingBundles(agingBundles);
        return agingBundles.size();
    };
    void UpdateTotalDataBytesAfterUninstalled(const int64_t dataBytes)
    {
        tatalDataBytes -= dataBytes;
    };
    int64_t GetTotalDataBytes() const
    {
        return tatalDataBytes;
    };
    void SetTotalDataBytes(const int64_t allBundleDataBytes)
    {
        tatalDataBytes = allBundleDataBytes;
    };
    void RequestReset();

public:
    static int64_t GetTotalDataBytesThreshold()
    {
        return totalDataBytesThreshold;
    };
    static int64_t GetOneDayTimeMs()
    {
        return oneDayTimeMs;
    };

private:
    void InitAgingPolicySystemParameters();
    std::vector<AgingBundleInfo> agingBundles;
    int64_t tatalDataBytes = 0;

private:
    static int64_t totalDataBytesThreshold;
    static int64_t oneDayTimeMs;
};
}  //  namespace AppExecFwk
}  //  namespace OHOS

#endif  //  FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_REQUEST_H
