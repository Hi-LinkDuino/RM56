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

#include "ui_test_opacity.h"
#include "common/screen.h"
#include "components/ui_swipe_view.h"
#include "font/ui_font.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
const int16_t TITLE_HEIGHT = 20;
const uint16_t LABEL_WIDTH = 200;
const uint16_t LABEL_HEIGHT = 50;
const uint16_t BUTTON_WIDTH = 150;
const uint16_t BUTTON_HEIGHT = 160;
const uint16_t CENTER_X = 100;
const uint16_t CENTER_Y = 100;
const uint16_t RADIUS = 80;
static ImageAnimatorInfo g_imageAnimatorInfo[4] = {
    {GREEN_IMAGE_PATH, {50, 50}, 94, 94, IMG_SRC_FILE_PATH},
    {YELLOW_IMAGE_PATH, {50, 50}, 94, 94, IMG_SRC_FILE_PATH},
    {BLUE_IMAGE_PATH, {50, 50}, 94, 94, IMG_SRC_FILE_PATH},
    {RED_IMAGE_PATH, {50, 50}, 94, 94, IMG_SRC_FILE_PATH},
};
static ImageAnimatorInfo g_imageAnimatorInfo2[4] = {
    {GREEN_IMAGE_PATH, {200, 50}, 94, 94, IMG_SRC_FILE_PATH},
    {YELLOW_IMAGE_PATH, {200, 50}, 94, 94, IMG_SRC_FILE_PATH},
    {BLUE_IMAGE_PATH, {200, 50}, 94, 94, IMG_SRC_FILE_PATH},
    {RED_IMAGE_PATH, {200, 50}, 94, 94, IMG_SRC_FILE_PATH},
};
} // namespace

void UITestOpacity::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
}

void UITestOpacity::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

UIViewGroup* UITestOpacity::CreateTestCaseGroup(const char* title) const
{
    UIViewGroup* group = new UIViewGroup();
    group->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    group->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group->SetPosition(0, 0);
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(0, 0, Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    group->Add(titleLabel);
    return group;
}

const UIView* UITestOpacity::GetTestView()
{
    UIKitOpacityTestUILabel001();
    UIKitOpacityTestUILabel002();
    UIKitOpacityTestUIButton001();
    UIKitOpacityTestUIButton002();
    UIKitOpacityTestUILabelButton001();
    UIKitOpacityTestUIArcLabel001();
    UIKitOpacityTestUIImageView001();
    UIKitOpacityTestUIImageAnimator001();
    UIKitOpacityTestUIView001();
    UIKitOpacityTestUIViewGroup001();
    UIKitOpacityTestUIList001();
    UIKitOpacityTestUIScrollView001();
    UIKitOpacityTestUISwipeView001();
    return container_;
}

UILabel* UITestOpacity::CreateTestCaseUILabel(const char* title, uint8_t opaScale) const
{
    UILabel* label = new UILabel();
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    label->SetPosition(10, 30); // 10 width 30: height
    label->Resize(LABEL_WIDTH, LABEL_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 20); // 20:font size
    label->SetText(title);
    label->SetOpaScale(opaScale); // 200: opacity
    return label;
}

void UITestOpacity::UIKitOpacityTestUILabel001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display dynamic text with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), LABEL_HEIGHT + 50); // 50 : height
        group->SetViewId("UIKitOpacityTestUILabel001");
        UILabel* label = CreateTestCaseUILabel("轻量级GUI-200", 200);  // 200: opacity
        UILabel* label2 = CreateTestCaseUILabel("轻量级GUI-100", 100); // 100: opacity
        label2->SetX(220);                                             // 220: width
        group->Add(label);
        group->Add(label2);
        container_->Add(group);
    }
}

void UITestOpacity::UIKitOpacityTestUILabel002()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display dynamic text with opacity 256  and -1, uint8_t");
        group->Resize(Screen::GetInstance().GetWidth(), LABEL_HEIGHT + 50); // 50 : height
        group->SetViewId("UIKitOpacityTestUILabel002");
        UILabel* label = CreateTestCaseUILabel("轻量级GUI-200", 256); // 256: opacity
        UILabel* label2 = CreateTestCaseUILabel("轻量级GUI-100", -1); // -1: opacity
        label2->SetX(220);                                            // 220: width
        group->Add(label);
        group->Add(label2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUILabel001", 10); // 10: height
    }
}

void UITestOpacity::UIKitOpacityTestUIButton001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIButton with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT);
        group->SetViewId("UIKitOpacityTestUIButton001");

        UIButton* button = new UIButton();
        button->SetPosition(10, 30, BUTTON_WIDTH, BUTTON_HEIGHT >> 1); // 10: width 30  :height
        button->SetOpaScale(200);                                      // 200: opacity
        UIButton* button2 = new UIButton();
        button2->SetPosition(200, 30, BUTTON_WIDTH, BUTTON_HEIGHT >> 1); // 200: width 30 : height
        button2->SetOpaScale(100);                                       // 100: opacity

        group->Add(button);
        group->Add(button2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUILabel002", 10); // 10: height
    }
}

