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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLE_EXCEPTION_HANDLE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLE_EXCEPTION_HANDLE_H

#include "bundle_data_storage_database.h"
#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
class BundleExceptionHandler {
public:
    explicit BundleExceptionHandler(const std::shared_ptr<BundleDataStorageDatabase> &dataStorage);
    virtual ~BundleExceptionHandler();

    /**
     * @brief to handle the invalid bundle in the storage.
     * @param info Indicates the bundle info of the application.
     * @param path Indicates the base bundle path or data path.
     */
    void HandleInvalidBundle(InnerBundleInfo &info, bool &isBundleValid);
    /**
     * @brief to remove the bundle path and data path according to the input bundle or data path.
     * @param info Indicates the bundle info of the application.
     * @param dataDir Indicates the dir needs to be removed.
     * @return Returns true for remove successfully, returns false otherwise.
     */
    bool RemoveBundleAndDataDir(const std::string &bundleDir, const std::string &bundleOrMoudleDir,
        int32_t userId) const;
    /**
     * @brief to obtain the bundle path and data path according to the input base bundle or data path.
     * @param info Indicates the bundle info of the application.
     * @param basePath Indicates the base bundle path or data path.
     * @return Returns string of the bundle or data path.
     */
    std::string GetBundleAndDataDir(const InnerBundleInfo &info, const std::string &basePath);
    /**
     * @brief to delete the bundle info from the storage.
     * @param info Indicates the bundle info of the application.
     */
    void DeleteBundleInfoFromStorage(const InnerBundleInfo &info);

private:
    std::weak_ptr<BundleDataStorageDatabase> dataStorage_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_SERVICES_BUNDLE_EXCEPTION_HANDLE_H