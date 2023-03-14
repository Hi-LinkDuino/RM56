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

#include "service_center_connection.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "parcel.h"
#include "free_install_params.h"

namespace OHOS {
namespace AppExecFwk {
const std::u16string ATOMIC_SERVICE_STATUS_CALLBACK_TOKEN = u"ohos.aafwk.IAtomicServiceStatusCallback";

ServiceCenterConnection::~ServiceCenterConnection()
{
}

void ServiceCenterConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    APP_LOGI("OnAbilityConnectDone start");
    if (resultCode != ERR_OK) {
        APP_LOGE("OnAbilityConnectDone resultCode = %{public}d", resultCode);
        cv_.notify_all();
        return;
    }

    if (remoteObject == nullptr) {
        APP_LOGE("OnAbilityConnectDone resultCode is nullptr");
        cv_.notify_all();
        return;
    }
    serviceCenterRemoteObject_ = remoteObject;
    connectState_ = ServiceCenterConnectState::CONNECTED;

    deathRecipient_ = (new (std::nothrow) ServiceCenterDeathRecipient(freeInstallParamsMap_));
    if (deathRecipient_ == nullptr) {
        APP_LOGE("Failed to create ServiceCenterDeathRecipient");
        cv_.notify_all();
        return;
    }
    APP_LOGI("Add death recipient for service center");
    if (!serviceCenterRemoteObject_->AddDeathRecipient(deathRecipient_)) {
        APP_LOGE("Failed to add AbilityManagerDeathRecipient");
    }
    cv_.notify_all();
    APP_LOGI("OnAbilityConnectDone end");
}

void ServiceCenterConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode)
{
    APP_LOGI("OnAbilityDisconnectDone start");
    if (serviceCenterRemoteObject_ != nullptr && deathRecipient_ != nullptr) {
        serviceCenterRemoteObject_->RemoveDeathRecipient(deathRecipient_);
    }

    connectState_ = ServiceCenterConnectState::DISCONNECTED;
    serviceCenterRemoteObject_ = nullptr;

    cv_.notify_all();
    APP_LOGI("OnAbilityDisconnectDone end");
}

sptr<IRemoteObject> ServiceCenterConnection::GetRemoteObject()
{
    return serviceCenterRemoteObject_;
}
}  // namespace AppExecFwk
}  // namespace OHOS