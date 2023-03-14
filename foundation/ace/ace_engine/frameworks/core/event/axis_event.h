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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_AXIS_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_AXIS_EVENT_H

#include <list>

#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "core/components/scroll/scroll_fade_painter.h"
#include "core/event/ace_events.h"

namespace OHOS::Ace {

constexpr int32_t MOUSE_WHEEL_DEGREES = 15;
constexpr int32_t DP_PER_LINE_DESKTOP = 40;
constexpr int32_t LINE_NUMBER_DESKTOP = 3;
constexpr int32_t DP_PER_LINE_PHONE = 64;
constexpr int32_t LINE_NUMBER_PHONE = 1;

enum class AxisDirection : int32_t {
    NONE = 0,
    UP = 1,
    DOWN = 2,
    LEFT = 4,
    RIGHT = 8,
};

struct AxisEvent final {
    float x = 0.0f;
    float y = 0.0f;
    float verticalAxis = 0.0f;
    float horizontalAxis = 0.0f;
    TimeStamp time;
    int64_t deviceId = 0;
    SourceType sourceType = SourceType::NONE;

    AxisEvent CreateScaleEvent(float scale) const
    {
        if (NearZero(scale)) {
            return { .x = x,
                .y = y,
                .verticalAxis = verticalAxis,
                .horizontalAxis = horizontalAxis,
                .time = time,
                .deviceId = deviceId,
                .sourceType = sourceType };
        }
        return { .x = x / scale,
            .y = y / scale,
            .verticalAxis = verticalAxis / scale,
            .horizontalAxis = horizontalAxis / scale,
            .time = time,
            .deviceId = deviceId,
            .sourceType = sourceType };
    }
};

class AxisEventTarget : public virtual AceType {
    DECLARE_ACE_TYPE(AxisEventTarget, AceType);

public:
    virtual void HandleEvent(const AxisEvent& event) = 0;
};

using AxisTestResult = std::list<RefPtr<AxisEventTarget>>;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_AXIS_EVENT_H