UIButton* UITestOpacity::CreateTestCaseUIButton(uint8_t opaScale) const
{
    UIButton* button = new UIButton();
    button->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    button->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    button->SetPosition(10, 30, BUTTON_WIDTH, BUTTON_HEIGHT); // 10 30 x y
    button->SetImageSrc(BLUE_IMAGE_PATH, YELLOW_IMAGE_PATH);
    button->SetOpaScale(opaScale); // 200: opacity
    return button;
}

void UITestOpacity::UIKitOpacityTestUIButton002()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIButton Image with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT + 50); // 50 : height
        group->SetViewId("UIKitOpacityTestUIButton002");

        UIButton* button = CreateTestCaseUIButton(200); // 200: opacity
        group->Add(button);
        UIButton* button2 = CreateTestCaseUIButton(100); // 100: opacity
        button2->SetPosition(200, 30);                   // 200: width 30 y
        group->Add(button2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIButton001", 10); // 10: height
    }
}

UILabelButton* UITestOpacity::CreateTestCaseUILabelButton(const char* title, uint8_t opaScale) const
{
    UILabelButton* labelButton = new UILabelButton();
    labelButton->SetPosition(10, 30, BUTTON_WIDTH, BUTTON_HEIGHT >> 1); // 10 x 30 : y
    labelButton->SetText(title);
    labelButton->SetOpaScale(opaScale);
    return labelButton;
}

void UITestOpacity::UIKitOpacityTestUILabelButton001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UILabelButton with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT);
        group->SetViewId("UIKitOpacityTestUILabelButton001");
        UILabelButton* labelButton = CreateTestCaseUILabelButton("200", 200); // 200 : opacity
        group->Add(labelButton);
        UILabelButton* labelButton2 = CreateTestCaseUILabelButton("100", 100); // 100: opacity
        labelButton2->SetX(200);                                               // 200: width
        group->Add(labelButton2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIButton002", 10); // 10: height
    }
}

UIArcLabel* UITestOpacity::CreateTestCaseUIArcLabel(const char* title, uint8_t opaScale) const
{
    UIArcLabel* label = new UIArcLabel();
    label->SetArcTextCenter(CENTER_X, CENTER_Y + 20); // 20 : height
    label->SetArcTextRadius(RADIUS);
    label->SetArcTextAngle(270, 0);                   // 270: start angle, 0: end angle
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    label->SetText(title);
    label->SetStyle(STYLE_LETTER_SPACE, 30); // 30: space
    label->SetOpaScale(opaScale);            // 200: opacity
    return label;
}
void UITestOpacity::UIKitOpacityTestUIArcLabel001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIArcLabel with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT + 50); // 50 : height
        group->SetViewId("UIKitOpacityTestUIArcLabel001");
        UIArcLabel* label = CreateTestCaseUIArcLabel("01234567", 200); // 200: opacity
        group->Add(label);
        UIArcLabel* label2 = CreateTestCaseUIArcLabel("01234567", 100); // 100: opacity
        label2->SetArcTextCenter(CENTER_X + 200, CENTER_Y + 20);        // 200: width 20 : height
        group->Add(label2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUILabelButton001", 10); // 10 : height
    }
}

UIImageView* UITestOpacity::CreateTestCaseUIImageView(const char* path, uint8_t opaScale) const
{
    UIImageView* imageView = new UIImageView();
    imageView->SetPosition(10, 30, 130, 130); // 10 x 30: y 130 : size
    imageView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    imageView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    imageView->SetSrc(path);
    imageView->SetOpaScale(opaScale); // 200: opacity
    return imageView;
}

void UITestOpacity::UIKitOpacityTestUIImageView001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIImageView with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT + 50); // 50 : height
        group->SetViewId("UIKitOpacityTestUIImageView001");
        UIImageView* imageView = CreateTestCaseUIImageView(BLUE_IMAGE_PATH, 200); // 200: opacity
        group->Add(imageView);
        UIImageView* imageView2 = CreateTestCaseUIImageView(BLUE_ARGB8888_IMAGE_PATH, 100); // 100: opacity
        imageView2->SetX(200);                                                                  // 200: width
        group->Add(imageView2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIArcLabel001", 10); // 10 : height
    }
}

