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

#include "ui_test_list_layout.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "layout/list_layout.h"

namespace OHOS {
namespace {
    static int16_t g_buttonH = 80;
    static int16_t g_buttonW = 200;
    static int16_t g_blank = 20;
    static int16_t g_gap = 8;
}

void UITestListLayout::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
    }
}

void UITestListLayout::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    positionX_ = 0;
    positionY_ = 0;
    listDirect_ = nullptr;
}

const UIView* UITestListLayout::GetTestView()
{
    UIKit_List_Vertical_Add_Test_Auto_Size_001();
    UIKit_List_Vertical_Remove_Test_Auto_Size_001();
    UIKit_List_Vertical_Remove_Test_Auto_Size_002();
    UIKit_List_Hor_Add_Test_Auto_Size_001();
    UIKit_List_Hor_Remove_Test_Auto_Size_001();
    UIKit_List_Hor_Remove_Test_Auto_Size_002();
    return container_;
}

void UITestListLayout::UIKit_List_Vertical_Add_Test_Auto_Size_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst高度自适应 Add Insert");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_ + g_blank);

    ListLayout* list = new ListLayout();
    list->SetDirection(ListLayout::VERTICAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, label->GetY() + TITLE_LABEL_DEFAULT_HEIGHT + g_gap);
    list->Resize(Screen::GetInstance().GetWidth() - 100, 0); // 100: list reduce width
    container_->Add(list);

    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    list->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    list->Insert(button1, button2);

    if (listDirect_ == nullptr) {
        listDirect_ = new UILabel();
    }
    listDirect_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    listDirect_->SetPosition(list->GetX(), list->GetY(), 100, 100); // 100: list dir width and height
    if (list->GetDirection() == ListLayout::VERTICAL) {
        listDirect_->SetText("VERTICAL");
    } else {
        listDirect_->SetText("HROIZONTAL");
    }

    container_->Add(listDirect_);

    SetLastPos(list);
}

void UITestListLayout::UIKit_List_Vertical_Remove_Test_Auto_Size_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst高度自适应 Remove");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_ + g_blank);

    ListLayout* list = new ListLayout(ListLayout::VERTICAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);

    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, label->GetY() + TITLE_LABEL_DEFAULT_HEIGHT + g_gap,
        Screen::GetInstance().GetWidth() - 100, 0);  // 100: list reduce width
    container_->Add(list);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    list->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    list->Add(button2);
    list->Remove(button1);
    SetLastPos(list);
}

void UITestListLayout::UIKit_List_Vertical_Remove_Test_Auto_Size_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst高度自适应 RemoveAll");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_ + g_blank);

    ListLayout* list = new ListLayout(ListLayout::VERTICAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);

    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, label->GetY() + TITLE_LABEL_DEFAULT_HEIGHT + g_gap,
        Screen::GetInstance().GetWidth() - 100, 0); // 100: list reduce width
    container_->Add(list);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    list->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    list->Add(button2);
    list->RemoveAll();
    SetLastPos(list);
}

void UITestListLayout::UIKit_List_Hor_Add_Test_Auto_Size_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst宽度自适应 Add Insert");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_ + g_blank);

    ListLayout* list = new ListLayout(ListLayout::HORIZONTAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);

    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, label->GetY() + TITLE_LABEL_DEFAULT_HEIGHT + g_gap, 0,
                      Screen::GetInstance().GetHeight());
    container_->Add(list);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    list->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    list->Insert(button1, button2);
    SetLastPos(list);
}

void UITestListLayout::UIKit_List_Hor_Remove_Test_Auto_Size_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst宽度自适应 Remove");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_ + g_blank);

    ListLayout* list = new ListLayout(ListLayout::HORIZONTAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);

    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, label->GetY() + TITLE_LABEL_DEFAULT_HEIGHT + g_gap, 0,
                      Screen::GetInstance().GetHeight());
    container_->Add(list);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    list->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    list->Add(button2);
    list->Remove(button1);
    SetLastPos(list);
}

void UITestListLayout::UIKit_List_Hor_Remove_Test_Auto_Size_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UILIst宽度自适应 RemoveAll");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_ + g_blank);

    ListLayout* list = new ListLayout(ListLayout::HORIZONTAL);
    list->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);

    list->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, label->GetY() + TITLE_LABEL_DEFAULT_HEIGHT + g_gap, 0,
                      Screen::GetInstance().GetHeight());
    container_->Add(list);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    list->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    list->Add(button2);
    list->RemoveAll();
    SetLastPos(list);
}

void UITestListLayout::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    positionX_ = view->GetX();
    positionY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS