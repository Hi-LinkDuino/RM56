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

#include "flutter/lib/ui/painting/canvas.h"
#include "gtest/gtest.h"

#include "base/log/log.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/checkable/radio_group_component.h"
#include "core/components/checkable/render_checkable.h"
#include "core/components/flex/flex_component.h"
#include "core/components/test/json/json_frontend.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/rosen_render_context.h"
#define protected public
#include "core/components/checkable/render_switch.h"
#include "core/components/test/unittest/mock/mock_render_depend.h"
#include "core/components/text/render_text.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

using CheckableEventCallback = std::function<void(const std::string&, const std::string&)>;
using UpdateCheckboxCallback = std::function<void(const RefPtr<CheckboxComponent>&)>;
using UpdateSwitchCallback = std::function<void(const RefPtr<SwitchComponent>&)>;

const std::string TEST_CHANGE_EVENT_ID = "change_event_id";
constexpr int32_t SURFACE_WIDTH = 1080;
constexpr int32_t SURFACE_HEIGHT = 2244;
constexpr int32_t DRAG_DELTA_SMALL = 40;
constexpr int32_t DRAG_DELTA_LARGE = 80;
constexpr bool DRAG_LESS_THAN_HALF = true;
constexpr bool DRAG_MORE_THAN_HALF = false;
constexpr bool DRAG_FORWARD = false;
constexpr bool DRAG_BACKWARD = true;

constexpr int64_t MICROSEC_TO_NANOSEC = 1000000;
constexpr int64_t VSYNC_INTERVAL_MILLISEC = 16;

const std::string TEST_TEXT_ON = "Onn";
const std::string TEST_TEXT_OFF = "Offf";
const std::string TEST_TEXT_COLOR_ON = "#FF0000";
const std::string TEST_TEXT_COLOR_OFF = "#0000FF";
const std::string TEST_TEXT_FONT_FAMILY = "serif";
constexpr FontWeight TEST_TEXT_FONT_WEIGHT = FontWeight::W400;
constexpr FontStyle TEST_TEXT_FONT_STYLE = FontStyle::NORMAL;
constexpr TextDecoration TEST_TEXT_DECORATION = TextDecoration::NONE;
const double TEST_TEXT_PADDING = 12;
constexpr double TEST_TEXT_FONT_SIZE = 30.0;
constexpr Dimension TEST_TEXT_LETTER_SPACING = 10.0_px;
constexpr double TEST_TEXT_SIZE = 200.0;
constexpr bool SHOW_TEXT = true;
const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();

const TouchEvent MOCK_DOWN_TOUCH_EVENT { 10, 10, 10, TouchType::DOWN };
const TouchEvent MOCK_MOVE_TOUCH_EVENT { 10, 10, 10, TouchType::MOVE };
const TouchEvent MOCK_UP_TOUCH_EVENT { 10, 10, 10, TouchType::UP };
const TouchEvent MOCK_DOWN_TOUCH_EVENT_AT_END { 10, 1000, 10, TouchType::DOWN };
const TouchEvent MOCK_MOVE_TOUCH_EVENT_AT_END { 10, 1000, 10, TouchType::MOVE };
enum class TestGestureType {
    CLICK = 0,
    DRAG_LESS_THAN_HALF,
    DRAG_MORE_THAN_HALF,
    DRAG_BACK_LESS_THAN_HALF,
    DRAG_BACK_MORE_THAN_HALF,
    DRAG_MORE_THAN_HALF_AND_DRAG_BACK,
    DRAG_BACK_MORE_THAN_HALF_AND_DRAG_BACK,
};

int64_t g_timeStamp = 0;

int64_t GetTickCount()
{
    return g_timeStamp;
}

class TestCheckableEventHander : public MockEventHandler {
public:
    explicit TestCheckableEventHander(const CheckableEventCallback& eventCallback) : eventCallback_(eventCallback) {}
    ~TestCheckableEventHander() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    };

private:
    const CheckableEventCallback eventCallback_;
};

} // namespace

flutter::Canvas* FlutterRenderContext::GetCanvas()
{
    return nullptr;
}

