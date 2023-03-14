/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui_test_scroll_bar.h"
#include "components/ui_label_button.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "test_resource_config.h"

namespace {
constexpr int16_t LABEL_WIDTH = 150;
constexpr int16_t LABEL_HEIGHT = 74;
constexpr int16_t LABEL_INTERVAL = 50;
constexpr int16_t LABEL_LEFT_INTERVAL = 125;
constexpr int16_t VIEW_WIDTH = 400;
constexpr int16_t VIEW_HEIGHT = 400;
}
namespace OHOS {
void UITestScrollBar::SetUp()
{
    BaseGfxEngine::GetInstance()->SetScreenShape(ScreenShape::CIRCLE);

    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    if (scrollView_ == nullptr) {
        scrollView_ = new UIScrollView();
    }
    if (list_ == nullptr) {
        list_ = new UIList();
    }
    if (foreImg1_ == nullptr) {
        foreImg1_ = new UIImageView();
    }
    if (foreImg2_ == nullptr) {
        foreImg2_ = new UIImageView();
    }
    if (adapter_ == nullptr) {
        adapter_ = new TextAdapter();
    }
    if (adapterData_ == nullptr) {
        adapterData_ = new List<const char*>();
        adapterData_->PushBack("abcd0");
        adapterData_->PushBack("abcd1");
        adapterData_->PushBack("abcd2");
        adapterData_->PushBack("abcd3");
        adapterData_->PushBack("abcd4");
        adapterData_->PushBack("abcd5");
        adapterData_->PushBack("abcd6");
        adapterData_->PushBack("abcd7");
        adapterData_->PushBack("abcd8");
        adapterData_->PushBack("abcd9");
        adapterData_->PushBack("abcd10");
        adapterData_->PushBack("abcd11");
        adapterData_->PushBack("abcd12");
        adapterData_->PushBack("abcd13");
        adapterData_->PushBack("abcd14");
        adapterData_->PushBack("abcd15");
    }

    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    container_->Add(scrollView_);
    scrollView_->SetPosition(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
    scrollView_->SetYScrollBarVisible(true);
    scrollView_->SetHorizontalScrollState(false);
    scrollView_->SetReboundSize(50); // 50: rebound size
    scrollView_->SetScrollBarCenter({200, 200}); // 200: x, y of center

    container_->Add(list_);
    list_->SetPosition(450, 0, VIEW_WIDTH, VIEW_HEIGHT); // 450: x
    list_->SetYScrollBarVisible(true);
    list_->SetScrollBarSide(SCROLL_BAR_LEFT_SIDE);
    list_->SetReboundSize(50); // 50: rebound size
}

void UITestScrollBar::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    scrollView_ = nullptr;
    list_ = nullptr;
    foreImg1_ = nullptr;
    foreImg2_ = nullptr;
    if (adapterData_ != nullptr) {
        adapterData_->Clear();
        delete adapterData_;
        adapterData_ = nullptr;
    }
    if (adapter_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }

    BaseGfxEngine::GetInstance()->SetScreenShape(ScreenShape::RECTANGLE);
}

const UIView* UITestScrollBar::GetTestView()
{
    UIKit_ScrollBar_TEST_Scroll_view_001();
    UIKit_ScrollBar_TEST_List_001();
    return container_;
}

void UITestScrollBar::UIKit_ScrollBar_TEST_Scroll_view_001()
{
    UILabelButton* label1 = new UILabelButton();
    scrollView_->Add(label1);
    label1->LayoutTopOfParent(LABEL_INTERVAL);
    label1->LayoutLeftOfParent(LABEL_LEFT_INTERVAL);
    label1->SetWidth(LABEL_WIDTH);
    label1->SetHeight(LABEL_HEIGHT);
    label1->SetViewId("label1");
    label1->SetText("label1");

    UILabelButton* label2 = new UILabelButton();
    scrollView_->Add(label2);
    label2->LayoutBottomToSibling("label1", LABEL_INTERVAL);
    label2->LayoutLeftOfParent(LABEL_LEFT_INTERVAL);
    label2->SetWidth(LABEL_WIDTH);
    label2->SetHeight(LABEL_HEIGHT);
    label2->SetViewId("label2");
    label2->SetText("label2");

    UILabelButton* label3 = new UILabelButton();
    scrollView_->Add(label3);
    label3->LayoutBottomToSibling("label2", LABEL_INTERVAL);
    label3->LayoutLeftOfParent(LABEL_LEFT_INTERVAL);
    label3->SetWidth(LABEL_WIDTH);
    label3->SetHeight(LABEL_HEIGHT);
    label3->SetViewId("label3");
    label3->SetText("label3");

    UILabelButton* label4 = new UILabelButton();
    scrollView_->Add(label4);
    label4->LayoutBottomToSibling("label3", LABEL_INTERVAL);
    label4->LayoutLeftOfParent(LABEL_LEFT_INTERVAL);
    label4->SetWidth(LABEL_WIDTH);
    label4->SetHeight(LABEL_HEIGHT);
    label4->SetViewId("label4");
    label4->SetText("label4");

    UILabelButton* label5 = new UILabelButton();
    scrollView_->Add(label5);
    label5->LayoutBottomToSibling("label4", LABEL_INTERVAL);
    label5->LayoutLeftOfParent(LABEL_LEFT_INTERVAL);
    label5->SetWidth(LABEL_WIDTH);
    label5->SetHeight(LABEL_HEIGHT);
    label5->SetViewId("label5");
    label5->SetText("label5");

    foreImg1_->SetSrc(TEST_CIRCLE_FORE_IMAGE);
    container_->Add(foreImg1_);
    foreImg1_->SetPosition(0, 0);
}

void UITestScrollBar::UIKit_ScrollBar_TEST_List_001()
{
    adapter_->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
    adapter_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    adapter_->SetHeight(LABEL_HEIGHT);
    adapter_->SetWidth(VIEW_WIDTH);
    adapter_->SetData(adapterData_);

    list_->SetAdapter(adapter_);
    list_->SetIntercept(true);
    list_->SetYScrollBarVisible(true);

    foreImg2_->SetSrc(TEST_CIRCLE_FORE_IMAGE);
    container_->Add(foreImg2_);
    foreImg2_->SetPosition(450, 0); // 450: x
}
} // namespace OHOS
