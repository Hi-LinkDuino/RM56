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

#include "layout/list_layout.h"

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
class ListLayoutTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static ListLayout* listLayout_;
};

ListLayout* ListLayoutTest::listLayout_ = nullptr;

void ListLayoutTest::SetUpTestCase(void)
{
    if (listLayout_ == nullptr) {
        listLayout_ = new ListLayout();
    }
}

void ListLayoutTest::TearDownTestCase(void)
{
    if (listLayout_ != nullptr) {
        delete listLayout_;
        listLayout_ = nullptr;
    }
}

/**
 * @tc.name: ListLayoutSetDirection_001
 * @tc.desc: Verify SetDirection function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(ListLayoutTest, ListLayoutSetDirection_001, TestSize.Level0)
{
    if (listLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(listLayout_->GetDirection(), 1);

    listLayout_->SetDirection(0);
    EXPECT_EQ(listLayout_->GetDirection(), 0);
}

/**
 * @tc.name: ListLayout_001
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(ListLayoutTest, ListLayout_001, TestSize.Level1)
{
    if (listLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    listLayout_->Resize(600, 300); // 600: layout width; 300: layout height
    EXPECT_EQ(listLayout_->GetHeight(), 300); // 300: set layout height
    listLayout_->SetDirection(ListLayout::VERTICAL);
    auto view1 = CreatView();
    listLayout_->Add(view1);
    auto view2 = CreatView();
    listLayout_->Add(view2);
    EXPECT_EQ(view1->GetX(), 250);  // 250: view x after layout
    EXPECT_EQ(view1->GetY(), 0);
    EXPECT_EQ(view2->GetX(), 250); // 250: view x after layout
    EXPECT_EQ(view2->GetY(), 100);  // 100: view y after layout
    EXPECT_EQ(listLayout_->GetHeight(), 200); // 200: Sum view height
    listLayout_->RemoveAll();
    delete view1;
    delete view2;
}

/**
 * @tc.name: ListLayout_002
 * @tc.desc: Normal Process.
 * @tc.type: FUNC
 * @tc.require: AR000DSMR7
 */
HWTEST_F(ListLayoutTest, ListLayout_002, TestSize.Level1)
{
    if (listLayout_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    listLayout_->Resize(600, 300); // 600: layout width; 300: layout height
    EXPECT_EQ(listLayout_->GetWidth(), 600); // 600: set layout width
    listLayout_->SetDirection(ListLayout::HORIZONTAL);
    auto view1 = CreatView();
    listLayout_->Add(view1);
    auto view2 = CreatView();
    listLayout_->Add(view2);
    EXPECT_EQ(view1->GetX(), 0);
    EXPECT_EQ(view1->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(view2->GetX(), 100); // 100: view x after layout
    EXPECT_EQ(view2->GetY(), 100); // 100: view y after layout
    EXPECT_EQ(listLayout_->GetWidth(), 200); // 200: Sum view height
    listLayout_->RemoveAll();
    delete view1;
    delete view2;
}
} // namespace OHOS
