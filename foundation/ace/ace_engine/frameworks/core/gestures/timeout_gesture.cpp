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

#include "core/gestures/timeout_gesture.h"
#include "core/gestures/timeout_recognizer.h"

namespace OHOS::Ace {
TimeoutGesture::TimeoutGesture(const std::chrono::duration<float>& timeout)
    : timeout_(timeout)
{
}

RefPtr<GestureRecognizer> TimeoutGesture::CreateRecognizer(WeakPtr<PipelineContext> context)
{
    auto timeout = AceType::MakeRefPtr<OHOS::Ace::TimeoutRecognizer>(
            context,
            CreateChildRecognizer(context),
            timeout_);

    timeout->SetPriority(priority_);
    timeout->SetPriorityMask(gestureMask_);

    return timeout;
}
} // namespace OHOS::Ace
