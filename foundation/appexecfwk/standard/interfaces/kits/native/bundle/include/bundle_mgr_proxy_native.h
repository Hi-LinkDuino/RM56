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

#ifndef FOUNDATION_APPEXECFWK_KITS_APPKIT_NATIVE_BUNDLE_INCLUDE_BUNDLE_MGR_PROXY_NATIVE_H
#define FOUNDATION_APPEXECFWK_KITS_APPKIT_NATIVE_BUNDLE_INCLUDE_BUNDLE_MGR_PROXY_NATIVE_H

#include <string>

#include "iremote_proxy.h"

#include "bundle_mgr_interface.h"
#include "element_name.h"
#include "bundle_status_callback_interface.h"
#include "clean_cache_callback_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrProxyNative : public IRemoteProxy<IBundleMgr> {
public:
    explicit BundleMgrProxyNative(const sptr<IRemoteObject> &impl);
    virtual ~BundleMgrProxyNative() override;
    /**
     * @brief Obtains the application ID based on the given bundle name and user ID.
     * @param bundleName Indicates the bundle name of the application.
     * @param userId Indicates the user ID.
     * @return Returns the application ID if successfully obtained; returns empty string otherwise.
     */
    virtual std::string GetAppIdByBundleName(const std::string &bundleName, const int userId) override;
private:
    /**
     * @brief Send a command message from the proxy object.
     * @param code Indicates the message code to be sent.
     * @param data Indicates the objects to be sent.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if message send successfully; returns false otherwise.
     */
    bool SendTransactCmd(IBundleMgr::Message code, MessageParcel &data, MessageParcel &reply);
    static inline BrokerDelegator<BundleMgrProxyNative> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_KITS_APPKIT_NATIVE_BUNDLE_INCLUDE_BUNDLE_MGR_PROXY_NATIVE_H