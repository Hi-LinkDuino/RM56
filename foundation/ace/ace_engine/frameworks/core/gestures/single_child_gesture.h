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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SINGLE_CHILD_GESTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SINGLE_CHILD_GESTURE_H

#include "core/gestures/gesture_info.h"

namespace OHOS::Ace {
class ACE_EXPORT SingleChildGesture : public Gesture {
    DECLARE_ACE_TYPE(SingleChildGesture, Gesture);

public:
    SingleChildGesture() = default;

    void SetChild(RefPtr<Gesture> child);
    const RefPtr<Gesture>& GetChild() const;
    RefPtr<GestureRecognizer> CreateChildRecognizer(WeakPtr<PipelineContext> context);

private:
    RefPtr<Gesture> child_;
};
} // namespace OHOS::Ace::FrameWork
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SINGLE_CHILD_GESTURE_H
