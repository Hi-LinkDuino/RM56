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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_BUNDLE_INFO_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_BUNDLE_INFO_H

#include <iostream>
#include <string>

namespace OHOS {
namespace AppExecFwk {
class AgingBundleInfo {
public:
    AgingBundleInfo() = default;
    AgingBundleInfo(const std::string &name, int64_t time, int64_t bundleDataBytes, int uid)
        : bundleName(name), recentlyUsedTime(time), dataBytes(bundleDataBytes), bundleUid(uid)
    {
    };
    virtual ~AgingBundleInfo() = default;

    const std::string &GetBundleName() const
    {
        return bundleName;
    };

    int64_t GetRecentlyUsedTime() const
    {
        return recentlyUsedTime;
    };

    int64_t GetDataBytes() const
    {
        return dataBytes;
    };

    int GetBundleUid() const
    {
        return bundleUid;
    };

private:
    std::string bundleName;
    int64_t recentlyUsedTime = 0;
    int64_t dataBytes = 0;
    int bundleUid = -1;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_BUNDLE_INFO_H