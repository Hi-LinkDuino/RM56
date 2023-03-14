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

#include "ui_test_qrcode.h"
#include "common/screen.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
const int16_t TITLE_HEIGHT = 29;
} // namespace

void UITestQrcode::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
}

void UITestQrcode::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestQrcode::GetTestView()
{
    UIKitUIQrcodeTestDisplay001();
    UIKitUIQrcodeTestDisplay002();
    UIKitUIQrcodeTestDisplay003();
    UIKitUIQrcodeTestDisplay004();
    UIKitUIQrcodeTestDisplay005();
    UIKitUIQrcodeTestDisplay006();
    UIKitUIQrcodeTestDisplay007();
    UIKitUIQrcodeTestDisplay008();
    UIKitUIQrcodeTestDisplay009();
    return container_;
}

UIViewGroup* UITestQrcode::CreateTestCaseGroup(const char* title) const
{
    UIViewGroup* group = new UIViewGroup();
    group->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group->SetPosition(0, 0);
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(0, 0, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    group->Add(titleLabel);
    return group;
}

void UITestQrcode::UIKitUIQrcodeTestDisplay001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display qrcode 60 * 60");
        group->Resize(Screen::GetInstance().GetWidth(), 100); // 100 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay001");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30, 60, 60); // 20 x 30 y 150 width 60 height
        const char* str = "Hello\n Test of GUI";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
    }
}
void UITestQrcode::UIKitUIQrcodeTestDisplay002()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display qrcode 120 * 250");
        group->Resize(Screen::GetInstance().GetWidth(), 300); // 300 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay002");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30); // 20 x 30 y
        const char* str = "Hello\n Test of GUI";
        qrcode->SetQrcodeInfo(str);
        qrcode->SetWidth(120);  // 120 width
        qrcode->SetHeight(250); // 250 height
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay001", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay003()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display qrcode 400 * 200");
        group->Resize(Screen::GetInstance().GetWidth(), 450); // 450 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay003");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30); // 20 x 30 y
        const char* str =
            "Hello\n GUIddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
        ColorType backgroundColor = Color::Green();
        ColorType qrColor = Color::Blue();
        qrcode->SetQrcodeInfo(str, backgroundColor, qrColor);
        qrcode->SetWidth(400);  // 400 width
        qrcode->SetHeight(200); // 200 height
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay002", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay004()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 50 * 50, width is less than minimum qrcode size, error!");
        group->Resize(Screen::GetInstance().GetWidth(), 100); // 100 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay004");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30, 50, 50); // 20 x 30 y 50 width 50 height
        const char* str =
            "Hello\n GUIddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd \
            dddddddddddddddddddddddddddddddddddddddddddddddddddddd";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay003", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay005()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 250, val length is greater than MAX_LENGTH, error!");
        group->Resize(Screen::GetInstance().GetWidth(), 300); // 300 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay005");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30, 250, 250); // 20 x 30 y 250 width 250 height
        const int16_t length = 2050; // 2050 length
        char text[length];
        for (int16_t i = 0; i < length - 1; i++) {
            text[i] = 'd';
        }
        text[length - 1] = '\0';
        qrcode->SetQrcodeInfo(text);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay004", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay006()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 250, str = \"\" ");
        group->Resize(Screen::GetInstance().GetWidth(), 300); // 300 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay006");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30, 250, 250); // 20 x 30 y 250 width 250 height
        const char* str = "";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay005", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay007()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 250, str = \" \"");
        group->Resize(Screen::GetInstance().GetWidth(), 300); // 300 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay007");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30, 250, 250); // 20 x 30 y 250 width 250 height
        const char* str = " ";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay006", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay008()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 250, str = nullptr");
        group->Resize(Screen::GetInstance().GetWidth(), 300); // 300 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay008");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(20, 30, 250, 250); // 20 x 30 y 250 width 250 height
        const char* str = nullptr;
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay007", 10); // 10: height
    }
}

void UITestQrcode::UIKitUIQrcodeTestDisplay009()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup("qrcode 250 * 250, position x = -50");
        group->Resize(Screen::GetInstance().GetWidth(), 300); // 300 : height
        group->SetViewId("UIKitUIQrcodeTestDisplay009");

        UIQrcode* qrcode = new UIQrcode();
        qrcode->SetPosition(-50, 30, 250, 250); // -50 x 30 y 250 width 250 height
        const char* str = "Hello GUI";
        qrcode->SetQrcodeInfo(str);
        group->Add(qrcode);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitUIQrcodeTestDisplay008", 10); // 10: height
    }
}
} // namespace OHOS
