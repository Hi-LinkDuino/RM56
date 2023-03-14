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

#include "ui_test_vector_font.h"
#if ENABLE_VECTOR_FONT
#include "common/screen.h"
#include "components/ui_label.h"
#if ENABLE_MULTI_FONT
#include "font/ui_multi_font_manager.h"
#endif
namespace {
const int16_t GAP = 5;
const int16_t TITLE_HEIGHT = 29;
const uint16_t LABEL_WIDTH = 200;
const uint16_t TWO = 2;
const uint16_t NINE = 9;
const uint16_t ELEVEN = 11;
const uint16_t THIRTEEN = 13;
const uint16_t SIXTEEN = 16;
const uint16_t LABEL_HEIGHT = 50;
const uint16_t BUF_SIZE = 200;
const uint16_t LANGUAGE_FILE_ID_MAX = 9;
const char* HYQIHEI_65S = "HYQiHei-65S.otf";
const char* NOTONASKHARABIC_REGULAR = "NotoNaskhArabic-Regular.ttf";
const char* ROBOTOCONDENSED_REGULAR = "RobotoCondensed-Regular.ttf";
const char* NOTOSANSCJKJP_REGULAR = "NotoSansCJKjp-Regular.otf";
const char* NOTOSANSTHAI_REGULAR = "NotoSansThai-Regular.ttf";
const char* NOTOSANSHEBREW_REGULAR = "NotoSansHebrew-Regular.ttf";
const char* NOTOSANSDEVANAGARI_REGULAR = "NotoSansDevanagari-Regular.otf";
const char* NOTOSANSMYANMAR_CONDENSED = "NotoSansMyanmar-Condensed.ttf";
const char* NOTOSANSBENGALI_REGULAR = "NotoSansBengali-Regular.ttf";
} // namespace
namespace OHOS {
void UITestVectorFont::SetUp()
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

void UITestVectorFont::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

void UITestVectorFont::InnerTestTitle(const char* title)
{
    UILabel* titleLabel = new UILabel();
    UIFont::GetInstance()->RegisterFontInfo(DEFAULT_VECTOR_FONT_FILENAME);
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(),
                            TITLE_LABEL_DEFAULT_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + GAP;
}

const UIView* UITestVectorFont::GetTestView()
{
#if ENABLE_VECTOR_FONT
    TestDrawText_ITALY_BOLD();
#endif
    Font_FontEngine_Vector_Test_CH_Lang_001();
    Font_FontEngine_Vector_Test_CH_Lang_002();
    Font_FontEngine_Vector_Test_JA_Lang_001();
    Font_FontEngine_Vector_Test_JA_Lang_002();
    Font_FontEngine_Vector_Test_KR_Lang_001();
    Font_FontEngine_Vector_Test_VI_Lang_001();
#if ENABLE_MULTI_FONT
    Font_FontEngine_MultiLanguage_Test_JA_Lang_001();
    Font_FontEngine_MultiLanguage_Test_JA_Lang_002();
    Font_FontEngine_MultiLanguage_Test_KR_Lang_001();
    Font_FontEngine_MultiLanguage_Test_VI_Lang_001();
    Font_FontEngine_MultiLanguage_Test_CS_Lang_001();
    Font_FontEngine_MultiLanguage_Test_EL_Lang_001();
    Font_FontEngine_MultiLanguage_Test_RO_Lang_001();
#endif
    return container_;
}

void UITestVectorFont::Font_FontEngine_Vector_Test_CH_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("CH Language");
        UILabel* label = new UILabel();
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(HYQIHEI_65S, 24); // 24 : size
        label->SetText("测试中文");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_Vector_Test_CH_Lang_002()
{
    if (container_ != nullptr) {
        InnerTestTitle("Multiline text");
        UILabel* label = new UILabel();
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT * 2); // 2：double
        label->SetFont(HYQIHEI_65S, 24); // 24 : size
        label->SetText("测试中英文混合：text和text text和text text和text");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT * 2 + GAP; // 2：double
    }
}

