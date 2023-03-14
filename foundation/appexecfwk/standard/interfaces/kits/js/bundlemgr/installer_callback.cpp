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

#include "installer_callback.h"

namespace OHOS {
namespace AppExecFwk {
void InstallerCallback::OnFinished(const int32_t resultCode, [[maybe_unused]] const std::string &resultMsg)
{
    resultCodeSignal_.set_value(resultCode);
    resultMsgSignal_.set_value(resultMsg);
}

void InstallerCallback::OnStatusNotify(const int32_t progress)
{
    return;
}

int32_t InstallerCallback::GetResultCode()
{
    auto future = resultCodeSignal_.get_future();
    future.wait();
    return future.get();
}

std::string InstallerCallback::GetResultMsg()
{
    auto future = resultMsgSignal_.get_future();
    future.wait();
    return future.get();
}
}  // namespace AppExecFwk
}  // namespace OHOS