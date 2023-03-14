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

#include "components/ui_view_group.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t POS_X = 10;
    const int16_t POS_Y = 20;
    const int16_t DEFAULE_WIDTH = 100;
    const int16_t DEFAULE_HEIGHT = 100;
    const int16_t BUTTON_WIDTH = 20;
    const int16_t BUTTON_HEIGHT = 20;
    const int16_t MARGIN_LEFT = 50;
    const int16_t MARGIN_TOP = 30;
    const int16_t PADDING_LEFT = 10;
    const int16_t PADDING_TOP = 20;
    const int16_t PADDING_RIGHT = 30;
    const int16_t PADDING_BOTTOM = 40;
    const int16_t BORDER_WIDTH = 10;
    const float POS_X_PERCENT = 0.1;
    const float POS_Y_PERCENT = 0.2;
    const float DEFAULT_WIDTH_PERCENT = 0.3;
    const float DEFAULT_HEIGHT_PERCENT = 0.4;
    const float INVALID_PERCENT_ZERO = 0.0f;
    const float INVALID_PERCENT_ONE = 1.0f;
    const int16_t DEFAULE_ANGLE = 45;
    const Vector2<float> VIEW_CENTER = {50, 50};
    const Vector2<float> SCALE_VALUE = {0.5f, 0.5f};
    const Vector2<int16_t> POLYGON1 = {0, 0};
    const Vector2<int16_t> POLYGON2 = {70, 70};
    const Vector2<int16_t> POLYGON3 = {0, 140};
    const Vector2<int16_t> POLYGON4 = {-70, 70};
}

class UIViewTest : public testing::Test {
public:
    UIViewTest() : view_(nullptr) {}
    virtual ~UIViewTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();
    UIView* view_;
};

void UIViewTest::SetUp()
{
    if (view_ == nullptr) {
        view_ = new UIView();
    }
}

void UIViewTest::TearDown()
{
    if (view_ != nullptr) {
        delete view_;
        view_ = nullptr;
    }
}

