/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "ui_test_font.h"
#include "common/screen.h"
#if ENABLE_VECTOR_FONT
#else
#include "common/ui_text_language.h"
#endif
#include "components/ui_label.h"
#include "font/ui_font.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif

namespace OHOS {
namespace {
const int16_t GAP = 5;
const int16_t TITLE_HEIGHT = 20;
const uint16_t LABEL_WIDTH = 400;
const uint16_t LABEL_HEIGHT = 50;
const uint16_t FONT_SIZE = 30;
const char* SOURCE_HAN_SANS_SC_REGULAR = "SourceHanSansSC-Regular.otf";
const char* ROBOTO_CONDENSED_REGULAR = "RobotoCondensed-Regular.ttf";
const char* DEFAULT_EMOJI_FILENAME = "NotoColorEmojiCompat.ttf";
#if ENABLE_MULTI_FONT
const int16_t F_HWEMOJI_REGULAR_30_32 = 99;
#endif
} // namespace

void UITestFont::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        positionX_ = 50; // 50: init position x
        positionY_ = 5;  // 5: init position y
    }
}

void UITestFont::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

void UITestFont::InnerTestTitle(const char* title)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(),
                            TITLE_LABEL_DEFAULT_HEIGHT);
#if ENABLE_VECTOR_FONT
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DEFAULT_VECTOR_FONT_SIZE);
#else
    titleLabel->SetFontId(F_SOURCEHANSANSSC_REGULAR_18_4);
#endif
    titleLabel->SetText(title);
    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + GAP;
}

const UIView* UITestFont::GetTestView()
{
    UIKitFontTestDisplay001();
    UIKitFontTestDisplay002();
    UIKitFontTestDisplay003();
    UIKitFontTestDisplay004();
    UIKitFontTestDisplay005();
    UIKitFontTestDisplay006();
    UIKitFontTestDisplay007();
    UIKitFontTestDisplay008();
    UIKitFontTestDisplay009();
    UIKitFontTestDisplay010();
    UIKitFontTestBaseline001();
    UIKitFontTestBaseline002();
    UIKitFontTestLineHeight001();
    UIKitFontTestLineHeight002();
#if ENABLE_MULTI_FONT
    UIKitFontMultiLanguage001();
    UIKitFontMultiLanguage002();
#endif
    UIKitFontTestBackgroundColor();
    UIKitFontTestForegroundColor();
    UIKitFontTestLineBackgroundSpan();
    UIKitFontTestAbsoluteSizeSpan();
    UIKitFontTestRelativeSizeSpan();
    UIKitFontTestDisplayEmoji001();
    UIKitFontTestDisplayEmoji002();
    UIKitFontTestDisplayEmoji003();
    return container_;
}

void UITestFont::UIKitFontTestDisplay001()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display single line Chinese");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("轻量图形子系统");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontTestDisplay002()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display multiline Chinese");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText(" 你好\n轻量图形子系统");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP;  // 2 : double
}


void UITestFont::UIKitFontTestDisplay003()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display single line English");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("uikit test");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT + GAP;
}


void UITestFont::UIKitFontTestDisplay004()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display multiline English");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText(" Hello\n uikit");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}


void UITestFont::UIKitFontTestDisplay005()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display single line Chinese and English");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("你好，轻量级图形 uikit");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontTestDisplay006()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display multiline Chinese and English");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("你好\n轻量级图形 uikit");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}


void UITestFont::UIKitFontTestDisplay007()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display single line text color");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
    label->SetText("你好，轻量级图形");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontTestDisplay008()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display multiline text color");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label->SetText("你好\n 轻量级图形 uikit");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}


void UITestFont::UIKitFontTestDisplay009()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display multiline text color");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_ + 100, positionY_);  // 100 : offset x
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
    label->SetLineBreakMode(UILabel::LineBreakMode::LINE_BREAK_ADAPT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->Rotate(90, Vector2<float> {0, 0}); // 90 : angle
    label->SetText("轻量级图形");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 4 + GAP; // 4 : coefficient
}


void UITestFont::UIKitFontTestDisplay010()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display multiline text color");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_ + 100, positionY_); // 100 : offset x
    label->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT); // 2 : half
    label->SetLineBreakMode(UILabel::LineBreakMode::LINE_BREAK_MARQUEE);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->Rotate(90, Vector2<float> {0, 0}); // 90 : angle
    label->SetText("轻量级图形,轻量级图形,轻量级图形");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 4 + GAP; // 4 : coefficient
}


void UITestFont::UIKitFontTestBaseline001()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("Font baseline alignment");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT); // 2 : half
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("hello, uikit");

    UILabel* label2 = new UILabel();
    label2->SetPosition(positionX_ + (LABEL_WIDTH / 2), positionY_); // 2 : half
    label2->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(ROBOTO_CONDENSED_REGULAR);
    label2->SetFont(ROBOTO_CONDENSED_REGULAR, FONT_SIZE);
#else
    label2->SetFontId(F_ROBOTOCONDENSED_REGULAR_30_4);
#endif
    label2->SetText("hello, uikit");

    container_->Add(label);
    container_->Add(label2);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontTestBaseline002()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Font baseline alignment");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT); // 2 : half
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("hello, uikit");
    label->SetStyle(STYLE_LINE_HEIGHT, 30); // 30 : line height

    UILabel* label2 = new UILabel();
    label2->SetPosition(positionX_ + (LABEL_WIDTH / 2), positionY_); // 2 : half
    label2->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(ROBOTO_CONDENSED_REGULAR);
    label2->SetFont(ROBOTO_CONDENSED_REGULAR, FONT_SIZE);
#else
    label2->SetFontId(F_ROBOTOCONDENSED_REGULAR_30_4);
#endif
    label2->SetText("hello, uikit");
    label2->SetStyle(STYLE_LINE_HEIGHT, 30); // 30 : line height

    container_->Add(label);
    container_->Add(label2);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontTestLineHeight001()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Font lineheight alignment");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT * 2); // 2 : half
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("hello,\n uikit");

    UILabel* label2 = new UILabel();
    label2->SetPosition(positionX_ + (LABEL_WIDTH / 2), positionY_); // 2 : half
    label2->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT * 2); // 2 : half
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(ROBOTO_CONDENSED_REGULAR);
    label2->SetFont(ROBOTO_CONDENSED_REGULAR, FONT_SIZE);
#else
    label2->SetFontId(F_ROBOTOCONDENSED_REGULAR_30_4);
#endif
    label2->SetText("hello,\n uikit");

    container_->Add(label);
    container_->Add(label2);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}

void UITestFont::UIKitFontTestLineHeight002()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Font lineheight alignment");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT * 2); // 2 : half
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("hello,\n uikit");
    label->SetStyle(STYLE_LINE_HEIGHT, 40); // 40 : line height

    UILabel* label2 = new UILabel();
    label2->SetPosition(positionX_ + (LABEL_WIDTH / 2), positionY_); // 2 : half
    label2->Resize(LABEL_WIDTH / 2, LABEL_HEIGHT * 2); // 2 : half
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(ROBOTO_CONDENSED_REGULAR);
    label2->SetFont(ROBOTO_CONDENSED_REGULAR, FONT_SIZE);
#else
    label2->SetFontId(F_ROBOTOCONDENSED_REGULAR_30_4);
#endif
    label2->SetText("hello,\n uikit");
    label2->SetStyle(STYLE_LINE_HEIGHT, 40); // 40 : line height

    container_->Add(label);
    container_->Add(label2);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}

#if ENABLE_MULTI_FONT
void UITestFont::UIKitFontMultiLanguage001()
{
    if (container_ == nullptr) {
        return;
    }
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    UIFont::GetInstance()->RegisterFontInfo(ROBOTO_CONDENSED_REGULAR);
    uint8_t findPath[] = {UIFont::GetInstance()->GetFontId(SOURCE_HAN_SANS_SC_REGULAR)};
    UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(ROBOTO_CONDENSED_REGULAR),
                                                         findPath, sizeof(findPath));
