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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_STATUS_CALLBACK_PROXY_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_STATUS_CALLBACK_PROXY_H

#include <string>

#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "i_service_center_status_callback.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * interface for ServiceCenterStatusCallbackProxy.
 */
class ServiceCenterStatusCallbackProxy : public IRemoteProxy<IServiceCenterStatusCallback> {
public:
    explicit ServiceCenterStatusCallbackProxy(const sptr<IRemoteObject> &impl);
    /**
     * @brief OnRemoteInstallFinished, Download hap is complete.
     * @param installResult Result of install
     */
    void OnRemoteInstallFinished(std::string installResult) override;

private:
    void SendRequestCommon(int32_t errcode);

private:
    static inline BrokerDelegator<ServiceCenterStatusCallbackProxy> delegator_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_FREE_INSTALL_SERVICE_CENTER_STATUS_CALLBACK_PROXY_H
