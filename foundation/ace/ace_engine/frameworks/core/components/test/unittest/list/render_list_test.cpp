/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/animation/curves.h"
#include "core/components/list/list_component.h"
#include "core/components/list/render_list.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scroll_edge_effect.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/test/unittest/list/list_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/theme/theme_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const RefPtr<ThemeManager> THEME_MANAGER = AceType::MakeRefPtr<ThemeManager>();
const char LIST_ITEM_GROUP_EVENT_GROUPCLICK[] = "groupclick";

} // namespace

class RenderListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<MockRenderList> renderList_;
    RefPtr<MockRenderScroll> renderScroll_;
    int32_t index_ = 0;
};

void RenderListTest::SetUpTestCase() {}
void RenderListTest::TearDownTestCase() {}

void RenderListTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    renderList_ = AceType::MakeRefPtr<MockRenderList>();
    renderList_->Attach(mockContext_);
    renderScroll_ = AceType::MakeRefPtr<MockRenderScroll>();
    renderScroll_->Attach(mockContext_);
    index_ = 0;
}

void RenderListTest::TearDown()
{
    mockContext_ = nullptr;
    renderList_ = nullptr;
    renderScroll_ = nullptr;
}

/**
 * @tc.name: RenderListTest001
 * @tc.desc: Verify List PerformLayout can calculate children with row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7B
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 10;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(400.0, 400.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderList_->GetLayoutSize().Width(), 400.0));
}

/**
 * @tc.name: RenderListTest002
 * @tc.desc: Verify List PerformLayout can calculate children with column direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7C
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(500.0, 500.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderList_->GetLayoutSize().Height(), 500.0));

}

/**
 * @tc.name: RenderListTest003
 * @tc.desc: Verify List PerformLayout can load children by step.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0A
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by default direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 100;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "builder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(0, node);
        index_++;
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(200.0, 200.0));
    layoutParam.SetMaxSize(Size(600.0, 600.0));
    renderList_->SetLayoutParam(layoutParam);
    ASSERT_TRUE(index_ == 0);
    renderList_->PerformLayout();
    ASSERT_TRUE(index_ > 0);
}

/**
 * @tc.name: RenderListTest004
 * @tc.desc: Verify List Scroll Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0J
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive Scroll event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "list_scroll_id";
    index_ = 0;
    RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string scrollResult = std::string("\"scroll\",{\"scrollX\":")
                                           .append(std::to_string(450.0 * (index_ % 3)))
                                           .append(",\"scrollY\":")
                                           .append(std::to_string(450.0 * (index_ % 3)))
                                           .append(",\"scrollState\":")
                                           .append(std::to_string(index_ % 3))
                                           .append("},null");
            EXPECT_EQ(param, scrollResult);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderList and add 10 child by row Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    int32_t totalCount = 200;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    renderList_->Update(listComponent);
    for (int32_t i = 0; i < 10; ++i) {
        RefPtr<RenderBox> box = ListTestUtils::CreateRenderBox(400.0, 400.0);
        box->Attach(mockContext_);
        renderList_->AddChild(box);
    }
    ASSERT_TRUE(renderList_->GetChildren().size() == 10);
}

/**
 * @tc.name: RenderListTest005
 * @tc.desc: Verify List ScrollBottom Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DC9BF
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollBottom event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "list_scrollbottom_id";
    RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrollbottom\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderList and add 10 child by COLUMN Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    int32_t totalCount = 99;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    renderList_->Update(listComponent);
    renderList_->RegisterRequestItemsCallback([this](int32_t index, int32_t count) {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(200.0, 500.0, mockContext_);
        renderList_->AddListItem(1, node);
    });
    ASSERT_TRUE(renderList_->GetChildren().size() == 0);
}

/**
 * @tc.name: RenderListTest006
 * @tc.desc: Verify List ScrollTop Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0K
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollTop event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "list_scrolltop_id";
    index_ = 0;
    RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrolltop\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderList and add 10 child by ROW_REVERSE Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    int32_t totalCount = 0;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    renderList_->Update(listComponent);
    renderList_->RegisterRequestItemsCallback([this](int32_t index, int32_t count) {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(500.0, 600.0, mockContext_);
        renderList_->AddListItem(2, node);
    });
    ASSERT_TRUE(renderList_->GetChildren().size() == 0);
}

/**
 * @tc.name: RenderListTest007
 * @tc.desc: Verify List ScrollEnd Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0M
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollEnd event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "list_scrollend_id";
    index_ = 0;
    RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrollend\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderList and add 10 child by COLUMN_REVERSE Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    int32_t totalCount = 0;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    renderList_->Update(listComponent);
    RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(150.0, 500.0, mockContext_);
    renderList_->AddListItem(100, node);
    ASSERT_TRUE(renderList_->GetChildren().size() == 0);
}

/**
 * @tc.name: RenderListTest008
 * @tc.desc: Verify List ScrollTouchUp Event can callback when event trigger.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0L
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create Event listener to receive ScrollTouchUp event.
     * @tc.expected: step1. Event listener create success.
     */
    std::string scrollEventId = "list_scrolltouchup_id";
    index_ = 0;
    RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
        [this, scrollEventId](const std::string& eventId, const std::string& param) {
            std::string ret = std::string("\"scrolltouchup\",null");
            EXPECT_EQ(param, ret);
            EXPECT_EQ(eventId, scrollEventId);
            index_++;
        });
    mockContext_->RegisterEventHandler(eventHander);

    /**
     * @tc.steps: step2. Create RenderList and add 10 child by ROW Direction.
     * @tc.expected: step2. Properties and children are calculated correctly.
     */
    int32_t totalCount = -100;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == 0);
    renderList_->Update(listComponent);
    renderList_->RegisterRequestItemsCallback([this](int32_t index, int32_t count) {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(150.0, 150.0, mockContext_);
        renderList_->AddListItem(-1, node);
    });
    ASSERT_TRUE(renderList_->GetChildren().size() == 0);
}

