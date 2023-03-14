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

#include "bundle_death_recipient.h"

#include "app_log_wrapper.h"
#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    const std::string STRING_FAIL = "[BUNDLER_MANAGER_SERVICE_UNAVAILABLE]";
}
BundleDeathRecipient::BundleDeathRecipient(const sptr<StatusReceiverHost>& statusReceiver)
    : statusReceiver_(statusReceiver)
{
}

BundleDeathRecipient::~BundleDeathRecipient()
{
    statusReceiver_ = nullptr;
}

void BundleDeathRecipient::OnRemoteDied([[maybe_unused]] const wptr<IRemoteObject>& remote)
{
    APP_LOGD("BundleManagerService is died");
    if (statusReceiver_ == nullptr) {
        APP_LOGI("statusReceiver_ is nullptr");
        return;
    }
    statusReceiver_->OnFinished(IStatusReceiver::ERR_FAILED_SERVICE_DIED, STRING_FAIL);
}
} // namespace AppExecFwk
} // namespace OHOS