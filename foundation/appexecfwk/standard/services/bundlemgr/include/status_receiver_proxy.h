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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_STATUS_RECEIVER_PROXY_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_STATUS_RECEIVER_PROXY_H

#include "iremote_proxy.h"

#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {
class StatusReceiverProxy : public IRemoteProxy<IStatusReceiver> {
public:
    explicit StatusReceiverProxy(const sptr<IRemoteObject> &object);
    virtual ~StatusReceiverProxy() override;
    /**
     * @brief Called when install status changed, with the percentage of installation progress
     * @param progress Indicates the percentage of the installation progress.
     */
    virtual void OnStatusNotify(const int32_t progress) override;
    /**
     * @brief Called when an application is installed, updated, or uninstalled.
     * @param resultCode Indicates the status code returned for the application installation, update, or uninstallation
     *                   result.
     * @param resultMsg Indicates the result message returned with the status code.
     */
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;

    virtual void SetStreamInstallId(uint32_t installerId) override;
private:
    void TransformResult(const int32_t resultCode);
    void CloseStreamInstaller(uint32_t installerId);

private:
    int32_t resultCode_ = -1;
    std::string resultMsg_;
    uint32_t streamInstallerId_ = 0;
    static inline BrokerDelegator<StatusReceiverProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_STATUS_RECEIVER_PROXY_H