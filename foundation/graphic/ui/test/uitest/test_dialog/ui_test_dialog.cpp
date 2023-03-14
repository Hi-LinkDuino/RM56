/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ui_test_dialog.h"
#include "common/screen.h"
#include "components/ui_dialog.h"
#include "components/ui_label.h"
#include "graphic_config.h"
#include "test_resource_config.h"

#if ENABLE_WINDOW
namespace OHOS {
namespace {
const int16_t GAP = 5;
const int16_t TITLE_HEIGHT = 20;
const uint16_t LABEL_WIDTH = 350;
const uint16_t LABEL_HEIGHT = 50;
const uint16_t BUTTON_WIDTH = 100;
const uint16_t BUTTON_HEIGHT = 50;
} // namespace

class TestUIDialogButtonListener : public UIView::OnClickListener {
public:
    TestUIDialogButtonListener(UIDialog::DialogButtonType buttonType, UILabel* label)
    {
        dialog_ = nullptr;
        buttonType_ = buttonType;
        label_ = label;
    }
    virtual ~TestUIDialogButtonListener() {}

    void SetDialog(UIDialog** dialog)
    {
        dialog_ = dialog;
    }

    bool OnClick(UIView &view, const ClickEvent& event) override
    {
        switch (buttonType_) {
            case UIDialog::DialogButtonType::BUTTON_LEFT:
                label_->SetText("button left click!");
                break;
            case UIDialog::DialogButtonType::BUTTON_MID:
                label_->SetText("button mid click!");
                break;
            case UIDialog::DialogButtonType::BUTTON_RIGHT:
                label_->SetText("button right click!");
                break;
            default:
                break;
        }
        if (*dialog_ != nullptr) {
            delete *dialog_;
            *dialog_ = nullptr;
        }
        return true;
    }

private:
    UIDialog** dialog_;
    UIDialog::DialogButtonType buttonType_;
    UILabel* label_;
};

class TestUIDialogOnCancelListener : public UIView::OnClickListener {
public:
    explicit TestUIDialogOnCancelListener(UILabel* label)
    {
        dialog_ = nullptr;
        label_ = label;
    }
    virtual ~TestUIDialogOnCancelListener() {}

    void SetDialog(UIDialog** dialog)
    {
        dialog_ = dialog;
    }

    bool OnClick(UIView &view, const ClickEvent& event) override
    {
        label_->SetText("Click outside the dialog.");
        if (*dialog_ != nullptr) {
            delete *dialog_;
            *dialog_ = nullptr;
        }
        return true;
    }

private:
    UIDialog** dialog_;
    UILabel* label_;
};

void UITestDialog::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - LABEL_HEIGHT);
        positionX_ = 50; // 50: init position x0
        positionY_ = 5;  // 5: init position y
    }

    if (label_ == nullptr) {
        label_ = new UILabel();
        label_->SetPosition(0, positionY_);
        label_->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 18); // 18: font size
        label_->SetText("label");
        container_->Add(label_);
        positionY_ += LABEL_HEIGHT + GAP;
    }
    if (listener1_ == nullptr) {
        listener1_ = new TestUIDialogButtonListener(UIDialog::DialogButtonType::BUTTON_LEFT, label_);
    }
    if (listener2_ == nullptr) {
        listener2_ = new TestUIDialogButtonListener(UIDialog::DialogButtonType::BUTTON_MID, label_);
    }
    if (listener3_ == nullptr) {
        listener3_ = new TestUIDialogButtonListener(UIDialog::DialogButtonType::BUTTON_RIGHT, label_);
    }
    if (listener_ == nullptr) {
        listener_ = new TestUIDialogOnCancelListener(label_);
    }
}

void UITestDialog::TearDown()
{
    DeleteChildren(container_);
    if (listener1_ != nullptr) {
        delete listener1_;
        listener1_ = nullptr;
    }
    if (listener2_ != nullptr) {
        delete listener2_;
        listener2_ = nullptr;
    }
    if (listener3_ != nullptr) {
        delete listener3_;
        listener3_ = nullptr;
    }
    if (listener_ != nullptr) {
        delete listener_;
        listener_ = nullptr;
    }
    if (dialog_ != nullptr) {
        delete dialog_;
        dialog_ = nullptr;
    }
    label_ = nullptr;
    container_ = nullptr;
}

void UITestDialog::InnerTestTitle(const char* title)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(0, positionY_, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    titleLabel->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    titleLabel->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    titleLabel->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);

    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + GAP;
}

const UIView* UITestDialog::GetTestView()
{
    UIKitDialogTest001();
    UIKitDialogTest002();
    UIKitDialogTest003();
    UIKitDialogTest004();
    UIKitDialogTest005();
    UIKitDialogTest006();
    UIKitDialogTest007();
    UIKitDialogTest008();
    UIKitDialogTest009();
    UIKitDialogTest010();
    return container_;
}

