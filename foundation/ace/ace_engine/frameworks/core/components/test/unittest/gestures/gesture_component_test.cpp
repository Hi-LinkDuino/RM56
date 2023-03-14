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

#include "core/components/gesture_listener/render_gesture_listener.h"
#include "core/components/touch_listener/render_touch_listener.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/raw_recognizer.h"

using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double LOCATION_X = 200;
constexpr double LOCATION_Y = 400;
constexpr double SIZE = 600;

const std::string TOUCH_DOWN_TYPE = "onTouchDown";
const std::string TOUCH_MOVE_TYPE = "onTouchMove";
const std::string TOUCH_UP_TYPE = "onTouchUp";
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

class ClickEventResult {
public:
    ClickEventResult() : clickInfo_(0) {}
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

class LongPressEventResult {
public:
    LongPressEventResult() : longPressInfo_(0) {}
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

class GestureComponentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GestureComponentTest::SetUpTestCase() {}

void GestureComponentTest::TearDownTestCase() {}

void GestureComponentTest::SetUp() {}

void GestureComponentTest::TearDown() {}

/**
 * @tc.name: TouchListener001
 * @tc.desc: Verify the touch listener component can response to touch down event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGO
 * @tc.author: yanshuifeng
 */
HWTEST_F(GestureComponentTest, TouchListener001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create touch listener component and set touch down event callback.
     */
    TouchEventResult onTouchDown(TOUCH_DOWN_TYPE);
    auto touchListener = AceType::MakeRefPtr<RenderTouchListener>();
    touchListener->SetOnTouchDown([&onTouchDown](const TouchEventInfo& info) { onTouchDown.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. set component size for touch testing.
     */
    Rect rect(0, 0, SIZE, SIZE);
    touchListener->SetPaintRect(rect);

    /**
     * @tc.steps: step3. do touch test to collect recognizer.
     * @tc.expected: step3. receive raw recognizer.
     */
    Point point { LOCATION_X, LOCATION_Y };
    TouchTestResult result;
    touchListener->TouchTest(point, point, { TouchRestrict::NONE }, result);
    ASSERT_FALSE(result.empty());

    /**
     * @tc.steps: step4. send touch down event.
     * @tc.expected: step4. receive touch down callback and touch point result is right.
     */
    TouchEvent touchDown {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::DOWN, .time = std::chrono::high_resolution_clock::now()
    };
    result.front()->HandleEvent(touchDown);
    ASSERT_FALSE(onTouchDown.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchDown.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step5. check the touch location info.
     * @tc.expected: step5. the touch location info is right.
     */
    auto& locationInfo = onTouchDown.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: TouchListener002
 * @tc.desc: Verify the touch listener component can response to touch move event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGO
 * @tc.author: huye
 */
HWTEST_F(GestureComponentTest, TouchListener002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create touch listener component and set touch move event callback.
     */
    TouchEventResult onTouchMove(TOUCH_MOVE_TYPE);
    auto touchListener = AceType::MakeRefPtr<RenderTouchListener>();
    touchListener->SetOnTouchMove([&onTouchMove](const TouchEventInfo& info) { onTouchMove.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. set component size for touch testing.
     */
    Rect rect(0, 0, SIZE, SIZE);
    touchListener->SetPaintRect(rect);

    /**
     * @tc.steps: step3. do touch test to collect recognizer.
     * @tc.expected: step3. receive raw recognizer.
     */
    Point point { LOCATION_X, LOCATION_Y };
    TouchTestResult result;
    touchListener->TouchTest(point, point, { TouchRestrict::NONE }, result);
    ASSERT_FALSE(result.empty());

    /**
     * @tc.steps: step4. send touch move event.
     * @tc.expected: step4. receive touch move callback and touch point result is right.
     */
    TouchEvent touchMove {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::MOVE, .time = std::chrono::high_resolution_clock::now()
    };
    result.front()->HandleEvent(touchMove);
    ASSERT_FALSE(onTouchMove.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchMove.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step5. check the touch location info.
     * @tc.expected: step5. the touch location info is right.
     */
    auto& locationInfo = onTouchMove.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: TouchListener003
 * @tc.desc: Verify the touch listener component can response to touch up event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGO
 * @tc.author: huye
 */
HWTEST_F(GestureComponentTest, TouchListener003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create touch listener component and set touch up event callback.
     */
    TouchEventResult onTouchUp(TOUCH_UP_TYPE);
    auto touchListener = AceType::MakeRefPtr<RenderTouchListener>();
    touchListener->SetOnTouchUp([&onTouchUp](const TouchEventInfo& info) { onTouchUp.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. set component size for touch testing.
     */
    Rect rect(0, 0, SIZE, SIZE);
    touchListener->SetPaintRect(rect);

    /**
     * @tc.steps: step3. do touch test to collect recognizer.
     * @tc.expected: step3. receive raw recognizer.
     */
    Point point { LOCATION_X, LOCATION_Y };
    TouchTestResult result;
    touchListener->TouchTest(point, point, { TouchRestrict::NONE }, result);
    ASSERT_FALSE(result.empty());

    /**
     * @tc.steps: step4. send touch up event.
     * @tc.expected: step4. receive touch move callback and touch point result is right.
     */
    TouchEvent touchMove {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::UP, .time = std::chrono::high_resolution_clock::now()
    };
    result.front()->HandleEvent(touchMove);
    ASSERT_FALSE(onTouchUp.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchUp.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step5. check the touch location info.
     * @tc.expected: step5. the touch location info is right.
     */
    auto& locationInfo = onTouchUp.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: TouchListener004
 * @tc.desc: Verify the touch listener component can response to touch cancel event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGO
 * @tc.author: huye
 */
HWTEST_F(GestureComponentTest, TouchListener004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create touch listener component and set touch cancel event callback.
     */
    TouchEventResult onTouchCancel(TOUCH_CANCEL_TYPE);
    auto touchListener = AceType::MakeRefPtr<RenderTouchListener>();
    touchListener->SetOnTouchCancel(
        [&onTouchCancel](const TouchEventInfo& info) { onTouchCancel.SetTouchEventInfo(info); });

    /**
     * @tc.steps: step2. set component size for touch testing.
     */
    Rect rect(0, 0, SIZE, SIZE);
    touchListener->SetPaintRect(rect);

    /**
     * @tc.steps: step3. do touch test to collect recognizer.
     * @tc.expected: step3. receive raw recognizer.
     */
    Point point { LOCATION_X, LOCATION_Y };
    TouchTestResult result;
    touchListener->TouchTest(point, point, { TouchRestrict::NONE }, result);
    ASSERT_FALSE(result.empty());

    /**
     * @tc.steps: step4. send touch up event.
     * @tc.expected: step4. receive touch move callback and touch point result is right.
     */
    TouchEvent touchMove {
        .x = LOCATION_X, .y = LOCATION_Y, .type = TouchType::CANCEL, .time = std::chrono::high_resolution_clock::now()
    };
    result.front()->HandleEvent(touchMove);
    ASSERT_FALSE(onTouchCancel.GetTouchEventInfo().GetTouches().empty());
    ASSERT_TRUE(onTouchCancel.GetTouchEventInfo().GetChangedTouches().empty());

    /**
     * @tc.steps: step5. check the touch location info.
     * @tc.expected: step5. the touch location info is right.
     */
    auto& locationInfo = onTouchCancel.GetTouchEventInfo().GetTouches().front();
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetX(), LOCATION_X);
    ASSERT_EQ(locationInfo.GetGlobalLocation().GetY(), LOCATION_Y);
}

/**
 * @tc.name: GestureListener001
 * @tc.desc: Verify the gesture listener component can response to touch click event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGP
 * @tc.author: huye
 */
HWTEST_F(GestureComponentTest, GestureListener001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create gesture listener component and set click event callback.
     */
    ClickEventResult onClickResult;
    auto gestureListener = AceType::MakeRefPtr<RenderGestureListener>();
    gestureListener->SetOnClickCallback([&onClickResult](const ClickInfo& info) { onClickResult.SetClickInfo(info); });

    /**
     * @tc.steps: step2. set component size for touch testing.
     */
    Rect rect(0, 0, SIZE, SIZE);
    gestureListener->SetPaintRect(rect);

    /**
     * @tc.steps: step3. do touch test to collect recognizer.
     * @tc.expected: step3. receive raw recognizer.
     */
    Point point { LOCATION_X, LOCATION_Y };
    TouchTestResult result;
    gestureListener->TouchTest(point, point, { TouchRestrict::NONE }, result);
    ASSERT_FALSE(result.empty());

    /**
     * @tc.steps: step4. send touch down and up event.
     * @tc.expected: step4. receive click callback and touch point result is right.
     */
    TouchEvent touchClick { .id = 2,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    result.front()->HandleEvent(touchClick);
    ASSERT_TRUE(onClickResult.GetClickInfo().GetGlobalLocation().IsZero());

    touchClick.type = TouchType::UP;
    result.front()->HandleEvent(touchClick);

    /**
     * @tc.steps: step5. check the click location info.
     * @tc.expected: step5. the click location info is right.
     */
    ASSERT_FALSE(onClickResult.GetClickInfo().GetGlobalLocation().IsZero());
    ASSERT_EQ(onClickResult.GetClickInfo().GetGlobalLocation(), touchClick.GetOffset());
    ASSERT_EQ(onClickResult.GetClickInfo().GetFingerId(), touchClick.id);
}

/**
 * @tc.name: GestureListener002
 * @tc.desc: Verify the gesture listener component can response to touch click event.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGP
 * @tc.author: huye
 */
HWTEST_F(GestureComponentTest, GestureListener002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create gesture listener component and set click event callback.
     */
    LongPressEventResult onLongPressResult;
    auto gestureListener = AceType::MakeRefPtr<RenderGestureListener>();
    gestureListener->SetOnLongPressCallback([&onLongPressResult](const LongPressInfo& info) {
        onLongPressResult.SetLongPress(true);
        onLongPressResult.SetLongPressInfo(info);
    });

    /**
     * @tc.steps: step2. set component size for touch testing.
     */
    Rect rect(0, 0, SIZE, SIZE);
    gestureListener->SetPaintRect(rect);

    /**
     * @tc.steps: step3. do touch test to collect recognizer.
     * @tc.expected: step3. receive raw recognizer.
     */
    Point point { LOCATION_X, LOCATION_Y };
    TouchTestResult result;
    gestureListener->TouchTest(point, point, { TouchRestrict::NONE }, result);
    ASSERT_FALSE(result.empty());

    /**
     * @tc.steps: step4. send touch down and up event.
     * @tc.expected: step4. would not receive long press callback except click callback .
     */
    TouchEvent touchClick { .id = 2,
        .x = LOCATION_X,
        .y = LOCATION_Y,
        .type = TouchType::DOWN,
        .time = std::chrono::high_resolution_clock::now() };
    result.front()->HandleEvent(touchClick);
    ASSERT_FALSE(onLongPressResult.GetLongPress());

    touchClick.type = TouchType::UP;
    result.front()->HandleEvent(touchClick);

    /**
     * @tc.steps: step5. check the click location info.
     * @tc.expected: step5. can't receive long press callback.
     */
    ASSERT_FALSE(onLongPressResult.GetLongPress());
}

} // namespace OHOS::Ace
