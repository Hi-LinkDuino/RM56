/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STATUS_CALLBACK_PROXY_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STATUS_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "bundle_status_callback_interface.h"

namespace OHOS {
namespace AppExecFwk {
class BundleStatusCallbackProxy : public IRemoteProxy<IBundleStatusCallback> {
public:
    explicit BundleStatusCallbackProxy(const sptr<IRemoteObject> &object);
    virtual ~BundleStatusCallbackProxy() override;
    /**
     * @brief Called when the installation, update, or uninstallation state of an application changes from proxy side.
     * @param installType Indicates the installation, update, or uninstallation state.
     *                    The value <b>0</b> indicates that the application is being installed or updated,
     *                    and <b>1</b> indicates that the application is being uninstalled.
     * @param resultCode Indicates the status code returned for the application installation, update, or uninstallation
     *                   result.
     * @param resultMessage Indicates the result message returned with the status code.
     * @param bundleName Indicates the name of the bundle whose state has changed.
     */
    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) override;
    /**
     * @brief Called when a new application package has been installed on the device.
     * @param bundleName Indicates the name of the bundle whose state has been installed.
     * @param userId Indicates the id of the bundle whose state has been installed.
     */
    virtual void OnBundleAdded(const std::string &bundleName, const int userId) override {};
    /**
     * @brief Called when a new application package has been Updated on the device.
     * @param bundleName Indicates the name of the bundle whose state has been Updated.
     * @param userId Indicates the id of the bundle whose state has been Updated.
     */
    virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override {};
    /**
     * @brief Called when a new application package has been Removed on the device.
     * @param bundleName Indicates the name of the bundle whose state has been Removed.
     * @param userId Indicates the id of the bundle whose state has been Removed.
     */
    virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override {};

private:
    static inline BrokerDelegator<BundleStatusCallbackProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLE_STATUS_CALLBACK_PROXY_H
