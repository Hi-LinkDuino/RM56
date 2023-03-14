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

#include "mock_bundle_status.h"

namespace OHOS {
namespace AppExecFwk {
void MockBundleStatus::OnBundleStateChanged(
    const uint8_t installType, const int32_t resultCode, const std::string &resultMsg, const std::string &bundleName)
{
    signal_.set_value(resultCode);
}

sptr<IRemoteObject> MockBundleStatus::AsObject()
{
    return nullptr;
}

int32_t MockBundleStatus::GetResultCode()
{
    auto future = signal_.get_future();
    std::future_status status = future.wait_for(std::chrono::seconds(1));
    if (status == std::future_status::timeout) {
        return ERR_TIMED_OUT;
    } else {
        return future.get();
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS