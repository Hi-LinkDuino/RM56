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

#include "gtest/gtest.h"

#include "core/common/clipboard/clipboard_proxy.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/text_field/render_text_field.h"
#include "core/components/text_field/text_field_component.h"
#include "core/components/text_field/text_field_controller.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components/text_overlay/render_text_overlay.h"
#include "core/components/text_overlay/text_overlay_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Color TEST_HANDLE_COLOR = Color(0xFF007DFF);
const Color TEST_SELECT_TEXT_COLOR = Color(0x33254FF7);
const double TEST_HANDLE_DIAMETER = 16.0;
const int64_t SEC_TO_NANOSEC = 1000000000;
const int64_t MILLISEC_TO_MICROSEC = 1000;
const int64_t VSYNC_INTERVAL_MILLISEC = 1;
const int64_t VSYNC_INTERVAL_MICROSEC = VSYNC_INTERVAL_MILLISEC * MILLISEC_TO_MICROSEC;
const int64_t WAIT_TIME = 1000;
const Offset OFFSET_FIRST_LETTER = Offset(0.0, 0.0);
const Offset OFFSET_SECOND_LETTER = Offset(10.0, 0.0);
const Offset OFFSET_THIRD_LETTER = Offset(20.0, 0.0);
const Offset OFFSET_FOURTH_LETTER = Offset(30.0, 0.0);
const Offset OFFSET_END = Offset(40.0, 0.0);

static int64_t GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * SEC_TO_NANOSEC + ts.tv_nsec);
}

} // namespace

class MockRenderTextField final : public RenderTextField {
    DECLARE_ACE_TYPE(MockRenderTextField, RenderTextField);

public:
    MockRenderTextField() = default;
    ~MockRenderTextField() override = default;

    bool IsPhone() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::PHONE;
    }

    int32_t GetTextMaxLines() const
    {
        return maxLines_;
    }

    size_t GetMaxLength() const
    {
        return maxLength_;
    }

    bool ShowCursor() const
    {
        return showCursor_;
    }

    const Color& GetCursorColor()
    {
        return cursorColor_;
    }

    const std::string& GetPlaceholder() const
    {
        return placeholder_;
    }

    const Color& GetPlaceholderColor() const
    {
        return placeholderColor_;
    }

    TextInputType GetTextInputType() const
    {
        return keyboard_;
    }

    TextInputAction GetAction() const
    {
        return action_;
    }

    TextAlign GetTextAlign() const
    {
        return textAlign_;
    }

    bool IsEnabled() const
    {
        return enabled_;
    }

    bool IsExtend() const
    {
        return extend_;
    }

    int32_t GetCursorPositionForMoveUp() override
    {
        return 0;
    }

    int32_t GetCursorPositionForMoveDown() override
    {
        return 0;
    }

    int32_t GetCursorPositionForClick(const Offset& offset) override
    {
        if (offset == OFFSET_FIRST_LETTER) {
            return 0;
        } else if (offset == OFFSET_SECOND_LETTER) {
            return 1;
        } else if (offset == OFFSET_THIRD_LETTER) {
            return 2;
        } else if (offset == OFFSET_FOURTH_LETTER) {
            return 3;
        } else if (offset == OFFSET_END) {
            return 4;
        } else {
            return 0;
        }
    }

    double PreferredLineHeight() override
    {
        return 0.0;
    }

    int32_t AdjustCursorAndSelection(int32_t currentCursorPosition) override
    {
        return 0;
    }

    DirectionStatus GetDirectionStatusOfPosition(int32_t position) const override
    {
        return DirectionStatus::LEFT_LEFT;
    }

    bool GetCaretRect(int32_t extent, Rect& caretRect, double caretHeightOffset = 0.0) const override
    {
        return false;
    }

    Offset GetHandleOffset(int32_t extend) override
    {
        return Offset();
    }

    Size ComputeDeflateSizeOfErrorAndCountText() const override
    {
        return Size();
    }

    const Color& GetSelectedColor() const
    {
        return selectedColor_;
    }

    void AppendText()
    {
        TextEditingValue textEditingValue;
        textEditingValue.text = "TEXT";
        textEditingValue.UpdateSelection(0, 0);
        SetEditingValue(std::move(textEditingValue));
    }

    void ClearText()
    {
        TextEditingValue emptyValue;
        SetEditingValue(std::move(emptyValue));
    }

    const TextEditingValue& GetText() const
    {
        return GetEditingValue();
    }

    void HandleLongPress(const LongPressInfo& longPressInfo)
    {
        OnLongPress(longPressInfo);
    }

    void HandleClick(const ClickInfo& clickInfo)
    {
        OnClick(clickInfo);
    }
};

