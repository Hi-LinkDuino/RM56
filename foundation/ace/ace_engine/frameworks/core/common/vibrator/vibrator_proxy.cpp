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

#include "core/common/vibrator/vibrator_proxy.h"

namespace OHOS::Ace {

VibratorProxy::VibratorProxy() = default;

VibratorProxy::~VibratorProxy() = default;

void VibratorProxy::SetDelegate(std::unique_ptr<VibratorInterface>&& delegate)
{
    delegate_ = std::move(delegate);
}

RefPtr<Vibrator> VibratorProxy::GetVibrator(const RefPtr<TaskExecutor>& taskExecutor)
{
    if (!delegate_) {
        return nullptr;
    }
    return delegate_->GetVibrator(taskExecutor);
}

} // namespace OHOS::Ace
