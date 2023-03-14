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

#include "components/ui_dialog.h"
#include "common/screen.h"
#include "common/typed_text.h"
#if ENABLE_DEBUG
#include "gfx_utils/graphic_assert.h"
#endif
#include "gfx_utils/graphic_log.h"
#if ENABLE_WINDOW
#include "window/window.h"
#endif

#if ENABLE_WINDOW
namespace OHOS {
class UIDialogLabelButton : public UILabelButton {
public:
    explicit UIDialogLabelButton(UIDialog* dialog)
    {
        dialog_ = dialog;
    }
    virtual ~UIDialogLabelButton() {}
    virtual bool OnClickEvent(const ClickEvent& event)
    {
        bool ret = true;
        if (dialog_ != nullptr) {
            dialog_->DestoryWindow();
            dialog_->isShowing_ = false;
        }
        if (onClickListener_ != nullptr) {
            ret = onClickListener_->OnClick(*this, event);
        }
        return ret;
    }
private:
    UIDialog* dialog_;
};

class UIDialogClickListener : public UIView::OnClickListener {
public:
    explicit UIDialogClickListener(UIDialog* dialog)
    {
        dialog_ = dialog;
    }
    virtual ~UIDialogClickListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        bool ret = true;
        if ((dialog_ != nullptr) && (dialog_->dialogLayer_ != nullptr) &&
            !dialog_->dialogLayer_->GetRect().IsContains(event.GetCurrentPos())) {
            if (dialog_->enableAutoCancel_) {
                dialog_->DestoryWindow();
                dialog_->isShowing_ = false;
            }
            if (dialog_->onCancelListener_ != nullptr) {
                ret = dialog_->onCancelListener_->OnClick(view, event);
            }
        }
        return ret;
    }
private:
    UIDialog* dialog_;
};

UIDialog::UIDialog()
    : isShowing_(false),
      enableAutoCancel_(false),
      buttonNum_(0),
      title_(nullptr),
      text_(nullptr),
      button1_(nullptr),
      button2_(nullptr),
      button3_(nullptr),
      dialogLayer_(nullptr),
      windowRootView_(nullptr),
      onCancelListener_(nullptr),
      dialogClickListener_(nullptr),
      window_(nullptr),
      line1_(nullptr),
      line2_(nullptr),
      titleText_(nullptr),
      textText_(nullptr)
{
    uint16_t screenWidth = Screen::GetInstance().GetWidth();
    uint16_t screenHeight = Screen::GetInstance().GetHeight();
    // 100: calculate percentage
    widthMax_ = screenWidth * MAX_WIDTH_PERCENT / 100;
    // 100: calculate percentage
    heightMax_ = screenHeight * MAX_HEIGHT_PERCENT / 100;
    colorType1_ = Color::White();
    colorType2_ = Color::White();
    colorType3_ = Color::White();
}

UIDialog::~UIDialog()
{
    onCancelListener_ = nullptr;
    if (dialogLayer_ != nullptr) {
        dialogLayer_->RemoveAll();
        delete dialogLayer_;
        dialogLayer_ = nullptr;
    }
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (text_ != nullptr) {
        delete text_;
        text_ = nullptr;
    }
    if (button1_ != nullptr) {
        delete button1_;
        button1_ = nullptr;
    }
    if (button2_ != nullptr) {
        delete button2_;
        button2_ = nullptr;
    }
    if (button3_ != nullptr) {
        delete button3_;
        button3_ = nullptr;
    }
    if (line1_ != nullptr) {
        delete line1_;
        line1_ = nullptr;
    }
    if (line2_ != nullptr) {
        delete line2_;
        line2_ = nullptr;
    }
    if ((windowRootView_ != nullptr) && !RootView::DestoryWindowRootView(windowRootView_)) {
        windowRootView_ = nullptr;
    }
    if (dialogClickListener_ != nullptr) {
        delete dialogClickListener_;
        dialogClickListener_ = nullptr;
    }
}

void UIDialog::SetTitle(const char* title)
{
    if (title == nullptr) {
        return;
    }
    titleText_ = title;
}

void UIDialog::SetText(const char* text)
{
    if (text == nullptr) {
        return;
    }
    textText_ = text;
}