/**
 * @tc.name: ScrollMotionTest001
 * @tc.desc: Verify scroll can calculate attributes with different edge effect.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UO AR000DQ1UP
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ScrollMotionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    RefPtr<ScrollEdgeEffect> scrollEffect = AceType::MakeRefPtr<ScrollSpringEffect>();
    listComponent->SetScrollEffect(scrollEffect);

    /**
     * @tc.steps: step2. trigger Update for render scroll.
     * @tc.expected: step2. attribute set correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderScroll_->AddChild(renderList_);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();
    renderScroll_->JumpToPosition(-100.0);
    EXPECT_EQ(0.0, renderScroll_->GetCurrentPosition());
}

/**
 * @tc.name: ScrollMotionTest002
 * @tc.desc: Verify scroll can calculate attributes with different edge effect.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UO AR000DQ1UQ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ScrollMotionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    RefPtr<ScrollEdgeEffect> scrollEffect =
        AceType::DynamicCast<ScrollEdgeEffect>(AceType::MakeRefPtr<ScrollFadeEffect>(Color::RED));
    listComponent->SetScrollEffect(scrollEffect);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(500.0, 500.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger Update for render scroll.
     * @tc.expected: step3. attribute set correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderScroll_->AddChild(renderList_);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();
    renderScroll_->JumpToPosition(-100.0);
    EXPECT_EQ(-100.0, renderScroll_->GetOverScroll());
    EXPECT_EQ(0.0, renderScroll_->GetCurrentPosition());
    RefPtr<ScrollFadeEffect> fadeEffect = AceType::DynamicCast<ScrollFadeEffect>(renderScroll_->GetScrollEffect());
    EXPECT_TRUE(fadeEffect);
    EXPECT_EQ(Color::RED, fadeEffect->GetFadeColor());
}

/**
 * @tc.name: ScrollMotionTest003
 * @tc.desc: Verify scroll can calculate attributes with different edge effect.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UO AR000DQ1UQ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ScrollMotionTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    RefPtr<ScrollEdgeEffect> scrollEffect = AceType::MakeRefPtr<ScrollEdgeEffect>(EdgeEffect::NONE);
    listComponent->SetScrollEffect(scrollEffect);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(500.0, 500.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger Update for render scroll.
     * @tc.expected: step3. attribute set correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderScroll_->AddChild(renderList_);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();
    renderScroll_->JumpToPosition(-100.0);
    EXPECT_EQ(0, renderScroll_->GetOverScroll());
    EXPECT_EQ(0.0, renderScroll_->GetCurrentPosition());
}

/**
 * @tc.name: ScrollMotionTest004
 * @tc.desc: Verify scroll can calculate attributes with different edge effect.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UQ
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ScrollMotionTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    RefPtr<ScrollEdgeEffect> scrollEffect =
        AceType::DynamicCast<ScrollEdgeEffect>(AceType::MakeRefPtr<ScrollFadeEffect>(Color::BLUE));
    listComponent->SetScrollEffect(scrollEffect);

    /**
     * @tc.steps: step2. trigger Update for render scroll.
     * @tc.expected: step2. fade color set correct.
     */
    renderScroll_->AddChild(renderList_);
    renderScroll_->Update(listComponent);
    RefPtr<ScrollFadeEffect> fadeEffect = AceType::DynamicCast<ScrollFadeEffect>(renderScroll_->GetScrollEffect());
    EXPECT_TRUE(fadeEffect);
    EXPECT_EQ(Color::BLUE, fadeEffect->GetFadeColor());
}

