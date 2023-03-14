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
class UIViewGroupTest : public testing::Test {
public:
    UIViewGroupTest() : viewGroup_(nullptr) {}
    ~UIViewGroupTest() {}
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp(void);
    void TearDown(void);
    UIViewGroup* viewGroup_;
};

void UIViewGroupTest::SetUp(void)
{
    if (viewGroup_ == nullptr) {
        viewGroup_ = new UIViewGroup();
    }
}

void UIViewGroupTest::TearDown(void)
{
    if (viewGroup_ != nullptr) {
        delete viewGroup_;
        viewGroup_ = nullptr;
    }
}

/**
 * @tc.name: UIViewGroupAdd_001
 * @tc.desc: Verify Add function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupAdd_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    viewGroup_->Add(view);
    EXPECT_EQ(view, viewGroup_->GetChildrenHead());
    EXPECT_EQ(view->GetParent(), viewGroup_);

    delete view;
}

/**
 * @tc.name: UIViewGroupInsert_001
 * @tc.desc: Verify Insert function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupInsert_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* preView = new UIView();
    if (preView == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();

    viewGroup_->Add(preView);
    viewGroup_->Insert(preView, view);
    EXPECT_EQ(view, preView->GetNextSibling());

    delete preView;
    delete view;
}

/**
 * @tc.name: UIViewGroupRemove_001
 * @tc.desc: Verify Remove function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupRemove_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    viewGroup_->Add(view);
    viewGroup_->Remove(view);
    EXPECT_EQ(nullptr, viewGroup_->GetChildrenHead());
    EXPECT_EQ(nullptr, view->GetParent());

    delete view;
}

/**
 * @tc.name: UIViewGroupRemoveAll_001
 * @tc.desc: Verify RemoveAll function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupRemoveAll_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();
    UIView* view2 = new UIView();

    viewGroup_->Add(view);
    viewGroup_->RemoveAll();
    EXPECT_EQ(nullptr, viewGroup_->GetChildrenHead());

    delete view;
    delete view2;
}

/**
 * @tc.name: UIViewGroupGetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupGetViewType_001, TestSize.Level1)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(viewGroup_->GetViewType(), UI_VIEW_GROUP);
}
} // namespace OHOS