void UIDialog::SetButton(DialogButtonType buttonType, const char* text, UIView::OnClickListener* listener)
{
    switch (buttonType) {
        case DialogButtonType::BUTTON_LEFT:
            if (button1_ == nullptr) {
                button1_ = new UIDialogLabelButton(this);
                if (button1_ == nullptr) {
                    GRAPHIC_LOGE("new UIDialogLabelButton fail");
                    return;
                }
                button1_->SetViewId(BUTTON1_ID);
                AddButton(buttonType, button1_, text, listener);
            } else {
                button1_->SetText(text);
                button1_->SetOnClickListener(listener);
            }
            break;
        case DialogButtonType::BUTTON_MID:
            if (button2_ == nullptr) {
                button2_ = new UIDialogLabelButton(this);
                if (button2_ == nullptr) {
                    GRAPHIC_LOGE("new UIDialogLabelButton fail");
                    return;
                }
                button2_->SetViewId(BUTTON2_ID);
                AddButton(buttonType, button2_, text, listener);
            } else {
                button2_->SetText(text);
                button2_->SetOnClickListener(listener);
            }
            break;
        case DialogButtonType::BUTTON_RIGHT:
            if (button3_ == nullptr) {
                button3_ = new UIDialogLabelButton(this);
                if (button3_ == nullptr) {
                    GRAPHIC_LOGE("new UIDialogLabelButton fail");
                    return;
                }
                AddButton(buttonType, button3_, text, listener);
            } else {
                button3_->SetText(text);
                button3_->SetOnClickListener(listener);
            }
            break;
        default:
            break;
    }
}

void UIDialog::AddButton(DialogButtonType buttonType,
                         UILabelButton* button,
                         const char* text,
                         UIView::OnClickListener* listener)
{
    buttonNum_++;
    button->SetText(text);
    button->SetOnClickListener(listener);
    button->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FONT_SIZE);
    button->SetTextColor(Color::Blue());
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::White().full, UIButton::ButtonState::RELEASED);
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::ButtonState::PRESSED);
    button->SetStyleForState(STYLE_BACKGROUND_OPA, BUTTON_PRESS_OPA, UIButton::ButtonState::PRESSED);
    button->SetStyleForState(STYLE_BORDER_WIDTH, 0, UIButton::ButtonState::RELEASED);
    button->SetStyleForState(STYLE_BORDER_WIDTH, 0, UIButton::ButtonState::PRESSED);
    button->SetHeight(BUTTON_HEIGHT);
}

void UIDialog::Show()
{
    if (isShowing_) {
        return;
    }
    SetTitleLabel();
    SetTextLabel();
    InitDialog();
    AddComponents();
    MeasureSize();
    Layout();
    CreateDialogWindow();
    window_->Show();
    isShowing_ = true;
}

void UIDialog::SetTitleLabel()
{
    if (titleText_ == nullptr) {
        return;
    }
    if (title_ == nullptr) {
        title_ = new UILabel();
        if (title_ == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        title_->SetViewId(TITLE_ID);
        title_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_FONT_SIZE);
        title_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);
        title_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
        title_->SetStyle(STYLE_TEXT_OPA, TITLE_TEXT_OPA);
        title_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    }
    title_->SetText(titleText_);
}

void UIDialog::SetTextLabel()
{
    if (textText_ == nullptr) {
        return;
    }
    if (text_ == nullptr) {
        text_ = new UILabel();
        if (text_ == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        text_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_FONT_SIZE);
        text_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
        text_->SetStyle(STYLE_TEXT_OPA, TITLE_TEXT_OPA);
        text_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    }
    text_->SetText(textText_);
}

void UIDialog::InitDialog()
{
    if (dialogLayer_ == nullptr) {
        dialogLayer_ = new UIViewGroup();
        if (dialogLayer_ == nullptr) {
            GRAPHIC_LOGE("new UIViewGroup fail");
            return;
        }
        dialogLayer_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    }
    if (windowRootView_ == nullptr) {
        windowRootView_ = RootView::GetWindowRootView();
        windowRootView_->SetWidth(Screen::GetInstance().GetWidth());
        windowRootView_->SetHeight(Screen::GetInstance().GetHeight());
        windowRootView_->SetTouchable(true);
        windowRootView_->Add(dialogLayer_);
    }
    if (dialogClickListener_ == nullptr) {
        dialogClickListener_ = new UIDialogClickListener(this);
        if (dialogClickListener_ == nullptr) {
            GRAPHIC_LOGE("new UIDialogClickListener fail");
            return;
        }
        windowRootView_->SetOnClickListener(dialogClickListener_);
    }
}

