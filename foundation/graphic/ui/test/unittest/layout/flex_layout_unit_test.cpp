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

#include "layout/flex_layout.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
UIView* CreatView()
{
    uint16_t width = 100; // 100  view width
    uint16_t height = 100; // 100 view height
    auto view = new UIView();
    view->Resize(width, height);
    EXPECT_EQ(view->GetX(), 0);
    EXPECT_EQ(view->GetY(), 0);
    return view;
}
}
class FlexLayoutTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static FlexLayout* flexLayout_;
};

FlexLayout* FlexLayoutTest::flexLayout_ = nullptr;

void FlexLayoutTest::SetUpTestCase(void)
{
    if (flexLayout_ == nullptr) {
        flexLayout_ = new FlexLayout();
        flexLayout_->SetPosition(0, 0, 600, 300); // 600: layout width; 300: layout height
    }
}

void FlexLayoutTest::TearDownTestCase(void)
{
    if (flexLayout_ != nullptr) {
        delete flexLayout_;
        flexLayout_ = nullptr;
    }
}

/**
 * @tc.name: FlexLayout_001
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_001, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_HOR);
    flexLayout_->SetMajorAxisAlign(ALIGN_START);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 0);
    EXPECT_EQ(view2->GetX(), 100); // 100: view x after layout
    EXPECT_EQ(view2->GetY(), 0);
    EXPECT_EQ(view3->GetX(), 200); // 200: view x after layout
    EXPECT_EQ(view3->GetY(), 0);
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_002
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_002, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_HOR);
    flexLayout_->SetMajorAxisAlign(ALIGN_START);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_CENTER);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 100); // 100: view x after layout
    EXPECT_EQ(view2->GetX(), 100); // 100: view x after layout
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 200); // 200: view x after layout
    EXPECT_EQ(view3->GetY(), 100); // 100: view y after layout
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_003
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_003, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_HOR_R);
    flexLayout_->SetMajorAxisAlign(ALIGN_START);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_CENTER);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 500); // 500: view x after layout
    EXPECT_EQ(view1->GetY(), 100); // 100: view x after layout
    EXPECT_EQ(view2->GetX(), 400); // 400: view x after layout
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 300); // 300: view x after layout
    EXPECT_EQ(view3->GetY(), 100); // 100: view y after layout
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_004
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_004, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_HOR_R);
    flexLayout_->SetMajorAxisAlign(ALIGN_CENTER);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_CENTER);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 350); // 350: view x after layout
    EXPECT_EQ(view1->GetY(), 100); // 100: view x after layout
    EXPECT_EQ(view2->GetX(), 250); // 250: view x after layout
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 150); // 150: view x after layout
    EXPECT_EQ(view3->GetY(), 100); // 100: view y after layout
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_005
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_005, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_VER);
    flexLayout_->SetMajorAxisAlign(ALIGN_EVENLY);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 0);
    EXPECT_EQ(view2->GetX(), 0);
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 0);
    EXPECT_EQ(view3->GetY(), 200); // 200: view y after layout
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_006
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_006, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_VER);
    flexLayout_->SetMajorAxisAlign(ALIGN_AROUND);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 0);
    EXPECT_EQ(view2->GetX(), 0);
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 0);
    EXPECT_EQ(view3->GetY(), 200); // 200: view y after layout
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_007
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_007, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_VER_R);
    flexLayout_->SetMajorAxisAlign(ALIGN_BETWEEN);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 200);
    EXPECT_EQ(view2->GetX(), 0);
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 0);
    EXPECT_EQ(view3->GetY(), 0);
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_008
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_008, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetLayoutDirection(LAYOUT_VER_R);
    flexLayout_->SetMajorAxisAlign(ALIGN_CENTER);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 200); // 200: view y after layout
    EXPECT_EQ(view2->GetX(), 0);
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view3->GetX(), 0);
    EXPECT_EQ(view3->GetY(), 0);
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
}

/**
 * @tc.name: FlexLayout_009
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_009, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(false);
    flexLayout_->SetPosition(0, 0, 300, 300); // 300: layout width; 300: layout height
    flexLayout_->SetLayoutDirection(LAYOUT_HOR);
    flexLayout_->SetMajorAxisAlign(ALIGN_START);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    auto view4 = CreatView();
    flexLayout_->Add(view4);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 0);
    EXPECT_EQ(view2->GetX(), 100);  // 100: view x after layout
    EXPECT_EQ(view2->GetY(), 0);
    EXPECT_EQ(view3->GetX(), 200);  // 200: view x after layout
    EXPECT_EQ(view3->GetY(), 0);
    EXPECT_EQ(view4->GetX(), 300);  // 300: view x after layout
    EXPECT_EQ(view4->GetY(), 0);
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
    delete view4;
}

/**
 * @tc.name: FlexLayout_010
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(FlexLayoutTest, FlexLayout_010, TestSize.Level1)
{
    if (flexLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    flexLayout_->SetFlexWrap(true);
    flexLayout_->SetPosition(0, 0, 300, 300); // 300: layout width; 300: layout height
    flexLayout_->SetLayoutDirection(LAYOUT_HOR);
    flexLayout_->SetMajorAxisAlign(ALIGN_START);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_START);
    auto view1 = CreatView();
    flexLayout_->Add(view1);
    auto view2 = CreatView();
    flexLayout_->Add(view2);
    auto view3 = CreatView();
    flexLayout_->Add(view3);
    auto view4 = CreatView();
    flexLayout_->Add(view4);
    flexLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 0);
    EXPECT_EQ(view2->GetX(), 100);  // 100: view x after layout
    EXPECT_EQ(view2->GetY(), 0);
    EXPECT_EQ(view3->GetX(), 200);  // 200: view x after layout
    EXPECT_EQ(view3->GetY(), 0);
    EXPECT_EQ(view4->GetX(), 0);
    EXPECT_EQ(view4->GetY(), 100); // 100: view y after layout
    flexLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
    delete view4;
}
} // namespace OHOS
