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

#include "base/geometry/dimension.h"
#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/wrap/wrap_test_utils.h"
#include "core/components/wrap/wrap_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double BOX_WIDTH = 500;
constexpr double BOX_HEIGHT = 40;
constexpr double SMALL_BOX_WIDTH = 100;
constexpr double SMALL_BOX_HEIGHT = 100;
constexpr double MID_BOX_HEIGHT = 500;
constexpr double MID_BOX_WIDTH = 500;
constexpr double MID_LARGE_BOX_HEIGHT = 1000;

constexpr double BIG_BOX_WIDTH = 2000;
constexpr double BIG_BOX_HEIGHT = 2000;
constexpr double SPACE = 20;
constexpr double CROSS_LENGTH = 20;
constexpr double MAIN_LENGTH = 80;

constexpr double ALIGNMENT_OFFSET_VALUE = 290;
constexpr double SPACE_BETWEEN_OFFSET = 80;
constexpr double SPACE_AROUND_OFFSET_WIDTH = 560;
constexpr double SPACE_AROUND_OFFSET = 290;

// constants for test18
constexpr double VERTICAL_SPACE_AROUND_OFFSET = 1424;
constexpr double VERTICAL_SPACE_AROUND_OFFSET_HEIGHT = 1948;

// constants for test19
constexpr double FIRST_BOX_HEIGHT_OFFSET = 12;
constexpr double SECOND_BOX_HEIGHT_OFFSET = 1036;
constexpr double THIRD_BOX_HEIGHT_OFFSET = 524;

} // namespace

class RenderWrapTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RenderWrapPerformLayout001
 * @tc.desc: Wrap component layout children correctly in horizontal
 * @tc.type: FUNC
 * @tc.require: AR000DBAOR
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 40], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. call PerformLayout interface
     * @tc.expected: step3. the positions of three boxes are set correctly
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(BOX_WIDTH, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout002
 * @tc.desc: Wrap component layout children correctly in vertical
 * @tc.type: FUNC
 * @tc.require: AR000DBAOR
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 40], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    wrap->Attach(mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the positions of three boxes are set correctly
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, BOX_HEIGHT + BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout003
 * @tc.desc: Wrap component layout children correctly when the constrain is smaller than requested
 * @tc.type: FUNC
 * @tc.require: AR000DBAOR
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [2000, 2000], [2000, 2000], [2000, 2000]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BIG_BOX_WIDTH, BIG_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BIG_BOX_WIDTH, BIG_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BIG_BOX_WIDTH, BIG_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and box size
     * @tc.expected: step3. the position and size of three boxes are set correctly
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, BIG_BOX_HEIGHT));

    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SCREEN_WIDTH, BIG_BOX_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(SCREEN_WIDTH, BIG_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(SCREEN_WIDTH, BIG_BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout004
 * @tc.desc: Wrap component layout children in vertical correctly when the constrain is smaller than requested
 * @tc.type: FUNC
 * @tc.require: AR000DBAOR
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [2000, 2000], [2000, 2000], [2000, 2000]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BIG_BOX_WIDTH, BIG_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BIG_BOX_WIDTH, BIG_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BIG_BOX_WIDTH, BIG_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and box size
     * @tc.expected: step3. the position and size of three boxes are set correctly
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());

    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(SCREEN_WIDTH, BIG_BOX_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(SCREEN_WIDTH, BIG_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(SCREEN_WIDTH, BIG_BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout005
 * @tc.desc: Wrap component layout children in horizontal correctly when the constrain is smaller than requested
 * @tc.type: FUNC
 * @tc.require: AR000DBAOR
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [100, 100]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(SMALL_BOX_WIDTH, SMALL_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and check the size of whole wrap
     * @tc.expected: step3. the position and size of two boxes are set correctly and wrap size match
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(BOX_WIDTH, 0));

    EXPECT_TRUE(wrap->GetLayoutSize() == Size(SCREEN_WIDTH, SMALL_BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout006
 * @tc.desc: Wrap component layout children in horizontal correctly and wrap size is correct
 * @tc.type: FUNC
 * @tc.require: AR000DBAOR
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [100, 100]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(SMALL_BOX_WIDTH, SMALL_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and check the size of whole wrap
     * @tc.expected: step3. the position and size of two boxes are set correctly and wrap size match
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, BOX_HEIGHT));

    EXPECT_TRUE(wrap->GetLayoutSize() == Size(BOX_WIDTH, SCREEN_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout007
 * @tc.desc: Wrap component layout children in horizontal correctly and set space
 * @tc.type: FUNC
 * @tc.require: AR000DBAOS
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 500], [500, 500], [500, 500]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);
    wrapComponent->SetSpacing(Dimension(SPACE, DimensionUnit::PX));
    wrapComponent->SetContentSpacing(Dimension(SPACE, DimensionUnit::PX));

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(MID_BOX_WIDTH, MID_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(MID_BOX_WIDTH, MID_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(MID_BOX_WIDTH, MID_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and check the size of whole wrap
     * @tc.expected: step3. the position and size of two boxes are set correctly and wrap size match
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    GTEST_LOG_(INFO) << "Posistion: %lf, %lf";
    EXPECT_TRUE(secondBox->GetPosition() == Offset(MID_BOX_WIDTH + SPACE, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, MID_BOX_HEIGHT + SPACE));
}

/**
 * @tc.name: RenderWrapPerformLayout008
 * @tc.desc: Wrap component layout children in vertical correctly and set space
 * @tc.type: FUNC
 * @tc.require: AR000DBAOS
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 500], [500, 500], [500, 500]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);
    wrapComponent->SetSpacing(Dimension(SPACE, DimensionUnit::PX));
    wrapComponent->SetContentSpacing(Dimension(SPACE, DimensionUnit::PX));

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and check the size of whole wrap
     * @tc.expected: step3. the position and size of two boxes are set correctly and wrap size match
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, MID_LARGE_BOX_HEIGHT + SPACE));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BOX_HEIGHT + SPACE, 0));
}

/**
 * @tc.name: RenderWrapPerformLayout009
 * @tc.desc: Wrap component layout children in horizontal correctly. Children align by main:START and cross:CENTER
 * alignment.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 20], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::CENTER);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, CROSS_LENGTH);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(BOX_WIDTH, (BOX_HEIGHT - CROSS_LENGTH) / 2));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout010
 * @tc.desc: Wrap component layout children in horizontal correctly. Children align by main:CENTER and cross:START.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 20], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::CENTER);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, CROSS_LENGTH);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(BOX_HEIGHT, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(BOX_WIDTH + BOX_HEIGHT, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(ALIGNMENT_OFFSET_VALUE, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout011
 * @tc.desc: Wrap component layout children in horizontal correctly. Children align by main:END and cross:END.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 20], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::END);
    wrapComponent->SetCrossAlignment(WrapAlignment::END);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, CROSS_LENGTH);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(MAIN_LENGTH, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(MID_BOX_WIDTH + MAIN_LENGTH, CROSS_LENGTH));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(MID_BOX_WIDTH + MAIN_LENGTH, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout012
 * @tc.desc: Wrap component layout children in vertical correctly. Children align by main:START and cross:CENTER.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [40, 1000], [20, 1000], [40, 1000]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::CENTER);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH, MID_LARGE_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset((BOX_HEIGHT - CROSS_LENGTH) / 2, MID_LARGE_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BOX_HEIGHT, 0));
}

/**
 * @tc.name: RenderWrapPerformLayout013
 * @tc.desc: Wrap component layout children in vertical correctly. Children align by main:CENTER and cross:START.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [40, 1000], [20, 1000], [40, 1000]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::CENTER);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH, MID_LARGE_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, (SCREEN_HEIGHT - BIG_BOX_HEIGHT) / 2));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, MID_LARGE_BOX_HEIGHT + (SCREEN_HEIGHT - BIG_BOX_HEIGHT) / 2));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BOX_HEIGHT, MID_BOX_HEIGHT + (SCREEN_HEIGHT - BIG_BOX_HEIGHT) / 2));
}

/**
 * @tc.name: RenderWrapPerformLayout014
 * @tc.desc: Wrap component layout children in vertical correctly. Children align by main:END and cross:END.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [40, 1000], [20, 1000], [40, 1000]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::END);
    wrapComponent->SetCrossAlignment(WrapAlignment::END);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH, MID_LARGE_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, SCREEN_HEIGHT - BIG_BOX_HEIGHT));
    EXPECT_TRUE(
        secondBox->GetPosition() == Offset(CROSS_LENGTH, MID_LARGE_BOX_HEIGHT + SCREEN_HEIGHT - BIG_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT + SCREEN_HEIGHT - BIG_BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout015
 * @tc.desc: Wrap component layout children in horizontal correctly. Children align by main alignment: space_between.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 20], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::SPACE_BETWEEN);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, CROSS_LENGTH);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition().IsZero());
    EXPECT_TRUE(secondBox->GetPosition() == Offset(MID_BOX_WIDTH + SPACE_BETWEEN_OFFSET, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout016
 * @tc.desc: Wrap component layout children in horizontal correctly. Children align by main alignment: space_around.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 20], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::SPACE_AROUND);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, CROSS_LENGTH);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(CROSS_LENGTH, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(SPACE_AROUND_OFFSET_WIDTH, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(SPACE_AROUND_OFFSET, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout017
 * @tc.desc: Wrap component layout children in horizontal correctly. Children align by cross alignment: stretch.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [500, 40], [500, 20], [500, 40]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::STRETCH);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, CROSS_LENGTH);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(BOX_WIDTH, BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and size match
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(MID_BOX_WIDTH, 0));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, BOX_HEIGHT));

    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(BOX_WIDTH, BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout018
 * @tc.desc: Wrap component layout children in vertical correctly. Children align by main alignment: space_between.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [40, 1000], [20, 100], [10, 100]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::SPACE_BETWEEN);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH, SMALL_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    // half of the cross length
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH / 2, SMALL_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and size match
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, VERTICAL_SPACE_AROUND_OFFSET));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(0, VERTICAL_SPACE_AROUND_OFFSET_HEIGHT));

    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(CROSS_LENGTH, SMALL_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(CROSS_LENGTH / 2, SMALL_BOX_HEIGHT));
}

/**
 * @tc.name: RenderWrapPerformLayout019
 * @tc.desc: Wrap component layout children in vertical correctly. Children align by main alignment: space_around.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [40, 1000], [20, 100], [10, 100]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::SPACE_AROUND);
    wrapComponent->SetCrossAlignment(WrapAlignment::START);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH, MID_LARGE_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    // half of the cross length
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH / 2, MID_LARGE_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and size match
     * @tc.expected: step3. the position matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, FIRST_BOX_HEIGHT_OFFSET));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, SECOND_BOX_HEIGHT_OFFSET));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BOX_HEIGHT, THIRD_BOX_HEIGHT_OFFSET));
}

/**
 * @tc.name: RenderWrapPerformLayout020
 * @tc.desc: Wrap component layout children in vertical correctly. Children align by cross alignment: stretch.
 * @tc.type: FUNC
 * @tc.require: AR000DBAOT
 * @tc.author: Yao Yuchi
 */
HWTEST_F(RenderWrapTest, RenderWrapPerformLayout020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create three boxes with size [40, 1000], [20, 100], [10, 100]
     */
    RefPtr<RenderRoot> root = WrapTestUtils::CreateRenderRoot();
    std::list<RefPtr<Component>> children;
    RefPtr<WrapComponent> wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetDirection(WrapDirection::VERTICAL);
    wrapComponent->SetAlignment(WrapAlignment::START);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::STRETCH);

    auto mockContext = MockRenderCommon::GetMockContext();
    RefPtr<RenderWrap> wrap = WrapTestUtils::CreateRenderWrap(wrapComponent, mockContext);
    root->AddChild(wrap);
    RefPtr<RenderBox> firstBox = WrapTestUtils::CreateRenderBox(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT);
    firstBox->Attach(mockContext);
    wrap->AddChild(firstBox);
    RefPtr<RenderBox> secondBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH, MID_LARGE_BOX_HEIGHT);
    secondBox->Attach(mockContext);
    wrap->AddChild(secondBox);
    // half of the cross length
    RefPtr<RenderBox> thirdBox = WrapTestUtils::CreateRenderBox(CROSS_LENGTH / 2, MID_LARGE_BOX_HEIGHT);
    thirdBox->Attach(mockContext);
    wrap->AddChild(thirdBox);

    /**
     * @tc.steps: step2. call PerformLayout interface
     */
    root->PerformLayout();

    /**
     * @tc.steps: step3. compare boxes position and size matches
     * @tc.expected: step3. the position matches and size matches
     */
    EXPECT_TRUE(firstBox->GetPosition() == Offset(0, 0));
    EXPECT_TRUE(secondBox->GetPosition() == Offset(0, MID_LARGE_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetPosition() == Offset(BOX_HEIGHT, 0));

    EXPECT_TRUE(firstBox->GetLayoutSize() == Size(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT));
    EXPECT_TRUE(secondBox->GetLayoutSize() == Size(BOX_HEIGHT, MID_LARGE_BOX_HEIGHT));
    EXPECT_TRUE(thirdBox->GetLayoutSize() == Size(CROSS_LENGTH / 2, MID_LARGE_BOX_HEIGHT));
}

} // namespace OHOS::Ace