void UIDialog::AddComponents()
{
    if (title_ != nullptr) {
        dialogLayer_->Add(title_);
    }
    if (text_ != nullptr) {
        dialogLayer_->Add(text_);
    }
    if (button1_ != nullptr) {
        dialogLayer_->Add(button1_);
    }
    if (button2_ != nullptr) {
        dialogLayer_->Add(button2_);
    }
    if (button3_ != nullptr) {
        dialogLayer_->Add(button3_);
    }
    if (buttonNum_ > 1) {
        line1_ = new UIView();
        if (line1_ == nullptr) {
            GRAPHIC_LOGE("new UIView fail");
            return;
        }
        line1_->SetHeight(LINE_HEIGHT);
        line1_->SetWidth(LINE_WIDTH);
        line1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        line1_->SetStyle(STYLE_BACKGROUND_OPA, LINE_OPA);
        dialogLayer_->Add(line1_);
    }
    if (buttonNum_ == 3) { // 3: three buttons
        line2_ = new UIView();
        if (line2_ == nullptr) {
            GRAPHIC_LOGE("new UIView fail");
            return;
        }
        line2_->SetHeight(LINE_HEIGHT);
        line2_->SetWidth(LINE_WIDTH);
        line2_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
        line2_->SetStyle(STYLE_BACKGROUND_OPA, LINE_OPA);
        dialogLayer_->Add(line2_);
    }
}

void UIDialog::MeasureSize()
{
    uint16_t width = MeasureMaxWidth();
    uint16_t height = 0;

    if (buttonNum_ > 0) {
        // 2: there are 2 paddings horizontally
        uint16_t buttonWidth = (width - 2 * BUTTON_PADDING - (buttonNum_ - 1) * BUTTON_MID_PADDING) / buttonNum_;
        // 2: there are 2 paddings horizontally
        width = static_cast<uint32_t>(buttonWidth) * buttonNum_ + (buttonNum_ - 1) * BUTTON_MID_PADDING +
            2 * BUTTON_PADDING;
        height += BUTTON_TOTAL_HEIGHT;
        height += TEXT_BUTTON_PADDING;
        if (button1_ != nullptr) {
            button1_->SetWidth(buttonWidth);
        }
        if (button2_ != nullptr) {
            button2_->SetWidth(buttonWidth);
        }
        if (button3_ != nullptr) {
            button3_->SetWidth(buttonWidth);
        }
    } else {
        height += PADDING;
    }

    if (title_ != nullptr) {
        title_->SetWidth(width - 2 * PADDING); // 2: there are 2 paddings horizontally
        height += TITLE_TOTAL_HEIGHT;
    } else {
        height += PADDING;
    }

    if (text_ != nullptr) {
        text_->SetWidth(width - 2 * PADDING); // 2: there are 2 paddings horizontally
        uint16_t textHightMax = heightMax_ - height;
        if (text_->GetTextHeight() < textHightMax) {
            text_->SetHeight(text_->GetTextHeight());
        } else {
            text_->SetHeight(textHightMax);
            text_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
        }
        height += text_->GetHeight();
    }

    uint16_t dialogHeight = height;
    dialogLayer_->SetHeight(dialogHeight);
    dialogLayer_->SetWidth(width);
}

uint16_t UIDialog::MeasureMaxWidth()
{
    uint16_t titleWidth = 0;
    uint16_t textWidth = 0;
    uint16_t buttonTotalWidth = 0;

    if (title_ != nullptr) {
        titleWidth = MeasureTitleWidth();
        titleWidth += 2 * PADDING; // 2: there are 2 paddings horizontally
    }
    if (text_ != nullptr) {
        textWidth = MeasureTextWidth();
        textWidth += 2 * PADDING; // 2: there are 2 paddings horizontally
    }
    if (buttonNum_ > 0) {
        buttonTotalWidth =
            static_cast<uint32_t>(MeasureButtonWidth()) * buttonNum_ + (buttonNum_ - 1) * BUTTON_MID_PADDING;
        buttonTotalWidth += 2 * BUTTON_PADDING; // 2: there are 2 paddings horizontally
    }
    return MATH_MAX(titleWidth, MATH_MAX(textWidth, buttonTotalWidth));
}

uint16_t UIDialog::MeasureTitleWidth()
{
    uint16_t titleWidth = 0;
    uint16_t widthMaxNoPadding = widthMax_ - 2 * PADDING; // 2: there are 2 paddings horizontally
    title_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    if (title_->GetTextWidth() > widthMaxNoPadding) {
        titleWidth = widthMaxNoPadding;
        title_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    } else {
        titleWidth = title_->GetTextWidth();
    }
    title_->SetHeight(title_->GetTextHeight());
    return titleWidth;
}

