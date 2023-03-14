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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_LONG_PRESS_GESTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_LONG_PRESS_GESTURE_H

#include <functional>
#include <string>
#include <vector>

#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/gestures/gesture_info.h"

namespace OHOS::Ace {

class ACE_EXPORT LongPressGesture : public Gesture {
    DECLARE_ACE_TYPE(LongPressGesture, Gesture);

public:
    LongPressGesture(int32_t fingers, bool repeat, int32_t duration)
        : Gesture(fingers), repeat_(repeat), duration_(duration)
    {}
    ~LongPressGesture() override = default;

protected:
    RefPtr<GestureRecognizer> CreateRecognizer(WeakPtr<PipelineContext> context) override;

private:
    bool repeat_ = false;
    int32_t duration_ = 500;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_LONG_PRESS_GESTURE_H
