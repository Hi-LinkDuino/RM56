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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLE_SANDBOX_EXCEPTION_HANDLE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLE_SANDBOX_EXCEPTION_HANDLE_H

#include "bundle_data_storage_database.h"

namespace OHOS {
namespace AppExecFwk {
class BundleSandboxExceptionHandler final {
public:
    explicit BundleSandboxExceptionHandler(const std::shared_ptr<BundleDataStorageDatabase> &dataStorage);
    ~BundleSandboxExceptionHandler();

    /**
     * @brief to remove the sandbox data dir when the bms service inits
     * @param info indicates the inner bundleInfo of the application.
     */
    void RemoveSandboxApp(InnerBundleInfo &info);

private:
    static void RemoveSandboxDataDirAndTokenId(const std::string &bundleName,
        const std::vector<SandboxAppPersistentInfo> &sandboxPersistentInfo);
    void UpdateBundleInfoToStorage(const InnerBundleInfo &info);

    std::weak_ptr<BundleDataStorageDatabase> dataStorage_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLE_SANDBOX_EXCEPTION_HANDLE_H
