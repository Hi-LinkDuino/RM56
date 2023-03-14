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

#include <gtest/gtest.h>

#include "mock/gesture_mock.h"

#include "core/common/platform_window.h"
#include "core/common/window.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/gesture_referee.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/pan_recognizer.h"
#include "core/gestures/pinch_recognizer.h"
#include "core/gestures/raw_recognizer.h"
#include "core/gestures/rotation_recognizer.h"
#include "core/gestures/velocity_tracker.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double LOCATION_X = 200.0;
constexpr double LOCATION_Y = 400.0;
constexpr double LOCATION_X1 = 400.0;
constexpr double LOCATION_Y1 = 200.0;
constexpr double LOCATION_X2 = 600.0;
constexpr double LOCATION_Y2 = 600.0;
constexpr double LOCATION_STATIC = 0.0;
constexpr int32_t TIME_MILLISECOND = 1000;
constexpr int32_t TIME_COUNTS = 500;

constexpr double MAX_THRESHOLD = 20.0;
constexpr double MIN_PAN_DISTANCE = 15.0;
constexpr double MIN_PINCH_DISTANCE = 10.0;
constexpr double MIN_ROTATION_ANGLE = 1.0;
constexpr double DEFAULT_LONGPRESS_DURATION = 500;

const std::string TOUCH_DOWN_TYPE = "onTouchDown";
const std::string TOUCH_UP_TYPE = "onTouchUp";
const std::string TOUCH_MOVE_TYPE = "onTouchMove";
const std::string TOUCH_CANCEL_TYPE = "onTouchCancel";

} // namespace

class TouchEventResult {
public:
    explicit TouchEventResult(const std::string& type) : touchEventInfo_(type) {}
    ~TouchEventResult() = default;

    const TouchEventInfo& GetTouchEventInfo() const
    {
        return touchEventInfo_;
    }

    void SetTouchEventInfo(const TouchEventInfo& touchEventInfo)
    {
        touchEventInfo_ = touchEventInfo;
    }

private:
    TouchEventInfo touchEventInfo_;
};

class LongPressEventResult {
public:
    explicit LongPressEventResult() : longPressInfo_(0) {}
    ~LongPressEventResult() = default;

    void SetLongPress(bool longPress)
    {
        longPress_ = longPress;
    }

    bool GetLongPress() const
    {
        return longPress_;
    }

    void SetLongPressInfo(const LongPressInfo& longPressInfo)
    {
        longPressInfo_ = longPressInfo;
    }

    const LongPressInfo& GetLongPressInfo()
    {
        return longPressInfo_;
    }

private:
    bool longPress_ = false;
    LongPressInfo longPressInfo_;
};

class ClickEventResult {
public:
    explicit ClickEventResult() : clickInfo_(0) {}
    ~ClickEventResult() = default;

    const ClickInfo& GetClickInfo() const
    {
        return clickInfo_;
    }

    void SetClickInfo(const ClickInfo& clickInfo)
    {
        clickInfo_ = clickInfo;
    }

private:
    ClickInfo clickInfo_;
};

class GestureRefereeResult {
public:
    GestureRefereeResult() : gestureName_("") {}
    ~GestureRefereeResult() = default;

    void SetGestureName(const std::string& gestureName)
    {
        gestureName_ = gestureName;
    }

    const std::string& GetGestureName() const
    {
        return gestureName_;
    }

private:
    std::string gestureName_;
};

class DragEventResult {
public:
    DragEventResult() : dragStartInfo_(0), dragUpdateInfo_(0), dragEndInfo_(0) {}
    ~DragEventResult() = default;

    void SetDragStart(const DragStartInfo& info)
    {
        dragStartInfo_ = info;
    }

    void SetDragUpdate(const DragUpdateInfo& info)
    {
        dragUpdateInfo_ = info;
    }

    void SetDragEnd(const DragEndInfo& info)
    {
        dragEndInfo_ = info;
    }

    const DragStartInfo& GetDragStart() const
    {
        return dragStartInfo_;
    }

    const DragUpdateInfo& GetDragUpdate() const
    {
        return dragUpdateInfo_;
    }

    const DragEndInfo& GetDragEnd() const
    {
        return dragEndInfo_;
    }

    void SetDragCancel(bool dragCancel)
    {
        dragCancel_ = dragCancel;
    }

    bool GetDragCancel() const
    {
        return dragCancel_;
    }

private:
    DragStartInfo dragStartInfo_;
    DragUpdateInfo dragUpdateInfo_;
    DragEndInfo dragEndInfo_;
    bool dragCancel_ = false;
};

class TapEventResult {
public:
    TapEventResult() = default;
    ~TapEventResult() = default;

    const GestureEvent& GetTapInfo() const
    {
        return tapInfo_;
    }

    void SetTapInfo(const GestureEvent& tapInfo)
    {
        tapInfo_ = tapInfo;
    }

private:
    GestureEvent tapInfo_;
};

class PanEventResult {
public:
    PanEventResult() = default;
    ~PanEventResult() = default;

    const GestureEvent& GetPanInfo() const
    {
        return panInfo_;
    }

    void SetPanInfo(const GestureEvent& panInfo)
    {
        panInfo_ = panInfo;
    }

    void SetPanCancel(bool panCancel)
    {
        panCancel_ = panCancel;
    }

    bool GetPanCancel() const
    {
        return panCancel_;
    }

private:
    GestureEvent panInfo_;
    bool panCancel_ = false;
};

class PinchEventResult {
public:
    PinchEventResult() = default;
    ~PinchEventResult() = default;

    const GestureEvent& GetPinchInfo() const
    {
        return pinchInfo_;
    }

    void SetPinchInfo(const GestureEvent& pinchInfo)
    {
        pinchInfo_ = pinchInfo;
    }

    void SetPinchCancel(bool pinchCancel)
    {
        pinchCancel_ = pinchCancel;
    }

    bool GetPinchCancel() const
    {
        return pinchCancel_;
    }

private:
    GestureEvent pinchInfo_;
    bool pinchCancel_ = false;
};

class RotationEventResult {
public:
    RotationEventResult() = default;
    ~RotationEventResult() = default;

    const GestureEvent& GetRotationInfo() const
    {
        return rotationInfo_;
    }

    void SetRotationInfo(const GestureEvent& rotationInfo)
    {
        rotationInfo_ = rotationInfo;
    }

    void SetRotationCancel(bool rotationCancel)
    {
        rotationCancel_ = rotationCancel;
    }

    bool GetRotationCancel() const
    {
        return rotationCancel_;
    }

private:
    GestureEvent rotationInfo_;
    bool rotationCancel_ = false;
};

class MultiLongPressEventResult {
public:
    MultiLongPressEventResult() = default;
    ~MultiLongPressEventResult() = default;

    const GestureEvent& GetMultiLongPressInfo() const
    {
        return multiLongPressInfo_;
    }

    void SetMultiLongPressInfo(const GestureEvent& multiLongPressInfo)
    {
        multiLongPressInfo_ = multiLongPressInfo;
    }

    void SetMultiLongPressCancel(bool multiLongPressCancel)
    {
        multiLongPressCancel_ = multiLongPressCancel;
    }

    bool GetMultiLongPressCancel() const
    {
        return multiLongPressCancel_;
    }

private:
    GestureEvent multiLongPressInfo_;
    bool multiLongPressCancel_ = false;
};

class GesturesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GesturesTest::SetUpTestCase() {}

void GesturesTest::TearDownTestCase() {}

void GesturesTest::SetUp() {}

void GesturesTest::TearDown() {}

