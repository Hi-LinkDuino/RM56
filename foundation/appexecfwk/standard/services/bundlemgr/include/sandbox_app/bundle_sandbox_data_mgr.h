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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_SANDBOX_DATA_MGR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_SANDBOX_DATA_MGR_H

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>

#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
class BundleSandboxDataMgr final {
public:
    BundleSandboxDataMgr();
    ~BundleSandboxDataMgr();

    void SaveSandboxAppInfo(const InnerBundleInfo &info, const int32_t &appIndex);
    void DeleteSandboxAppInfo(const std::string &bundleName, const int32_t &appIndex);
    ErrCode GetSandboxAppBundleInfo(
        const std::string &bundleName, const int32_t &appIndex, const int32_t &userId, BundleInfo &info) const;
    int32_t GenerateSandboxAppIndex(const std::string &bundleName);
    bool DeleteSandboxAppIndex(const std::string &bundleName, int32_t appIndex);
    std::unordered_map<std::string, InnerBundleInfo> GetSandboxAppInfoMap() const;
    ErrCode GetSandboxAppInfo(
        const std::string &bundleName, const int32_t &appIndex, int32_t &userId, InnerBundleInfo &info) const;

private:
    mutable std::shared_mutex sandboxAppMutex_;
    mutable std::mutex sandboxAppIndexMapMutex_;
    // key: bundleName_appindex
    std::unordered_map<std::string, InnerBundleInfo> sandboxAppInfos_;
    std::unordered_map<std::string, std::set<int32_t>> sandboxAppIndexMap_;
};
} // AppExecFwk
} // OHOS


#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_SANDBOX_DATA_MGR_H