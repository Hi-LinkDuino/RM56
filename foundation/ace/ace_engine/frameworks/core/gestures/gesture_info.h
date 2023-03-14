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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_INFO_H

#include <functional>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/geometry/offset.h"
#include "base/geometry/point.h"
#include "base/image/pixel_map.h"
#include "base/memory/ace_type.h"
#include "base/utils/event_callback.h"
#include "base/utils/macros.h"
#include "base/utils/type_definition.h"
#include "core/event/ace_events.h"

namespace OHOS::Ace {

namespace {
constexpr int32_t DEFAULT_PAN_FINGER = 1;
constexpr double DEFAULT_PAN_DISTANCE = 5.0;
constexpr double DEFAULT_SLIDE_DISTANCE = DEFAULT_PAN_DISTANCE;
constexpr int32_t DEFAULT_SLIDE_FINGER = DEFAULT_PAN_FINGER;
constexpr double DEFAULT_SLIDE_SPEED = 300.0;
} // namespace

class GestureRecognizer;
class PipelineContext;

enum class GesturePriority {
    Begin = -1,
    Low = 0,
    High,
    Parallel,
    End,
};

enum class GestureMask {
    Begin = -1,
    Normal = 0,
    IgnoreInternal,
    End,
};

enum class GestureMode {
    Begin = -1,
    Sequence = 0,
    Parallel,
    Exclusive,
    End,
};

enum class Direction {
    BEGIN = -1,
    ALL = 0,
    HORIZONTAL,
    VERTICAL,
    END,
};

enum class DragEventAction {
    DRAG_EVENT_START = 0,
    DRAG_EVENT_MOVE,
    DRAG_EVENT_END,
};

struct PanDirection final {
    static constexpr uint32_t NONE = 0;
    static constexpr uint32_t LEFT = 1;
    static constexpr uint32_t RIGHT = 2;
    static constexpr uint32_t HORIZONTAL = 3;
    static constexpr uint32_t UP = 4;
    static constexpr uint32_t DOWN = 8;
    static constexpr uint32_t VERTICAL = 12;
    static constexpr uint32_t ALL = 15;

    uint32_t type = ALL;
};

using OnPanFingersFunc = EventCallback<void(int32_t fingers)>;
using PanFingersFuncType = OnPanFingersFunc::FunctionType;
using OnPanDirectionFunc = EventCallback<void(const PanDirection& direction)>;
using PanDirectionFuncType = OnPanDirectionFunc::FunctionType;
using OnPanDistanceFunc = EventCallback<void(double distance)>;
using PanDistanceFuncType = OnPanDistanceFunc::FunctionType;

class PanGestureOption : public AceType {
    DECLARE_ACE_TYPE(PanGestureOption, AceType);

public:
    PanGestureOption() {}
    ~PanGestureOption() = default;

    void SetDirection(PanDirection direction)
    {
        direction_ = direction;
        for (const auto& callback : onPanDirectionIds_) {
            (callback.second.GetCallback())(direction);
        }
    }

    const PanDirection GetDirection() const
    {
        return direction_;
    }

    void SetDistance(double distance)
    {
        distance_ = distance;
        for (const auto& callback : onPanDistanceIds_) {
            (callback.second.GetCallback())(distance);
        }
    }

    double GetDistance()
    {
        return distance_;
    }

    void SetFingers(int32_t fingers)
    {
        fingers_ = fingers;
        for (const auto& callback : onPanFingersIds_) {
            (callback.second.GetCallback())(fingers);
        }
    }

    int32_t GetFingers()
    {
        return fingers_;
    }

    std::unordered_map<typename OnPanFingersFunc::IdType, OnPanFingersFunc>& GetOnPanFingersIds()
    {
        return onPanFingersIds_;
    }

    void SetOnPanFingersId(const OnPanFingersFunc& onPanFingersId)
    {
        onPanFingersIds_.emplace(onPanFingersId.GetId(), onPanFingersId);
    }

    std::unordered_map<typename OnPanDirectionFunc::IdType, OnPanDirectionFunc>& GetOnPanDirectionIds()
    {
        return onPanDirectionIds_;
    }

    void SetOnPanDirectionId(const OnPanDirectionFunc& onPanDirectionId)
    {
        onPanDirectionIds_.emplace(onPanDirectionId.GetId(), onPanDirectionId);
    }

    std::unordered_map<typename OnPanDistanceFunc::IdType, OnPanDistanceFunc>& GetOnPanDistanceIds()
    {
        return onPanDistanceIds_;
    }