void UITestDialog::UIKitDialogTest001()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置较长标题、较短正文、单个较短按钮");
    button1_ = new UILabelButton();
    button1_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button1_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button1_->SetText("Dialog1");
    button1_->SetOnClickListener(this);
    container_->Add(button1_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest002()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置较短标题、较长正文、两个较短按钮");
    button2_ = new UILabelButton();
    button2_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button2_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button2_->SetText("Dialog2");
    button2_->SetOnClickListener(this);
    container_->Add(button2_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest003()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置较短标题、较短正文、单个较长按钮");
    button3_ = new UILabelButton();
    button3_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button3_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button3_->SetText("Dialog3");
    button3_->SetOnClickListener(this);
    container_->Add(button3_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest004()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置较短标题、较短正文、三个长按钮");
    button4_ = new UILabelButton();
    button4_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button4_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button4_->SetText("Dialog4");
    button4_->SetOnClickListener(this);
    container_->Add(button4_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest005()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置较短正文、自动关闭、弹框外点击事件监听");
    button5_ = new UILabelButton();
    button5_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button5_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button5_->SetText("Dialog5");
    button5_->SetOnClickListener(this);
    container_->Add(button5_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest006()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试只设置较长正文，点击弹框外自动关闭");
    button6_ = new UILabelButton();
    button6_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button6_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button6_->SetText("Dialog6");
    button6_->SetOnClickListener(this);
    container_->Add(button6_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest007()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置长标题，长正文，三个长按钮");
    button7_ = new UILabelButton();
    button7_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button7_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button7_->SetText("Dialog7");
    button7_->SetOnClickListener(this);
    container_->Add(button7_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest008()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置按钮颜色不带透明度");
    button8_ = new UILabelButton();
    button8_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button8_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button8_->SetText("Dialog8");
    button8_->SetOnClickListener(this);
    container_->Add(button8_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest009()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试设置按钮颜色带透明度");
    button9_ = new UILabelButton();
    button9_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button9_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button9_->SetText("Dialog9");
    button9_->SetOnClickListener(this);
    container_->Add(button9_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

void UITestDialog::UIKitDialogTest010()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("测试连续多次设置标题、正文、按钮");
    button10_ = new UILabelButton();
    button10_->SetPosition(0, positionY_, BUTTON_WIDTH, BUTTON_HEIGHT);
    button10_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button10_->SetText("Dialog10");
    button10_->SetOnClickListener(this);
    container_->Add(button10_);
    positionY_ += BUTTON_HEIGHT + GAP;
}

bool UITestDialog::OnClick(UIView &view, const ClickEvent& event)
{
    if (&view == button1_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetTitle("标题标题标题标题标题");
        dialog_->SetText("段落文本");
        listener1_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, "Button1", listener1_);
        dialog_->Show();
    } else if (&view == button2_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetTitle("标题");
        dialog_->SetText("段落文本段落文本段落文本段落文本段落文本段落文本段落文本");
        listener1_->SetDialog(&dialog_);
        listener3_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, "Button1", listener1_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_RIGHT, "Button2", listener3_);
        dialog_->Show();
    } else if (&view == button3_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetTitle("标题");
        dialog_->SetText("段落文本");
        listener2_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "Button1Button1Button1Button1", listener2_);
        dialog_->Show();
    } else if (&view == button4_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetTitle("标题");
        dialog_->SetText("段落文本");
        listener1_->SetDialog(&dialog_);
        listener2_->SetDialog(&dialog_);
        listener3_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, "Button111111111111111111", listener1_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "Button22222222222222222", listener2_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_RIGHT, "Button33333333333333333", listener3_);
        dialog_->Show();
    } else if (&view == button5_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetText("段落文本");
        dialog_->EnableAutoCancel(true);
        listener_->SetDialog(&dialog_);
        dialog_->SetOnCancelListener(listener_);
        dialog_->Show();
    } else {
        return ClickExpand(view, event);
    }
    return true;
}

bool UITestDialog::ClickExpand(UIView &view, const ClickEvent& event)
{
    if (&view == button6_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetText("段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本");
        dialog_->EnableAutoCancel(true);
        listener_->SetDialog(&dialog_);
        dialog_->SetOnCancelListener(listener_);
        dialog_->Show();
    } else if (&view == button7_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetTitle("段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本");
        dialog_->SetText(
            "段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文 \
            段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文本段落文");
        listener1_->SetDialog(&dialog_);
        listener2_->SetDialog(&dialog_);
        listener3_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, "Button111111111111111111111", listener1_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "Button22222222222222222222", listener2_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_RIGHT, "Button33333333333333333333", listener3_);
        dialog_->Show();
    } else if (&view == button8_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetText("段落正文");
        listener2_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "button", listener2_);
        dialog_->SetButtonColor(UIDialog::DialogButtonType::BUTTON_MID, Color::Red());
        dialog_->Show();
    } else if (&view == button9_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetText("段落正文");
        listener2_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "button", listener2_);
        // 0xFF, 0x00, 0x00, 0x7F: color red with 50% opacity
        ColorType color = Color::GetColorFromRGBA(0xFF, 0x00, 0x00, 0x7F);
        dialog_->SetButtonColor(UIDialog::DialogButtonType::BUTTON_MID, color);
        dialog_->Show();
    } else if (&view == button10_) {
        if (dialog_ != nullptr) {
            delete dialog_;
            dialog_ = nullptr;
        }
        dialog_ = new UIDialog();
        dialog_->SetTitle("标题1");
        dialog_->SetText("段落正文1");
        listener2_->SetDialog(&dialog_);
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "按钮1", listener2_);
        dialog_->SetTitle("标题2");
        dialog_->SetText("段落正文2");
        dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_MID, "按钮2", listener2_);
        dialog_->Show();
    }
    return true;
}
} // namespace OHOS
#endif // ENABLE_WINDOW