uint16_t UIDialog::MeasureTextWidth()
{
    uint16_t textWidth = 0;
    uint16_t widthMaxNoPadding = widthMax_ - 2 * PADDING; // 2: there are 2 paddings horizontally
    if (title_ != nullptr) {
        text_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);
    } else {
        text_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    }
    text_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    textWidth = text_->GetTextWidth();
    if (text_->GetTextWidth() > widthMaxNoPadding) {
        text_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);
        textWidth = widthMaxNoPadding;
        text_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    }
    return textWidth;
}

uint16_t UIDialog::MeasureButtonWidth()
{
    if (buttonNum_ == 0) {
        return 0;
    }

    uint16_t buttonTextWidth = 0;
    // 2: there are 2 paddings horizontally
    uint16_t buttonMaxWidth = (widthMax_ - 2 * BUTTON_PADDING - (buttonNum_ - 1) * BUTTON_MID_PADDING) / buttonNum_;

    if (button1_ != nullptr) {
        const char* text1 = button1_->GetText();
        buttonTextWidth =
            MATH_MAX(buttonTextWidth, TypedText::GetTextSize(text1, button1_->GetFontId(), BUTTON_FONT_SIZE,
                                                             button1_->GetStyleConst().letterSpace_,
                                                             button1_->GetStyleConst().lineHeight_, widthMax_, 0).x);
    }
    if (button2_ != nullptr) {
        const char* text2 = button2_->GetText();
        buttonTextWidth =
            MATH_MAX(buttonTextWidth, TypedText::GetTextSize(text2, button2_->GetFontId(), BUTTON_FONT_SIZE,
                                                             button2_->GetStyleConst().letterSpace_,
                                                             button2_->GetStyleConst().lineHeight_, widthMax_, 0).x);
    }
    if (button3_ != nullptr) {
        const char* text3 = button3_->GetText();
        buttonTextWidth =
            MATH_MAX(buttonTextWidth, TypedText::GetTextSize(text3, button3_->GetFontId(), BUTTON_FONT_SIZE,
                                                             button3_->GetStyleConst().letterSpace_,
                                                             button3_->GetStyleConst().lineHeight_, widthMax_, 0).x);
    }
    return (buttonTextWidth + BUTTON_HEIGHT) > buttonMaxWidth ? buttonMaxWidth : (buttonTextWidth + BUTTON_HEIGHT);
}

void UIDialog::Layout()
{
    if (title_ != nullptr) {
        // 2: there are 2 paddings vertically
        uint8_t padding = (TITLE_TOTAL_HEIGHT - title_->GetHeight()) / 2;
        title_->LayoutLeftOfParent(PADDING);
        title_->LayoutTopOfParent(padding);
        if (text_ != nullptr) {
            text_->LayoutLeftOfParent(PADDING);
            text_->LayoutBottomToSibling(TITLE_ID, padding);
        }
    } else {
        if (text_ != nullptr) {
            text_->LayoutLeftOfParent(PADDING);
            text_->LayoutTopOfParent(PADDING);
        }
    }
    LayoutButton();
}

void UIDialog::LayoutButton()
{
    if (button1_ != nullptr) {
        button1_->LayoutLeftOfParent(BUTTON_PADDING);
        button1_->LayoutBottomOfParent(BUTTON_PADDING);
        if (button2_ != nullptr) {
            button2_->LayoutRightToSibling(BUTTON1_ID, BUTTON_MID_PADDING);
            button2_->LayoutBottomOfParent(BUTTON_PADDING);
        }
    } else if (button2_ != nullptr) {
        button2_->LayoutLeftOfParent(BUTTON_PADDING);
        button2_->LayoutBottomOfParent(BUTTON_PADDING);
    }
    if (button3_ != nullptr) {
        button3_->LayoutRightOfParent(BUTTON_PADDING);
        button3_->LayoutBottomOfParent(BUTTON_PADDING);
    }

    if (buttonNum_ == 3) { // 3: three buttons
        line1_->LayoutBottomOfParent(LINE_BOTTOM_PADDING);
        line1_->LayoutRightToSibling(BUTTON1_ID, LINE_BUTTON_PADDING);
        line2_->LayoutBottomOfParent(LINE_BOTTOM_PADDING);
        line2_->LayoutRightToSibling(BUTTON2_ID, LINE_BUTTON_PADDING);
    } else if (buttonNum_ == 2) { // 2: two buttons
        if (button1_ != nullptr) {
            line1_->LayoutBottomOfParent(LINE_BOTTOM_PADDING);
            line1_->LayoutRightToSibling(BUTTON1_ID, LINE_BUTTON_PADDING);
        } else {
            line1_->LayoutBottomOfParent(LINE_BOTTOM_PADDING);
            line1_->LayoutRightToSibling(BUTTON2_ID, LINE_BUTTON_PADDING);
        }
    }
}