/**
 * @tc.name: RawRecognizer001
 * @tc.desc: Verify the raw recognizer recognizes corresponding touch down event.
 * @tc.type: FUNC
 * @tc.require: AR000DAPU9
 * @tc.author: yanshuifeng
 */
HWTEST_F(GesturesTest, RawRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create raw recognizer and set touch down event callback.
     */
    TouchEventResult onTouchDown(TOUCH_DOWN_TYPE);
    auto rawRecognizer = AceType::MakeRefPtr<RawRecognizer>();
    rawRecognizer->SetOnTouchDown([&onTouchDown](const TouchEventInfo& info) { onTouchDown.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    rawRecognizer->HandleEvent(point);
    ASSERT_FALSE(onTouchDown.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchDown.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step3. check the touch location info.
     * @tc.expected: step3. the touch location info is right.
     */
    auto& locationInfo = onTouchDown.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: RawRecognizer002
 * @tc.desc: Verify the raw recognizer recognizes corresponding touch up event.
 * @tc.type: FUNC
 * @tc.require: AR000DAPU9
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, RawRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create raw recognizer and set touch up event callback.
     */
    TouchEventResult onTouchUp(TOUCH_UP_TYPE);
    auto rawRecognizer = AceType::MakeRefPtr<RawRecognizer>();
    rawRecognizer->SetOnTouchUp([&onTouchUp](const TouchEventInfo& info) { onTouchUp.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. send touch up event.
     * @tc.expected: step2. receive touch up callback and touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::UP, .time = std::chrono::high_resolution_clock::now()
    };
    rawRecognizer->HandleEvent(point);
    ASSERT_FALSE(onTouchUp.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchUp.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step3. check the touch location info.
     * @tc.expected: step3. the touch location info is right.
     */
    auto& locationInfo = onTouchUp.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: RawRecognizer003
 * @tc.desc: Verify the raw recognizer recognizes corresponding touch cancel event.
 * @tc.type: FUNC
 * @tc.require: AR000DAPU9
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, RawRecognizer003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create raw recognizer and set touch cancel event callback.
     */
    TouchEventResult onTouchCancel(TOUCH_CANCEL_TYPE);
    auto rawRecognizer = AceType::MakeRefPtr<RawRecognizer>();
    rawRecognizer->SetOnTouchCancel(
        [&onTouchCancel](const TouchEventInfo& info) { onTouchCancel.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. send touch cancel event.
     * @tc.expected: step2. receive touch cancel callback and touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::CANCEL, .time = std::chrono::high_resolution_clock::now()
    };
    rawRecognizer->HandleEvent(point);
    ASSERT_FALSE(onTouchCancel.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchCancel.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step3. check the touch location info.
     * @tc.expected: step3. the touch location info is right.
     */
    auto& locationInfo = onTouchCancel.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: RawRecognizer004
 * @tc.desc: Verify the raw recognizer recognizes corresponding touch move event.
 * @tc.type: FUNC
 * @tc.require: AR000DAPU9
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, RawRecognizer004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create raw recognizer and set touch move event callback.
     */
    TouchEventResult onTouchMove(TOUCH_MOVE_TYPE);
    auto rawRecognizer = AceType::MakeRefPtr<RawRecognizer>();
    rawRecognizer->SetOnTouchMove([&onTouchMove](const TouchEventInfo& info) { onTouchMove.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. send touch move event.
     * @tc.expected: step2. receive touch move callback and touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };
    rawRecognizer->HandleEvent(point);
    ASSERT_FALSE(onTouchMove.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchMove.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step3. check the touch location info.
     * @tc.expected: step3. the touch location info is right.
     */
    auto& locationInfo = onTouchMove.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: VelocityTracker001
 * @tc.desc: Verify the velocity vracker recognizes velocity vracker.
 * @tc.type: FUNC
 * @tc.require: AR000DAPUA
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, VelocityTracker001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create velocity vracker.
     */
    VelocityTracker onVelocityTracker(Axis::VERTICAL);

    /**
     * @tc.steps: step2. send start point.
     * @tc.expected: step2. receive first point to calculate the velocity.
     */
    TouchEvent pointFirst {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };
    onVelocityTracker.UpdateTouchPoint(pointFirst);
    ASSERT_EQ(onVelocityTracker.GetMainAxisPos(), LOCATION_Y);
    ASSERT_EQ(onVelocityTracker.GetMainAxisDeltaPos(), LOCATION_STATIC);

    /**
     * @tc.steps: step3. send end point.
     * @tc.expected: step3. receive first point to calculate the velocity.
     */
    usleep(TIME_MILLISECOND);
    TouchEvent pointSecond {
        .x = LOCATION_X, .y = 2 * LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };

    onVelocityTracker.UpdateTouchPoint(pointSecond);
    ASSERT_EQ(onVelocityTracker.GetMainAxisPos(), 2 * LOCATION_Y);
    ASSERT_EQ(onVelocityTracker.GetMainAxisDeltaPos(), LOCATION_Y);

    // nanoseconds duration to seconds.
    const std::chrono::duration<double> duration = pointSecond.time - pointFirst.time;
    if (!NearZero(duration.count())) {
        Velocity velocity((pointSecond.GetOffset() - pointFirst.GetOffset()) / duration.count());
        ASSERT_EQ(onVelocityTracker.GetMainAxisVelocity(), velocity.GetVelocityY());
    }
}

/**
 * @tc.name: VelocityTracker002
 * @tc.desc: Verify the velocity tracker recognizes horizontal velocity.
 * @tc.type: FUNC
 * @tc.require: AR000DAPUA
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, VelocityTracker002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create horizontal velocity tracker.
     */
    VelocityTracker onVelocityTracker(Axis::HORIZONTAL);

    /**
     * @tc.steps: step2. send start point.
     * @tc.expected: step2. receive first point to calculate the velocity.
     */
    TouchEvent pointFirst {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };
    onVelocityTracker.UpdateTouchPoint(pointFirst);
    ASSERT_EQ(onVelocityTracker.GetMainAxisPos(), LOCATION_X);
    ASSERT_EQ(onVelocityTracker.GetMainAxisDeltaPos(), LOCATION_STATIC);

    /**
     * @tc.steps: step3. send end point.
     * @tc.expected: step3. receive first point to calculate the velocity.
     */
    usleep(TIME_MILLISECOND);
    TouchEvent pointSecond {
        .x = 2 * LOCATION_X, .y = LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };

    onVelocityTracker.UpdateTouchPoint(pointSecond);
    ASSERT_EQ(onVelocityTracker.GetMainAxisPos(), 2 * LOCATION_X);
    ASSERT_EQ(onVelocityTracker.GetMainAxisDeltaPos(), LOCATION_X);

    // nanoseconds duration to seconds.
    const std::chrono::duration<double> duration = pointSecond.time - pointFirst.time;
    if (!NearZero(duration.count())) {
        Velocity velocity((pointSecond.GetOffset() - pointFirst.GetOffset()) / duration.count());
        ASSERT_EQ(onVelocityTracker.GetMainAxisVelocity(), velocity.GetVelocityX());
    }
}

/**
 * @tc.name: VelocityTracker003
 * @tc.desc: Verify the velocity tracker recognizes free velocity.
 * @tc.type: FUNC
 * @tc.require: AR000DAPUA
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, VelocityTracker003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create free velocity tracker.
     */
    VelocityTracker velTracker(Axis::FREE);

    /**
     * @tc.steps: step2. send start point.
     * @tc.expected: step2. receive first point to calculate the velocity.
     */
    TouchEvent pointFirst {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };
    velTracker.UpdateTouchPoint(pointFirst);
    ASSERT_EQ(velTracker.GetMainAxisPos(), pointFirst.GetOffset().GetDistance());
    ASSERT_EQ(velTracker.GetMainAxisDeltaPos(), LOCATION_STATIC);

    /**
     * @tc.steps: step3. send end point.
     * @tc.expected: step3. receive first point to calculate the velocity.
     */
    usleep(TIME_MILLISECOND);
    TouchEvent pointSecond { .x = 2 * LOCATION_X,
        .y = 2 * LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };

    velTracker.UpdateTouchPoint(pointSecond);
    ASSERT_EQ(velTracker.GetMainAxisPos(), pointSecond.GetOffset().GetDistance());
    ASSERT_EQ(velTracker.GetMainAxisDeltaPos(), (pointSecond.GetOffset() - pointFirst.GetOffset()).GetDistance());

    // nanoseconds duration to seconds.
    const std::chrono::duration<double> duration = pointSecond.time - pointFirst.time;
    if (!NearZero(duration.count())) {
        Velocity velocity((pointSecond.GetOffset() - pointFirst.GetOffset()) / duration.count());
        ASSERT_EQ(velTracker.GetMainAxisVelocity(), velocity.GetVelocityValue());
    }
}

/**
 * @tc.name: LongPressRecognizer001
 * @tc.desc: Verify the long press recognizer recognizes corresponding long press event.
 * @tc.type: FUNC
 * @tc.require: AR000DB0UK
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, LongPressRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create long press recognizer and set long press event callback.
     */
    LongPressEventResult onLongPress;
    WeakPtr<PipelineContext> context;
    auto longPressRecognizer = AceType::MakeRefPtr<LongPressRecognizer>(context);
    longPressRecognizer->SetOnLongPress([&onLongPress](const LongPressInfo& info) {
        onLongPress.SetLongPress(true);
        onLongPress.SetLongPressInfo(info);
    });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());

    /**
     * @tc.steps: step3. end touch up event. check the touch location info.
     * @tc.expected: step3. receive touch long press callback and the touch location info is right.
     */
    point.type = TouchType::UP;
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());
}

/**
 * @tc.name: LongPressRecognizer002
 * @tc.desc: Verify the long press recognizer recognizes corresponding long press event.
 * @tc.type: FUNC
 * @tc.require: AR000DB0UK
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, LongPressRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create long press recognizer and set long press event callback.
     */
    LongPressEventResult onLongPress;
    WeakPtr<PipelineContext> context;
    auto longPressRecognizer = AceType::MakeRefPtr<LongPressRecognizer>(context);
    longPressRecognizer->SetOnLongPress([&onLongPress](const LongPressInfo& info) {
        onLongPress.SetLongPress(true);
        onLongPress.SetLongPressInfo(info);
    });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());

    /**
     * @tc.steps: step3. end touch move event. move range less than max threshold.check the touch location info.
     * @tc.expected: step3. check the long press statusis right.
     */
    point.type = TouchType::MOVE;
    point.x += (MAX_THRESHOLD - 1);

    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());

    /**
     * @tc.steps: step4. end touch move event. check the touch location info.
     * @tc.expected: step4. receive touch long press callback and check the long press status is right.
     */
    point.type = TouchType::UP;
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());
}

