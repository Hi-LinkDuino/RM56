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

#include "core/gestures/slide_gesture.h"

#include "base/geometry/dimension.h"
#include "core/gestures/slide_recognizer.h"

namespace OHOS::Ace {

RefPtr<GestureRecognizer> Ace::SwipeGesture::CreateRecognizer(WeakPtr<PipelineContext> context)
{
    auto newContext = context.Upgrade();
    if (!newContext) {
        LOGE("fail to create pan recognizer due to context is nullptr");
        return nullptr;
    }
    double speed = newContext->NormalizeToPx(Dimension(speed_, DimensionUnit::VP));
    RefPtr<OHOS::Ace::SlideRecognizer> slideRecognizer;
    LOGD("AceType::MakeRefPtr<SlideRecognizer>(context, fingers_, direction_, speed)");
    slideRecognizer = AceType::MakeRefPtr<SlideRecognizer>(context, fingers_, direction_, speed);
    if (onActionId_) {
        slideRecognizer->SetOnAction(*onActionId_);
    }

    slideRecognizer->SetPriority(priority_);
    slideRecognizer->SetPriorityMask(gestureMask_);
    return slideRecognizer;
}

} // namespace OHOS::Ace