/**
 * @tc.name: RenderListItemExtentTest001
 * @tc.desc: Verify List PerformLayout can calculate children with row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListItemExtentTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetFlexAlign(FlexAlign::CENTER);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(250.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(250.0, 300.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(250.0 * index, 350.0));
        index++;
    }
}

/**
 * @tc.name: RenderListItemExtentTest002
 * @tc.desc: Verify List PerformLayout can calculate children with row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListItemExtentTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 10;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_END);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(0.5, DimensionUnit::PERCENT));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(300.0, 500.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(700.0, 500.0 * index));
        index++;
    }
}

/**
 * @tc.name: RenderListItemExtentTest003
 * @tc.desc: Verify List PerformLayout can calculate children with row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListItemExtentTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetColumnCount(2);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(150.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(150.0, 300.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(150.0 * (index / 2), 500 * (index % 2)));
        index++;
    }
}

/**
 * @tc.name: RenderListItemExtentTest004
 * @tc.desc: Verify List PerformLayout can calculate children with row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListItemExtentTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 10;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::STRETCH);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetColumnCount(4);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(0.4, DimensionUnit::PERCENT));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetColumnCount() == 4);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(100.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(250.0, 400.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(250.0 * (index % 4), 400.0 * (index / 4)));
        index++;
    }
}

/**
 * @tc.name: ListInteractionTest001
 * @tc.desc: Verify list interaction effect when item clicked on phone.
 * @tc.type: FUNC
 * @tc.require: AR000DQ4FT
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListInteractionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
    if (listItem) {
        listItem->ResetFocusEffect(AceType::WeakClaim(Referenced::RawPtr(mockContext_)));
        listItem->PlayPressDownAnimation();
        auto controller = listItem->GetEffectController();
        ASSERT_FALSE(controller == nullptr);
        auto animator = controller->GetAnimator();
        ASSERT_FALSE(animator == nullptr);
        animator->SetStartDelay(0);
        animator->TriggerFrame(17);
        auto alpha01 = controller->GetAlpha();
        animator->TriggerFrame(57);
        auto alpha02 = controller->GetAlpha();
        ASSERT_FALSE(NearEqual(alpha02, alpha01));
    }
}

/**
 * @tc.name: ListInteractionTest002
 * @tc.desc: Verify list interaction effect when on TV.
 * @tc.type: FUNC
 * @tc.require: AR000DQ4FU
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListInteractionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    if (renderList_->IsTV()) {
        RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
        if (listItem) {
            listItem->ResetFocusEffect(AceType::WeakClaim(Referenced::RawPtr(mockContext_)));
            listItem->HandleClicked();
            auto controller = listItem->GetEffectController();
            auto specialController = AceType::DynamicCast<TVInteractiveEffect>(controller);
            ASSERT_FALSE(specialController == nullptr);
        }
    }
}

/**
 * @tc.name: ListInteractionTest003
 * @tc.desc: Verify list interaction effect when on watch.
 * @tc.type: FUNC
 * @tc.require: AR000DQ4FV
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListInteractionTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    if (renderList_->IsWatch()) {
        RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
        if (listItem) {
            listItem->ResetFocusEffect(AceType::WeakClaim(Referenced::RawPtr(mockContext_)));
            listItem->HandleClicked();
            auto controller = listItem->GetEffectController();
            auto specialController = AceType::DynamicCast<WatchInteractiveEffect>(controller);
            ASSERT_FALSE(specialController == nullptr);
        }
    }
}

/**
 * @tc.name: ListItemFocusTest001
 * @tc.desc: Verify list item effect when focus switch.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T7
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListItemFocusTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    if (renderList_->IsTV()) {
        RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
        if (listItem) {
            listItem->ResetFocusEffect(AceType::WeakClaim(Referenced::RawPtr(mockContext_)));
            listItem->HandleFocusEvent(true);
            auto controller = listItem->GetEffectController();
            ASSERT_FALSE(controller == nullptr);

            auto animator = controller->GetAnimator();
            ASSERT_FALSE(animator == nullptr);

            animator->TriggerFrame(17);
            auto scale01 = controller->GetScale();
            animator->TriggerFrame(32);
            auto scale02 = controller->GetScale();
            ASSERT_TRUE(scale02 > scale01);
            ASSERT_FALSE(NearEqual(controller->GetScale(), 1.0));
        }
    }
}

/**
 * @tc.name: ListItemFocusTest002
 * @tc.desc: Verify list item effect when focus switch.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T8
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListItemFocusTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    if (renderList_->IsTV()) {
        RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
        if (listItem) {
            listItem->ResetFocusEffect(AceType::WeakClaim(Referenced::RawPtr(mockContext_)));
            auto controller = listItem->GetEffectController();
            ASSERT_FALSE(controller == nullptr);

            auto animator = controller->GetAnimator();
            ASSERT_FALSE(animator == nullptr);

            controller->SetScale(1.05);
            listItem->HandleFocusEvent(false);
            animator->TriggerFrame(16);
            auto scale01 = controller->GetScale();
            animator->TriggerFrame(32);
            auto scale02 = controller->GetScale();
            ASSERT_TRUE(scale02 < scale01);
            ASSERT_FALSE(NearEqual(controller->GetScale(), 1.0));
        }
    }
}

/**
 * @tc.name: ListItemFocusTest003
 * @tc.desc: Verify list item effect when item clicked.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T6
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListItemFocusTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    if (!renderList_->IsTV()) {
        RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
        if (listItem) {
            listItem->ResetFocusEffect(AceType::WeakClaim(Referenced::RawPtr(mockContext_)));

            listItem->HandleClicked();
            auto controller = listItem->GetEffectController();
            ASSERT_FALSE(controller == nullptr);

            auto animator = controller->GetAnimator();
            ASSERT_FALSE(animator == nullptr);
        }
    }
}

/**
 * @tc.name: RenderListFocusMoveTest001
 * @tc.desc: Verify List can move focus when single column and Direction::ROW.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetColumnCount(1);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetColumnCount() == 1);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(200.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 300.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(600.0, 300.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 1); // RIGHT
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 2); // RIGHT
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 1); // LEFT
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == -1); // DOWN
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == -1);  // UP
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 0);  // LEFT
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: RenderListFocusMoveTest002
 * @tc.desc: Verify List can move focus when single column and Direction::ROW and RTL.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction and RTL.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetRightToLeft(true);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetRightToLeft());

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(200.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 300.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(600.0, 300.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 1); // LEFT
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 2); // LEFT
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 1); // RIGHT
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == -1); // DOWN
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == -1);  // UP
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 0); // RIGHT
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: RenderListFocusMoveTest003
 * @tc.desc: Verify List can move focus when single column and Direction::COLUMN.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(300.0, 600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(300.0, 600.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 1); // DOWN
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 2); // DOWN
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 1); // UP
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == -1);  // LEFT
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 0);    // UP
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: RenderListFocusMoveTest004
 * @tc.desc: Verify List can move focus when multi columns and Direction::ROW.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetColumnCount(2);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(200.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(600.0, 600.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 2); // RIGHT
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 4); // RIGHT
    renderList_->ListItemFocused(4);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 5); // DOWN
    renderList_->ListItemFocused(5);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 3); // LEFT
    renderList_->ListItemFocused(3);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 1); // LEFT
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == -1); // DOWN
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 0);   // UP
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: RenderListFocusMoveTest005
 * @tc.desc: Verify List can move focus when multi columns and Direction::ROW and RTL.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by row direction and RTL.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::ROW);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetColumnCount(2);
    listComponent->SetRightToLeft(true);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetColumnCount() == 2);
    ASSERT_TRUE(listComponent->GetRightToLeft());

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(200.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(600.0, 600.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 2); // LEFT
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 4); // LEFT
    renderList_->ListItemFocused(4);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 5); // DOWN
    renderList_->ListItemFocused(5);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 3); // RIGHT
    renderList_->ListItemFocused(3);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 1); // RIGHT
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == -1); // DOWN
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 0);   // UP
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: RenderListFocusMoveTest006
 * @tc.desc: Verify List can move focus when multi columns and Direction::COLUMN.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetColumnCount(2);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    ASSERT_TRUE(listComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(600.0, 600.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 2); // DOWN
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 4); // DOWN
    renderList_->ListItemFocused(4);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 5); // RIGHT
    renderList_->ListItemFocused(5);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 3); // UP
    renderList_->ListItemFocused(3);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 1); // UP
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == -1); // RIGHT
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 0);   // LEFT
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: RenderListFocusMoveTest007
 * @tc.desc: Verify List can move focus when multi columns and Direction::COLUMN and RTL.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UC
 * @tc.author: zhongjianfei
 */