#else
    uint8_t findPath[] = {F_SOURCEHANSANSSC_REGULAR_30_4};
    UIMultiFontManager::GetInstance()->SetSearchFontList(F_ROBOTOCONDENSED_REGULAR_30_4, findPath, sizeof(findPath));
#endif
    InnerTestTitle(" Display multilingual display");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    label->SetFont(ROBOTO_CONDENSED_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_ROBOTOCONDENSED_REGULAR_30_4);
#endif
    label->SetText("Hello, Test of GUI");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontMultiLanguage002()
{
    if (container_ == nullptr) {
        return;
    }
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    UIFont::GetInstance()->RegisterFontInfo(ROBOTO_CONDENSED_REGULAR);
    uint8_t findPath[] = {UIFont::GetInstance()->GetFontId(SOURCE_HAN_SANS_SC_REGULAR)};
    UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(ROBOTO_CONDENSED_REGULAR),
                                                         findPath, sizeof(findPath));
#else
    uint8_t findPath[] = {F_SOURCEHANSANSSC_REGULAR_30_4};
    UIMultiFontManager::GetInstance()->SetSearchFontList(F_ROBOTOCONDENSED_REGULAR_30_4, findPath, sizeof(findPath));
#endif
    InnerTestTitle(" Display multilingual display");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    label->SetFont(ROBOTO_CONDENSED_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_ROBOTOCONDENSED_REGULAR_30_4);
#endif
    label->SetText("Hello\n Test of GUI");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}
#endif // ENABLE_MULTI_FONT

void UITestFont::UIKitFontTestBackgroundColor()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display background color ");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetBackgroundColorSpan(Color::Red(), 0, 2); // 0 ： start index; 2 : end index
    label->SetBackgroundColorSpan(Color::Yellow(), 5, 6); // 5 ： start index; 6 : end index
    label->SetText("轻量图形子系统");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT + GAP;
}

void UITestFont::UIKitFontTestForegroundColor()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Change some font colors ");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetForegroundColorSpan(Color::Blue(), 1, 3); // 1 ： start index; 2 : end index
    label->SetForegroundColorSpan(Color::Red(), 6, 9); // 6 ： start index; 9 : end index
    label->SetText("轻量图形子\n系统轻量图形子系统轻量图形子系统轻量图形子系统轻量图形子系统轻量图形子系统");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}

void UITestFont::UIKitFontTestLineBackgroundSpan()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle(" Display LineBackgroundSpan ");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetLineBackgroundSpan(Color::Blue(), 1, 3); // 1 ： start index; 3 : end index
    label->SetLineBackgroundSpan(Color::Red(), 5, 5); // 5 ： start index; 5 : end index
    label->SetText("轻量图\n形子系统");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}

void UITestFont::UIKitFontTestAbsoluteSizeSpan()
{
    if (container_ == nullptr) {
            return;
        }
    InnerTestTitle(" Display absolute size span");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("轻量图形子系统\n轻量图形子系统");
    label->SetAbsoluteSizeSpan(1, 2, 38); // 1 ： start index; 2 : end index; 38 absolute font size
    label->SetAbsoluteSizeSpan(2, 3, 18); // 2 ： start index; 3 : end index; 18 absolute font size
    label->SetAbsoluteSizeSpan(3, 4, 24); // 3 ： start index; 4 : end index; 24 absolute font size
    label->SetAbsoluteSizeSpan(10, 11, 14); // 10 ： start index; 11 : end index; 14 font size
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}

void UITestFont::UIKitFontTestRelativeSizeSpan()
{
    if (container_ == nullptr) {
            return;
        }
    InnerTestTitle(" Display Relative size span");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2 : double
#if ENABLE_VECTOR_FONT
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
#else
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#endif
    label->SetText("轻量图形子系统\n轻量图形子系统");
    label->SetRelativeSizeSpan(1, 2, 0.8f); // 1 ： start index; 2 : end index; 0.8f relative font size
    label->SetRelativeSizeSpan(10, 11, 0.6f); // 1 ： start index; 2 : end index; 0.6f relative font size
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double
}

