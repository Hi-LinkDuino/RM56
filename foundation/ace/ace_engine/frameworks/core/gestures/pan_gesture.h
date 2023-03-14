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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PAN_GESTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PAN_GESTURE_H

#include <functional>
#include <string>
#include <vector>

#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/gestures/gesture_info.h"

namespace OHOS::Ace {

class ACE_EXPORT PanGesture : public Gesture {
    DECLARE_ACE_TYPE(PanGesture, Gesture);

public:
    PanGesture(int32_t fingers, const PanDirection& direction, double distance)
    {
        fingers_ = fingers;
        direction_ = direction;
        distance_ = distance;
    };
    explicit PanGesture(RefPtr<PanGestureOption> panGestureOption)
    {
        panGestureOption_ = panGestureOption;
    };
    ~PanGesture() override = default;

protected:
    RefPtr<GestureRecognizer> CreateRecognizer(WeakPtr<PipelineContext> context) override;

private:
    PanDirection direction_;
    double distance_ = 0.0;
    RefPtr<PanGestureOption> panGestureOption_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PAN_GESTURE_H