UIImageAnimatorView* UITestOpacity::CreateTestCaseUIImageAnimator(const ImageAnimatorInfo imageAnimatorInfo[],
                                                                  uint8_t opaScale) const
{
    UIImageAnimatorView* imageAnimator = new UIImageAnimatorView();
    imageAnimator->SetPosition(10, 30, 200, 200); // 10 : offset 30 : offset 200 : offset 200: offset
    imageAnimator->SetOpaScale(opaScale);
    imageAnimator->SetImageAnimatorSrc(imageAnimatorInfo, 4, 10); // 4: image number, 10: updating time
    imageAnimator->Start();
    return imageAnimator;
}

void UITestOpacity::UIKitOpacityTestUIImageAnimator001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIImageAnimator with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT + 50); // 50: height
        group->SetViewId("UIKitOpacityTestUIImageAnimator001");
        UIImageAnimatorView* imageAnimator = CreateTestCaseUIImageAnimator(g_imageAnimatorInfo, 200); // 200: opacity
        group->Add(imageAnimator);
        UIImageAnimatorView* imageAnimator2 = CreateTestCaseUIImageAnimator(g_imageAnimatorInfo2, 100); // 100: opacity
        imageAnimator2->SetPosition(230, 30); // 230: width 30: height
        group->Add(imageAnimator2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIImageView001", 10); // 10: height
    }
}
UIView* UITestOpacity::CreateTestCaseUIView(uint8_t opaScale) const
{
    UIView* view = new UIView();
    view->SetPosition(10, 30, BUTTON_WIDTH, BUTTON_HEIGHT); // 10: x 30: y
    view->SetStyle(STYLE_BACKGROUND_COLOR, (Color::White()).full);
    view->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view->SetOpaScale(opaScale);
    return view;
}

void UITestOpacity::UIKitOpacityTestUIView001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIView with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT + 50); // 50 : height
        group->SetViewId("UIKitOpacityTestUIView001");

        UIView* view = CreateTestCaseUIView(200); // 200 : opacity
        group->Add(view);
        UIView* view2 = CreateTestCaseUIView(100); // 100 : opacity
        view2->SetX(200);                          // 200: wdith
        group->Add(view2);

        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIImageAnimator001", 10); // 10 : height
    }
}

void UITestOpacity::UIKitOpacityTestUIViewGroup001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIViewGroup with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), (BUTTON_HEIGHT << 1) + 100); // 100: height
        group->SetViewId("UIKitOpacityTestUIViewGroup001");

        UIViewGroup* group1 = new UIViewGroup();
        group1->SetPosition(10, 30, (BUTTON_WIDTH << 1) + 50, (BUTTON_HEIGHT << 1) + 100); // 30 y 50: width 100: height
        group1->SetStyle(STYLE_BACKGROUND_COLOR, (Color::Red()).full);
        UIViewGroup* group2 = new UIViewGroup();
        group2->SetPosition(170, 250, 150, 150); // 170: x 250: y 150: width 150: height
        group2->SetStyle(STYLE_BACKGROUND_COLOR, (Color::White()).full);

        UILabelButton* labelButton = new UILabelButton();
        labelButton->SetPosition(10, 10, 60, 30); // 10: x y 60 wdith 30 height
        labelButton->SetText("Test1");
        UILabelButton* labelButton2 = new UILabelButton();
        labelButton2->SetPosition(170, 10, 60, 30); // 170: x 10: y 60 wdith 30 height
        labelButton2->SetText("Test2");
        labelButton2->SetOpaScale(100); // 100: opacity
        UIButton* button = new UIButton();
        button->SetPosition(10, 60, BUTTON_WIDTH, BUTTON_HEIGHT); // 10: x 60: y
        UIButton* button2 = new UIButton();
        button2->SetPosition(170, 60, BUTTON_WIDTH, BUTTON_HEIGHT); // 170: x 60: y
        button2->SetImageSrc(BLUE_IMAGE_PATH, YELLOW_IMAGE_PATH);
        button2->SetOpaScale(100); // 100: opacity
        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(10, 250, 50, 50); // 10: x 250: y 50 : size
        imageView->SetSrc(BLUE_IMAGE_PATH);
        imageView->SetOpaScale(100); // 100: opacity
        UIImageView* imageView2 = new UIImageView();
        imageView2->SetPosition(10, 10, 50, 50); // 10: x y 50 : size
        imageView2->SetSrc(YELLOW_IMAGE_PATH);

        group2->Add(imageView2);
        group1->Add(labelButton);
        group1->Add(labelButton2);
        group1->Add(button);
        group1->Add(button2);
        group1->Add(imageView);
        group1->Add(group2);
        group2->SetOpaScale(100); // 100: opacity
        group1->SetOpaScale(200); // 200: opacity

        group->Add(group1);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIView001", 10); // 10 : height
    }
}

