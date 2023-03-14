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

#include "core/gestures/single_child_gesture.h"
#include "core/gestures/timeout_recognizer.h"

namespace OHOS::Ace {
void SingleChildGesture::SetChild(RefPtr<Gesture> child)
{
    child_ = std::move(child);
}

const RefPtr<Gesture>& SingleChildGesture::GetChild() const
{
    return child_;
}

RefPtr<GestureRecognizer> SingleChildGesture::CreateChildRecognizer(WeakPtr<PipelineContext> context)
{
    return child_->CreateRecognizer(context);
}
} // namespace OHOS::Ace