SkCanvas* RosenRenderContext::GetCanvas()
{
    return nullptr;
}

const std::shared_ptr<RSNode>& RosenRenderContext::GetRSNode()
{
    return nullptr;
}

class MockRenderSwitch final : public RenderSwitch {
    DECLARE_ACE_TYPE(MockRenderSwitch, RenderSwitch);

public:
    MockRenderSwitch() = default;
    ~MockRenderSwitch() override = default;

    Size CalculateTextSize(const std::string& text, RefPtr<RenderText>& renderText) override
    {
        return Size(TEST_TEXT_SIZE, TEST_TEXT_SIZE);
    }
};

class CheckableComponentTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "CheckableComponentTest SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "CheckableComponentTest TearDownTestCase";
    }

    void SetUp()
    {
        auto platformWindow = PlatformWindow::Create(nullptr);
        auto window = std::make_unique<Window>(std::move(platformWindow));
        auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        frontend_ = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend_, 0);
    }
    void TearDown() {}

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<Frontend> frontend_;
    void CreateCheckboxAndRender(UpdateCheckboxCallback updateCheckable);
    void CreateSwitchAndRender(UpdateSwitchCallback updateCheckable);
    void ClickCheckable();
    void MoveTouchPoint(TouchEvent& touchPoint, bool dragLessHalf, bool backward);
    void DragSwitch(const TestGestureType gestureType);
    void ApplyAction(const TestGestureType gestureType, const bool& testSuccess);
    void WaitAndMockVsync(int64_t waitFor);
    void TestCheckboxChangedEvent(bool initValue, const std::string& expectValue);
    void TestSwitchChangedEvent(bool initValue, const std::string& expectValue, const TestGestureType& gestureType);
    void TestRadioChangedEvent(bool clickStatusOn);
    void SetTextSwitchComponent(RefPtr<SwitchComponent>& switchComponent);
    void VerifyRenderSwitchUpdate(RefPtr<SwitchComponent>& switchComponent);
};

void CheckableComponentTest::VerifyRenderSwitchUpdate(RefPtr<SwitchComponent>& switchComponent)
{
    RefPtr<MockRenderSwitch> renderSwitch = AceType::MakeRefPtr<MockRenderSwitch>();
    renderSwitch->Attach(context_);
    renderSwitch->Update(switchComponent);
    EXPECT_TRUE(renderSwitch->showText_);
    EXPECT_EQ(renderSwitch->textOn_, TEST_TEXT_ON);
    EXPECT_EQ(renderSwitch->textOff_, TEST_TEXT_OFF);
    EXPECT_EQ(renderSwitch->pointTextPadding_.Value(), TEST_TEXT_PADDING);
    ASSERT_TRUE(renderSwitch->renderTextOn_);
    ASSERT_TRUE(renderSwitch->renderTextOff_);
    EXPECT_EQ(renderSwitch->renderTextOff_->textStyle_.GetTextColor(), Color::FromString(TEST_TEXT_COLOR_OFF));
    TextStyle textStyle = renderSwitch->renderTextOn_->textStyle_;
    EXPECT_EQ(textStyle.GetTextColor(), Color::FromString(TEST_TEXT_COLOR_ON));
    EXPECT_EQ(textStyle.GetFontWeight(), TEST_TEXT_FONT_WEIGHT);
    EXPECT_EQ(textStyle.GetFontStyle(), TEST_TEXT_FONT_STYLE);
    EXPECT_EQ(textStyle.GetTextDecoration(), TEST_TEXT_DECORATION);
    EXPECT_EQ(textStyle.GetFontSize().Value(), TEST_TEXT_FONT_SIZE);
    ASSERT_TRUE(NearEqual(textStyle.GetLetterSpacing().Value(), TEST_TEXT_LETTER_SPACING.Value()));
    EXPECT_EQ(textStyle.GetFontFamilies()[0], TEST_TEXT_FONT_FAMILY);
    renderSwitch->PerformLayout();
    EXPECT_EQ(renderSwitch->textOnSize_, Size(TEST_TEXT_SIZE, TEST_TEXT_SIZE));
}

