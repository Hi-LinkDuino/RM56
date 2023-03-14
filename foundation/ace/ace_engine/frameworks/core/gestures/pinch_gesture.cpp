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

#include "core/gestures/pinch_gesture.h"

#include "base/geometry/dimension.h"
#include "core/gestures/pinch_recognizer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

RefPtr<GestureRecognizer> PinchGesture::CreateRecognizer(WeakPtr<PipelineContext> context)
{
    auto newContext = context.Upgrade();
    if (!newContext) {
        LOGE("fail to create pinch recognizer due to context is nullptr");
        return nullptr;
    }

    double distance = newContext->NormalizeToPx(Dimension(distance_, DimensionUnit::VP));
    auto pinchRecognizer = AceType::MakeRefPtr<OHOS::Ace::PinchRecognizer>(fingers_, distance);
    if (onActionStartId_) {
        pinchRecognizer->SetOnActionStart(*onActionStartId_);
    }

    if (onActionUpdateId_) {
        pinchRecognizer->SetOnActionUpdate(*onActionUpdateId_);
    }

    if (onActionEndId_) {
        pinchRecognizer->SetOnActionEnd(*onActionEndId_);
    }

    if (onActionCancelId_) {
        pinchRecognizer->SetOnActionCancel(*onActionCancelId_);
    }

    pinchRecognizer->SetPriority(priority_);
    pinchRecognizer->SetPriorityMask(gestureMask_);

    return pinchRecognizer;
}

} // namespace OHOS::Ace
