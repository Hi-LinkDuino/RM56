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

#include "gfx_utils/vector.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS::Graphic;

namespace OHOS {
namespace {
const uint16_t FIRST_VALUE = 1;
const uint16_t SECOND_VALUE = 2;
const uint16_t THIRD_VALUE = 3;
} // namespace

class VectorTest : public testing::Test {
public:
    VectorTest() = default;
    ~VectorTest() = default;
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp();
    void TearDown();

    Vector<uint16_t>* vector_ = nullptr;
};

void VectorTest::SetUp()
{
    if (vector_ == nullptr) {
        vector_ = new Vector<uint16_t>();
    }
}

void VectorTest::TearDown()
{
    if (vector_ != nullptr) {
        delete vector_;
        vector_ = nullptr;
    }
}
/**
 * @tc.name: VectorFront_001
 * @tc.desc: Verify Front function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorFront_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    vector_->PushBack(FIRST_VALUE);
    vector_->PushBack(SECOND_VALUE);
    EXPECT_EQ(vector_->Front(), FIRST_VALUE);
}

/**
 * @tc.name: VectorBack_001
 * @tc.desc: Verify Back function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorBack_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    vector_->PushBack(FIRST_VALUE);
    vector_->PushBack(SECOND_VALUE);
    EXPECT_EQ(vector_->Back(), SECOND_VALUE);
}

/**
 * @tc.name: VectorPushBack_001
 * @tc.desc: Verify PushBack/PopBack function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorPushBack_001, TestSize.Level0)
{
    uint16_t size = 0;
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    vector_->PushBack(FIRST_VALUE);
    size++;
    vector_->PushBack(SECOND_VALUE);
    size++;
    EXPECT_EQ(vector_->Size(), size);
    EXPECT_EQ(vector_->Front(), FIRST_VALUE);
    EXPECT_EQ(vector_->Back(), SECOND_VALUE);

    vector_->PopBack();
    size--;
    EXPECT_EQ(vector_->Size(), size);
    EXPECT_EQ(vector_->Front(), FIRST_VALUE);
    EXPECT_EQ(vector_->Back(), FIRST_VALUE);
}

/**
 * @tc.name: VectorClear_001
 * @tc.desc: Verify IsEmpty/Clear/Size function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorClear_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    uint16_t size = 0;
    EXPECT_TRUE(vector_->IsEmpty());

    vector_->PushBack(FIRST_VALUE);
    ++size;
    vector_->PushBack(SECOND_VALUE);
    ++size;
    EXPECT_EQ(vector_->Size(), size);
    EXPECT_FALSE(vector_->IsEmpty());

    vector_->Clear();
    EXPECT_TRUE(vector_->IsEmpty());
    EXPECT_EQ(vector_->Size(), 0);
}

/**
 * @tc.name: VectorBegin_001
 * @tc.desc: Verify Begin/End function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorBegin_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    vector_->PushBack(FIRST_VALUE);
    vector_->PushBack(SECOND_VALUE);
    EXPECT_EQ(*(vector_->Begin()), FIRST_VALUE);
    auto p = vector_->End();
    EXPECT_EQ((--p)[0], SECOND_VALUE);
}

/**
 * @tc.name: VectorSize_001
 * @tc.desc: Verify Size function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorSize_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    uint16_t size = 0;

    vector_->PushBack(FIRST_VALUE);
    size++;
    EXPECT_EQ(vector_->Size(), size);
    vector_->Clear();
    EXPECT_EQ(vector_->Size(), 0);
}

/**
 * @tc.name: VectorCapacity_001
 * @tc.desc: Verify Capacity/ReSize function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorCapacity_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    uint16_t capacity = 1;
    uint16_t size = capacity;

    vector_->PushBack(FIRST_VALUE);
    EXPECT_EQ(vector_->Capacity(), capacity);
    vector_->PushBack(SECOND_VALUE);
    capacity <<= 1; // 1: capacity double
    EXPECT_EQ(vector_->Capacity(), capacity);
    vector_->PushBack(THIRD_VALUE);
    capacity <<= 1; // 1: capacity double
    EXPECT_EQ(vector_->Capacity(), capacity);

    EXPECT_EQ(vector_->ReSize(size), size);
}

/**
 * @tc.name: VectorErase_001
 * @tc.desc: Verify Erase function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorErase_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    uint16_t idx = 0;

    vector_->PushBack(FIRST_VALUE);
    idx++;
    vector_->PushBack(SECOND_VALUE);
    vector_->PushBack(THIRD_VALUE);

    vector_->Erase(idx); // remove SECOND_VALUE
    EXPECT_EQ((*vector_)[idx], THIRD_VALUE);
    EXPECT_EQ(vector_->Size(), ++idx);
}

/**
 * @tc.name: VectorSwap_001
 * @tc.desc: Verify Swap function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000FCKJR
 */
HWTEST_F(VectorTest, VectorSwap_001, TestSize.Level0)
{
    if (vector_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    vector_->PushBack(FIRST_VALUE);
    Vector<uint16_t> copy;
    copy = (*vector_);

    vector_->PushBack(SECOND_VALUE);
    copy.Swap(*vector_);

    EXPECT_EQ(copy.Size(), SECOND_VALUE);
    EXPECT_EQ(copy[0], FIRST_VALUE);
    EXPECT_EQ(copy[1], SECOND_VALUE);
    EXPECT_EQ(vector_->Size(), FIRST_VALUE);
    EXPECT_EQ((*vector_)[0], FIRST_VALUE);
}
} // namespace OHOS