HWTEST_F(RenderListTest, RenderListFocusMoveTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction and RTL.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetRightToLeft(true);
    listComponent->SetColumnCount(2);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);
    listComponent->SetRightToLeft(true);
    ASSERT_TRUE(listComponent->GetColumnCount() == 2);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(600.0, 600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 600.0, Offset(0.0, 0.0), Size(600.0, 600.0));
    renderList_->PerformLayout();

    /**
     * @tc.steps: step4. trigger focus move for render list.
     * @tc.expected: step4. Focus move correct.
     */
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 2); // DOWN
    renderList_->ListItemFocused(2);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, false) == 4); // DOWN
    renderList_->ListItemFocused(4);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == 5); // LEFT
    renderList_->ListItemFocused(5);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 3); // UP
    renderList_->ListItemFocused(3);
    ASSERT_TRUE(renderList_->RequestNextFocus(true, true) == 1); // UP
    renderList_->ListItemFocused(1);
    ASSERT_TRUE(renderList_->RequestNextFocus(false, true) == -1); // LEFT
    ASSERT_TRUE(renderList_->RequestNextFocus(false, false) == 0); // RIGHT
    renderList_->ListItemFocused(0);
}

/**
 * @tc.name: AceListExpansion001
 * @tc.desc: Verify List can create groupItem without second Level item.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TB
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderList_->GetLayoutSize().Height(), 200.0));
}

/**
 * @tc.name: AceListExpansion002
 * @tc.desc: Verify List can create groupItem with second Level item.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TC
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_, true);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->GetLayoutManager()->SetExpandAll(true);
    renderList_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderList_->GetLayoutSize().Height(), 400.0));
}

/**
 * @tc.name: AceListExpansion003
 * @tc.desc: Verify List can create groupItem with multiple second Level item and not specify primary.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TC
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_, true, false);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->GetLayoutManager()->SetExpandAll(true);
    renderList_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderList_->GetLayoutSize().Height(), 400.0));
}

/**
 * @tc.name: AceListExpansion006
 * @tc.desc: Verify itemGroup click event.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TD
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component and set click event.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_, true, false);
            std::string scrollEventId = "groupclick_id";
            index_ = 0;
            RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
                [this, scrollEventId](const std::string& eventId, const std::string& param) {
                    std::string scrollResult = std::string("\"")
                                                   .append(LIST_ITEM_GROUP_EVENT_GROUPCLICK)
                                                   .append("\", ")
                                                   .append("{\"groupid\": \"")
                                                   .append("test")
                                                   .append("\"}");
                    EXPECT_EQ(param, scrollResult);
                    EXPECT_EQ(eventId, scrollEventId);
                    index_++;
                });
            mockContext_->RegisterEventHandler(eventHander);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    RefPtr<MockRenderListItemGroup> listItem =
        AceType::DynamicCast<MockRenderListItemGroup>(renderList_->GetItemByIndex(0));
    if (listItem) {
        bool lastExpand = listItem->GetExpand();
        listItem->HandleClicked();
        ASSERT_FALSE(listItem->GetExpand() == lastExpand);
    }
}

/**
 * @tc.name: AceListExpansion007
 * @tc.desc: Verify itemGroup coollapse event.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TD
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component and set click event.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_, true, false);
            std::string scrollEventId = "groupcollapse_id";
            index_ = 0;
            RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
                [this, scrollEventId](const std::string& eventId, const std::string& param) {
                    std::string scrollResult = std::string("\"")
                                                   .append(LIST_ITEM_GROUP_EVENT_GROUPCLICK)
                                                   .append("\", ")
                                                   .append("{\"groupid\": \"")
                                                   .append("test")
                                                   .append("\"}");
                    EXPECT_EQ(param, scrollResult);
                    EXPECT_EQ(eventId, scrollEventId);
                    index_++;
                });
            mockContext_->RegisterEventHandler(eventHander);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    RefPtr<MockRenderListItemGroup> listItem =
        AceType::DynamicCast<MockRenderListItemGroup>(renderList_->GetItemByIndex(0));
    if (listItem) {
        bool lastExpand = listItem->GetExpand();
        listItem->HandleClicked();
        ASSERT_FALSE(listItem->GetExpand() == lastExpand);
    }
}

/**
 * @tc.name: AceListExpansion008
 * @tc.desc: Verify itemGroup expand event.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TD
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component and set click event.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_, true, false);
            std::string scrollEventId = "groupexpand_id";
            index_ = 0;
            RefPtr<TestListEventHander> eventHander = AceType::MakeRefPtr<TestListEventHander>(
                [this, scrollEventId](const std::string& eventId, const std::string& param) {
                    std::string scrollResult = std::string("\"")
                                                   .append(LIST_ITEM_GROUP_EVENT_GROUPCLICK)
                                                   .append("\", ")
                                                   .append("{\"groupid\": \"")
                                                   .append("test")
                                                   .append("\"}");
                    EXPECT_EQ(param, scrollResult);
                    EXPECT_EQ(eventId, scrollEventId);
                    index_++;
                });
            mockContext_->RegisterEventHandler(eventHander);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    RefPtr<MockRenderListItemGroup> listItem =
        AceType::DynamicCast<MockRenderListItemGroup>(renderList_->GetItemByIndex(0));
    if (listItem) {
        bool lastExpand = listItem->GetExpand();
        listItem->HandleClicked();
        ASSERT_FALSE(listItem->GetExpand() == lastExpand);
    }
}

/**
 * @tc.name: AceListExpansion010
 * @tc.desc: Verify List can create groupItem with multiple second Level item and not specify primary.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TH
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, AceListExpansion010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 6;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        if (index == 0) {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItemGroup(300.0, 200.0, mockContext_, true, false);
            renderList_->AddListItem(index, node);
        } else {
            RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
            renderList_->AddListItem(index, node);
        }
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(100.0, 100.0));
    layoutParam.SetMaxSize(Size(900.0, 900.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    ASSERT_TRUE(NearEqual(renderList_->GetLayoutSize().Height(), 200.0));
}

/**
 * @tc.name: ListDividerTest001
 * @tc.desc: Verify list divider.
 * @tc.type: FUNC
 * @tc.require: AR000DQ4FT
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListDividerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
    if (listItem) {
        ASSERT_FALSE(listItem->NeedDivider());
        ASSERT_TRUE(NearEqual(listItem->GetDividerHeight().Value(), 1.0));
        ASSERT_TRUE(NearEqual(listItem->GetDividerOrigin().Value(), 0.0));
        ASSERT_TRUE(NearEqual(listItem->GetDividerLength().Value(), 0.0));
    }
}

/**
 * @tc.name: ListDividerTest002
 * @tc.desc: Verify list divider.
 * @tc.type: FUNC
 * @tc.require: AR000DQ4FT
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderListTest, ListDividerTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetTotalCount(5);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();

    RefPtr<MockRenderListItem> listItem = AceType::DynamicCast<MockRenderListItem>(renderList_->GetItemByIndex(0));
    if (listItem) {
        ASSERT_FALSE(listItem->NeedDivider());
        ASSERT_TRUE(NearEqual(listItem->GetDividerHeight().Value(), 1.0));

        listItem->MarkNeedDivider();
        listItem->SetDividerHeight(Dimension(5.0, DimensionUnit::PX));
        listItem->PerformLayout();
        ASSERT_TRUE(NearEqual(listItem->GetLayoutSize().Height(), 305.0));
    }
}

#ifndef WEARABLE_PRODUCT
/**
 * @tc.name: AceListScrollBar001
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBO
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 15;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);
    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(150.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(300.0, 150.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(0.0, 150.0 * index));
        index++;
    }
    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());

    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->NeedScrollBar() == true);
}

/**
 * @tc.name: AceListScrollBar002
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBK
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
        scrollBar->SetTouchWidth(scrollBarTheme->GetTouchWidth());
        scrollBar->SetBackgroundColor(scrollBarTheme->GetBackgroundColor());
        scrollBar->SetForegroundColor(scrollBarTheme->GetForegroundColor());
        scrollBar->SetPadding(scrollBarTheme->GetPadding());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(150.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(960.0, 960.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 960.0, Offset(0.0, 0.0), Size(960.0, 960.0));
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(300.0, 150.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(0.0, 150.0 * index));
        index++;
    }
    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());

    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetController()->GetInactiveWidth().Value(), 4.0));
    ASSERT_TRUE(NearEqual(bar->GetController()->GetActiveWidth().Value(), 8.0));
    ASSERT_TRUE(NearEqual(bar->GetMinHeight().Value(), 48.0));
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 768));
}

/**
 * @tc.name: AceListScrollBar003
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBL
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 10;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
        scrollBar->SetTouchWidth(scrollBarTheme->GetTouchWidth());
        scrollBar->SetBackgroundColor(scrollBarTheme->GetBackgroundColor());
        scrollBar->SetForegroundColor(scrollBarTheme->GetForegroundColor());
        scrollBar->SetPadding(scrollBarTheme->GetPadding());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(200.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1600.0, 1600.0));
    renderList_->ResetLayoutRange(0.0, 1600.0, Offset(0.0, 0.0), Size(1600.0, 1600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());

    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetController()->GetInactiveWidth().Value(), 4.0));
    ASSERT_TRUE(NearEqual(bar->GetController()->GetActiveWidth().Value(), 8.0));
    ASSERT_TRUE(NearEqual(bar->GetMinHeight().Value(), 48.0));
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 1280));
}

/**
 * @tc.name: AceListScrollBar004
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBM
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 12;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
        scrollBar->SetTouchWidth(scrollBarTheme->GetTouchWidth());
        scrollBar->SetBackgroundColor(scrollBarTheme->GetBackgroundColor());
        scrollBar->SetForegroundColor(scrollBarTheme->GetForegroundColor());
        scrollBar->SetPadding(scrollBarTheme->GetPadding());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(100.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 100.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(300.0, 300.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 300.0, Offset(0.0, 0.0), Size(300.0, 300.0));
    renderList_->PerformLayout();

    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());

    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetController()->GetInactiveWidth().Value(), 4.0));
    ASSERT_TRUE(NearEqual(bar->GetController()->GetActiveWidth().Value(), 8.0));
    ASSERT_TRUE(NearEqual(bar->GetMinHeight().Value(), 48.0));
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());

    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 150));
    renderScroll_->JumpToPosition(2000.0);

    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(Offset(0.0, std::abs(renderScroll_->GetScrollBarOutBoundaryExtent())),
        renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 75));
}

/**
 * @tc.name: AceListScrollBar005
 * @tc.desc: Verify the scrollBar param.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBN
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(150.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(960.0, 960.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 960.0, Offset(0.0, 0.0), Size(960.0, 960.0));
    renderList_->PerformLayout();

    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();
    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetController()->GetActiveWidth().Value(), 8.0));
}

/**
 * @tc.name: AceListScrollBar006
 * @tc.desc: Verify the scrollBar param.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBK
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 8;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetMinDynamicHeight(scrollBarTheme->GetMinDynamicHeight());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(150.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(960.0, 960.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 960.0, Offset(0.0, 0.0), Size(960.0, 960.0));
    renderList_->PerformLayout();

    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();
    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetMinDynamicHeight().Value(), 4.0));
}

/**
 * @tc.name: AceListScrollBar007
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBL
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 12;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::AUTO);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(100.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 100.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(300.0, 300.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 300.0, Offset(0.0, 0.0), Size(300.0, 300.0));
    renderList_->PerformLayout();

    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());

    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetController()->GetInactiveWidth().Value(), 4.0));
    ASSERT_TRUE(NearEqual(bar->GetController()->GetActiveWidth().Value(), 8.0));
    ASSERT_TRUE(NearEqual(bar->GetMinHeight().Value(), 48.0));
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());

    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 150.0));
}

/**
 * @tc.name: AceListScrollBar008
 * @tc.desc: Verify the scrollBar param.
 * @tc.type: FUNC
 * @tc.require:AR000F3CBM
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 10;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::OFF);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(200.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 200.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1600.0, 1600.0));
    renderList_->ResetLayoutRange(0.0, 1600.0, Offset(0.0, 0.0), Size(1600.0, 1600.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());

    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() == nullptr);
}

/**
 * @tc.name: AceListScrollBar009
 * @tc.desc: Verify the scrollBar param.
 * @tc.type: FUNC
 * @tc.require:AR000F3L35
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct scrollBar.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetReservedHeight(scrollBarTheme->GetReservedHeight());
    }

    ASSERT_TRUE(NearEqual(scrollBar->GetReservedHeight().Value(), 24.0));
}

/**
 * @tc.name: AceListScrollBar010
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3L36
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ScrollBar.
     * @tc.expected: step1. properties are set correctly.
     */
    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetTouchWidth(scrollBarTheme->GetTouchWidth());
    }

    ASSERT_TRUE(NearEqual(scrollBar->GetActiveWidth().Value(), 8.0));
    ASSERT_TRUE(NearEqual(scrollBar->GetTouchWidth().Value(), 32.0));
}