void CheckableComponentTest::SetTextSwitchComponent(RefPtr<SwitchComponent>& switchComponent)
{
    switchComponent->SetShowText(SHOW_TEXT);
    switchComponent->SetTextOn(TEST_TEXT_ON);
    switchComponent->SetTextOff(TEST_TEXT_OFF);
    switchComponent->SetTextColorOn(Color::FromString(TEST_TEXT_COLOR_ON));
    switchComponent->SetTextColorOff(Color::FromString(TEST_TEXT_COLOR_OFF));
    switchComponent->SetTextPadding(Dimension(TEST_TEXT_PADDING));
    TextStyle textStyle;
    textStyle.SetFontSize(Dimension(TEST_TEXT_FONT_SIZE));
    textStyle.SetFontWeight(TEST_TEXT_FONT_WEIGHT);
    textStyle.SetFontStyle(TEST_TEXT_FONT_STYLE);
    textStyle.SetTextDecoration(TEST_TEXT_DECORATION);
    textStyle.SetLetterSpacing(TEST_TEXT_LETTER_SPACING);
    std::vector<std::string> fontFamilies = { TEST_TEXT_FONT_FAMILY };
    textStyle.SetFontFamilies(fontFamilies);
    switchComponent->SetTextStyle(textStyle);
}

void CheckableComponentTest::CreateCheckboxAndRender(UpdateCheckboxCallback updateCheckable)
{
    RefPtr<CheckboxTheme> theme = THEME_MANAGER->GetTheme<CheckboxTheme>();
    RefPtr<CheckboxComponent> checkboxComponent = AceType::MakeRefPtr<CheckboxComponent>(theme);
    context_->SetTimeProvider([] { return g_timeStamp + 10 * MICROSEC_TO_NANOSEC; });
    if (updateCheckable) {
        updateCheckable(checkboxComponent);
    }

    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", checkboxComponent);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(SURFACE_WIDTH, SURFACE_HEIGHT);
}

void CheckableComponentTest::CreateSwitchAndRender(UpdateSwitchCallback updateCheckable)
{
    RefPtr<SwitchTheme> theme = THEME_MANAGER->GetTheme<SwitchTheme>();
    RefPtr<SwitchComponent> switchComponent = AceType::MakeRefPtr<SwitchComponent>(theme);
    context_->SetTimeProvider([] { return g_timeStamp + 10 * MICROSEC_TO_NANOSEC; });

    if (updateCheckable) {
        updateCheckable(switchComponent);
    }

    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", switchComponent);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(SURFACE_WIDTH, SURFACE_HEIGHT);
}

void CheckableComponentTest::WaitAndMockVsync(int64_t waitFor)
{
    int64_t runningTime = 0;
    do {
        runningTime += VSYNC_INTERVAL_MILLISEC;
        context_->OnVsyncEvent(GetTickCount(), 0);
        g_timeStamp += VSYNC_INTERVAL_MILLISEC * MICROSEC_TO_NANOSEC;
    } while (runningTime < waitFor);
    context_->OnVsyncEvent(GetTickCount(), 0);
}

void CheckableComponentTest::ClickCheckable()
{
    context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT);
    context_->OnTouchEvent(MOCK_UP_TOUCH_EVENT);
    context_->OnVsyncEvent(GetTickCount(), 0);
}

void CheckableComponentTest::MoveTouchPoint(TouchEvent& touchPoint, bool dragLessHalf, bool backward)
{
    double moveLength = dragLessHalf ? DRAG_DELTA_SMALL : DRAG_DELTA_LARGE;
    if (backward) {
        moveLength *= -1;
    }
    for (int i = 0; i < 3; i++) {
        context_->OnTouchEvent(touchPoint);
        touchPoint.x += moveLength;
    }
}

