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

#include "layout/grid_layout.h"

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
class GridLayoutTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static GridLayout* gridLayout_;
};

GridLayout* GridLayoutTest::gridLayout_ = nullptr;

void GridLayoutTest::SetUpTestCase(void)
{
    if (gridLayout_ == nullptr) {
        gridLayout_ = new GridLayout();
        gridLayout_->SetPosition(0, 0, 600, 300); // 600: layout width; 300: layout height
        gridLayout_->SetRows(2); // 2: row
        gridLayout_->SetCols(3); // 3: col
    }
}

void GridLayoutTest::TearDownTestCase(void)
{
    if (gridLayout_ != nullptr) {
        delete gridLayout_;
        gridLayout_ = nullptr;
    }
}

/**
 * @tc.name: GridLayout_001
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(GridLayoutTest, GridLayout_001, TestSize.Level1)
{
    if (gridLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    /*
     * 1 2 3
     * 4 5 6
     */
    gridLayout_->SetLayoutDirection(LAYOUT_HOR);
    auto view1 = CreatView();
    gridLayout_->Add(view1);
    auto view2 = CreatView();
    gridLayout_->Add(view2);
    auto view3 = CreatView();
    gridLayout_->Add(view3);
    auto view4 = CreatView();
    gridLayout_->Add(view4);
    gridLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 50);  // 50: view x after layout
    EXPECT_EQ(view1->GetY(), 25);  // 25: view y after layout
    EXPECT_EQ(view2->GetX(), 250); // 250: view x after layout
    EXPECT_EQ(view2->GetY(), view1->GetY());
    EXPECT_EQ(view3->GetX(), 450); // 450: view x after layout
    EXPECT_EQ(view3->GetY(), view1->GetY());
    EXPECT_EQ(view4->GetX(), view1->GetX());
    EXPECT_EQ(view4->GetY(), 175); // 175: view x after layout
    gridLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
    delete view4;
}

/**
 * @tc.name: GridLayout_002
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(GridLayoutTest, GridLayout_002, TestSize.Level1)
{
    if (gridLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    /*
     * 1 3 5
     * 2 4 6
     */
    gridLayout_->SetLayoutDirection(LAYOUT_HOR_R);
    auto view1 = CreatView();
    gridLayout_->Add(view1);
    auto view2 = CreatView();
    gridLayout_->Add(view2);
    auto view3 = CreatView();
    gridLayout_->Add(view3);
    auto view4 = CreatView();
    gridLayout_->Add(view4);
    gridLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 50);  // 50: view x after layout
    EXPECT_EQ(view1->GetY(), 25);  // 25: view y after layout
    EXPECT_EQ(view2->GetX(), view1->GetX());
    EXPECT_EQ(view2->GetY(), 175); // 175: view y afert layout
    EXPECT_EQ(view3->GetX(), 250); // 250: view x after layout
    EXPECT_EQ(view3->GetY(), view1->GetY());
    EXPECT_EQ(view4->GetX(), view3->GetX());
    EXPECT_EQ(view4->GetY(), view2->GetY());
    gridLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
    delete view4;
}

/**
 * @tc.name: GridLayout_003
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(GridLayoutTest, GridLayout_003, TestSize.Level1)
{
    if (gridLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    /*
     * 1 3 5
     * 2 4 6
     */
    gridLayout_->SetLayoutDirection(LAYOUT_VER);
    auto view1 = CreatView();
    gridLayout_->Add(view1);
    auto view2 = CreatView();
    gridLayout_->Add(view2);
    auto view3 = CreatView();
    gridLayout_->Add(view3);
    auto view4 = CreatView();
    gridLayout_->Add(view4);
    gridLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 50);  // 50: view x after layout
    EXPECT_EQ(view1->GetY(), 25);  // 25: view y after layout
    EXPECT_EQ(view2->GetX(), view1->GetX());
    EXPECT_EQ(view2->GetY(), 175); // 175: view y afert layout
    EXPECT_EQ(view3->GetX(), 250); // 250: view x after layout
    EXPECT_EQ(view3->GetY(), view1->GetY());
    EXPECT_EQ(view4->GetX(), view3->GetX());
    EXPECT_EQ(view4->GetY(), view2->GetY());
    gridLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
    delete view4;
}

/**
 * @tc.name: GridLayout_004
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(GridLayoutTest, GridLayout_004, TestSize.Level1)
{
    if (gridLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    /*
     * 1 2 3
     * 4 5 6
     */
    gridLayout_->SetLayoutDirection(LAYOUT_VER_R);
    auto view1 = CreatView();
    gridLayout_->Add(view1);
    auto view2 = CreatView();
    gridLayout_->Add(view2);
    auto view3 = CreatView();
    gridLayout_->Add(view3);
    auto view4 = CreatView();
    gridLayout_->Add(view4);
    gridLayout_->LayoutChildren();
    EXPECT_EQ(view1->GetX(), 50);  // 50: view x after layout
    EXPECT_EQ(view1->GetY(), 25);  // 25: view y after layout
    EXPECT_EQ(view2->GetX(), view1->GetX());
    EXPECT_EQ(view2->GetY(), 175); // 175: view y afert layout
    EXPECT_EQ(view3->GetX(), 250); // 250: view x after layout
    EXPECT_EQ(view3->GetY(), view1->GetY());
    EXPECT_EQ(view4->GetX(), view3->GetX());
    EXPECT_EQ(view4->GetY(), view2->GetY());
    gridLayout_->RemoveAll();
    delete view1;
    delete view2;
    delete view3;
    delete view4;
}
} // namespace OHOS