/**
 * @tc.name: LongPressRecognizer003
 * @tc.desc: Verify the long press recognizer recognizes corresponding long press event.
 * @tc.type: FUNC
 * @tc.require: AR000DB0UK
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, LongPressRecognizer003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create long press recognizer and set long press event callback.
     */
    LongPressEventResult onLongPress;
    WeakPtr<PipelineContext> context;
    auto longPressRecognizer = AceType::MakeRefPtr<LongPressRecognizer>(context);

    longPressRecognizer->SetOnLongPress([&onLongPress](const LongPressInfo& info) {
        onLongPress.SetLongPress(true);
        onLongPress.SetLongPressInfo(info);
    });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());

    /**
     * @tc.steps: step3. end touch move event.move range more than max threshold. check the touch location info.
     * @tc.expected: step3. check the long press statusis right.
     */
    point.type = TouchType::MOVE;
    point.x += (MAX_THRESHOLD + 1);

    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());

    /**
     * @tc.steps: step4. end touch move event. check the touch location info.
     * @tc.expected: step4. receive touch long press callback and check the long press status is right.
     */
    point.type = TouchType::UP;

    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());
}

/**
 * @tc.name: LongPressRecognizer004
 * @tc.desc: Verify the long press recognizer recognizes corresponding touch long press.
 * @tc.type: FUNC
 * @tc.require: AR000DB0UK
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, LongPressRecognizer004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create long press recognizer and set long press event callback.
     */
    LongPressEventResult onLongPress;
    WeakPtr<PipelineContext> context;
    auto longPressRecognizer = AceType::MakeRefPtr<LongPressRecognizer>(context);

    longPressRecognizer->SetOnLongPress([&onLongPress](const LongPressInfo& info) {
        onLongPress.SetLongPress(true);
        onLongPress.SetLongPressInfo(info);
    });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. touch point result is right.
     */
    TouchEvent point {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());

    /**
     * @tc.steps: step3. time wait longger than the max times(500ms).
     * @tc.expected: step3. check the long press statusis right.
     */
    usleep((TIME_COUNTS + 1) * TIME_MILLISECOND);

    /**
     * @tc.steps: step4. end touch up event. check the touch location info.
     * @tc.expected: step4. receive touch long press callback and check the long press status is right.
     */
    point.type = TouchType::UP;
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetLongPress());
}