void CheckableComponentTest::DragSwitch(const TestGestureType gestureType)
{
    context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT);
    TouchEvent touchPoint = MOCK_MOVE_TOUCH_EVENT;
    switch (gestureType) {
        case TestGestureType::DRAG_LESS_THAN_HALF:
            MoveTouchPoint(touchPoint, DRAG_LESS_THAN_HALF, DRAG_FORWARD);
            break;
        case TestGestureType::DRAG_MORE_THAN_HALF:
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_FORWARD);
            break;
        case TestGestureType::DRAG_BACK_LESS_THAN_HALF:
            context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT_AT_END);
            touchPoint = MOCK_MOVE_TOUCH_EVENT_AT_END;
            MoveTouchPoint(touchPoint, DRAG_LESS_THAN_HALF, DRAG_BACKWARD);
            break;
        case TestGestureType::DRAG_BACK_MORE_THAN_HALF:
            context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT_AT_END);
            touchPoint = MOCK_MOVE_TOUCH_EVENT_AT_END;
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_BACKWARD);
            break;
        case TestGestureType::DRAG_MORE_THAN_HALF_AND_DRAG_BACK:
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_FORWARD);
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_BACKWARD);
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_BACKWARD);
            break;
        case TestGestureType::DRAG_BACK_MORE_THAN_HALF_AND_DRAG_BACK:
            context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT_AT_END);
            touchPoint = MOCK_MOVE_TOUCH_EVENT_AT_END;
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_BACKWARD);
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_FORWARD);
            MoveTouchPoint(touchPoint, DRAG_MORE_THAN_HALF, DRAG_FORWARD);
            break;
        default:
            break;
    }
    TouchEvent touchEndPoint = MOCK_UP_TOUCH_EVENT;
    touchEndPoint.x = touchPoint.x;
    context_->OnTouchEvent(touchEndPoint);
}

void CheckableComponentTest::TestCheckboxChangedEvent(const bool initValue, const std::string& expectValue)
{
    bool testSuccess = false;
    RefPtr<TestCheckableEventHander> eventHander = AceType::MakeRefPtr<TestCheckableEventHander>(
        [this, expectValue, &testSuccess](const std::string& eventId, const std::string& param) mutable {
            std::string value = expectValue;
            std::string expectChangeResult = std::string("\"change\",{\"checked\":").append(value.append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, TEST_CHANGE_EVENT_ID);
            testSuccess = true;
        });

    context_->RegisterEventHandler(eventHander);

    CreateCheckboxAndRender([this, initValue](const RefPtr<CheckboxComponent>& checkbox) {
        EventMarker changeEvent { TEST_CHANGE_EVENT_ID };
        checkbox->SetChangeEvent(changeEvent);
        checkbox->SetValue(initValue);
    });
    ClickCheckable();
    WaitAndMockVsync(200);
    EXPECT_TRUE(testSuccess);
}

