/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/gestures/raw_recognizer.h"

#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

const char ON_TOUCH_DOWN_EVENT[] = "onTouchDown";
const char ON_TOUCH_MOVE_EVENT[] = "onTouchMove";
const char ON_TOUCH_UP_EVENT[] = "onTouchUp";
const char ON_TOUCH_CANCEL_EVENT[] = "onTouchCancel";

} // namespace

void RawRecognizer::HandleEvent(const TouchEvent& point, uint32_t stage)
{
    LOGD("raw recognizer handle event, event type is %{public}zu stage=%u", point.type, stage);
    switch (point.type) {
        case TouchType::MOVE: {
            auto callback = onEventCallbacks_[stage][EventType::TOUCH_MOVE];
            if (callback) {
                callback(CreateTouchEventInfo(ON_TOUCH_MOVE_EVENT, point));
            }
            break;
        }
        case TouchType::DOWN: {
            auto callback = onEventCallbacks_[stage][EventType::TOUCH_DOWN];
            if (callback) {
                callback(CreateTouchEventInfo(ON_TOUCH_DOWN_EVENT, point));
            }
            break;
        }
        case TouchType::UP: {
            auto callback = onEventCallbacks_[stage][EventType::TOUCH_UP];
            if (callback) {
                lastPoint_ = point;
                callback(CreateTouchEventInfo(ON_TOUCH_UP_EVENT, point, true));
            }
            break;
        }
        case TouchType::CANCEL: {
            auto callback = onEventCallbacks_[stage][EventType::TOUCH_CANCEL];
            if (callback) {
                lastPoint_ = point;
                callback(CreateTouchEventInfo(ON_TOUCH_CANCEL_EVENT, point, true));
            }
            break;
        }
        default:
            LOGW("unknown touch type");
            break;
    }
    isFirstTrack_ = point.type == TouchType::DOWN;
    lastPoint_ = point;
}

bool RawRecognizer::DispatchEvent(const TouchEvent& point)
{
    HandleEvent(point, EventStage::CAPTURE);
    CatchTouchEventCallback catchCallback;
    if (point.type == TouchType::DOWN) {
        catchCallback = catcheventCallbacks_[EventStage::CAPTURE][EventType::TOUCH_DOWN];
    } else if (point.type == TouchType::MOVE) {
        catchCallback = catcheventCallbacks_[EventStage::CAPTURE][EventType::TOUCH_MOVE];
    } else if (point.type == TouchType::UP) {
        catchCallback = catcheventCallbacks_[EventStage::CAPTURE][EventType::TOUCH_UP];
    } else if (point.type == TouchType::CANCEL) {
        catchCallback = catcheventCallbacks_[EventStage::CAPTURE][EventType::TOUCH_CANCEL];
    }

    if (catchCallback) {
        catchCallback();
        return false;
    }
    return true;
}

bool RawRecognizer::HandleEvent(const TouchEvent& point)
{
    HandleEvent(point, EventStage::BUBBLE);
    CatchTouchEventCallback catchCallback;
    if (point.type == TouchType::DOWN) {
        catchCallback = catcheventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_DOWN];
    } else if (point.type == TouchType::MOVE) {
        catchCallback = catcheventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_MOVE];
    } else if (point.type == TouchType::UP) {
        catchCallback = catcheventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_UP];
    } else if (point.type == TouchType::CANCEL) {
        catchCallback = catcheventCallbacks_[EventStage::BUBBLE][EventType::TOUCH_CANCEL];
    }

    if (catchCallback) {
        catchCallback();
        return false;
    }
    return true;
};

TouchEventInfo RawRecognizer::CreateTouchEventInfo(
    const std::string& type, const TouchEvent& point, bool ignoreCurrent) const
{
    auto useNewVersion = true;
    TouchEventInfo info(type);
    if (useNewVersion) {
        TouchLocationInfo changedTouchLocationInfo(point.id);
        changedTouchLocationInfo.SetGlobalLocation(point.GetOffset())
            .SetLocalLocation(point.GetOffset() - coordinateOffset_)
            .SetSize(point.size);
        changedTouchLocationInfo.SetForce(point.force);
        info.AddChangedTouchLocationInfo(std::move(changedTouchLocationInfo));
        for (auto&& touchPoint : point.pointers) {
            TouchLocationInfo touchLocationInfo(touchPoint.id);
            auto offset = Offset(touchPoint.x, touchPoint.y);
            touchLocationInfo.SetGlobalLocation(offset)
                .SetLocalLocation(offset - coordinateOffset_)
                .SetSize(touchPoint.size);
            touchLocationInfo.SetForce(touchPoint.force);
            info.AddTouchLocationInfo(std::move(touchLocationInfo));
        }
        return info;
    }
    if (!isFirstTrack_) {
        TouchLocationInfo lastTouchLocationInfo(lastPoint_.id);
        lastTouchLocationInfo.SetGlobalLocation(lastPoint_.GetOffset())
            .SetLocalLocation(lastPoint_.GetOffset() - coordinateOffset_)
            .SetSize(lastPoint_.size);
        lastTouchLocationInfo.SetForce(lastPoint_.force);
        info.AddChangedTouchLocationInfo(std::move(lastTouchLocationInfo));
        info.SetDeviceId(lastPoint_.deviceId);
        if (ignoreCurrent) {
            info.SetTimeStamp(lastPoint_.time);
            return info;
        }
    }
    info.SetTimeStamp(point.time);
    info.SetDeviceId(point.deviceId);
    TouchLocationInfo currentTouchLocationInfo(point.id);
    currentTouchLocationInfo.SetGlobalLocation(point.GetOffset())
        .SetLocalLocation(point.GetOffset() - coordinateOffset_)
        .SetSize(point.size);
    currentTouchLocationInfo.SetForce(point.force);
    info.AddTouchLocationInfo(std::move(currentTouchLocationInfo));
    return info;
}

} // namespace OHOS::Ace