UIList* UITestOpacity::CreateTestCaseUIList(uint8_t opaScale) const
{
    List<const char*>* adapterData = new List<const char*>();
    adapterData->PushBack("000");
    adapterData->PushBack("111");
    adapterData->PushBack("222");
    adapterData->PushBack("333");
    adapterData->PushBack("444");
    adapterData->PushBack("555");
    adapterData->PushBack("666");
    adapterData->PushBack("777");
    adapterData->PushBack("888");
    adapterData->PushBack("999");

    TextAdapter* adapter = new TextAdapter();
    adapter->SetData(adapterData);
    UIList* list = new UIList(UIList::VERTICAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    list->SetPosition(10, 30, BUTTON_WIDTH, BUTTON_HEIGHT); // 10 x 30 y
    list->SetAdapter(adapter);
    list->SetOpaScale(opaScale); // 200: opacity
    return list;
}

void UITestOpacity::UIKitOpacityTestUIList001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIList with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT + 100); // 100: height
        group->SetViewId("UIKitOpacityTestUIList001");
        UIList* list = CreateTestCaseUIList(200); // 200 : opacity
        group->Add(list);
        UIList* list2 = CreateTestCaseUIList(100); // 100 : opacity
        list2->SetX(200);                          // 200: width
        group->Add(list2);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIViewGroup001", 10); // 10 : height
    }
}

void UITestOpacity::UIKitOpacityTestUIScrollView001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UIScrollView with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT << 1);
        group->SetViewId("UIKitOpacityTestUIScrollView001");

        UIScrollView* group1 = new UIScrollView();
        group1->SetPosition(10, 30, (BUTTON_WIDTH << 1) + 50, (BUTTON_HEIGHT) + 100); // 10 x 30 y 50: width 100: height
        group1->SetStyle(STYLE_BACKGROUND_COLOR, (Color::Red()).full);
        UIScrollView* group2 = new UIScrollView();
        group2->SetPosition(170, 80, 150, 150); // 170: x 80: y 150: width 150: height
        group2->SetStyle(STYLE_BACKGROUND_COLOR, (Color::White()).full);
        UILabelButton* labelButton = new UILabelButton();
        labelButton->SetPosition(10, 10, 80, 40); // 10 x y 80: width 40: height
        labelButton->SetText("test1");
        UILabelButton* labelButton2 = new UILabelButton();
        labelButton2->SetPosition(170, 10, 80, 40); // 170: x 10: y 80: width 40: height
        labelButton2->SetText("test2");
        labelButton2->SetOpaScale(100); // 100: opacity
        UIImageView* imageView = new UIImageView();
        imageView->SetPosition(10, 80, 50, 50); // 10: x 80: y 50 : size
        imageView->SetSrc(BLUE_IMAGE_PATH);
        imageView->SetOpaScale(100); // 100: opacity
        UIImageView* imageView2 = new UIImageView();
        imageView2->SetPosition(10, 10, 50, 50); // 10: x y 50 : size
        imageView2->SetSrc(YELLOW_IMAGE_PATH);

        group2->Add(imageView2);
        group1->Add(labelButton);
        group1->Add(labelButton2);
        group1->Add(imageView);
        group1->Add(group2);
        group2->SetOpaScale(100); // 100: opacity
        group1->SetOpaScale(200); // 200: opacity
        group->Add(group1);
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIList001", 10); // 10 : height
    }
}

void UITestOpacity::UIKitOpacityTestUISwipeView001()
{
    if (container_ != nullptr) {
        UIViewGroup* group = CreateTestCaseGroup(" display UISwipeView with opacity 200 and 100");
        group->Resize(Screen::GetInstance().GetWidth(), BUTTON_HEIGHT);
        group->SetViewId("UIKitOpacityTestUISwipeView001");

        UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
        swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        swipe->SetPosition(10, 30, BUTTON_WIDTH << 1, BUTTON_HEIGHT); // 10 30 size
        swipe->SetBlankSize(50);                                      // 50: is blank size
        group->Add(swipe);

        UILabelButton* button1 = new UILabelButton();
        button1->SetPosition(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT >> 1);
        button1->SetText("button1");
        swipe->Add(button1);

        UILabelButton* button2 = new UILabelButton();
        button2->SetPosition(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT >> 1);
        button2->SetText("button2");
        button2->SetOpaScale(200); // 200: opacity
        swipe->Add(button2);

        UILabelButton* button3 = new UILabelButton();
        button3->SetPosition(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT >> 1);
        button3->SetText("button3");
        button3->SetOpaScale(100); // 100: opacity
        swipe->Add(button3);

        swipe->SetOpaScale(200); // 200: opacity
        container_->Add(group);
        group->LayoutBottomToSibling("UIKitOpacityTestUIScrollView001", 10); // 10 : height
    }
}
} // namespace OHOS