void UITestVectorFont::Font_FontEngine_Vector_Test_JA_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("JA Language");
        UILabel* label = new UILabel();
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("ランニン");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_Vector_Test_JA_Lang_002()
{
    if (container_ != nullptr) {
        InnerTestTitle("JA Language");
        UILabel* label = new UILabel();
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 30); // 30 : size
        label->SetText("ランニン");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_Vector_Test_KR_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("KR Language");
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("韩语리기");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_Vector_Test_VI_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("VI Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(ROBOTOCONDENSED_REGULAR, 24); // 24 : size
        label->SetText("Kết thúc tập luyện?");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

#if ENABLE_MULTI_FONT
void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_JA_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("CH+JA Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(ROBOTOCONDENSED_REGULAR);
        uint8_t findPath[] = {UIFont::GetInstance()->GetFontId(HYQIHEI_65S),
                              UIFont::GetInstance()->GetFontId(ROBOTOCONDENSED_REGULAR)};
        UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(HYQIHEI_65S), findPath,
                                                             sizeof(findPath));
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(HYQIHEI_65S, 24); // 24 : size
        label->SetText("日语ランニン");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_JA_Lang_002()
{
    if (container_ != nullptr) {
        InnerTestTitle("UnRegister JA Language");
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("韩语日语리기ランニン");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_KR_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("CH+KR Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(ROBOTOCONDENSED_REGULAR);
        uint8_t findPath[] = {UIFont::GetInstance()->GetFontId(HYQIHEI_65S),
                              UIFont::GetInstance()->GetFontId(ROBOTOCONDENSED_REGULAR)};
        UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(NOTOSANSCJKJP_REGULAR),
                                                             findPath, sizeof(findPath));
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("韩语리기");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_VI_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("CH+VI Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(ROBOTOCONDENSED_REGULAR);
        uint8_t findPath4[] = {UIFont::GetInstance()->GetFontId(HYQIHEI_65S),
                               UIFont::GetInstance()->GetFontId(ROBOTOCONDENSED_REGULAR)};
        UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(NOTOSANSCJKJP_REGULAR),
                                                             findPath4, sizeof(findPath4));
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("越南语Kết thúc tập luyện?");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_CS_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("CH+CS Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(ROBOTOCONDENSED_REGULAR);
        uint8_t findPath4[] = {UIFont::GetInstance()->GetFontId(HYQIHEI_65S),
                               UIFont::GetInstance()->GetFontId(ROBOTOCONDENSED_REGULAR)};
        UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(NOTOSANSCJKJP_REGULAR),
                                                             findPath4, sizeof(findPath4));
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("捷克语sledování tepové frekvence");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_EL_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("CH+EL Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(ROBOTOCONDENSED_REGULAR);
        uint8_t findPath4[] = {UIFont::GetInstance()->GetFontId(HYQIHEI_65S),
                               UIFont::GetInstance()->GetFontId(ROBOTOCONDENSED_REGULAR)};
        UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(NOTOSANSCJKJP_REGULAR),
                                                             findPath4, sizeof(findPath4));
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("西班牙España");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}

void UITestVectorFont::Font_FontEngine_MultiLanguage_Test_RO_Lang_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("RO Language");
        UIFont::GetInstance()->RegisterFontInfo(NOTOSANSCJKJP_REGULAR);
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(ROBOTOCONDENSED_REGULAR);
        uint8_t findPath4[] = {UIFont::GetInstance()->GetFontId(HYQIHEI_65S),
                               UIFont::GetInstance()->GetFontId(ROBOTOCONDENSED_REGULAR)};
        UIMultiFontManager::GetInstance()->SetSearchFontList(UIFont::GetInstance()->GetFontId(NOTOSANSCJKJP_REGULAR),
                                                             findPath4, sizeof(findPath4));
        UILabel* label = new UILabel();
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
        label->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        label->SetFont(NOTOSANSCJKJP_REGULAR, 24); // 24 : size
        label->SetText("罗马尼亚Română");
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}
#endif
#if ENABLE_VECTOR_FONT
void UITestVectorFont::TestDrawText_ITALY_BOLD()
{
    if (container_ != nullptr) {
        InnerTestTitle("Display bold italic bold_italic");
        UILabel* label = new UILabel();
        UIFont::GetInstance()->RegisterFontInfo(HYQIHEI_65S);
        UIFont::GetInstance()->RegisterFontInfo(DEFAULT_VECTOR_FONT_FILENAME);
        label->SetPosition(positionX_, positionY_);
        label->Resize(LABEL_WIDTH * TWO, LABEL_HEIGHT);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        label->SetText("图形子系统展示正常粗体斜体粗斜体");
        SpannableString spannableString(label->GetText());
        spannableString.SetTextStyle(TextStyle::TEXT_STYLE_ITALIC, ELEVEN, THIRTEEN);
        spannableString.SetTextStyle(TEXT_STYLE_BOLD, NINE, ELEVEN);
        spannableString.SetTextStyle(TEXT_STYLE_BOLD_ITALIC, THIRTEEN, SIXTEEN);
        label->SetText(&spannableString);
        container_->Add(label);
        positionY_ += LABEL_HEIGHT + GAP;
    }
}
#endif
} // namespace OHOS
#endif // ENABLE_VECTOR_FONT
