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

#include "core/gestures/tap_gesture.h"

#include "core/gestures/click_recognizer.h"

namespace OHOS::Ace {

RefPtr<GestureRecognizer> TapGesture::CreateRecognizer(WeakPtr<PipelineContext> context)
{
    auto clickRecognizer = AceType::MakeRefPtr<OHOS::Ace::ClickRecognizer>(context, fingers_, count_);
    if (onActionId_) {
        clickRecognizer->SetOnAction(*onActionId_);
    }

    clickRecognizer->SetPriority(priority_);
    clickRecognizer->SetPriorityMask(gestureMask_);
    return clickRecognizer;
}

} // namespace OHOS::Ace
