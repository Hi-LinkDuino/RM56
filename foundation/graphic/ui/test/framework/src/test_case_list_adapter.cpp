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

#include "test_case_list_adapter.h"
#include "common/screen.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "gfx_utils/list.h"
#include "test_resource_config.h"
#include "ui_test.h"
#include "ui_test_group.h"

namespace OHOS {
namespace {
const uint16_t TESTCASE_BUTTON_HEIGHT = 64;
const uint16_t STYLE_BORDER_WIDTH_VALUE = 4;
const uint16_t STYLE_BORDER_RADIUS_VALUE = 12;
const char* g_uiTestId = "graphic_ui_test_case_id";
} // namespace

uint16_t TestCaseListAdapter::GetCount()
{
    return UITestGroup::GetTestCase().Size();
}

class BtnOnClickBackListener : public UIView::OnClickListener {
public:
    BtnOnClickBackListener(UIViewGroup* uiView,
                           UIViewGroup* mainMenu,
                           UITest* uiTest,
                           UILabel* testCaseLabel)
        : rootView_(uiView), mainMenu_(mainMenu), uiTest_(uiTest), testCaseLabel_(testCaseLabel)
    {
    }

    ~BtnOnClickBackListener() {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if ((rootView_ == nullptr) || (mainMenu_ == nullptr) || (uiTest_ == nullptr) || (testCaseLabel_ == nullptr)) {
            return false;
        }

        rootView_->Remove(testCaseLabel_);
        rootView_->Remove(&view);

        UIView* tempView = rootView_->GetChildById(g_uiTestId);
        if (tempView != nullptr) {
            rootView_->Remove(tempView);
            uiTest_->TearDown();
        }

        rootView_->Add(mainMenu_);
        rootView_->Invalidate();
        return true;
    }

private:
    UIViewGroup* rootView_;
    UIViewGroup* mainMenu_;
    UILabel* testCaseLabel_;
    UITest* uiTest_;
};

class BtnOnClickUiTestListener : public UIView::OnClickListener {
public:
    BtnOnClickUiTestListener(UIViewGroup* uiView,
                             UIViewGroup* mainMenu_,
                             UILabelButton* backBtn,
                             TestCaseInfo* uiTestInfo,
                             UILabel* testCaseLabel)
        : rootView_(uiView),
          mainMenu_(mainMenu_),
          backBtn_(backBtn),
          testCaseLabel_(testCaseLabel),
          uiTest_(nullptr),
          sliceId_(nullptr)
    {
        if (uiTestInfo != nullptr) {
            uiTest_ = uiTestInfo->testObj;
            sliceId_ = uiTestInfo->sliceId;
        }
    }
    ~BtnOnClickUiTestListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if ((rootView_ == nullptr) || (mainMenu_ == nullptr) || (backBtn_ == nullptr) || (testCaseLabel_ == nullptr) ||
            (uiTest_ == nullptr) || (sliceId_ == nullptr)) {
            return false;
        }
        rootView_->Remove(mainMenu_);

        UIView::OnClickListener* click = backBtn_->GetOnClickListener();
        if (click != nullptr) {
            delete click;
            click = nullptr;
        }
        click = new BtnOnClickBackListener(rootView_, mainMenu_, uiTest_, testCaseLabel_);
        backBtn_->SetOnClickListener(click);
        rootView_->Add(backBtn_);
        if (testCaseLabel_ != nullptr) {
            testCaseLabel_->SetText(sliceId_);
        }
        rootView_->Add(testCaseLabel_);

        uiTest_->SetUp();
        UIView* tempView = const_cast<UIView*>(uiTest_->GetTestView());
        if (tempView != nullptr) {
            tempView->SetViewId(g_uiTestId);
            tempView->SetPosition(tempView->GetX(), tempView->GetY() + backBtn_->GetHeight());
            rootView_->Add(tempView);
        }
        rootView_->Invalidate();
        return true;
    }

private:
    UIViewGroup* rootView_;
    UIViewGroup* mainMenu_;
    UILabelButton* backBtn_;
    UITest* uiTest_;
    UILabel* testCaseLabel_;
    const char* sliceId_;
};

UIView* TestCaseListAdapter::GetView(UIView* inView, int16_t index)
{
    List<TestCaseInfo> testCaseList = UITestGroup::GetTestCase();
    if (testCaseList.IsEmpty()) {
        return nullptr;
    }
    if ((index > testCaseList.Size() - 1) || (index < 0)) {
        return nullptr;
    }
    UILabelButton* item = nullptr;
    if (inView == nullptr) {
        item = new UILabelButton();
        item->SetPosition(0, 0);
        item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::RELEASED);
        item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::PRESSED);
        item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::INACTIVE);
        item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::RELEASED);
        item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::PRESSED);
        item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::INACTIVE);
        item->Resize(Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE, TESTCASE_BUTTON_HEIGHT);
    } else {
        item = static_cast<UILabelButton*>(inView);
    }

    UIView::OnClickListener* listener = item->GetOnClickListener();
    if (listener != nullptr) {
        delete listener;
        listener = nullptr;
    }
    ListNode<TestCaseInfo>* node = testCaseList.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }
    listener = new BtnOnClickUiTestListener(rootView_, mainMenu_, backBtn_, &node->data_, testCaseLabel_);
    item->SetOnClickListener(listener);
    item->SetText(node->data_.sliceId);
    item->SetViewId(node->data_.sliceId);
    item->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 24); // 24: means font size
    item->SetViewIndex(index);
    item->SetAlign(TEXT_ALIGNMENT_LEFT);
    item->SetLabelPosition(24, 0); // 24: lable x-coordinate
    item->SetImageSrc(TEST_RIGHT_ARROW, TEST_RIGHT_ARROW);
    // 2: half of button height; 18: half px of image height
    item->SetImagePosition(item->GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE, TESTCASE_BUTTON_HEIGHT / 2 - 18);
    item->SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    item->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    return item;
}

int16_t TestCaseListAdapter::GetItemWidthWithMargin(int16_t index)
{
    // 2: two borders on both sides
    return Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE + STYLE_BORDER_WIDTH_VALUE * 2;
}

int16_t TestCaseListAdapter::GetItemHeightWithMargin(int16_t index)
{
    return TESTCASE_BUTTON_HEIGHT + STYLE_BORDER_WIDTH_VALUE * 2; // 2: two borders on both sides
}
} // namespace OHOS
