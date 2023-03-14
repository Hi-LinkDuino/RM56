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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_USER_MGR_PROXY_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_USER_MGR_PROXY_H

#include "iremote_proxy.h"

#include "bundle_user_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleUserMgrProxy : public IRemoteProxy<IBundleUserMgr> {
public:
    explicit BundleUserMgrProxy(const sptr<IRemoteObject> &object);
    virtual ~BundleUserMgrProxy() override;

    /**
     * @brief Create new user.
     * @param userId Indicates the userId.
     */
    void CreateNewUser(int32_t userId) override;
    /**
     * @brief Remove user.
     * @param userId Indicates the userId.
     */
    void RemoveUser(int32_t userId) override;

private:
    bool SendRequest(const int32_t& code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option);
    static inline BrokerDelegator<BundleUserMgrProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_USER_MGR_PROXY_H