void CheckableComponentTest::TestSwitchChangedEvent(
    bool initValue, const std::string& expectValue, const TestGestureType& gestureType)
{
    bool testSuccess = false;
    RefPtr<TestCheckableEventHander> eventHander = AceType::MakeRefPtr<TestCheckableEventHander>(
        [this, expectValue, &testSuccess](const std::string& eventId, const std::string& param) mutable {
            std::string value = expectValue;
            std::string expectChangeResult = std::string("\"change\",{\"checked\":").append(value.append("},null"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, TEST_CHANGE_EVENT_ID);
            testSuccess = true;
        });

    context_->RegisterEventHandler(eventHander);

    CreateSwitchAndRender([this, initValue](const RefPtr<SwitchComponent>& switchComponent) {
        EventMarker changeEvent { TEST_CHANGE_EVENT_ID };
        switchComponent->SetChangeEvent(changeEvent);
        switchComponent->SetValue(initValue);
    });
    ApplyAction(gestureType, testSuccess);
}

void CheckableComponentTest::ApplyAction(const TestGestureType gestureType, const bool& testSuccess)
{
    switch (gestureType) {
        case TestGestureType::CLICK:
            ClickCheckable();
            WaitAndMockVsync(500);
            EXPECT_TRUE(testSuccess);
            break;
        case TestGestureType::DRAG_LESS_THAN_HALF:
            DragSwitch(TestGestureType::DRAG_LESS_THAN_HALF);
            WaitAndMockVsync(500);
            EXPECT_FALSE(testSuccess);
            break;
        case TestGestureType::DRAG_MORE_THAN_HALF:
            DragSwitch(TestGestureType::DRAG_MORE_THAN_HALF);
            WaitAndMockVsync(500);
            EXPECT_TRUE(testSuccess);
            break;
        case TestGestureType::DRAG_BACK_LESS_THAN_HALF:
            DragSwitch(TestGestureType::DRAG_BACK_LESS_THAN_HALF);
            WaitAndMockVsync(500);
            EXPECT_FALSE(testSuccess);
            break;
        case TestGestureType::DRAG_BACK_MORE_THAN_HALF:
            DragSwitch(TestGestureType::DRAG_BACK_MORE_THAN_HALF);
            WaitAndMockVsync(500);
            EXPECT_TRUE(testSuccess);
            break;
        case TestGestureType::DRAG_MORE_THAN_HALF_AND_DRAG_BACK:
            DragSwitch(TestGestureType::DRAG_MORE_THAN_HALF_AND_DRAG_BACK);
            WaitAndMockVsync(500);
            EXPECT_FALSE(testSuccess);
            break;
        case TestGestureType::DRAG_BACK_MORE_THAN_HALF_AND_DRAG_BACK:
            DragSwitch(TestGestureType::DRAG_BACK_MORE_THAN_HALF_AND_DRAG_BACK);
            WaitAndMockVsync(500);
            EXPECT_FALSE(testSuccess);
            break;
        default:
            break;
    }
}

void CheckableComponentTest::TestRadioChangedEvent(bool clickStatusOn)
{
    // [radioSelected] indicates whether the radio whose [value] is "red" being selected.
    // [radioSelected] only changes via [ChangeEvent] triggered by radio whose [value] is "red".
    bool radioSelected = false;

    RefPtr<TestCheckableEventHander> eventHander = AceType::MakeRefPtr<TestCheckableEventHander>(
        [this, &radioSelected](const std::string& eventId, const std::string& param) mutable {
            if (param.find("\"checked\":true") == std::string::npos) {
                return;
            }
            std::string checked = "true";
            std::string value = "red";
            std::string expectChangeResult = std::string("\"change\",{\"checked\":")
                                                 .append(checked)
                                                 .append(",\"value\":\"")
                                                 .append(value)
                                                 .append("\"},null");
            EXPECT_EQ(eventId, TEST_CHANGE_EVENT_ID);
            // if [ChangeEvent] triggers, [radioSelected] must be true, considering radio with value "red" is clicked.
            radioSelected = (param == expectChangeResult);
        });

    context_->RegisterEventHandler(eventHander);

    std::list<RefPtr<OHOS::Ace::Component>> children;
    RadioGroupComponent<std::string> radioGroup;
    std::vector<std::string> radioValues { "red", "green", "yellow" };
    uint32_t selectRadioIndex = clickStatusOn ? 0 : radioValues.size() - 1;
    std::string groupValue = radioValues[selectRadioIndex];
    RefPtr<RadioTheme> theme = THEME_MANAGER->GetTheme<RadioTheme>();
    for (const auto& value : radioValues) {
        RefPtr<RadioComponent<std::string>> radio = AceType::MakeRefPtr<RadioComponent<std::string>>(theme);
        radio->SetValue(value);
        EventMarker changeEvent { TEST_CHANGE_EVENT_ID };
        radio->SetChangeEvent(changeEvent);
        radio->SetGroupValue(groupValue);
        radioGroup.AddRadio(radio);
        children.push_back(radio);
    }

    RefPtr<RowComponent> row =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::FLEX_START, children);
    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", row);
    context_->SetTimeProvider([] { return g_timeStamp + 10 * MICROSEC_TO_NANOSEC; });
    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(SURFACE_WIDTH, SURFACE_HEIGHT);

    ClickCheckable();
    if (clickStatusOn) {
        // case when [ChangeEvent] will not trigger hence [radioSelected] will remains its initial value [false].
        EXPECT_FALSE(radioSelected);
    } else {
        // case when [ChangeEvent] triggers hence [radioSelected] will change to [true].
        EXPECT_TRUE(radioSelected);
    }
}