/**
 * @tc.name: AceListScrollBar011
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3L37
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 12;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetInactiveWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetNormalWidth(scrollBarTheme->GetNormalWidth());
        scrollBar->SetActiveWidth(scrollBarTheme->GetActiveWidth());
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
        scrollBar->SetTouchWidth(scrollBarTheme->GetTouchWidth());
        scrollBar->SetBackgroundColor(scrollBarTheme->GetBackgroundColor());
        scrollBar->SetForegroundColor(scrollBarTheme->GetForegroundColor());
        scrollBar->SetPadding(scrollBarTheme->GetPadding());
        scrollBar->SetReservedHeight(scrollBarTheme->GetReservedHeight());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(100.0, DimensionUnit::PX));
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 100.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(300.0, 300.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 300.0, Offset(0.0, 0.0), Size(300.0, 300.0));
    renderList_->PerformLayout();

    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());

    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetController()->GetInactiveWidth().Value(), 4.0));
    ASSERT_TRUE(NearEqual(bar->GetController()->GetActiveWidth().Value(), 8.0));
    ASSERT_TRUE(NearEqual(bar->GetMinHeight().Value(), 48.0));
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 138.0));
    renderScroll_->JumpToPosition(2000.0);

    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(Offset(0.0, std::abs(renderScroll_->GetScrollBarOutBoundaryExtent())),
        renderScroll_->GetLayoutSize(), renderScroll_->GetLastOffset(), renderScroll_->GetEstimatedHeight());
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 69.0));
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Bottom(), 276.0));
}

/**
 * @tc.name: AceListScrollBar012
 * @tc.desc: Verify List can create scrollBar.
 * @tc.type: FUNC
 * @tc.require:AR000F3L37
 * @tc.author: liujinwei
 */
