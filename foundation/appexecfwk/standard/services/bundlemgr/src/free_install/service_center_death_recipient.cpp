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

#include "service_center_death_recipient.h"

#include "app_log_wrapper.h"
#include "free_install_params.h"
#include "ohos/aafwk/content/want.h"
#include "parcel.h"

namespace OHOS {
namespace AppExecFwk {
const std::u16string ATOMIC_SERVICE_STATUS_CALLBACK_TOKEN = u"ohos.aafwk.IAtomicServiceStatusCallback";
constexpr uint32_t FREE_INSTALL_DONE = 0;

void ServiceCenterDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &wptrDeath)
{
    APP_LOGI("service center died");

    if (wptrDeath == nullptr) {
        APP_LOGE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (object == nullptr) {
        APP_LOGE("object is null");
        return;
    }

    APP_LOGI("Send request to ability manager service");
    std::map<std::string, FreeInstallParams>::iterator itr;
    for (itr = freeInstallParamsMap_.begin(); itr != freeInstallParamsMap_.end();) {
        std::string transactId = itr->first;
        FreeInstallParams freeInstallParams = itr->second;
        SendCallBack(freeInstallParams, transactId);
    }
}

void ServiceCenterDeathRecipient::SendCallBack(FreeInstallParams &freeInstallParams, std::string transactId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ATOMIC_SERVICE_STATUS_CALLBACK_TOKEN)) {
        APP_LOGE("Write interface token failed");
        return;
    }
    int32_t resultCode = FreeInstallErrorCode::SERVICE_CENTER_CRASH;
    if (!data.WriteInt32(resultCode)) {
        APP_LOGE("Write result code error");
        return;
    }
    OHOS::AAFwk::Want want = freeInstallParams.want;
    if (!data.WriteParcelable(&want)) {
        APP_LOGE("Write want failed");
        return;
    }
    int32_t userId = freeInstallParams.userId;
    if (!data.WriteInt32(userId)) {
        APP_LOGE("Write userId error");
        return;
    }
    MessageParcel reply;
    MessageOption option;
    if (freeInstallParams.callback->SendRequest(FREE_INSTALL_DONE, data, reply, option) != ERR_OK) {
        APP_LOGE("BundleConnectAbilityMgr::SendCallBack SendRequest failed");
    }
    freeInstallParamsMap_.erase(transactId);
}
}  // namespace AppExecFwk
}  // namespace OHOS