/**
 * @tc.name: UIViewSetOnDragListener_001
 * @tc.desc: Verify SetOnDragListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetOnDragListener_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView::OnDragListener* listener = new UIView::OnDragListener();
    view_->SetOnDragListener(listener);
    EXPECT_EQ(view_->GetOnDragListener(), listener);
    delete listener;
}

/**
 * @tc.name: UIViewSetOnClickListener_001
 * @tc.desc: Verify SetOnClickListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetOnClickListener_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView::OnClickListener* listener = new UIView::OnClickListener();
    view_->SetOnClickListener(listener);
    EXPECT_EQ(view_->GetOnClickListener(), listener);
    delete listener;
}

/**
 * @tc.name: UIViewSetOnLongPressListener_001
 * @tc.desc: Verify SetOnLongPressListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetOnLongPressListener_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView::OnLongPressListener* listener = new UIView::OnLongPressListener();
    view_->SetOnLongPressListener(listener);
    EXPECT_EQ(view_->GetOnLongPressListener(), listener);
    delete listener;
}

/**
 * @tc.name: UIViewSetOnTouchListener_001
 * @tc.desc: Verify SetOnTouchListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetOnTouchListener_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView::OnTouchListener* listener = new UIView::OnTouchListener();
    view_->SetOnTouchListener(listener);
    EXPECT_EQ(view_->GetTouchListener(), listener);
    delete listener;
}

/**
 * @tc.name: UIViewSetParent_001
 * @tc.desc: Verify SetParent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetParent_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIViewGroup* viewGroup = new UIViewGroup();
    if (viewGroup == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetParent(viewGroup);
    EXPECT_EQ(view_->GetParent(), viewGroup);

    delete viewGroup;
}

/**
 * @tc.name: UIViewSetNextSibling_001
 * @tc.desc: Verify SetNextSibling function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetNextSibling_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIView* tempView = new UIView();
    if (tempView == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetNextSibling(tempView);
    EXPECT_EQ(view_->GetNextSibling(), tempView);

    delete tempView;
}

/**
 * @tc.name: UIViewSetVisible_001
 * @tc.desc: Verify SetVisible function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetVisible_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetVisible(true);
    EXPECT_EQ(view_->IsVisible(), true);
}

/**
 * @tc.name: UIViewSetTouchable_001
 * @tc.desc: Verify SetTouchable function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetTouchable_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetTouchable(true);
    EXPECT_EQ(view_->IsTouchable(), true);
}

/**
 * @tc.name: UIViewSetDraggable_001
 * @tc.desc: Verify SetDraggable function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetDraggable_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetDraggable(true);
    EXPECT_EQ(view_->IsDraggable(), true);
}

/**
 * @tc.name: UIViewSetDragParentInstead_001
 * @tc.desc: Verify SetDragParentInstead function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetDragParentInstead_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetDragParentInstead(true);
    EXPECT_EQ(view_->IsDragParentInstead(), true);
}

/**
 * @tc.name: UIViewResizeVisibleArea_001
 * @tc.desc: Verify ResizeVisibleArea function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewResizeVisibleArea_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->ResizeVisibleArea(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);

    EXPECT_EQ(view_->GetVisibleRect().GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetVisibleRect().GetHeight(), DEFAULE_HEIGHT);
}

/**
 * @tc.name: UIViewSetWidth_001
 * @tc.desc: Verify SetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetWidth_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetWidth(0);
    EXPECT_EQ(view_->GetWidth(), 0);
    view_->SetWidth(DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
}

/**
 * @tc.name: UIViewSetHeight_001
 * @tc.desc: Verify SetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetHeight_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetHeight(0);
    EXPECT_EQ(view_->GetHeight(), 0);
    view_->SetHeight(DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
}

/**
 * @tc.name: UIViewResize_001
 * @tc.desc: Verify Resize function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewResize_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->Resize(0, 0);
    EXPECT_EQ(view_->GetHeight(), 0);
    EXPECT_EQ(view_->GetWidth(), 0);
    view_->Resize(DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
}

/**
 * @tc.name: UIViewSetX_001
 * @tc.desc: Verify SetX function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetX_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetX(0);
    EXPECT_EQ(view_->GetX(), 0);
    view_->SetX(POS_X);
    EXPECT_EQ(view_->GetX(), POS_X);
}

/**
 * @tc.name: UIViewSetY_001
 * @tc.desc: Verify SetY function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetY_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetY(0);
    EXPECT_EQ(view_->GetY(), 0);
    view_->SetY(POS_Y);
    EXPECT_EQ(view_->GetY(), POS_Y);
}

/**
 * @tc.name: UIViewSetPosition_001
 * @tc.desc: Verify SetPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetPosition_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(0, 0);
    EXPECT_EQ(view_->GetX(), 0);
    EXPECT_EQ(view_->GetY(), 0);
    view_->SetPosition(POS_X, POS_Y);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);
}

/**
 * @tc.name: UIViewSetPosition_002
 * @tc.desc: Verify SetPosition function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetPosition_002, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(0, 0, 0, 0);
    EXPECT_EQ(view_->GetHeight(), 0);
    EXPECT_EQ(view_->GetWidth(), 0);
    EXPECT_EQ(view_->GetX(), 0);
    EXPECT_EQ(view_->GetY(), 0);
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);
}

/**
 * @tc.name: UIViewSetWidthPercent_001
 * @tc.desc: Verify SetWidthPercent function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PE8
 */
HWTEST_F(UIViewTest, UIViewSetWidthPercent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, SetWidthPercent failed
    view_->SetWidthPercent(POS_X_PERCENT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);

    // Parent does not SetWidth, view SetWidthPercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->SetWidthPercent(POS_X_PERCENT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, SetWidthPercent failed
    view_->SetWidthPercent(INVALID_PERCENT_ZERO);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    view_->SetWidthPercent(INVALID_PERCENT_ONE);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);

    view_->SetWidthPercent(POS_X_PERCENT);
    EXPECT_EQ(view_->GetWidth(), (int16_t)(DEFAULE_WIDTH * POS_X_PERCENT));

    delete viewGroup;
}

/**
 * @tc.name: UIViewSetHeightPercent_001
 * @tc.desc: Verify SetHeightPercent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4770
 */
HWTEST_F(UIViewTest, UIViewSetHeightPercent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, SetHeightPercent failed
    view_->SetHeightPercent(POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);

    // Parent does not SetHeight, view SetHeightPercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->SetHeightPercent(POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, SetHeightPercent failed
    view_->SetHeightPercent(INVALID_PERCENT_ZERO);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    view_->SetHeightPercent(INVALID_PERCENT_ONE);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);

    view_->SetHeightPercent(POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), (int16_t)(DEFAULE_HEIGHT * POS_Y_PERCENT));

    delete viewGroup;
}

