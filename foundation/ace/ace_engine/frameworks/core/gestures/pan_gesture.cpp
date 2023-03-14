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

#include "core/gestures/pan_gesture.h"

#include "base/geometry/dimension.h"
#include "core/gestures/pan_recognizer.h"

namespace OHOS::Ace {

RefPtr<GestureRecognizer> Ace::PanGesture::CreateRecognizer(WeakPtr<PipelineContext> context)
{
    auto newContext = context.Upgrade();
    if (!newContext) {
        LOGE("fail to create pan recognizer due to context is nullptr");
        return nullptr;
    }
    double distance = newContext->NormalizeToPx(Dimension(distance_, DimensionUnit::VP));
    RefPtr<OHOS::Ace::PanRecognizer> panRecognizer;
    if (panGestureOption_) {
        LOGD("AceType::MakeRefPtr<OHOS::Ace::PanRecognizer>(context, panGestureOption_)");
        panRecognizer = AceType::MakeRefPtr<OHOS::Ace::PanRecognizer>(context, panGestureOption_);
    } else {
        LOGD("AceType::MakeRefPtr<OHOS::Ace::PanRecognizer>(context, fingers_, direction_, distance)");
        panRecognizer = AceType::MakeRefPtr<OHOS::Ace::PanRecognizer>(context, fingers_, direction_, distance);
    }
    if (onActionStartId_) {
        panRecognizer->SetOnActionStart(*onActionStartId_);
    }

    if (onActionUpdateId_) {
        panRecognizer->SetOnActionUpdate(*onActionUpdateId_);
    }

    if (onActionEndId_) {
        panRecognizer->SetOnActionEnd(*onActionEndId_);
    }

    if (onActionCancelId_) {
        panRecognizer->SetOnActionCancel(*onActionCancelId_);
    }

    panRecognizer->SetPriority(priority_);
    panRecognizer->SetPriorityMask(gestureMask_);
    return panRecognizer;
}

} // namespace OHOS::Ace