/**
 * @tc.name: ClickRecognizer001
 * @tc.desc: Verify the click recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGG
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, ClickRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizer and set touch click event callback.
     */
    ClickEventResult onClick;
    auto clickRecognizer = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer->SetOnClick([&onClick](const ClickInfo& info) { onClick.SetClickInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent point { .id = 2,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    clickRecognizer->HandleEvent(point);
    ASSERT_TRUE(onClick.GetClickInfo().GetGlobalLocation().IsZero());

    /**
     * @tc.steps: step3. check the touch up event.
     * @tc.expected: step3. the touch location info is right.
     */
    point.type = TouchType::UP;
    clickRecognizer->HandleEvent(point);
    ASSERT_FALSE(onClick.GetClickInfo().GetGlobalLocation().IsZero());
    ASSERT_EQ(onClick.GetClickInfo().GetGlobalLocation(), point.GetOffset());
    ASSERT_EQ(onClick.GetClickInfo().GetFingerId(), point.id);
}

/**
 * @tc.name: ClickRecognizer002
 * @tc.desc: Verify the click recognizer recognizes corresponding touch down event and move and up  event. move not far
 * than MAX_THRESHOLD
 * @tc.type: FUNC
 * @tc.require: AR000DAIGG
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, ClickRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizer and set touch click event callback.
     */
    ClickEventResult onClick;
    auto clickRecognizer = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer->SetOnClick([&onClick](const ClickInfo& info) { onClick.SetClickInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is
     * right.
     */
    TouchEvent pointStart {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    clickRecognizer->HandleEvent(pointStart);
    ASSERT_TRUE(onClick.GetClickInfo().GetGlobalLocation().IsZero());

    /**
     * @tc.steps: step3. send the touch move event. move not far than max threshold
     * @tc.expected: step3. the touch location info is right.
     */
    TouchEvent pointEnd { .x = LOCATION_X + MAX_THRESHOLD,
        .y = LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };
    clickRecognizer->HandleEvent(pointEnd);
    ASSERT_TRUE(onClick.GetClickInfo().GetGlobalLocation().IsZero());

    /**
     * @tc.steps: step4. send the touch up event.
     * @tc.expected: step4. the touch location info is right.
     */
    pointEnd.type = TouchType::UP;
    clickRecognizer->HandleEvent(pointEnd);
    ASSERT_FALSE(onClick.GetClickInfo().GetGlobalLocation().IsZero());
    ASSERT_EQ(onClick.GetClickInfo().GetGlobalLocation(), pointEnd.GetOffset());
}

/**
 * @tc.name: ClickRecognizer003
 * @tc.desc: Verify the click recognizer recognizes corresponding touch down event and move and up  event. move far than
 * MAX_THRESHOLD
 * @tc.type: FUNC
 * @tc.require: AR000DAIGG
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, ClickRecognizer003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizer and set touch click event callback.
     */
    ClickEventResult onClick;
    auto clickRecognizer = AceType::MakeRefPtr<ClickRecognizer>();
    clickRecognizer->SetOnClick([&onClick](const ClickInfo& info) { onClick.SetClickInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent pointStart {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    clickRecognizer->HandleEvent(pointStart);
    ASSERT_TRUE(onClick.GetClickInfo().GetGlobalLocation().IsZero());

    /**
     * @tc.steps: step3. send the touch move event. move far than max threshold
     * @tc.expected: step3. the touch location info is right.
     */
    TouchEvent pointEnd { .x = LOCATION_X + MAX_THRESHOLD + 0.1f,
        .y = LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };
    clickRecognizer->HandleEvent(pointEnd);
    ASSERT_TRUE(onClick.GetClickInfo().GetGlobalLocation().IsZero());

    /**
     * @tc.steps: step4. send the touch up event.
     * @tc.expected: step4. the touch location info is right.
     */
    pointEnd.type = TouchType::UP;
    clickRecognizer->HandleEvent(pointEnd);
    ASSERT_TRUE(onClick.GetClickInfo().GetGlobalLocation().IsZero());
}

/**
 * @tc.name: GestureReferee001
 * @tc.desc: Verify the gesture referee corresponding gesture recognizer's referee
 * @tc.type: FUNC
 * @tc.require: AR000DAIGF
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, GestureReferee001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture arbiter.
     */
    GestureRefereeResult refereeResult;
    auto clickRecognizerA = AceType::MakeRefPtr<ClickRecognizer>();
    auto clickRecognizerB = AceType::MakeRefPtr<ClickRecognizer>();
    auto clickRecognizerC = AceType::MakeRefPtr<ClickRecognizer>();

    clickRecognizerA->SetOnClick(
        [&refereeResult](const ClickInfo& info) { refereeResult.SetGestureName("clickRecognizerA"); });
    clickRecognizerB->SetOnClick(
        [&refereeResult](const ClickInfo& info) { refereeResult.SetGestureName("clickRecognizerB"); });
    clickRecognizerC->SetOnClick(
        [&refereeResult](const ClickInfo& info) { refereeResult.SetGestureName("clickRecognizerC"); });

    int32_t eventId = 2;
    clickRecognizerA->AddToReferee(eventId, clickRecognizerA);
    clickRecognizerB->AddToReferee(eventId, clickRecognizerB);
    clickRecognizerC->AddToReferee(eventId, clickRecognizerC);

    /**
     * @tc.steps: step2. send accept to gesture arbiter.
     * @tc.expected: step2. receive event callback and check result is right.
     */
    GestureReferee::GetInstance().Adjudicate(eventId, clickRecognizerC, GestureDisposal::ACCEPT);
    GestureReferee::GetInstance().Adjudicate(eventId, clickRecognizerB, GestureDisposal::ACCEPT);
    GestureReferee::GetInstance().Adjudicate(eventId, clickRecognizerA, GestureDisposal::ACCEPT);

    ASSERT_EQ(refereeResult.GetGestureName(), std::string("clickRecognizerC"));
}

/**
 * @tc.name: GestureReferee002
 * @tc.desc: Verify the gesture arbiter corresponding gesture recognizer's arbiter
 * @tc.type: FUNC
 * @tc.require: AR000DAIGF
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, GestureReferee002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture arbiter.
     */
    GestureRefereeResult refereeResult;
    auto clickRecognizerA = AceType::MakeRefPtr<ClickRecognizer>();

    clickRecognizerA->SetOnClick(
        [&refereeResult](const ClickInfo& info) { refereeResult.SetGestureName("clickRecognizerA"); });

    int32_t eventId = 0;
    GestureReferee::GetInstance().AddGestureRecognizer(++eventId, clickRecognizerA);

    /**
     * @tc.steps: step2. send accept to gesture arbiter.
     * @tc.expected: step2. receive event callback and check result is right.
     */
    GestureReferee::GetInstance().Adjudicate(eventId--, clickRecognizerA, GestureDisposal::REJECT);

    ASSERT_TRUE(refereeResult.GetGestureName().empty());
}

/**
 * @tc.name: DragRecognizer001
 * @tc.desc: verify the drag recognizer corresponding vertical drag event
 * @tc.type: FUNC
 * @tc.require: AR000DAIGH
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, DragRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture referee.
     */
    DragEventResult onDrag;
    auto dragRecognizer = AceType::MakeRefPtr<DragRecognizer>(Axis::VERTICAL);

    dragRecognizer->SetOnDragStart([&onDrag](const DragStartInfo& info) { onDrag.SetDragStart(info); });
    dragRecognizer->SetOnDragUpdate([&onDrag](const DragUpdateInfo& info) { onDrag.SetDragUpdate(info); });
    dragRecognizer->SetOnDragEnd([&onDrag](const DragEndInfo& info) { onDrag.SetDragEnd(info); });

    /**
     * @tc.steps: step2. send down event and vertical move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent pointStart { .id = 2,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointStart);

    TouchEvent pointEnd { .id = 2,
        .x = LOCATION_X,
        .y = 2 * LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointEnd);

    ASSERT_EQ(onDrag.GetDragStart().GetGlobalLocation(), pointStart.GetOffset());
    ASSERT_EQ(onDrag.GetDragStart().GetFingerId(), pointStart.id);
    ASSERT_EQ(onDrag.GetDragUpdate().GetGlobalLocation(), pointEnd.GetOffset());
    ASSERT_EQ(onDrag.GetDragUpdate().GetFingerId(), pointEnd.id);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    pointEnd.type = TouchType::UP;
    dragRecognizer->HandleEvent(pointEnd);

    ASSERT_EQ(onDrag.GetDragEnd().GetGlobalLocation(), pointEnd.GetOffset());
    ASSERT_EQ(onDrag.GetDragEnd().GetFingerId(), pointEnd.id);
}

/**
 * @tc.name: DragRecognizer002
 * @tc.desc: verify the drag recognizer corresponding horizontal drag event
 * @tc.type: FUNC
 * @tc.require: AR000DAIGH
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, DragRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture referee.
     */
    DragEventResult onDrag;
    auto dragRecognizer = AceType::MakeRefPtr<DragRecognizer>(Axis::HORIZONTAL);

    dragRecognizer->SetOnDragStart([&onDrag](const DragStartInfo& info) { onDrag.SetDragStart(info); });
    dragRecognizer->SetOnDragUpdate([&onDrag](const DragUpdateInfo& info) { onDrag.SetDragUpdate(info); });
    dragRecognizer->SetOnDragEnd([&onDrag](const DragEndInfo& info) { onDrag.SetDragEnd(info); });

    /**
     * @tc.steps: step2. send down event and horizontal move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent pointStart { .id = 2,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointStart);

    TouchEvent pointEnd { .id = 2,
        .x = 2 * LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointEnd);

    ASSERT_EQ(onDrag.GetDragStart().GetGlobalLocation(), pointStart.GetOffset());
    ASSERT_EQ(onDrag.GetDragStart().GetFingerId(), pointStart.id);
    ASSERT_EQ(onDrag.GetDragUpdate().GetGlobalLocation(), pointEnd.GetOffset());
    ASSERT_EQ(onDrag.GetDragUpdate().GetFingerId(), pointEnd.id);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    pointEnd.type = TouchType::UP;
    dragRecognizer->HandleEvent(pointEnd);

    ASSERT_EQ(onDrag.GetDragEnd().GetGlobalLocation(), pointEnd.GetOffset());
    ASSERT_EQ(onDrag.GetDragEnd().GetFingerId(), pointEnd.id);
}

/**
 * @tc.name: DragRecognizer003
 * @tc.desc: verify the drag recognizer corresponding free drag event
 * @tc.type: FUNC
 * @tc.require: AR000DAIGH
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, DragRecognizer003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture referee.
     */
    DragEventResult onDrag;
    auto dragRecognizer = AceType::MakeRefPtr<DragRecognizer>(Axis::FREE);

    dragRecognizer->SetOnDragStart([&onDrag](const DragStartInfo& info) { onDrag.SetDragStart(info); });
    dragRecognizer->SetOnDragUpdate([&onDrag](const DragUpdateInfo& info) { onDrag.SetDragUpdate(info); });
    dragRecognizer->SetOnDragEnd([&onDrag](const DragEndInfo& info) { onDrag.SetDragEnd(info); });

    /**
     * @tc.steps: step2. send down event and free move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent pointStart { .id = 2,
        .x = 2 * LOCATION_X,
        .y = 2 * LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointStart);

    TouchEvent pointEnd { .id = 2,
        .x = 2 * LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointEnd);

    ASSERT_EQ(onDrag.GetDragStart().GetGlobalLocation(), pointStart.GetOffset());
    ASSERT_EQ(onDrag.GetDragStart().GetFingerId(), pointStart.id);
    ASSERT_EQ(onDrag.GetDragUpdate().GetGlobalLocation(), pointEnd.GetOffset());
    ASSERT_EQ(onDrag.GetDragUpdate().GetFingerId(), pointEnd.id);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    pointEnd.type = TouchType::UP;
    dragRecognizer->HandleEvent(pointEnd);

    ASSERT_EQ(onDrag.GetDragEnd().GetGlobalLocation(), pointEnd.GetOffset());
    ASSERT_EQ(onDrag.GetDragEnd().GetFingerId(), pointEnd.id);
}

/**
 * @tc.name: DragRecognizer004
 * @tc.desc: verify the drag recognizer corresponding drag  cancel event
 * @tc.type: FUNC
 * @tc.require: AR000DAIGH
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, DragRecognizer004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture referee.
     */
    DragEventResult onDrag;
    auto dragRecognizer = AceType::MakeRefPtr<DragRecognizer>(Axis::HORIZONTAL);

    dragRecognizer->SetOnDragStart([&onDrag](const DragStartInfo& info) { onDrag.SetDragStart(info); });
    dragRecognizer->SetOnDragUpdate([&onDrag](const DragUpdateInfo& info) { onDrag.SetDragUpdate(info); });
    dragRecognizer->SetOnDragEnd([&onDrag](const DragEndInfo& info) { onDrag.SetDragEnd(info); });
    dragRecognizer->SetOnDragCancel([&onDrag]() { onDrag.SetDragCancel(true); });

    /**
     * @tc.steps: step2. send down event and move event.
     * @tc.expected: step2. receive event callback and check result is right.
     */
    TouchEvent pointStart { .id = 2,
        .x = 2 * LOCATION_X,
        .y = 2 * LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointStart);

    pointStart.type = TouchType::CANCEL;
    dragRecognizer->HandleEvent(pointStart);

    ASSERT_FALSE(onDrag.GetDragCancel());
    ASSERT_TRUE(onDrag.GetDragStart().GetGlobalLocation().IsZero());
    ASSERT_TRUE(onDrag.GetDragUpdate().GetGlobalLocation().IsZero());
    ASSERT_TRUE(onDrag.GetDragEnd().GetGlobalLocation().IsZero());
}

/**
 * @tc.name: DragRecognizer005
 * @tc.desc: verify the drag recognizer corresponding drag move and cancel event
 * @tc.type: FUNC
 * @tc.require: AR000DAIGH
 * @tc.author: huye
 */
HWTEST_F(GesturesTest, DragRecognizer005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create click recognizers and register to gesture referee.
     */
    DragEventResult onDrag;
    auto dragRecognizer = AceType::MakeRefPtr<DragRecognizer>(Axis::VERTICAL);

    dragRecognizer->SetOnDragStart([&onDrag](const DragStartInfo& info) { onDrag.SetDragStart(info); });
    dragRecognizer->SetOnDragUpdate([&onDrag](const DragUpdateInfo& info) { onDrag.SetDragUpdate(info); });
    dragRecognizer->SetOnDragEnd([&onDrag](const DragEndInfo& info) { onDrag.SetDragEnd(info); });
    dragRecognizer->SetOnDragCancel([&onDrag]() { onDrag.SetDragCancel(true); });

    /**
     * @tc.steps: step2. send down event and cancel event.
     * @tc.expected: step2. receive event callback and check result is right.
     */
    TouchEvent pointStart { .id = 2,
        .x = 2 * LOCATION_X,
        .y = 2 * LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    dragRecognizer->HandleEvent(pointStart);

    TouchEvent pointEnd { .id = 2,
        .x = 2 * LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::MOVE,
        .time = std::chrono::high_resolution_clock::now() };

    dragRecognizer->HandleEvent(pointEnd);
    ASSERT_EQ(onDrag.GetDragStart().GetGlobalLocation().GetY(), pointStart.GetOffset().GetY());
    ASSERT_EQ(onDrag.GetDragStart().GetFingerId(), pointStart.id);
    ASSERT_EQ(onDrag.GetDragUpdate().GetGlobalLocation().GetY(), pointEnd.GetOffset().GetY());
    ASSERT_EQ(onDrag.GetDragUpdate().GetFingerId(), pointEnd.id);
    ASSERT_FALSE(onDrag.GetDragCancel());

    /**
     * @tc.steps: step3. send cancel event.
     * @tc.expected: step3. receive event callback and check result is right.
     */
    pointEnd.type = TouchType::CANCEL;
    dragRecognizer->HandleEvent(pointEnd);
    ASSERT_TRUE(onDrag.GetDragCancel());
}

/**
 * @tc.name: TapRecognizer001
 * @tc.desc: Verify the tap recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK51
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, TapRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create tap recognizer and set touch tap event callback.
     * Test one-finger one-tap.
     */
    TapEventResult onTap;
    WeakPtr<PipelineContext> context;
    int32_t fingers = 1;
    int32_t count = 1;
    auto tapRecognizer = AceType::MakeRefPtr<ClickRecognizer>(context, fingers, count);
    tapRecognizer->SetOnAction([&onTap](const GestureEvent& info) { onTap.SetTapInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);

    /**
     * @tc.steps: step3. check the touch up event.
     * @tc.expected: step3. the touch timestamp info is right.
     */
    point.type = TouchType::UP;
    tapRecognizer->HandleEvent(point);
    ASSERT_EQ(onTap.GetTapInfo().GetTimeStamp(), point.time);
}

/**
 * @tc.name: TapRecognizer002
 * @tc.desc: Verify the tap recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK51
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, TapRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create tap recognizer and set touch tap event callback.
     * Test one-finger double-tap.
     */
    TapEventResult onTap;
    WeakPtr<PipelineContext> context;
    int32_t fingers = 1;
    int32_t count = 2;
    auto tapRecognizer = AceType::MakeRefPtr<ClickRecognizer>(context, fingers, count);
    tapRecognizer->SetOnAction([&onTap](const GestureEvent& info) { onTap.SetTapInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);

    /**
     * @tc.steps: step3. check the touch up event.
     * @tc.expected: step3. the touch timestamp info is right.
     */
    point.type = TouchType::UP;
    tapRecognizer->HandleEvent(point);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);

    /**
     * @tc.steps: step4. send touch down event.
     * @tc.expected: step4. receive touch down callback and touch point result is right.
     */
    TouchEvent point1 { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point1);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point1.time);

    /**
     * @tc.steps: step5. check the touch up event.
     * @tc.expected: step5. the touch timestamp info is right.
     */
    point1.type = TouchType::UP;
    tapRecognizer->HandleEvent(point1);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);
    ASSERT_EQ(onTap.GetTapInfo().GetTimeStamp(), point1.time);
}

/**
 * @tc.name: TapRecognizer003
 * @tc.desc: Verify the tap recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK51
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, TapRecognizer003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create tap recognizer and set touch tap event callback.
     * Test double-finger one-tap.
     */
    TapEventResult onTap;
    WeakPtr<PipelineContext> context;
    int32_t fingers = 2;
    int32_t count = 1;
    auto tapRecognizer = AceType::MakeRefPtr<ClickRecognizer>(context, fingers, count);
    tapRecognizer->SetOnAction([&onTap](const GestureEvent& info) { onTap.SetTapInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point1);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point1.time);

    /**
     * @tc.steps: step3. check the touch up event.
     * @tc.expected: step3. the touch timestamp info is right.
     */
    point.type = TouchType::UP;
    tapRecognizer->HandleEvent(point);
    point1.type = TouchType::UP;
    tapRecognizer->HandleEvent(point1);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);
    ASSERT_EQ(onTap.GetTapInfo().GetTimeStamp(), point1.time);
}

