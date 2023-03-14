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

#include "bundle_status_callback_death_recipient.h"

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"
#include "bundle_status_callback_interface.h"
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
BundleStatusCallbackDeathRecipient::BundleStatusCallbackDeathRecipient()
{
    APP_LOGI("create BundleStatusCallbackDeathRecipient instance");
}

BundleStatusCallbackDeathRecipient::~BundleStatusCallbackDeathRecipient()
{
    APP_LOGI("destroy BundleStatusCallbackDeathRecipient instance");
}

void BundleStatusCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    APP_LOGI("bundle status service died, remove the proxy object");
    sptr<IBundleStatusCallback> callback = iface_cast<IBundleStatusCallback>(remote.promote());
    APP_LOGI("bundle status service died");
    if (!callback) {
        APP_LOGE("callback is nullptr");
        return;
    }
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (dataMgr == nullptr) {
        APP_LOGE("DataMgr is nullptr");
        return;
    }
    dataMgr->ClearBundleStatusCallback(callback);
}
}  // namespace AppExecFwk
}  // namespace OHOS