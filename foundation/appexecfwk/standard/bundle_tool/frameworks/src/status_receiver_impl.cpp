/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "status_receiver_impl.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
const int32_t MAX_WAITING_TIME = 5;

StatusReceiverImpl::StatusReceiverImpl()
{
    APP_LOGI("create status receiver instance");
}

StatusReceiverImpl::~StatusReceiverImpl()
{
    APP_LOGI("destroy status receiver instance");
}

void StatusReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    APP_LOGI("on finished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
    resultMsgSignal_.set_value(resultCode);
}

void StatusReceiverImpl::OnStatusNotify(const int progress)
{
    APP_LOGI("on OnStatusNotify is %{public}d", progress);
}

int32_t StatusReceiverImpl::GetResultCode() const
{
    auto future = resultMsgSignal_.get_future();
    if (future.wait_for(std::chrono::seconds(MAX_WAITING_TIME)) == std::future_status::ready) {
        int32_t resultCode = future.get();
        return resultCode;
    }
    return ERR_OPERATION_TIME_OUT;
}
}  // namespace AppExecFwk
}  // namespace OHOS