/**
 * @tc.name: TapRecognizer004
 * @tc.desc: Verify the tap recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK51
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, TapRecognizer004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create tap recognizer and set touch tap event callback.
     * Test double-finger double-tap.
     */
    TapEventResult onTap;
    WeakPtr<PipelineContext> context;
    int32_t fingers = 2;
    int32_t count = 2;
    auto tapRecognizer = AceType::MakeRefPtr<ClickRecognizer>(context, fingers, count);
    tapRecognizer->SetOnAction([&onTap](const GestureEvent& info) { onTap.SetTapInfo(info); });

    /**
     * @tc.steps: step2. send touch down event.
     * @tc.expected: step2. receive touch down callback and touch point result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point1);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point1.time);

    /**
     * @tc.steps: step3. check the touch up event.
     * @tc.expected: step3. the touch timestamp info is right.
     */
    point.type = TouchType::UP;
    tapRecognizer->HandleEvent(point);
    point1.type = TouchType::UP;
    tapRecognizer->HandleEvent(point1);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point.time);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point1.time);

    /**
     * @tc.steps: step4. send touch down event.
     * @tc.expected: step4. receive touch down callback and touch point result is right.
     */
    TouchEvent point2 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point2);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point2.time);

    TouchEvent point3 { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };

    tapRecognizer->HandleEvent(point3);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point3.time);

    /**
     * @tc.steps: step5. check the touch up event.
     * @tc.expected: step5. the touch timestamp info is right.
     */
    point2.type = TouchType::UP;
    tapRecognizer->HandleEvent(point2);
    point3.type = TouchType::UP;
    tapRecognizer->HandleEvent(point3);
    ASSERT_NE(onTap.GetTapInfo().GetTimeStamp(), point2.time);
    ASSERT_EQ(onTap.GetTapInfo().GetTimeStamp(), point3.time);
}

