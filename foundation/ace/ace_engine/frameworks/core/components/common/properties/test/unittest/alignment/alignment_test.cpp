/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "core/components/common/properties/alignment.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double EXTRA_SMALL_LENGTH = 100.0;
constexpr double SMALL_LENGTH = 200.0;
constexpr double MIDDLE_LENGTH = 300.0;
constexpr double LARGE_LENGTH = 400.0;
constexpr double EXTRA_LARGE_LENGTH = 500.0;

} // namespace

class AlignmentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AlignmentTest::SetUpTestCase() {}
void AlignmentTest::TearDownTestCase() {}
void AlignmentTest::SetUp() {}
void AlignmentTest::TearDown() {}

/**
 * @tc.name: GetAlignPosition001
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::TOP_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::TOP_LEFT;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(0, 0).
     * @tc.expected: step3. The offset is Offset(0, 0).
     */
    ASSERT_TRUE(resultOffset == Offset(0, 0));
}

/**
 * @tc.name: GetAlignPosition002
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::TOP_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::TOP_CENTER;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(EXTRA_SMALL_LENGTH, 0).
     * @tc.expected: step3. The offset is Offset(EXTRA_SMALL_LENGTH, 0).
     */
    ASSERT_TRUE(resultOffset == Offset(EXTRA_SMALL_LENGTH, 0));
}

/**
 * @tc.name: GetAlignPosition003
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::TOP_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::TOP_RIGHT;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(SMALL_LENGTH, 0).
     * @tc.expected: step3. The offset is Offset(SMALL_LENGTH, 0).
     */
    ASSERT_TRUE(resultOffset == Offset(SMALL_LENGTH, 0));
}

/**
 * @tc.name: GetAlignPosition004
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::CENTER_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::CENTER_LEFT;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(0, EXTRA_SMALL_LENGTH).
     * @tc.expected: step3. The offset is Offset(0, EXTRA_SMALL_LENGTH).
     */
    ASSERT_TRUE(resultOffset == Offset(0, EXTRA_SMALL_LENGTH));
}

/**
 * @tc.name: GetAlignPosition005
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is set to Alignment::CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::CENTER;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH).
     * @tc.expected: step3. The offset is Offset(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH).
     */
    ASSERT_TRUE(resultOffset == Offset(EXTRA_SMALL_LENGTH, EXTRA_SMALL_LENGTH));
}

/**
 * @tc.name: GetAlignPosition006
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::CENTER_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::CENTER_RIGHT;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(SMALL_LENGTH, EXTRA_SMALL_LENGTH).
     * @tc.expected: step3. The offset is Offset(SMALL_LENGTH, EXTRA_SMALL_LENGTH).
     */
    ASSERT_TRUE(resultOffset == Offset(SMALL_LENGTH, EXTRA_SMALL_LENGTH));
}

/**
 * @tc.name: GetAlignPosition007
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::BOTTOM_LEFT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::BOTTOM_LEFT;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(0, SMALL_LENGTH).
     * @tc.expected: step3. The offset is Offset(0, SMALL_LENGTH).
     */
    ASSERT_TRUE(resultOffset == Offset(0, SMALL_LENGTH));
}

/**
 * @tc.name: GetAlignPosition008
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::BOTTOM_CENTER
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::BOTTOM_CENTER;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(EXTRA_SMALL_LENGTH, SMALL_LENGTH).
     * @tc.expected: step3. The offset is Offset(EXTRA_SMALL_LENGTH, SMALL_LENGTH).
     */
    ASSERT_TRUE(resultOffset == Offset(EXTRA_SMALL_LENGTH, SMALL_LENGTH));
}

/**
 * @tc.name: GetAlignPosition009
 * @tc.desc: Verify that GetAlignPosition can return correct offset value when alignment is Alignment::BOTTOM_RIGHT
 * @tc.type: FUNC
 * @tc.require: AR000DAQUD
 * @tc.author: chenxuankai
 */
HWTEST_F(AlignmentTest, GetAlignPosition009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set parentSize, childSize and alignment
     */
    Size parentSize = Size(LARGE_LENGTH, EXTRA_LARGE_LENGTH);
    Size childSize = Size(SMALL_LENGTH, MIDDLE_LENGTH);
    Alignment alignment = Alignment::BOTTOM_RIGHT;

    /**
     * @tc.steps: step2. call GetAlignPosition to calculate offset.
     */
    Offset resultOffset = Alignment::GetAlignPosition(parentSize, childSize, alignment);

    /**
     * @tc.steps: step3. Verify whether the offset is Offset(SMALL_LENGTH, SMALL_LENGTH).
     * @tc.expected: step3. The offset is Offset(SMALL_LENGTH, SMALL_LENGTH).
     */
    ASSERT_TRUE(resultOffset == Offset(SMALL_LENGTH, SMALL_LENGTH));
}

} // namespace OHOS::Ace