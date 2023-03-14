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

#include "components/ui_swipe_view.h"

#include <climits>
#include <gtest/gtest.h>
#include "common/screen.h"

using namespace testing::ext;
namespace OHOS {
namespace {
    constexpr uint8_t HORIZONTAL = 0;
    constexpr uint8_t VERTICAL = 1;
    const uint16_t DEFAULT_WIDTH = 100;
}
class UISwipeViewTest : public testing::Test {
public:
    UISwipeViewTest() : swipeView_(nullptr) {}
    virtual ~UISwipeViewTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();
    UISwipeView* swipeView_;
};

void UISwipeViewTest::SetUp()
{
    if (swipeView_ == nullptr) {
        swipeView_ = new UISwipeView();
    }
}

void UISwipeViewTest::TearDown()
{
    if (swipeView_ != nullptr) {
        delete swipeView_;
        swipeView_ = nullptr;
    }
}
/**
 * @tc.name: UISwipeViewGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewGetViewType_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(swipeView_->GetViewType(), UI_SWIPE_VIEW);
}

/**
 * @tc.name: UISwipeViewSetDirection_001
 * @tc.desc: Verify SetDirection function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewSetDirection_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    swipeView_->SetDirection(HORIZONTAL);
    EXPECT_EQ(swipeView_->GetDirection(), HORIZONTAL);

    swipeView_->SetDirection(VERTICAL);
    EXPECT_EQ(swipeView_->GetDirection(), VERTICAL);
}

/**
 * @tc.name: UISwipeViewAdd_001
 * @tc.desc: Verify Add function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewAdd_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    swipeView_->Add(view);
    EXPECT_EQ(view, swipeView_->GetChildrenHead());
    EXPECT_EQ(view->GetParent(), swipeView_);
    swipeView_->Remove(view);
    delete view;
}

/**
 * @tc.name: UISwipeViewInsert_001
 * @tc.desc: Verify Insert function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewInsert_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView* preView = new UIView();
    if (preView == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        delete preView;
        EXPECT_NE(0, 0);
        return;
    }
    swipeView_->Add(preView);
    swipeView_->Insert(preView, view);
    EXPECT_EQ(view, preView->GetNextSibling());

    swipeView_->Remove(preView);
    swipeView_->Remove(view);
    delete view;
    delete preView;
}

/**
 * @tc.name: UISwipeViewRemove_001
 * @tc.desc: Verify Remove function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewRemove_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    swipeView_->Add(view);
    swipeView_->Remove(view);
    EXPECT_EQ(nullptr, swipeView_->GetChildrenHead());
    EXPECT_EQ(nullptr, view->GetParent());

    delete view;
}

/**
 * @tc.name: UISwipeViewSetCurrentPage_001
 * @tc.desc: Verify SetCurrentPage function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewSetCurrentPage_001, TestSize.Level0)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t initPosX = 10;
    const int16_t initPosY = 20;
    const int16_t initWidth = 50;
    const int16_t initHeight = 30;
    const uint16_t size = 100;
    const uint16_t time = 100;

    swipeView_->SetPosition(initPosX, initPosY, initWidth, initHeight);
    swipeView_->SetLoopState(true);
    swipeView_->SetAnimatorTime(time);
    swipeView_->SetBlankSize(size);
    UIViewGroup* view = new UIViewGroup();
    if (view == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIViewGroup* view2 = new UIViewGroup();
    if (view2 == nullptr) {
        delete view;
        EXPECT_NE(0, 0);
        return;
    }

    view->SetPosition(initPosX, initPosY, initWidth, initHeight);
    swipeView_->Add(view);
    view2->SetPosition(initPosX, initPosY, initWidth, initHeight);
    swipeView_->Add(view2);
    swipeView_->SetCurrentPage(1);
    EXPECT_EQ(swipeView_->GetCurrentPage(), 1);

    swipeView_->Remove(view);
    swipeView_->Remove(view2);
    delete view;
    delete view2;
}

class OnTestSwipeListener : public UISwipeView::OnSwipeListener {
public:
    OnTestSwipeListener() {}
    virtual ~OnTestSwipeListener() {}
    void OnSwipe(UISwipeView& view) override
    {
        view.SetWidth(DEFAULT_WIDTH);
        return;
    }
};

class TestUISwipeView : public UISwipeView {
public:
    void TriggerStopAnimator()
    {
        StopAnimator();
    }
};

/**
 * @tc.name: UISwipeViewSetOnSwipeListener_001
 * @tc.desc: Verify SetOnSwipeListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewSetOnSwipeListener_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    auto testSwipeLis = new OnTestSwipeListener();

    EXPECT_EQ(swipeView_->GetOnSwipeListener(), nullptr);
    swipeView_->SetOnSwipeListener(testSwipeLis);
    EXPECT_EQ(swipeView_->GetOnSwipeListener(), testSwipeLis);

    EXPECT_NE(swipeView_->GetWidth(), DEFAULT_WIDTH);
    static_cast<TestUISwipeView *>(swipeView_)->TriggerStopAnimator();
    EXPECT_EQ(swipeView_->GetWidth(), DEFAULT_WIDTH);
    delete testSwipeLis;
}

/**
 * @tc.name: UISwipeViewGetViewByIndex_001
 * @tc.desc: Verify GetViewByIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UISwipeViewTest, UISwipeViewGetViewByIndex_001, TestSize.Level0)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    swipeView_->SetDirection(HORIZONTAL);
    const int16_t initWidth = 50;
    const int16_t initHeight = 30;

    swipeView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipeView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    swipeView_->SetLoopState(true);
    UIView* view1 = new UIView();
    view1->SetPosition(0, 0, initWidth, initHeight);
    swipeView_->Add(view1);
    UIView* view2 = new UIView();
    view2->SetPosition(0, 0, initWidth, initHeight);
    swipeView_->Add(view2);
    UIView* view3 = new UIView();
    view3->SetPosition(0, 0, initWidth, initHeight);
    swipeView_->Add(view3);

    UIView* view = swipeView_->GetViewByIndex(1);
    EXPECT_EQ(view2, view);
    swipeView_->Remove(view1);
    swipeView_->Remove(view2);
    swipeView_->Remove(view3);
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: Graphic_UISwipeView_Test_SetAlignMode_001
 * @tc.desc: Verify SetAlignMode function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVTV4
 */
HWTEST_F(UISwipeViewTest, Graphic_UISwipeView_Test_SetAlignMode_001, TestSize.Level1)
{
    if (swipeView_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    swipeView_->SetAlignMode(UISwipeView::ALIGN_LEFT);
    EXPECT_EQ(swipeView_->GetAlignMode(), UISwipeView::ALIGN_LEFT);
}
} // namespace OHOS