/**
 * @tc.name: PanRecognizer001
 * @tc.desc: Verify the pan recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK59
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PanRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pan recognizer and set touch pan event callback.
     * Test one-finger pan, direction is all.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PanEventResult onPan;
    int32_t fingers = 1;
    PanDirection direction;
    double distance = MIN_PAN_DISTANCE;
    auto panRecognizer = AceType::MakeRefPtr<PanRecognizer>(pipelineContext, fingers, direction, distance);
    panRecognizer->SetOnActionStart([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionUpdate([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionEnd([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionCancel([&onPan]() { onPan.SetPanCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point.y);

    // onActionStart
    point.type = TouchType::MOVE;
    point.x += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    // onActionUpdate
    point.x += 200;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 200 + MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 200 + MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onPan.SetPanCancel(true);
    point.x *= 2;
    point.y *= 2;
    panRecognizer->HandleEvent(point);
    ASSERT_TRUE(onPan.GetPanCancel());
}

/**
 * @tc.name: PanRecognizer002
 * @tc.desc: Verify the pan recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK59
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PanRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pan recognizer and set touch pan event callback.
     * Test one-finger pan, direction is horizontal.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PanEventResult onPan;
    int32_t fingers = 1;
    PanDirection direction;
    direction.type = PanDirection::HORIZONTAL;
    double distance = MIN_PAN_DISTANCE;
    auto panRecognizer = AceType::MakeRefPtr<PanRecognizer>(pipelineContext, fingers, direction, distance);
    panRecognizer->SetOnActionStart([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionUpdate([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionEnd([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionCancel([&onPan]() { onPan.SetPanCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point.y);

    // onActionStart
    point.type = TouchType::MOVE;
    point.x += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    // onActionUpdate
    point.y += 200;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onPan.SetPanCancel(true);
    point.x *= 2;
    point.y *= 2;
    panRecognizer->HandleEvent(point);
    ASSERT_TRUE(onPan.GetPanCancel());
}

/**
 * @tc.name: PanRecognizer003
 * @tc.desc: Verify the pan recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK59
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PanRecognizer003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pan recognizer and set touch pan event callback.
     * Test one-finger pan, direction is vertical.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PanEventResult onPan;
    int32_t fingers = 1;
    PanDirection direction;
    direction.type = PanDirection::VERTICAL;
    double distance = MIN_PAN_DISTANCE;
    auto panRecognizer = AceType::MakeRefPtr<PanRecognizer>(pipelineContext, fingers, direction, distance);
    panRecognizer->SetOnActionStart([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionUpdate([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionEnd([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionCancel([&onPan]() { onPan.SetPanCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point.y);

    // onActionStart
    point.type = TouchType::MOVE;
    point.y += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 0);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), MIN_PAN_DISTANCE * 4);

    // onActionUpdate
    point.x += 200;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 0);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), MIN_PAN_DISTANCE * 4);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 0);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), MIN_PAN_DISTANCE * 4);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onPan.SetPanCancel(true);
    point.x *= 2;
    point.y *= 2;
    panRecognizer->HandleEvent(point);
    ASSERT_TRUE(onPan.GetPanCancel());
}

/**
 * @tc.name: PanRecognizer004
 * @tc.desc: Verify the pan recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK59
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PanRecognizer004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pan recognizer and set touch pan event callback.
     * Test double-finger pan, direction is all.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PanEventResult onPan;
    int32_t fingers = 2;
    PanDirection direction;
    double distance = MIN_PAN_DISTANCE;
    auto panRecognizer = AceType::MakeRefPtr<PanRecognizer>(pipelineContext, fingers, direction, distance);
    panRecognizer->SetOnActionStart([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionUpdate([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionEnd([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionCancel([&onPan]() { onPan.SetPanCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point.y);

    point.type = TouchType::MOVE;
    point.x += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point1);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point1.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point1.y);

    // onActionStart
    point1.type = TouchType::MOVE;
    point1.x += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    // onActionUpdate
    point.x += 200;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 100 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 100 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    point1.type = TouchType::UP;
    panRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 100 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onPan.SetPanCancel(true);
    point.x *= 2;
    point.y *= 2;
    panRecognizer->HandleEvent(point);
    ASSERT_TRUE(onPan.GetPanCancel());
}

/**
 * @tc.name: PanRecognizer005
 * @tc.desc: Verify the pan recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK59
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PanRecognizer005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pan recognizer and set touch pan event callback.
     * Set to one-finger pan gesture, but actually we use a double-finger pan gesture.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PanEventResult onPan;
    int32_t fingers = 1;
    PanDirection direction;
    double distance = MIN_PAN_DISTANCE;
    auto panRecognizer = AceType::MakeRefPtr<PanRecognizer>(pipelineContext, fingers, direction, distance);
    panRecognizer->SetOnActionStart([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionUpdate([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionEnd([&onPan](const GestureEvent& info) { onPan.SetPanInfo(info); });
    panRecognizer->SetOnActionCancel([&onPan]() { onPan.SetPanCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point.y);

    point.type = TouchType::MOVE;
    point.x += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    panRecognizer->HandleEvent(point1);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetX(), point1.x);
    ASSERT_NE(onPan.GetPanInfo().GetOffsetY(), point1.y);

    // onActionStart
    point1.type = TouchType::MOVE;
    point1.x += (MIN_PAN_DISTANCE * 4);
    panRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), MIN_PAN_DISTANCE * 4 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    // onActionUpdate
    point.x += 200;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 100 + MIN_PAN_DISTANCE * 4 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    point1.type = TouchType::UP;
    panRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 100 + MIN_PAN_DISTANCE * 4 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    point.x += 200;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 300 + MIN_PAN_DISTANCE * 4 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    panRecognizer->HandleEvent(point);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetX(), 300 + MIN_PAN_DISTANCE * 4 + MIN_PAN_DISTANCE * 2);
    ASSERT_EQ(onPan.GetPanInfo().GetOffsetY(), 0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onPan.SetPanCancel(true);
    point.x *= 2;
    point.y *= 2;
    panRecognizer->HandleEvent(point);
    ASSERT_TRUE(onPan.GetPanCancel());
}

/**
 * @tc.name: PinchRecognizer001
 * @tc.desc: Verify the pinch recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK5D
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PinchRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pinch recognizer and set touch pinch event callback.
     * Test double-finger pinch.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PinchEventResult onPinch;
    int32_t fingers = 2;
    double distance = MIN_PINCH_DISTANCE;
    auto pinchRecognizer = AceType::MakeRefPtr<PinchRecognizer>(fingers, distance);
    pinchRecognizer->SetOnActionStart([&onPinch](const GestureEvent& info) { onPinch.SetPinchInfo(info); });
    pinchRecognizer->SetOnActionUpdate([&onPinch](const GestureEvent& info) { onPinch.SetPinchInfo(info); });
    pinchRecognizer->SetOnActionEnd([&onPinch](const GestureEvent& info) { onPinch.SetPinchInfo(info); });
    pinchRecognizer->SetOnActionCancel([&onPinch]() { onPinch.SetPinchCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    pinchRecognizer->HandleEvent(point);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    pinchRecognizer->HandleEvent(point1);

    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 1.0);

    // onActionStart
    point.type = TouchType::MOVE;
    point.x -= 200;
    point.y += 200;
    pinchRecognizer->HandleEvent(point);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 2.0);

    point1.type = TouchType::MOVE;
    point1.x += 200;
    point1.y -= 200;
    pinchRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 3.0);

    // onActionUpdate
    point.x -= 100;
    point.y += 100;
    point1.x += 100;
    point1.y -= 100;
    pinchRecognizer->HandleEvent(point);
    pinchRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 4.0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    point1.type = TouchType::UP;
    pinchRecognizer->HandleEvent(point);
    pinchRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 4.0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    point1.type = TouchType::CANCEL;
    onPinch.SetPinchCancel(true);
    pinchRecognizer->HandleEvent(point);
    pinchRecognizer->HandleEvent(point1);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 4.0);
    ASSERT_TRUE(onPinch.GetPinchCancel());
}

/**
 * @tc.name: PinchRecognizer002
 * @tc.desc: Verify the pinch recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK5D
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, PinchRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create pinch recognizer and set touch pinch event callback.
     * Test four-finger pinch.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    PinchEventResult onPinch;
    int32_t fingers = 4;
    double distance = MIN_PINCH_DISTANCE;
    auto pinchRecognizer = AceType::MakeRefPtr<PinchRecognizer>(fingers, distance);
    pinchRecognizer->SetOnActionStart([&onPinch](const GestureEvent& info) { onPinch.SetPinchInfo(info); });
    pinchRecognizer->SetOnActionUpdate([&onPinch](const GestureEvent& info) { onPinch.SetPinchInfo(info); });
    pinchRecognizer->SetOnActionEnd([&onPinch](const GestureEvent& info) { onPinch.SetPinchInfo(info); });
    pinchRecognizer->SetOnActionCancel([&onPinch]() { onPinch.SetPinchCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    pinchRecognizer->HandleEvent(point);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    pinchRecognizer->HandleEvent(point1);

    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 1.0);

    point.type = TouchType::MOVE;
    point.x -= 200;
    point.y += 200;
    pinchRecognizer->HandleEvent(point);
    ASSERT_NE(onPinch.GetPinchInfo().GetScale(), 2.0);

    point1.type = TouchType::MOVE;
    point1.x += 200;
    point1.y -= 200;
    pinchRecognizer->HandleEvent(point1);
    ASSERT_NE(onPinch.GetPinchInfo().GetScale(), 3.0);

    TouchEvent point2 { .id = 2,
        .x = LOCATION_X2,
        .y = LOCATION_Y2,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    pinchRecognizer->HandleEvent(point2);

    TouchEvent point3 {
        .id = 3, .x = 0, .y = 0, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    pinchRecognizer->HandleEvent(point3);

    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 1.0);

    // onActionStart && on ActionUpdate
    point2.type = TouchType::MOVE;
    point3.type = TouchType::MOVE;
    point.x += 150;
    point.y -= 150;
    point1.x -= 150;
    point1.y += 150;
    point2.x -= 150;
    point2.y -= 150;
    point3.x += 150;
    point3.y += 150;
    pinchRecognizer->HandleEvent(point);
    pinchRecognizer->HandleEvent(point1);
    pinchRecognizer->HandleEvent(point2);
    pinchRecognizer->HandleEvent(point3);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 0.5);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    point1.type = TouchType::UP;
    point2.type = TouchType::UP;
    pinchRecognizer->HandleEvent(point);
    pinchRecognizer->HandleEvent(point1);
    pinchRecognizer->HandleEvent(point2);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 0.5);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    point1.type = TouchType::CANCEL;
    point2.type = TouchType::CANCEL;
    onPinch.SetPinchCancel(true);
    pinchRecognizer->HandleEvent(point);
    pinchRecognizer->HandleEvent(point1);
    pinchRecognizer->HandleEvent(point2);
    ASSERT_EQ(onPinch.GetPinchInfo().GetScale(), 0.5);
    ASSERT_TRUE(onPinch.GetPinchCancel());
}

/**
 * @tc.name: RotationRecognizer001
 * @tc.desc: Verify the rotation recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK5H
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, RotationRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create rotation recognizer and set touch rotation event callback.
     * Test double-finger rotation.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    RotationEventResult onRotation;
    int32_t fingers = 2;
    double angle = MIN_ROTATION_ANGLE;
    auto rotationRecognizer = AceType::MakeRefPtr<RotationRecognizer>(fingers, angle);
    rotationRecognizer->SetOnActionStart([&onRotation](const GestureEvent& info) { onRotation.SetRotationInfo(info); });
    rotationRecognizer->SetOnActionUpdate(
        [&onRotation](const GestureEvent& info) { onRotation.SetRotationInfo(info); });
    rotationRecognizer->SetOnActionEnd([&onRotation](const GestureEvent& info) { onRotation.SetRotationInfo(info); });
    rotationRecognizer->SetOnActionCancel([&onRotation]() { onRotation.SetRotationCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    rotationRecognizer->HandleEvent(point);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    rotationRecognizer->HandleEvent(point1);

    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 0.0);

    // onActionStart
    point1.type = TouchType::MOVE;
    point1.y += 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 45.0);

    // onActionUpdate
    point1.y += 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    point1.type = TouchType::UP;
    rotationRecognizer->HandleEvent(point);
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    point1.type = TouchType::CANCEL;
    onRotation.SetRotationCancel(true);
    rotationRecognizer->HandleEvent(point);
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);
    ASSERT_TRUE(onRotation.GetRotationCancel());
}

/**
 * @tc.name: RotationRecognizer002
 * @tc.desc: Verify the rotation recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK5H
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, RotationRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create rotation recognizer and set touch rotation event callback.
     * Test three-finger rotation.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    RotationEventResult onRotation;
    int32_t fingers = 3;
    double angle = MIN_ROTATION_ANGLE;
    auto rotationRecognizer = AceType::MakeRefPtr<RotationRecognizer>(fingers, angle);
    rotationRecognizer->SetOnActionStart([&onRotation](const GestureEvent& info) { onRotation.SetRotationInfo(info); });
    rotationRecognizer->SetOnActionUpdate(
        [&onRotation](const GestureEvent& info) { onRotation.SetRotationInfo(info); });
    rotationRecognizer->SetOnActionEnd([&onRotation](const GestureEvent& info) { onRotation.SetRotationInfo(info); });
    rotationRecognizer->SetOnActionCancel([&onRotation]() { onRotation.SetRotationCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    rotationRecognizer->HandleEvent(point);

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    rotationRecognizer->HandleEvent(point1);

    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 0.0);

    point1.type = TouchType::MOVE;
    point1.y += 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_NE(onRotation.GetRotationInfo().GetAngle(), 45.0);

    point1.y += 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_NE(onRotation.GetRotationInfo().GetAngle(), 90.0);

    TouchEvent point2 { .id = 2,
        .x = LOCATION_X2,
        .y = LOCATION_Y2,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    rotationRecognizer->HandleEvent(point2);

    // onActionStart
    point1.type = TouchType::MOVE;
    point1.x -= 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 45.0);

    // onActionUpdate
    point1.x -= 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);

    // no effect
    point2.y += 200;
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    point1.type = TouchType::UP;
    rotationRecognizer->HandleEvent(point);
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    point1.type = TouchType::CANCEL;
    onRotation.SetRotationCancel(true);
    rotationRecognizer->HandleEvent(point);
    rotationRecognizer->HandleEvent(point1);
    ASSERT_EQ(onRotation.GetRotationInfo().GetAngle(), 90.0);
    ASSERT_TRUE(onRotation.GetRotationCancel());
}

/**
 * @tc.name: MultiLongPressRecognizer001
 * @tc.desc: Verify the long press recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK55
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, MultiLongPressRecognizer001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create long press recognizer and set touch long press event callback.
     * Test one-finger long press.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    MultiLongPressEventResult onLongPress;
    int32_t fingers = 1;
    bool repeat = true;
    double duration = DEFAULT_LONGPRESS_DURATION;
    auto longPressRecognizer = AceType::MakeRefPtr<LongPressRecognizer>(pipelineContext, duration, fingers, repeat);
    longPressRecognizer->SetOnAction(
        [&onLongPress](const GestureEvent& info) { onLongPress.SetMultiLongPressInfo(info); });
    longPressRecognizer->SetOnActionEnd(
        [&onLongPress](const GestureEvent& info) { onLongPress.SetMultiLongPressInfo(info); });
    longPressRecognizer->SetOnActionCancel([&onLongPress]() { onLongPress.SetMultiLongPressCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetMultiLongPressInfo().GetRepeat());

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetMultiLongPressInfo().GetRepeat());

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onLongPress.SetMultiLongPressCancel(true);
    longPressRecognizer->HandleEvent(point);
    ASSERT_TRUE(onLongPress.GetMultiLongPressCancel());
}

/**
 * @tc.name: MultiLongPressRecognizer002
 * @tc.desc: Verify the long press recognizer recognizes corresponding touch down event and up event.
 * @tc.type: FUNC
 * @tc.require: AR000FQK55
 * @tc.author: wangzezhen
 */
HWTEST_F(GesturesTest, MultiLongPressRecognizer002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create long press recognizer and set touch long press event callback.
     * Test one-finger long press.
     */
    std::unique_ptr<PlatformWindow> platformWindow = GestureTestUtils::CreatePlatformWindow();
    auto window = GestureTestUtils::CreateWindow(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = AceType::MakeRefPtr<Framework::MockFrontend>();

    auto pipelineContext = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);

    MultiLongPressEventResult onLongPress;
    int32_t fingers = 2;
    bool repeat = true;
    double duration = DEFAULT_LONGPRESS_DURATION;
    auto longPressRecognizer = AceType::MakeRefPtr<LongPressRecognizer>(pipelineContext, duration, fingers, repeat);
    longPressRecognizer->SetOnAction(
        [&onLongPress](const GestureEvent& info) { onLongPress.SetMultiLongPressInfo(info); });
    longPressRecognizer->SetOnActionEnd(
        [&onLongPress](const GestureEvent& info) { onLongPress.SetMultiLongPressInfo(info); });
    longPressRecognizer->SetOnActionCancel([&onLongPress]() { onLongPress.SetMultiLongPressCancel(true); });

    /**
     * @tc.steps: step2. send down event and all move event.
     * @tc.expected: step2. receive event callback and result is right.
     */
    TouchEvent point { .id = 0,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetMultiLongPressInfo().GetRepeat());

    TouchEvent point1 { .id = 1,
        .x = LOCATION_X1,
        .y = LOCATION_Y1,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    longPressRecognizer->HandleEvent(point1);
    ASSERT_FALSE(onLongPress.GetMultiLongPressInfo().GetRepeat());

    /**
     * @tc.steps: step3. send up event, check touch point result is right
     * @tc.expected: step3. touch point result is right.
     */
    // onActionEnd
    point.type = TouchType::UP;
    longPressRecognizer->HandleEvent(point);
    ASSERT_FALSE(onLongPress.GetMultiLongPressInfo().GetRepeat());

    /**
     * @tc.steps: step4. send cancel event, check touch point result is right
     * @tc.expected: step4. touch point result is right.
     */
    // onActionCancel
    point.type = TouchType::CANCEL;
    onLongPress.SetMultiLongPressCancel(true);
    longPressRecognizer->HandleEvent(point);
    ASSERT_TRUE(onLongPress.GetMultiLongPressCancel());
}

} // namespace OHOS::Ace
