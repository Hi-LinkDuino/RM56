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

#include "components/ui_list.h"
#include "components/text_adapter.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UIListTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIList* list_;
    static TextAdapter* adapter1_;
    static List<const char*>* adapterData1_;
};

UIList* UIListTest::list_ = nullptr;
TextAdapter* UIListTest::adapter1_ = nullptr;
List<const char*>* UIListTest::adapterData1_ = nullptr;

void UIListTest::SetUpTestCase(void)
{
    if (list_ == nullptr) {
        list_ = new UIList(UIList::VERTICAL);
    }
    if (adapterData1_ == nullptr) {
        adapterData1_ = new List<const char*>();
        adapterData1_->PushBack("abcd0");
        adapterData1_->PushBack("abcd1");
        adapterData1_->PushBack("abcd2");
        adapterData1_->PushBack("abcd3");
    }
    if (adapter1_ == nullptr) {
        adapter1_ = new TextAdapter();
    }
}

void UIListTest::TearDownTestCase(void)
{
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }
    if (adapterData1_ != nullptr) {
        adapterData1_->Clear();
        delete adapterData1_;
        adapterData1_ = nullptr;
    }
    if (adapter1_ != nullptr) {
        delete adapter1_;
        adapter1_ = nullptr;
    }
}

/**
 * @tc.name: UIListListScrollListener_001
 * @tc.desc: Verify SetScrollState function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(UIListTest, UIListListScrollListener_001, TestSize.Level1)
{
    auto listener = new ListScrollListener();
    EXPECT_EQ(0, listener->GetScrollState());

    listener->SetScrollState(1);
    EXPECT_EQ(1, listener->GetScrollState());

    delete listener;
}

/**
 * @tc.name: UIListGetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(UIListTest, UIListGetViewType_001, TestSize.Level2)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(UI_LIST, list_->GetViewType());
}

/**
 * @tc.name: UIListSetDirection_001
 * @tc.desc: Verify SetDirection function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(UIListTest, UIListSetDirection_001, TestSize.Level1)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(1, list_->GetDirection());

    list_->SetDirection(0);
    EXPECT_EQ(0, list_->GetDirection());
    list_->SetDirection(1);
    EXPECT_EQ(1, list_->GetDirection());
}

/**
 * @tc.name: UIListSetLoopState_001
 * @tc.desc: Verify SetLoopState function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(UIListTest, UIListSetLoopState_001, TestSize.Level1)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(false, list_->GetLoopState());

    list_->SetLoopState(true);
    EXPECT_EQ(true, list_->GetLoopState());
}

/**
 * @tc.name: UIListSetStartIndex_001
 * @tc.desc: Verify SetStartIndex function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(UIListTest, UIListSetStartIndex_001, TestSize.Level1)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(0, list_->GetStartIndex());

    list_->SetStartIndex(1);
    EXPECT_EQ(1, list_->GetStartIndex());
}

/**
 * @tc.name: UIListSetAutoAlignTime_001
 * @tc.desc: Verify SetAutoAlignTime function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQE
 */
HWTEST_F(UIListTest, UIListSetAutoAlignTime_001, TestSize.Level1)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(100, list_->GetAutoAlignTime()); // 100: DEFAULT_ALINE_TIMES

    uint16_t alignTime = 200;   // 200: align time
    list_->SetAutoAlignTime(alignTime);
    EXPECT_EQ(alignTime, list_->GetAutoAlignTime());
}
} // namespace OHOS