HWTEST_F(RenderListTest, AceListScrollBar012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 12;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();

    ASSERT_FALSE(renderScroll_->GetScrollBar());
    RefPtr<ScrollBar> scrollBar = AceType::MakeRefPtr<ScrollBar>(DisplayMode::ON);

    // scrollBar theme
    auto scrollBarTheme = THEME_MANAGER->GetTheme<ScrollBarTheme>();
    if (scrollBarTheme) {
        scrollBar->SetMinHeight(scrollBarTheme->GetMinHeight());
        scrollBar->SetPadding(scrollBarTheme->GetPadding());
        scrollBar->SetReservedHeight(scrollBarTheme->GetReservedHeight());
    }

    listComponent->SetScrollBar(scrollBar);
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    listComponent->SetItemExtent(Dimension(100.0, DimensionUnit::PX));

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 100.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. List and children layout correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(50.0, 50.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->ResetLayoutRange(0.0, 50.0, Offset(0.0, 0.0), Size(50.0, 50.0));
    renderList_->PerformLayout();

    renderScroll_->AddChild(renderList_);
    ASSERT_TRUE(listComponent->GetScrollBar() != nullptr);
    renderScroll_->Update(listComponent);
    EXPECT_TRUE(renderScroll_->NeedLayout());
    renderScroll_->SetLayoutParam(layoutParam);
    renderScroll_->PerformLayout();

    ASSERT_TRUE(renderScroll_->GetScrollBar());

    RefPtr<ScrollBar> bar = renderScroll_->GetScrollBar();
    ASSERT_TRUE(bar->IsScrollable() == true);
    ASSERT_TRUE(bar->IsActive() == false);
    ASSERT_TRUE(bar->GetController() != nullptr);
    ASSERT_TRUE(NearEqual(bar->GetMinHeight().Value(), 48.0));
    renderScroll_->SetMainScrollExtentForBar(renderScroll_->GetMainScrollExtent());
    bar->UpdateScrollBarRegion(
        Offset(0.0, 0.0), renderScroll_->GetLayoutSize(), Offset(0.0, 0.0), renderScroll_->GetEstimatedHeight());
    ASSERT_TRUE(NearEqual(bar->GetActiveRect().Height(), 48.0));
}
#endif

/**
 * @tc.name: RenderListItemCardTransitionTest001
 * @tc.desc: Verify that the list item can get correct RRect.
 * @tc.type: FUNC
 * @tc.require: AR000F3CA8
 * @tc.author: chenlien
 */
HWTEST_F(RenderListTest, RenderListItemCardTransitionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct ListComponent by column direction.
     * @tc.expected: step1. properties are set correctly.
     */
    int32_t totalCount = 10;
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetFlexAlign(FlexAlign::FLEX_START);
    listComponent->SetTotalCount(totalCount);
    listComponent->SetOnRequestItem(EventMarker { "itemBuilder_ID" });
    ASSERT_TRUE(listComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(listComponent->GetTotalCount() == totalCount);

    /**
     * @tc.steps: step2. construct RenderList with list component.
     * @tc.expected: step2. properties are set correctly.
     */
    renderList_->Update(listComponent);
    renderList_->RegisterBuildItemCallback([this](int32_t index) -> bool {
        RefPtr<RenderNode> node = ListTestUtils::CreateRenderItem(300.0, 300.0, mockContext_);
        renderList_->AddListItem(index, node);
        return true;
    });

    /**
     * @tc.steps: step3. trigger layout for render list.
     * @tc.expected: step3. get listItem rrect correct.
     */
    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderList_->ResetLayoutRange(0.0, 1000.0, Offset(0.0, 0.0), Size(1000.0, 1000.0));
    renderList_->SetLayoutParam(layoutParam);
    renderList_->PerformLayout();
    int32_t index = 0;
    for (auto iter : renderList_->GetItems()) {
        auto item = AceType::DynamicCast<MockRenderListItem>(iter.second->GetFirstChild());
        ASSERT_TRUE(item);
        ASSERT_TRUE(iter.second->GetLayoutSize() == Size(300.0, 300.0));
        ASSERT_TRUE(iter.second->GetPosition() == Offset(0.0, 300.0 * index));
        RRect rrect = item->GetRRect();
        NearEqual(rrect.GetCorner().topLeftRadius.GetX().Value(), 10.0);
        index++;
    }
}

} // namespace OHOS::Ace