class MockRenderTextOverlay final : public RenderTextOverlay {
    DECLARE_ACE_TYPE(MockRenderTextOverlay, RenderTextOverlay);

public:
    MockRenderTextOverlay() = default;
    ~MockRenderTextOverlay() override = default;

    const Dimension& GetHandleDiameter() const
    {
        return handleDiameter_;
    }

    const Dimension& GetHandleRadius() const
    {
        return handleRadius_;
    }

    const Color& GetHandleColor() const
    {
        return handleColor_;
    }
};

class MockClipboardJni final {
public:
    static bool MockGetData(
        const std::function<void(const std::string&)>& callback, const WeakPtr<TaskExecutor>& taskExecutor)
    {
        auto executor = taskExecutor.Upgrade();
        if (executor) {
            executor->PostTask(
                [callback] {
                    if (callback) {
                        callback("");
                    }
                },
                TaskExecutor::TaskType::BACKGROUND);
        }
        return true;
    };

private:
    MockClipboardJni() = delete;
    ~MockClipboardJni() = delete;
};

class MockClipboardImpl : public Clipboard {
public:
    explicit MockClipboardImpl(const RefPtr<TaskExecutor>& taskExecutor);
    ~MockClipboardImpl() override = default;

    // Implement Clipboard
    void SetData(const std::string& data) override {};
    void GetData(const std::function<void(const std::string&)>& callback) override
    {
        if (taskExecutor_) {
            taskExecutor_->PostTask(
                [callback, taskExecutor = WeakClaim(RawPtr(taskExecutor_))] {
                    MockClipboardJni::MockGetData(callback, taskExecutor);
                },
                TaskExecutor::TaskType::BACKGROUND);
        }
    };
    void Clear() override {};
};

MockClipboardImpl::MockClipboardImpl(const RefPtr<TaskExecutor>& taskExecutor) : Clipboard(taskExecutor) {}

class MockClipboardProxyImpl final : public ClipboardInterface {
public:
    MockClipboardProxyImpl() = default;
    ~MockClipboardProxyImpl() = default;

    // Implement ClipboardInterface
    RefPtr<Clipboard> GetClipboard(const RefPtr<TaskExecutor>& taskExecutor) const override
    {
        return AceType::MakeRefPtr<MockClipboardImpl>(taskExecutor);
    };
};

class RenderTextSelectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void WaitAndMockVsync(int64_t waitFor);
    RefPtr<PipelineContext> context_;
};

void RenderTextSelectTest::SetUpTestCase() {}
void RenderTextSelectTest::TearDownTestCase() {}
void RenderTextSelectTest::TearDown()
{
    context_ = nullptr;
}
void RenderTextSelectTest::SetUp()
{
    context_ = MockRenderCommon::GetMockContext();
    ClipboardProxy::GetInstance()->SetDelegate(std::make_unique<MockClipboardProxyImpl>());
}

void RenderTextSelectTest::WaitAndMockVsync(int64_t waitFor)
{
    int64_t runningTime = 0;
    while (runningTime <= waitFor) {
        runningTime += VSYNC_INTERVAL_MILLISEC;
        context_->OnVsyncEvent(GetTickCount(), 0);
        usleep(VSYNC_INTERVAL_MICROSEC);
    }
    context_->OnVsyncEvent(GetTickCount(), 0);
}

/**
 * @tc.name: RenderTextSelectTest001
 * @tc.desc: Verify the Update Interface of RenderTextSelect.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCI
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTextSelectTest, RenderTextSelectTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextOverlayComponent and RenderTextOverlay.
     * @tc.expected: step1. RenderTextOverlay is setted as theme
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<TextOverlayComponent> textOverlayComponent =
        AceType::MakeRefPtr<TextOverlayComponent>(themeManager, context_->GetAccessibilityManager());
    RefPtr<MockRenderTextOverlay> renderTextOverlay = AceType::MakeRefPtr<MockRenderTextOverlay>();
    renderTextOverlay->Update(textOverlayComponent);

    EXPECT_EQ(renderTextOverlay->GetHandleDiameter().Value(), TEST_HANDLE_DIAMETER);
    EXPECT_EQ(renderTextOverlay->GetHandleColor(), TEST_HANDLE_COLOR);
}

/**
 * @tc.name: RenderTextSelectTest002
 * @tc.desc: Verify the Update Interface of RenderTextSelect.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCM
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTextSelectTest, RenderTextSelectTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct textfield and get the theme.
     * @tc.expected: step1. theme is correct.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<TextFieldTheme> textFieldTheme = themeManager->GetTheme<TextFieldTheme>();
    ASSERT_TRUE(textFieldTheme);

    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetSelectedColor(textFieldTheme->GetSelectedColor());
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is setted as theme
     */
    renderTextField->Update(textField);
    EXPECT_EQ(renderTextField->GetSelectedColor(), TEST_SELECT_TEXT_COLOR);
}

/**
 * @tc.name: RenderTextSelectTest003
 * @tc.desc: Verify the selection of RenderTextSelect.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCJ
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTextSelectTest, RenderTextSelectTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct textfield and get the theme.
     * @tc.expected: step1. theme is correct.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<TextFieldTheme> textFieldTheme = themeManager->GetTheme<TextFieldTheme>();
    ASSERT_TRUE(textFieldTheme);

    // Add the component
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetSelectedColor(textFieldTheme->GetSelectedColor());
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    // Update the component
    context_->SetupRootElement();
    context_->OnVsyncEvent(GetTickCount(), 0);
    renderTextField->Attach(context_);
    renderTextField->Update(textField);
    renderTextField->AppendText();

    if (renderTextField->IsPhone()) {
        /**
         * @tc.steps: step2. construct longpress the first letter.
         * @tc.expected: step2. the text is selected from 0 to 1.
         */
        LongPressInfo longPressInfo(0);
        longPressInfo.SetGlobalLocation(OFFSET_FIRST_LETTER);
        renderTextField->HandleLongPress(longPressInfo);

        EXPECT_EQ(renderTextField->GetText().selection.GetStart(), 0);
        EXPECT_EQ(renderTextField->GetText().selection.GetEnd(), 1);

        /**
         * @tc.steps: step3. construct longpress the second letter.
         * @tc.expected: step3. the text is selected from 1 to 2.
         */
        longPressInfo.SetGlobalLocation(OFFSET_SECOND_LETTER);
        renderTextField->HandleLongPress(longPressInfo);

        EXPECT_EQ(renderTextField->GetText().selection.GetStart(), 1);
        EXPECT_EQ(renderTextField->GetText().selection.GetEnd(), 2);
    }
}

/**
 * @tc.name: RenderTextSelectTest004
 * @tc.desc: Verify the selection of RenderTextSelect.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCK
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTextSelectTest, RenderTextSelectTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct textfield and get the theme.
     * @tc.expected: step1. theme is correct.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<TextFieldTheme> textFieldTheme = themeManager->GetTheme<TextFieldTheme>();
    ASSERT_TRUE(textFieldTheme);

    // Add the component
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetSelectedColor(textFieldTheme->GetSelectedColor());
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    context_->SetupRootElement();
    renderTextField->Attach(context_);
    renderTextField->Update(textField);
    renderTextField->AppendText();

    if (renderTextField->IsPhone()) {
        /**
         * @tc.steps: step2. construct longpress the first letter.
         * @tc.expected: step2. IsTextOverlayPushed function return true.
         */
        LongPressInfo longPressInfo(0);
        longPressInfo.SetGlobalLocation(OFFSET_FIRST_LETTER);
        renderTextField->HandleLongPress(longPressInfo);

        WaitAndMockVsync(WAIT_TIME);
        EXPECT_TRUE(renderTextField->HasTextOverlayPushed());
        EXPECT_EQ(renderTextField->GetText().selection.GetStart(), 0);
        EXPECT_EQ(renderTextField->GetText().selection.GetEnd(), 1);
    }
}

/**
 * @tc.name: RenderTextSelectTest005
 * @tc.desc: Verify the selection of RenderTextSelect.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCL
 * @tc.author: liujinwei
 */
HWTEST_F(RenderTextSelectTest, RenderTextSelectTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct textfield and get the renderNode.
     * @tc.expected: step1. renderNode is correct.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();
    context_->SetupRootElement();
    renderTextField->Attach(context_);
    renderTextField->Update(textField);

    if (renderTextField->IsPhone()) {
        /**
         * @tc.steps: step2. construct click the last letter.
         * @tc.expected: step2. the textOverlay is not showed.
         */
        ClickInfo clickInfo(1);
        clickInfo.SetGlobalLocation(OFFSET_END);
        renderTextField->HandleClick(clickInfo);
        WaitAndMockVsync(WAIT_TIME);
        EXPECT_FALSE(renderTextField->HasTextOverlayPushed());

        /**
         * @tc.steps: step3. Append text then construct click the last letter.
         * @tc.expected: step3. the textOverlay is showed, and text select at the end.
         */
        renderTextField->AppendText();
        renderTextField->HandleClick(clickInfo);
        WaitAndMockVsync(WAIT_TIME);
        EXPECT_TRUE(renderTextField->HasTextOverlayPushed());
        EXPECT_EQ(renderTextField->GetText().selection.GetStart(), 4);
        EXPECT_EQ(renderTextField->GetText().selection.GetEnd(), 4);
    }
}

} // namespace OHOS::Ace