/**
 * @tc.name: UIViewResizePercent_001
 * @tc.desc: Verify ResizePercent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4770
 */
HWTEST_F(UIViewTest, UIViewResizePercent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, ResizePercent failed
    view_->ResizePercent(POS_X_PERCENT, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);

    // Parent does not SetWidth and SetHeight, view ResizePercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->ResizePercent(POS_X_PERCENT, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, ResizePercent failed
    view_->ResizePercent(INVALID_PERCENT_ZERO, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    view_->ResizePercent(POS_X_PERCENT, INVALID_PERCENT_ONE);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);

    view_->ResizePercent(POS_X_PERCENT, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetHeight(), (int16_t)(DEFAULE_HEIGHT * POS_Y_PERCENT));
    EXPECT_EQ(view_->GetWidth(), (int16_t)(DEFAULE_WIDTH * POS_X_PERCENT));

    delete viewGroup;
}

/**
 * @tc.name: UIViewSetXPercent_001
 * @tc.desc: Verify SetXPercent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4770
 */
HWTEST_F(UIViewTest, UIViewSetXPercent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, SetXPercent failed
    view_->SetXPercent(POS_X_PERCENT);
    EXPECT_EQ(view_->GetX(), POS_X);

    // Parent does not SetWidth, view SetXPercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->SetXPercent(POS_X_PERCENT);
    EXPECT_EQ(view_->GetX(), POS_X);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, SetXPercent failed
    view_->SetXPercent(INVALID_PERCENT_ZERO);
    EXPECT_EQ(view_->GetX(), POS_X);
    view_->SetXPercent(INVALID_PERCENT_ONE);
    EXPECT_EQ(view_->GetX(), POS_X);

    view_->SetXPercent(POS_X_PERCENT);
    EXPECT_EQ(view_->GetX(), (int16_t)(POS_X_PERCENT * DEFAULE_WIDTH));

    delete viewGroup;
}

/**
 * @tc.name: UIViewSetYPercent_001
 * @tc.desc: Verify SetYPercent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4770
 */
HWTEST_F(UIViewTest, UIViewSetYPercent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, SetYPercent failed
    view_->SetYPercent(POS_Y_PERCENT);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Parent does not SetHeight, view SetYPercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->SetYPercent(POS_Y_PERCENT);
    EXPECT_EQ(view_->GetY(), POS_Y);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, SetYPercent failed
    view_->SetYPercent(INVALID_PERCENT_ZERO);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->SetYPercent(INVALID_PERCENT_ONE);
    EXPECT_EQ(view_->GetY(), POS_Y);

    view_->SetYPercent(POS_Y_PERCENT);
    EXPECT_EQ(view_->GetY(), (int16_t)(POS_Y_PERCENT * DEFAULE_HEIGHT));

    delete viewGroup;
}

/**
 * @tc.name: UIViewSetPositionPercent_001
 * @tc.desc: Verify SetPositionPercent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4770
 */