    void SetOnPanDistanceId(const OnPanDistanceFunc& onPanDistanceId)
    {
        onPanDistanceIds_.emplace(onPanDistanceId.GetId(), onPanDistanceId);
    }
private:
    PanDirection direction_;
    double distance_ = DEFAULT_PAN_DISTANCE;
    int32_t fingers_ = DEFAULT_PAN_FINGER;
    std::unordered_map<typename OnPanFingersFunc::IdType, OnPanFingersFunc> onPanFingersIds_;
    std::unordered_map<typename OnPanDirectionFunc::IdType, OnPanDirectionFunc> onPanDirectionIds_;
    std::unordered_map<typename OnPanDistanceFunc::IdType, OnPanDistanceFunc> onPanDistanceIds_;
};

struct SwipeDirection final {
    static constexpr uint32_t NONE = 0;
    static constexpr uint32_t HORIZONTAL = 1;
    static constexpr uint32_t VERTICAL = 2;
    static constexpr uint32_t ALL = 3;

    uint32_t type = ALL;
};
using OnSwipeFingersFunc = EventCallback<void(int32_t fingers)>;
using SwipeFingersFuncType = OnSwipeFingersFunc::FunctionType;
using OnSwipeDirectionFunc = EventCallback<void(const SwipeDirection& direction)>;
using SwipeDirectionFuncType = OnSwipeDirectionFunc::FunctionType;
using OnSwipeSpeedFunc = EventCallback<void(double speed)>;
using SwipeSpeedFuncType = OnSwipeSpeedFunc::FunctionType;

class PasteData : public AceType {
    DECLARE_ACE_TYPE(PasteData, AceType);

public:
    PasteData() {}
    ~PasteData() = default;

    void SetPlainText(const std::string plainText)
    {
        plainText_ = plainText;
    }

    const std::string GetPlainText() const
    {
        return plainText_;
    }
private:
    std::string plainText_ = "";
};

class DragEvent : public AceType {
public:
    DragEvent() {}
    ~DragEvent() = default;

    void SetPasteData(const RefPtr<PasteData>& pasteData)
    {
        pasteData_ = pasteData;
    }

    RefPtr<PasteData> GetPasteData() const
    {
        return pasteData_;
    }

    double GetX() const
    {
        return x_;
    }

    double GetY() const
    {
        return y_;
    }

    void SetX(double x)
    {
        x_ = x;
    }

    void SetY(double y)
    {
        y_ = y;
    }

    void SetDescription(const std::string description)
    {
        description_ = description;
    }

    const std::string GetDescription() const
    {
        return description_;
    }

    void SetPixmap(const RefPtr<PixelMap>& pixelMap)
    {
        pixelMap_ = pixelMap;
    }

    RefPtr<PixelMap> GetPixmap() const
    {
        return pixelMap_;
    }

private:
    RefPtr<PasteData> pasteData_;
    double x_ = 0.0;
    double y_ = 0.0;
    std::string description_ = "";
    RefPtr<PixelMap> pixelMap_;
};

struct FingerInfo {
    int32_t fingerId_ = -1;
    // global position at which the touch point contacts the screen.
    Offset globalLocation_;
    // Different from global location, The local location refers to the location of the contact point relative to the
    // current node which has the recognizer.
    Offset localLocation_;
};

class ItemDragInfo : public BaseEventInfo {
public:
    ItemDragInfo() : BaseEventInfo("itemDrag") {}
    ~ItemDragInfo() = default;

    double GetX() const
    {
        return x_;
    }

    double GetY() const
    {
        return y_;
    }

    void SetX(double x)
    {
        x_ = x;
    }

    void SetY(double y)
    {
        y_ = y;
    }

private:
    double x_ = 0.0;
    double y_ = 0.0;
};

class GestureEvent : public BaseEventInfo {
public:
    GestureEvent() : BaseEventInfo("gesture") {}
    ~GestureEvent() = default;

    void SetRepeat(bool repeat)
    {
        repeat_ = repeat;
    }

    bool GetRepeat() const
    {
        return repeat_;
    }

    void SetOffsetX(double offsetX)
    {
        offsetX_ = offsetX;
    }

    double GetOffsetX() const
    {
        return offsetX_;
    }

    void SetOffsetY(double offsetY)
    {
        offsetY_ = offsetY;
    }

    double GetOffsetY() const
    {
        return offsetY_;
    }

    void SetScale(double scale)
    {
        scale_ = scale;
    }

    double GetScale() const
    {
        return scale_;
    }

    void SetAngle(double angle)
    {
        angle_ = angle;
    }

    double GetAngle() const
    {
        return angle_;
    }

    GestureEvent& SetGlobalPoint(const Point& globalPoint)
    {
        globalPoint_ = globalPoint;
        return *this;
    }

    const Point& GetGlobalPoint() const
    {
        return globalPoint_;
    }

    GestureEvent& SetScreenLocation(const Offset& screenLocation)
    {
        screenLocation_ = screenLocation;
        return *this;
    }

    const Offset& GetScreenLocation() const
    {
        return screenLocation_;
    }

    GestureEvent& SetGlobalLocation(const Offset& globalLocation)
    {
        globalLocation_ = globalLocation;
        return *this;
    }
    GestureEvent& SetLocalLocation(const Offset& localLocation)
    {
        localLocation_ = localLocation;
        return *this;
    }

    const Offset& GetLocalLocation() const
    {
        return localLocation_;
    }
    const Offset& GetGlobalLocation() const
    {
        return globalLocation_;
    }

    const Offset& GetPinchCenter() const
    {
        return pinchCenter_;
    }

    GestureEvent& SetPinchCenter(const Offset& pinchCenter)
    {
        pinchCenter_ = pinchCenter;
        return *this;
    }

    const std::list<FingerInfo>& GetFingerList() const
    {
        return fingerList_;
    }

    void SetFingerList(const std::list<FingerInfo>& fingerList)
    {
        fingerList_ = fingerList;
    }

    void SetSpeed(double speed)
    {
        speed_ = speed;
    }

    double GetSpeed() const
    {
        return speed_;
    }

    void SetPressed(bool pressed)
    {
        pressed_ = pressed;
    }

    bool GetPressed() const
    {
        return pressed_;
    }

private:
    bool repeat_ = false;
    bool pressed_ = false;
    double offsetX_ = 0.0;
    double offsetY_ = 0.0;
    double scale_ = 1.0;
    double angle_ = 0.0;
    double speed_ = 0.0;
    Point globalPoint_;
    // global position at which the touch point contacts the screen.
    Offset globalLocation_;
    // Different from global location, The local location refers to the location of the contact point relative to the
    // current node which has the recognizer.
    Offset localLocation_;
    Offset screenLocation_;
    Offset pinchCenter_;
    std::list<FingerInfo> fingerList_;
};

using GestureEventFunc = std::function<void(GestureEvent& info)>;
using GestureEventNoParameter = std::function<void()>;

class ACE_EXPORT Gesture : public virtual AceType {
    DECLARE_ACE_TYPE(Gesture, AceType);

public:
    Gesture() = default;
    explicit Gesture(int32_t fingers) : fingers_(fingers) {}
    ~Gesture() override = default;

    void SetOnActionId(const GestureEventFunc& onActionId)
    {
        onActionId_ = std::make_unique<GestureEventFunc>(onActionId);
    }
    void SetOnActionStartId(const GestureEventFunc& onActionStartId)
    {
        onActionStartId_ = std::make_unique<GestureEventFunc>(onActionStartId);
    }
    void SetOnActionUpdateId(const GestureEventFunc& onActionUpdateId)
    {
        onActionUpdateId_ = std::make_unique<GestureEventFunc>(onActionUpdateId);
    }
    void SetOnActionEndId(const GestureEventFunc& onActionEndId)
    {
        onActionEndId_ = std::make_unique<GestureEventFunc>(onActionEndId);
    }
    void SetOnActionCancelId(const GestureEventNoParameter& onActionCancelId)
    {
        onActionCancelId_ = std::make_unique<GestureEventNoParameter>(onActionCancelId);
    }
    void SetPriority(GesturePriority priority)
    {
        priority_ = priority;
    }
    void SetGestureMask(GestureMask gestureMask)
    {
        gestureMask_ = gestureMask;
    }

    virtual RefPtr<GestureRecognizer> CreateRecognizer(WeakPtr<PipelineContext> context) = 0;

protected:
    int32_t fingers_ = 1;
    GesturePriority priority_ = GesturePriority::Low;
    GestureMask gestureMask_ = GestureMask::Normal;
    std::unique_ptr<GestureEventFunc> onActionId_;
    std::unique_ptr<GestureEventFunc> onActionStartId_;
    std::unique_ptr<GestureEventFunc> onActionUpdateId_;
    std::unique_ptr<GestureEventFunc> onActionEndId_;
    std::unique_ptr<GestureEventNoParameter> onActionCancelId_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_GESTURE_INFO_H
