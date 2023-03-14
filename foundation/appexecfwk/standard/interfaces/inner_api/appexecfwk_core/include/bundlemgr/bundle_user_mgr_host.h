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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_USER_MGR_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_USER_MGR_HOST_H

#include "iremote_stub.h"
#include "nocopyable.h"

#include "bundle_user_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleUserMgrHost : public IRemoteStub<IBundleUserMgr> {
public:
    BundleUserMgrHost();
    virtual ~BundleUserMgrHost() override;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    /**
     * @brief Handles the CreateNewUser function called from a IBundleUserMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    void HandleCreateNewUser(Parcel &data, Parcel &reply);
    /**
     * @brief Handles the RemoveUser function called from a IBundleUserMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    void HandleRemoveUser(Parcel &data, Parcel &reply);

    DISALLOW_COPY_AND_MOVE(BundleUserMgrHost);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_USER_MGR_HOST_H