/**
 * @tc.name: CheckboxRender001
 * @tc.desc: Test when the state of the checkbox component is on, after clicking, you can listen to the event that the
 * state changes to off
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, CheckboxRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest CheckboxRender001 start";

    /**
     * @tc.steps: step1. Build a component tree containing checkbox component. The initial state of checkbox is on. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. click checkbox component.
     * @tc.expected: step2. The component receives onChange event listeners whose status becomes off.
     */
    TestCheckboxChangedEvent(true, "false");
    GTEST_LOG_(INFO) << "CheckableComponentTest CheckboxRender001 stop";
}

/**
 * @tc.name: CheckboxRender002
 * @tc.desc: Test when the state of the checkbox component is off, after clicking, you can listen to the event that the
 * state changes to on
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, CheckboxRender002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest CheckboxRender002 start";

    /**
     * @tc.steps: step1. Build a component tree containing checkbox component. The initial state of checkbox is off. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. click checkbox component.
     * @tc.expected: step2. The component receives onChange event listeners whose status becomes on.
     */
    TestCheckboxChangedEvent(false, "true");
    GTEST_LOG_(INFO) << "CheckableComponentTest CheckboxRender002 stop";
}

/**
 * @tc.name: SwitchRender001
 * @tc.desc: Test when the state of the switch component is on, after clicking, you can listen to the event that the
 * state changes to off
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender001 start";

    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is on. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. click switch component.
     * @tc.expected: step2. The component receives onChange event listeners whose status becomes off.
     */
    TestSwitchChangedEvent(true, "false", TestGestureType::CLICK);
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender001 stop";
}

/**
 * @tc.name: SwitchRender002
 * @tc.desc: Test when the state of the switch component is off, after clicking, you can listen to the event that the
 * state changes to on
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender002 start";

    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is off. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. click switch component.
     * @tc.expected: step2. The component receives onChange event listeners whose status becomes on.
     */
    TestSwitchChangedEvent(false, "true", TestGestureType::CLICK);
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender002 stop";
}

/**
 * @tc.name: SwitchRender003
 * @tc.desc: Test when the switch component is off, the release state does not change when dragging forth less than
 * half of the switch
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender003 start";

    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is off. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. Let go when you drag the switch component forth less than half of the switch.
     * @tc.expected: step2. The component can not receives onChange event listeners whose status becomes on.
     */
    TestSwitchChangedEvent(false, "false", TestGestureType::DRAG_LESS_THAN_HALF);
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender003 stop";
}

/**
 * @tc.name: SwitchRender004
 * @tc.desc: Test when the switch component is off, the release state changes when dragging forth more than
 * half of the switch
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender004 start";

    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is off. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. Release when the Switch component is dragged forth more than half of the switch.
     * @tc.expected: step2. The component receives onChange event listeners whose status becomes on.
     */
    TestSwitchChangedEvent(false, "true", TestGestureType::DRAG_MORE_THAN_HALF);
    GTEST_LOG_(INFO) << "CheckableComponentTest SwitchRender004 stop";
}

/**
 * @tc.name: SwitchRender005
 * @tc.desc: Test when the switch component is on, the release state does not change when dragging back less than
 * half of the switch
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is on. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. Release when the Switch component is dragged back less than half of the switch.
     * @tc.expected: step2. The component can not receives onChange event listeners whose status becomes off.
     */
    TestSwitchChangedEvent(true, "true", TestGestureType::DRAG_BACK_LESS_THAN_HALF);
}

/**
 * @tc.name: SwitchRender006
 * @tc.desc: Test when the switch component is on, the release state changes when dragging back more than
 * half of the switch
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is on. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. Release when the Switch component is dragged back more than half of the switch.
     * @tc.expected: step2. The component receives onChange event listeners whose status becomes off.
     */
    TestSwitchChangedEvent(true, "false", TestGestureType::DRAG_BACK_MORE_THAN_HALF);
}