HWTEST_F(UIViewTest, UIViewSetPositionPercent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, SetPositionPercent failed
    view_->SetPositionPercent(POS_X_PERCENT, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Parent does not SetWidth, view SetPositionPercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->SetPositionPercent(POS_X_PERCENT, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, SetPositionPercent failed
    view_->SetPositionPercent(INVALID_PERCENT_ZERO, INVALID_PERCENT_ONE);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    view_->SetPositionPercent(POS_X_PERCENT, POS_Y_PERCENT);
    EXPECT_EQ(view_->GetX(), (int16_t)(POS_X_PERCENT * DEFAULE_WIDTH));
    EXPECT_EQ(view_->GetY(), (int16_t)(POS_Y_PERCENT * DEFAULE_HEIGHT));

    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_SetPositionPercent_002
 * @tc.desc: Verify SetPositionPercent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4770
 */
HWTEST_F(UIViewTest, UIViewSetPositionPercent_002, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(POS_X, POS_Y, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Not has parent, SetPositionPercent failed
    view_->SetPositionPercent(POS_X_PERCENT, POS_Y_PERCENT, DEFAULT_WIDTH_PERCENT, DEFAULT_HEIGHT_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    // Parent does not SetWidth, view SetPositionPercent failed
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view_);
    view_->SetPositionPercent(POS_X_PERCENT, POS_Y_PERCENT, DEFAULT_WIDTH_PERCENT, DEFAULT_HEIGHT_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);

    viewGroup->SetWidth(DEFAULE_WIDTH);
    viewGroup->SetHeight(DEFAULE_HEIGHT);

    // Input invalid percent, SetPositionPercent failed
    view_->SetPositionPercent(INVALID_PERCENT_ZERO, INVALID_PERCENT_ONE, DEFAULT_WIDTH_PERCENT, DEFAULT_HEIGHT_PERCENT);
    EXPECT_EQ(view_->GetHeight(), DEFAULE_HEIGHT);
    EXPECT_EQ(view_->GetWidth(), DEFAULE_WIDTH);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);


    view_->SetPositionPercent(POS_X_PERCENT, POS_Y_PERCENT, DEFAULT_WIDTH_PERCENT, DEFAULT_HEIGHT_PERCENT);
    EXPECT_EQ(view_->GetHeight(), (int16_t)(DEFAULE_HEIGHT * DEFAULT_HEIGHT_PERCENT));
    EXPECT_EQ(view_->GetWidth(), (int16_t)(DEFAULE_WIDTH * DEFAULT_WIDTH_PERCENT));
    EXPECT_EQ(view_->GetX(), (int16_t)(POS_X_PERCENT * DEFAULE_WIDTH));
    EXPECT_EQ(view_->GetY(), (int16_t)(POS_Y_PERCENT * DEFAULE_HEIGHT));

    delete viewGroup;
}

/**
 * @tc.name: UIViewSetViewId_001
 * @tc.desc: Verify SetViewId function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetViewId_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* id = "graphic_unittest_view";

    view_->SetViewId(id);
    EXPECT_EQ(memcmp(view_->GetViewId(), id, strlen(id)), 0);
}

/**
 * @tc.name: UIViewSetViewIndex_001
 * @tc.desc: Verify SetViewIndex function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetViewIndex_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    int16_t index = 1;

    view_->SetViewIndex(index);
    EXPECT_EQ(view_->GetViewIndex(), index);
}

/**
 * @tc.name: UIViewGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewGetViewType_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(view_->GetViewType(), UI_NUMBER_MAX);
}

/**
 * @tc.name: UIViewSetOpaScale_001
 * @tc.desc: Verify SetOpaScale function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5D
 */
HWTEST_F(UIViewTest, UIViewSetOpaScale_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    view_->SetOpaScale(OPA_OPAQUE);
    EXPECT_EQ(view_->GetOpaScale(), OPA_OPAQUE);
}

/**
 * @tc.name: UIViewSetOpaScale_002
 * @tc.desc: Verify SetOpaScale function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEB
 */
HWTEST_F(UIViewTest, UIViewSetOpaScale_002, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    uint8_t opaScale = OPA_OPAQUE;

    view_->SetOpaScale(OPA_OPAQUE);
    view_->SetStyle(STYLE_BACKGROUND_OPA, opaScale >> 1);
    view_->SetStyle(STYLE_BORDER_OPA, opaScale >> 1);
    EXPECT_EQ(view_->GetOpaScale(), OPA_OPAQUE);
}

/**
 * @tc.name: UIViewSetStyle_001
 * @tc.desc: Verify SetStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewSetStyle_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    Style style;
    style.borderOpa_ = OPA_OPAQUE;
    view_->SetStyle(style);
    EXPECT_EQ(view_->GetStyleConst().borderOpa_, OPA_OPAQUE);
    view_->SetStyle(STYLE_LINE_OPA, OPA_OPAQUE);
    EXPECT_EQ(view_->GetStyle(STYLE_LINE_OPA), OPA_OPAQUE);
}

/**
 * @tc.name: UIViewLayoutOfParent_001
 * @tc.desc: Verify LayoutOfParent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewLayoutOfParent_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->SetPosition(POS_X, POS_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    viewGroup->Add(view_);
    int16_t centerX = (DEFAULE_WIDTH- BUTTON_WIDTH) / 2; // 2: center of x;
    int16_t centerY = (DEFAULE_HEIGHT- BUTTON_HEIGHT) / 2; // 2: center of y;
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->LayoutCenterOfParent();
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), centerY);
    view_->LayoutLeftOfParent();
    EXPECT_EQ(view_->GetX(), 0);
    EXPECT_EQ(view_->GetY(), centerY);
    view_->LayoutRightOfParent();
    EXPECT_EQ(view_->GetX(), DEFAULE_WIDTH- BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), centerY);
    view_->LayoutTopOfParent();
    EXPECT_EQ(view_->GetX(), DEFAULE_WIDTH- BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), 0);
    view_->LayoutBottomOfParent();
    EXPECT_EQ(view_->GetX(), DEFAULE_WIDTH- BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), DEFAULE_HEIGHT- BUTTON_HEIGHT);

    delete viewGroup;
}

/**
 * @tc.name: UIViewAlignToSibling_001
 * @tc.desc: Verify AlignToSibling function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewAlignToSibling_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->SetPosition(POS_X, POS_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    viewGroup->Add(view_);
    int16_t centerX = (DEFAULE_WIDTH- BUTTON_WIDTH) / 2; // 2: center of x;
    int16_t centerY = (DEFAULE_HEIGHT- BUTTON_HEIGHT) / 2; // 2: center of y;
    UIView* view2 = new UIView();
    view2->SetPosition(centerX, centerY, BUTTON_WIDTH, BUTTON_HEIGHT);
    view2->SetViewId("refView");
    viewGroup->Add(view2);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->AlignLeftToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->AlignRightToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->AlignTopToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), centerY);
    view_->AlignBottomToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), centerY);
    view_->AlignHorCenterToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), centerY);
    view_->AlignVerCenterToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX);
    EXPECT_EQ(view_->GetY(), centerY);

    delete view2;
    delete viewGroup;
}

/**
 * @tc.name: UIViewLayoutToSibling_001
 * @tc.desc: Verify LayoutToSibling function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewTest, UIViewLayoutToSibling_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->SetPosition(POS_X, POS_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    viewGroup->Add(view_);
    int16_t centerX = (DEFAULE_WIDTH- BUTTON_WIDTH) / 2; // 2: center of x;
    int16_t centerY = (DEFAULE_HEIGHT- BUTTON_HEIGHT) / 2; // 2: center of y;
    UIView* view2 = new UIView();
    view2->SetPosition(centerX, centerY, BUTTON_WIDTH, BUTTON_HEIGHT);
    view2->SetViewId("refView");
    viewGroup->Add(view2);
    EXPECT_EQ(view_->GetX(), POS_X);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->LayoutLeftToSibling("refView");
    EXPECT_EQ(view_->GetX(), BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->LayoutRightToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX + BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), POS_Y);
    view_->LayoutTopToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX + BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), BUTTON_HEIGHT);
    view_->LayoutBottomToSibling("refView");
    EXPECT_EQ(view_->GetX(), centerX + BUTTON_WIDTH);
    EXPECT_EQ(view_->GetY(), centerY + BUTTON_HEIGHT);

    delete view2;
    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_Rotate_001
 * @tc.desc: Verify Rotate function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEC
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Rotate_001, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->Rotate(DEFAULE_ANGLE, {0, 0});
    TransformMap& trans = view_->GetTransformMap();
    EXPECT_EQ(view_->GetOrigRect(), trans.GetTransMapRect());
}

/**
 * @tc.name: Graphic_UIView_Test_Rotate_002
 * @tc.desc: Verify Rotate function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5E
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Rotate_002, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->Rotate(DEFAULE_ANGLE, {0, 0});
    Polygon polygon = view_->GetTransformMap().GetPolygon();

    EXPECT_EQ(POLYGON1,  polygon[0]); // 0: polygon num
    EXPECT_EQ(POLYGON2,  polygon[1]); // 1: polygon num
    EXPECT_EQ(POLYGON3,  polygon[2]); // 2: polygon num
    EXPECT_EQ(POLYGON4,  polygon[3]); // 3: polygon num
}

/**
 * @tc.name: Graphic_UIView_Test_Scale_001
 * @tc.desc: Verify Scale function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000ERCQH
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Scale_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->Scale(SCALE_VALUE, VIEW_CENTER);
    TransformMap& trans = view_->GetTransformMap();
    EXPECT_EQ(view_->GetOrigRect(), trans.GetTransMapRect());
}

/**
 * @tc.name: Graphic_UIView_Test_Scale_002
 * @tc.desc: Verify Scale function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2T
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Scale_002, TestSize.Level1)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    view_->Scale(SCALE_VALUE, {0, 0});
    Polygon polygon = view_->GetTransformMap().GetPolygon();
    EXPECT_EQ(0, polygon[0].x_);
    EXPECT_EQ(0, polygon[0].y_);
    EXPECT_EQ(DEFAULE_WIDTH >> 1, polygon[2].x_); // 2: polygon num
    EXPECT_EQ(DEFAULE_HEIGHT >> 1, polygon[2].y_); // 2: polygon num
}

/**
 * @tc.name: Graphic_UIView_Test_Margin_001
 * @tc.desc: Test whether the Rect is normal in the margin scenario.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFP
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Margin_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetStyle(STYLE_MARGIN_LEFT, MARGIN_LEFT);
    view_->SetStyle(STYLE_MARGIN_TOP, MARGIN_TOP);
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    EXPECT_EQ(DEFAULE_WIDTH, view_->GetWidth());
    EXPECT_EQ(DEFAULE_HEIGHT, view_->GetHeight());
    EXPECT_EQ(0, view_->GetX());
    EXPECT_EQ(0, view_->GetY());
    Rect rect = view_->GetRelativeRect();
    EXPECT_EQ(MARGIN_LEFT, rect.GetX());
    EXPECT_EQ(MARGIN_TOP, rect.GetY());
}

/**
 * @tc.name: Graphic_UIView_Test_Padding_001
 * @tc.desc: Test whether the Rect is normal in the setting padding scene.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFP
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Padding_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetStyle(STYLE_PADDING_LEFT, PADDING_LEFT);
    view_->SetStyle(STYLE_PADDING_TOP, PADDING_TOP);
    view_->SetStyle(STYLE_PADDING_RIGHT, PADDING_RIGHT);
    view_->SetStyle(STYLE_PADDING_BOTTOM, PADDING_BOTTOM);
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    Rect rect = view_->GetContentRect();
    EXPECT_EQ(PADDING_LEFT, rect.GetX());
    EXPECT_EQ(PADDING_TOP, rect.GetY());
    EXPECT_EQ(PADDING_LEFT + DEFAULE_WIDTH - 1, rect.GetRight());
    EXPECT_EQ(PADDING_TOP + DEFAULE_HEIGHT - 1, rect.GetBottom());
    rect = view_->GetRelativeRect();
    EXPECT_EQ(0, rect.GetX());
    EXPECT_EQ(0, rect.GetY());
    EXPECT_EQ(PADDING_LEFT + PADDING_RIGHT + DEFAULE_WIDTH - 1, rect.GetRight());
    EXPECT_EQ(PADDING_TOP + PADDING_BOTTOM + DEFAULE_HEIGHT - 1, rect.GetBottom());
}

/**
 * @tc.name: Graphic_UIView_Test_Border_001
 * @tc.desc: Test whether the Rect is normal in the setting of the border scene.
 * @tc.type: FUNC
 * @tc.require: AR000FQNFP
 */
HWTEST_F(UIViewTest, Graphic_UIView_Test_Border_001, TestSize.Level0)
{
    if (view_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    view_->SetStyle(STYLE_BORDER_WIDTH, BORDER_WIDTH);
    view_->SetPosition(0, 0, DEFAULE_WIDTH, DEFAULE_HEIGHT);
    Rect rect = view_->GetContentRect();
    EXPECT_EQ(BORDER_WIDTH, rect.GetX());
    EXPECT_EQ(BORDER_WIDTH, rect.GetY());
    EXPECT_EQ(BORDER_WIDTH + DEFAULE_WIDTH - 1, rect.GetRight());
    EXPECT_EQ(BORDER_WIDTH + DEFAULE_HEIGHT - 1, rect.GetBottom());
    rect = view_->GetRelativeRect();
    EXPECT_EQ(0, rect.GetX());
    EXPECT_EQ(0, rect.GetY());
    EXPECT_EQ(BORDER_WIDTH + BORDER_WIDTH + DEFAULE_WIDTH - 1, rect.GetRight());
    EXPECT_EQ(BORDER_WIDTH + BORDER_WIDTH + DEFAULE_HEIGHT - 1, rect.GetBottom());
}
} // namespace OHOS
