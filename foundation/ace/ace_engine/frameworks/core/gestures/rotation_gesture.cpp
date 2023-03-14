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

#include "core/gestures/rotation_gesture.h"

#include "core/gestures/rotation_recognizer.h"

namespace OHOS::Ace {

RefPtr<GestureRecognizer> RotationGesture::CreateRecognizer(WeakPtr<PipelineContext> context)
{
    auto rotationRecognizer = AceType::MakeRefPtr<OHOS::Ace::RotationRecognizer>(fingers_, angle_);
    if (onActionStartId_) {
        rotationRecognizer->SetOnActionStart(*onActionStartId_);
    }

    if (onActionUpdateId_) {
        rotationRecognizer->SetOnActionUpdate(*onActionUpdateId_);
    }

    if (onActionEndId_) {
        rotationRecognizer->SetOnActionEnd(*onActionEndId_);
    }

    if (onActionCancelId_) {
        rotationRecognizer->SetOnActionCancel(*onActionCancelId_);
    }

    rotationRecognizer->SetPriority(priority_);
    rotationRecognizer->SetPriorityMask(gestureMask_);
    return rotationRecognizer;
}

} // namespace OHOS::Ace