/**
 * @tc.name: SwitchRender007
 * @tc.desc: Test when the switch component is dragged form off to on and off, the release state does not change
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is off. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. Release when the Switch component is dragged form off to on and off
     * @tc.expected: step2. The component doesn't receive onChange event listeners whose status becomes on.
     */
    TestSwitchChangedEvent(false, "false", TestGestureType::DRAG_MORE_THAN_HALF_AND_DRAG_BACK);
}

/**
 * @tc.name: SwitchRender008
 * @tc.desc: Test when the switch component is dragged form on to off and on, the release state does not change
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, SwitchRender008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a component tree containing switch component. The initial state of switch is on. Add
     * onChange event listeners to the component.
     */
    /**
     * @tc.steps: step2. Release when the Switch component is dragged form on to off and on
     * @tc.expected: step2. The component doesn't receives onChange event listeners whose status becomes on.
     */
    TestSwitchChangedEvent(true, "true", TestGestureType::DRAG_BACK_MORE_THAN_HALF_AND_DRAG_BACK);
}

/**
 * @tc.name: RadioRender001
 * @tc.desc: Test that when the radio component status is off, after clicking, you can listen to the status change event
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, RadioRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest RadioRender001 start";
    /**
     * @tc.steps: step1. Build a component tree containing a group of radio components. Initially only one radio is
     * selected.
     */
    /**
     * @tc.steps: step2. Click on the unselected radio.
     * @tc.expected: step2. The component receives the onChange event of the selected radio value.
     */
    TestRadioChangedEvent(false);
    GTEST_LOG_(INFO) << "CheckableComponentTest RadioRender001 end";
}

/**
 * @tc.name: RadioRender002
 * @tc.desc: Test when the radio component status is on, after clicking, you cannot listen to the status change event
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, RadioRender002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckableComponentTest RadioRender002 start";
    /**
     * @tc.steps: step1. Build a component tree containing a group of radio components. Initially only one radio is
     * selected.
     */
    /**
     * @tc.steps: step2. Click on the selected radio.
     * @tc.expected: step2. Component will not receive onChange event listener.
     */
    TestRadioChangedEvent(true);
    GTEST_LOG_(INFO) << "CheckableComponentTest RadioRender002 end";
}

/**
 * @tc.name: UpdateTextSwitch001
 * @tc.desc: Test switch component can update with text attributes correctly
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, UpdateTextSwitch001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a switchComponent with certain attributes.
     * @tc.expected: step1. The switchComponent is not nullptr.
     */
    RefPtr<SwitchTheme> theme = THEME_MANAGER->GetTheme<SwitchTheme>();
    RefPtr<SwitchComponent> switchComponent = AceType::MakeRefPtr<SwitchComponent>(theme);
    EXPECT_TRUE(switchComponent);
    SetTextSwitchComponent(switchComponent);

    /**
     * @tc.steps: step2. Create a renderSwitch to update with switchComponent.
     * @tc.expected: step2. Attributes of renderSwitch are as expected.
     */
    VerifyRenderSwitchUpdate(switchComponent);
}

/**
 * @tc.name: UpdateTextSwitch002
 * @tc.desc: Test switch component can update correctly when showtext is false
 * @tc.type: FUNC
 */
HWTEST_F(CheckableComponentTest, UpdateTextSwitch002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a switchComponent with showtext being false.
     * @tc.expected: step1. The switchComponent is not nullptr.
     */
    RefPtr<SwitchTheme> theme = THEME_MANAGER->GetTheme<SwitchTheme>();
    RefPtr<SwitchComponent> switchComponent = AceType::MakeRefPtr<SwitchComponent>(theme);
    EXPECT_TRUE(switchComponent);
    switchComponent->SetShowText(false);

    /**
     * @tc.steps: step2. Create a renderSwitch to update with switchComponent.
     * @tc.expected: step2. Both renderTextOn_ and renderTextOff_ are nullptr.
     */
    RefPtr<MockRenderSwitch> renderSwitch = AceType::MakeRefPtr<MockRenderSwitch>();
    renderSwitch->Update(switchComponent);
    EXPECT_TRUE(!renderSwitch->renderTextOn_);
    EXPECT_TRUE(!renderSwitch->renderTextOff_);
}

} // namespace OHOS::Ace
