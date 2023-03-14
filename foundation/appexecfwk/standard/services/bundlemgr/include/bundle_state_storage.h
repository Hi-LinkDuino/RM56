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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_STATE_STORAGE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_STATE_STORAGE_H

#include <map>
#include <mutex>

#include "bundle_user_info.h"
#include "json_serializer.h"

namespace OHOS {
namespace AppExecFwk {
class BundleStateStorage : public std::enable_shared_from_this<BundleStateStorage> {
public:
    BundleStateStorage() = default;
    ~BundleStateStorage() = default;
    /**
     * @brief Judge whether json db exists. If it does not exist, create it and return the judgment result.
     * @return Returns true if the json db exists; returns false otherwise.
     */
    bool HasBundleUserInfoJsonDb();
    /**
     * @brief Load all bundles state from json db to BundleUserInfo.
     * @param infos Indicates the map to save all installed bundles.
     * @return Returns true if load the data successfully; returns false otherwise.
     */
    bool LoadAllBundleStateData(std::map<std::string, std::map<int32_t, BundleUserInfo>> &infos);
    /**
     * @brief Save the bundles state to json db.
     * @param bundleName Indicates the bundleName object to be save.
     * @param userId Indicates the userId object to be save.
     * @param bundleUserInfo Indicates the bundleUserInfo object to be save.
     * @return Returns true if save the data successfully saved; returns false otherwise.
     */
    bool SaveBundleStateStorage(
        const std::string bundleName, int32_t userId, const BundleUserInfo &bundleUserInfo);
    /**
     * @brief Save the bundles state from json db.
     * @param bundleName Indicates the bundleName object to be save.
     * @param userId Indicates the userId object to be save.
     * @param bundleUserInfo Indicates the bundleUserInfo object to be save.
     * @return Returns true if get the data successfully; returns false otherwise.
     */
    bool GetBundleStateStorage(
        const std::string bundleName, int32_t userId, BundleUserInfo &bundleUserInfo);
    /**
     * @brief Delete the bundles state from json db.
     * @param bundleName Indicates the bundleName object to be save.
     * @param userId Indicates the userId object to be save.
     * @return Returns true if delete the data successfully; returns false otherwise.
     */
    bool DeleteBundleState(const std::string bundleName, int32_t userId);

private:
    bool LoadAllBundleStateDataFromJson(
        nlohmann::json &jParse, std::map<std::string, std::map<int32_t, BundleUserInfo>> &infos);

    mutable std::mutex bundleStateMutex_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_STATE_STORAGE_H
