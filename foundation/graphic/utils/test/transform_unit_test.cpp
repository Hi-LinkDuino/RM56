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

#include "gfx_utils/transform.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class TransformTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static TransformMap* transform_;
};

TransformMap* TransformTest::transform_ = nullptr;

void TransformTest::SetUpTestCase(void)
{
    if (transform_ == nullptr) {
        transform_ = new TransformMap();
    }
}

void TransformTest::TearDownTestCase(void)
{
    if (transform_ != nullptr) {
        delete transform_;
        transform_ = nullptr;
    }
}

/**
 * @tc.name: TransformSetPolygon_001
 * @tc.desc: Verify SetPolygon function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(TransformTest, TransformSetPolygon_001, TestSize.Level0)
{
    if (transform_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Polygon poly;
    const uint8_t verterNum = 8;
    poly.SetVertexNum(verterNum);
    transform_->SetPolygon(poly);
    EXPECT_EQ(transform_->GetPolygon().GetVertexNum(), verterNum);
}

/**
 * @tc.name: TransformGetPivot_001
 * @tc.desc: Verify GetPivot function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(TransformTest, TransformGetPivot_001, TestSize.Level0)
{
    if (transform_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(transform_->GetPivot().x, 0);
    EXPECT_EQ(transform_->GetPivot().y, 0);
}

/**
 * @tc.name: TransformGetClockWise_001
 * @tc.desc: Verify GetClockWise function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(TransformTest, TransformGetClockWise_001, TestSize.Level0)
{
    if (transform_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    transform_->GetPolygon().SetVertexNum(2); // 2 < VERTEX_NUM_MIN
    EXPECT_EQ(transform_->GetClockWise(), false);
}

/**
 * @tc.name: TransformIsInvalid_001
 * @tc.desc: Verify IsInvalid function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(TransformTest, TransformIsInvalid_001, TestSize.Level0)
{
    if (transform_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(transform_->IsInvalid(), true);
}

/**
 * @tc.name: TransformIsInvalid_002
 * @tc.desc: Verify IsInvalid function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(TransformTest, TransformIsInvalid_002, TestSize.Level0)
{
    TransformMap* transform = new TransformMap(Rect(1, 1, 1, 1));
    if (transform == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(transform->IsInvalid(), false);
    delete transform;
}

/**
 * @tc.name: TransformGetTransMapRect_001
 * @tc.desc: Verify IsInvalid function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(TransformTest, TransformGetTransMapRect_001, TestSize.Level0)
{
    TransformMap* transform = new TransformMap(Rect(1, 1, 1, 1));
    if (transform == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(transform->GetTransMapRect().GetLeft(), 1);
    EXPECT_EQ(transform->GetTransMapRect().GetTop(), 1);
    EXPECT_EQ(transform->GetTransMapRect().GetRight(), 1);
    EXPECT_EQ(transform->GetTransMapRect().GetBottom(), 1);
    delete transform;
}
} // namespace OHOS
