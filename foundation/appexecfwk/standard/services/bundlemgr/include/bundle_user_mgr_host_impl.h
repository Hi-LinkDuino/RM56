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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_USER_MGR_HOST_IMPL_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_USER_MGR_HOST_IMPL_H

#include "bundle_data_mgr.h"
#include "bundle_installer_host.h"
#include "bundle_user_mgr_host.h"

namespace OHOS {
namespace AppExecFwk {
class BundleUserMgrHostImpl : public BundleUserMgrHost {
public:
    /**
     * @brief Create new user.
     * @param userId Indicates the userId.
     * @param bundleUserStatus Indicates the bundleUserStatus.
     */
    virtual void CreateNewUser(int32_t userId) override;
    /**
     * @brief Remove user.
     * @param userId Indicates the userId.
     * @param bundleUserStatus Indicates the bundleUserStatus.
     */
    virtual void RemoveUser(int32_t userId) override;

private:
    const std::shared_ptr<BundleDataMgr> GetDataMgrFromService();
    const sptr<IBundleInstaller> GetBundleInstaller();
    void CheckInitialUser();

    std::mutex bundleUserMgrMutex_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_USER_MGR_HOST_IMPL_H
