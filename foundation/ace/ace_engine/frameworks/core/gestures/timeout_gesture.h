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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_TIMEOUT_GESTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_TIMEOUT_GESTURE_H

#include "core/gestures/gesture_info.h"
#include "core/gestures/single_child_gesture.h"

#include <chrono>

namespace OHOS::Ace {
class ACE_EXPORT TimeoutGesture : public SingleChildGesture {
    DECLARE_ACE_TYPE(TimeoutGesture, SingleChildGesture);

public:
    explicit TimeoutGesture(const std::chrono::duration<float>& timeout);

protected:
    RefPtr<GestureRecognizer> CreateRecognizer(WeakPtr<PipelineContext> context) override;

private:
    std::chrono::duration<float> timeout_;
};
} // namespace OHOS::Ace

#endif // jFOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_TIMEOUT_GESTURE_H