void UITestFont::UIKitFontTestDisplayEmoji001()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("Emoij");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);

#if ENABLE_VECTOR_FONT
    label->Resize(LABEL_WIDTH * 2, LABEL_HEIGHT * 3); // 2 : double; 3 : triple
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    UIFont::GetInstance()->RegisterFontInfo(DEFAULT_EMOJI_FILENAME);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
    label->SetText("💋💋😂😂😂");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 3 + GAP; // 3 : triple
#else
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2);
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#if ENABLE_MULTI_FONT
    uint8_t findPath[] = {F_HWEMOJI_REGULAR_30_32};
    UIMultiFontManager::GetInstance()->SetSearchFontList(F_SOURCEHANSANSSC_REGULAR_30_4, findPath, sizeof(findPath));
#endif
    label->SetText("\xEF\x80\x80\xEF\x80\x81\xEF\x80\x82\xEF\x80\x83\xEF\x80\x84\xEF\x80\x85\xEF\x80\x86\xEF\x80\x87"
                   "\xEF\x80\x80\xEF\x80\x81\xEF\x80\x82\xEF\x80\x83\xEF\x80\x84\xEF\x80\x85\xEF\x80\x86\xEF\x80\x87");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 2 + GAP; // 2 : double;
#endif
}

void UITestFont::UIKitFontTestDisplayEmoji002()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("Text and Emoij");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);

#if ENABLE_VECTOR_FONT
    label->Resize(LABEL_WIDTH * 2, LABEL_HEIGHT * 3); // 2 : double; 3 : triple
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    UIFont::GetInstance()->RegisterFontInfo(DEFAULT_EMOJI_FILENAME);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
    label->SetText("轻量图形子系统💋💋😂😂");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 3 + GAP; // 3 : triple
#else
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 3); // 3 : triple
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#if ENABLE_MULTI_FONT
    uint8_t findPath[] = {F_HWEMOJI_REGULAR_30_32};
    UIMultiFontManager::GetInstance()->SetSearchFontList(F_SOURCEHANSANSSC_REGULAR_30_4, findPath, sizeof(findPath));
#endif
    label->SetText("轻量图形子系统\xEF\x80\x80\xEF\x80\x81\xEF\x80\x82\xEF\x80\x83\xEF\x80\x84鴻蒙");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 3 + GAP; // 3 : triple
#endif
}

void UITestFont::UIKitFontTestDisplayEmoji003()
{
    if (container_ == nullptr) {
        return;
    }
    InnerTestTitle("Text and Emoij line break");
    UILabel* label = new UILabel();
    label->SetPosition(positionX_, positionY_);

#if ENABLE_VECTOR_FONT
    label->Resize(LABEL_WIDTH * 2, LABEL_HEIGHT * 6); // 2 : double; 6 : six times
    UIFont::GetInstance()->RegisterFontInfo(SOURCE_HAN_SANS_SC_REGULAR);
    UIFont::GetInstance()->RegisterFontInfo(DEFAULT_EMOJI_FILENAME);
    label->SetFont(SOURCE_HAN_SANS_SC_REGULAR, FONT_SIZE);
    label->SetText("轻量图形子系统💋😂😂😂您好，轻量图形子系统，鴻蒙😂");
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 6 + GAP; // 6 : six times
#else
    label->Resize(380, LABEL_HEIGHT * 3); // 3 : triple
    label->SetFontId(F_SOURCEHANSANSSC_REGULAR_30_4);
#if ENABLE_MULTI_FONT
    uint8_t findPath[] = {F_HWEMOJI_REGULAR_30_32};
    UIMultiFontManager::GetInstance()->SetSearchFontList(F_SOURCEHANSANSSC_REGULAR_30_4, findPath, sizeof(findPath));
#endif
    label->SetText("轻量图形子系统鴻蒙操作系統\xEF\x80\x80\xEF\x80\x80"); // EF8080
    container_->Add(label);
    positionY_ += LABEL_HEIGHT * 3 + GAP; // 3 : triple
#endif
}
} // namespace OHOS