void UIDialog::CreateDialogWindow()
{
    dialogLayer_->LayoutCenterOfParent();
    Rect rect = dialogLayer_->GetRect();
    int16_t offset_x = dialogLayer_->GetX();
    int16_t offset_y = dialogLayer_->GetY();

    windowRootView_->SetPosition(-offset_x, -offset_y);
    windowRootView_->Invalidate();

    WindowConfig config = {};
    config.rect = rect;
    config.rect.SetPosition(offset_x, offset_y);
    config.isModal = true;
    window_ = Window::CreateWindow(config);
    if (window_ != nullptr) {
        window_->BindRootView(windowRootView_);
    } else {
        GRAPHIC_LOGE("Create window false!");
    }
}

void UIDialog::SetButtonColor(DialogButtonType buttonType, ColorType color)
{
    switch (buttonType) {
        case DialogButtonType::BUTTON_LEFT:
            if (button1_ != nullptr) {
                button1_->SetStyleForState(STYLE_BACKGROUND_COLOR, color.full, UIButton::ButtonState::RELEASED);
                button1_->SetStyleForState(STYLE_BACKGROUND_OPA, color.alpha, UIButton::ButtonState::RELEASED);
                colorType1_ = color;
            }
            break;
        case DialogButtonType::BUTTON_MID:
            if (button2_ != nullptr) {
                button2_->SetStyleForState(STYLE_BACKGROUND_COLOR, color.full, UIButton::ButtonState::RELEASED);
                button2_->SetStyleForState(STYLE_BACKGROUND_OPA, color.alpha, UIButton::ButtonState::RELEASED);
                colorType2_ = color;
            }
            break;
        case DialogButtonType::BUTTON_RIGHT:
            if (button3_ != nullptr) {
                button3_->SetStyleForState(STYLE_BACKGROUND_COLOR, color.full, UIButton::ButtonState::RELEASED);
                button3_->SetStyleForState(STYLE_BACKGROUND_COLOR, color.alpha, UIButton::ButtonState::RELEASED);
                colorType3_ = color;
            }
            break;
        default:
            break;
    }
}

void UIDialog::SetOnCancelListener(UIView::OnClickListener* onCancelListener)
{
    if (onCancelListener == nullptr) {
        return;
    }
    onCancelListener_ = onCancelListener;
}

void UIDialog::EnableAutoCancel(bool enable)
{
    enableAutoCancel_ = enable;
}

void UIDialog::DestoryWindow()
{
    if (window_ != nullptr) {
        Window::DestoryWindow(window_);
        window_ = nullptr;
    }
}

#ifdef ENABLE_DEBUG
const char* UIDialog::GetButtonText(DialogButtonType buttonType) const
{
    switch (buttonType) {
        case DialogButtonType::BUTTON_LEFT:
            if (button1_ != nullptr) {
                return button1_->GetText();
            }
            return nullptr;
        case DialogButtonType::BUTTON_MID:
            if (button2_ != nullptr) {
                return button2_->GetText();
            }
            return nullptr;
        case DialogButtonType::BUTTON_RIGHT:
            if (button3_ != nullptr) {
                return button3_->GetText();
            }
            return nullptr;
        default:
            return nullptr;
    }
}

UIView::OnClickListener* UIDialog::GetButtonListener(DialogButtonType buttonType) const
{
    switch (buttonType) {
        case DialogButtonType::BUTTON_LEFT:
            if (button1_ != nullptr) {
                return button1_->GetOnClickListener();
            }
            return nullptr;
        case DialogButtonType::BUTTON_MID:
            if (button2_ != nullptr) {
                return button2_->GetOnClickListener();
            }
            return nullptr;
        case DialogButtonType::BUTTON_RIGHT:
            if (button3_ != nullptr) {
                return button3_->GetOnClickListener();
            }
            return nullptr;
        default:
            return nullptr;
    }
}

ColorType UIDialog::GetButtonColor(DialogButtonType buttonType) const
{
    switch (buttonType) {
        case DialogButtonType::BUTTON_LEFT:
            if (button1_ != nullptr) {
                return colorType1_;
            }
            break;
        case DialogButtonType::BUTTON_MID:
            if (button2_ != nullptr) {
                return colorType2_;
            }
            break;
        case DialogButtonType::BUTTON_RIGHT:
            if (button3_ != nullptr) {
                return colorType3_;
            }
            break;
        default:
            break;
    }
    ASSERT(0);
}
#endif // ENABLE_DEBUG
} // namespace OHOS
#endif // ENABLE_WINDOW
