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

#include "gfx_utils/list.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const uint16_t FIRST_VALUE = 1;
    const uint16_t SECOND_VALUE = 2;
    const uint16_t THIRD_VALUE = 3;
}

class ListTest : public testing::Test {
public:
    ListTest() : list_(nullptr) {}
    ~ListTest() {}
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp();
    void TearDown();

    List<uint16_t>* list_;
};

void ListTest::SetUp()
{
    if (list_ == nullptr) {
        list_ = new List<uint16_t>();
    }
}

void ListTest::TearDown()
{
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }
}
/**
 * @tc.name: ListFront_001
 * @tc.desc: Verify Front function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListFront_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    EXPECT_EQ(list_->Front(), FIRST_VALUE);
}

/**
 * @tc.name: ListBack_001
 * @tc.desc: Verify Back function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListBack_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    EXPECT_EQ(list_->Back(), SECOND_VALUE);
}

/**
 * @tc.name: ListPushBack_001
 * @tc.desc: Verify PushBack function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListPushBack_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    size++;
    list_->PushBack(SECOND_VALUE);
    size++;
    EXPECT_EQ(list_->Size(), size);
    EXPECT_EQ(list_->Front(), FIRST_VALUE);
    EXPECT_EQ(list_->Back(), SECOND_VALUE);
}

/**
 * @tc.name: ListPushFront_001
 * @tc.desc: Verify PushFront function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListPushFront_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushFront(FIRST_VALUE);
    size++;
    list_->PushFront(SECOND_VALUE);
    size++;
    EXPECT_EQ(list_->Size(), size);
    EXPECT_EQ(list_->Front(), SECOND_VALUE);
    EXPECT_EQ(list_->Back(), FIRST_VALUE);
}

/**
 * @tc.name: ListPopBack_001
 * @tc.desc: Verify PopBack function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListPopBack_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    size++;
    list_->PushBack(SECOND_VALUE);
    size++;
    list_->PopBack();
    size--;
    EXPECT_EQ(list_->Size(), size);
    EXPECT_EQ(list_->Front(), FIRST_VALUE);
    EXPECT_EQ(list_->Back(), FIRST_VALUE);
}

/**
 * @tc.name: ListPopFront_001
 * @tc.desc: Verify PopFront function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListPopFront_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    size++;
    list_->PushBack(SECOND_VALUE);
    size++;
    list_->PopFront();
    size--;
    EXPECT_EQ(list_->Size(), size);
    EXPECT_EQ(list_->Front(), SECOND_VALUE);
    EXPECT_EQ(list_->Back(), SECOND_VALUE);
}

/**
 * @tc.name: ListInsert_001
 * @tc.desc: Verify Insert function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListInsert_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    size++;
    list_->PushBack(SECOND_VALUE);
    size++;
    ListNode<uint16_t>* node = list_->Tail();
    list_->Insert(node, THIRD_VALUE);
    size++;
    EXPECT_EQ(list_->Size(), size);
    EXPECT_EQ(list_->Front(), FIRST_VALUE);
    EXPECT_EQ(list_->Head()->next_->data_, THIRD_VALUE);
    EXPECT_EQ(list_->Back(), SECOND_VALUE);
}

/**
 * @tc.name: ListRemove_001
 * @tc.desc: Verify Remove function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListRemove_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    size++;
    list_->PushBack(SECOND_VALUE);
    size++;
    ListNode<uint16_t>* node = list_->Head();
    auto nextNode = list_->Remove(node);
    size--;
    EXPECT_EQ(list_->Size(), size);
    EXPECT_EQ(nextNode->data_, SECOND_VALUE);
    EXPECT_EQ(list_->Front(), SECOND_VALUE);
    EXPECT_EQ(list_->Back(), SECOND_VALUE);
}

/**
 * @tc.name: ListClear_001
 * @tc.desc: Verify Clear function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListClear_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    list_->Clear();
    EXPECT_EQ(list_->Size(), 0);
}

/**
 * @tc.name: ListHead_001
 * @tc.desc: Verify Head function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListHead_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    EXPECT_EQ(list_->Head()->data_, FIRST_VALUE);
}

/**
 * @tc.name: ListTail_001
 * @tc.desc: Verify Tail function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListTail_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    EXPECT_EQ(list_->Tail()->data_, SECOND_VALUE);
}

/**
 * @tc.name: ListBegin_001
 * @tc.desc: Verify Begin function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListBegin_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    EXPECT_EQ(list_->Begin()->data_, FIRST_VALUE);
}

/**
 * @tc.name: ListNext_001
 * @tc.desc: Verify Next function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListNext_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    list_->PushBack(SECOND_VALUE);
    EXPECT_EQ(list_->Next(list_->Head())->data_, SECOND_VALUE);
}

/**
 * @tc.name: ListIsEmpty_001
 * @tc.desc: Verify IsEmpty function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListIsEmpty_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    list_->PushBack(FIRST_VALUE);
    EXPECT_EQ(list_->IsEmpty(), false);
    list_->Clear();
    EXPECT_EQ(list_->IsEmpty(), true);
}

/**
 * @tc.name: ListSize_001
 * @tc.desc: Verify Size function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ListTest, ListSize_001, TestSize.Level0)
{
    if (list_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint16_t size = 0;

    list_->PushBack(FIRST_VALUE);
    size++;
    EXPECT_EQ(list_->Size(), size);
    list_->Clear();
    EXPECT_EQ(list_->Size(), 0);
}
} // namespace OHOS
