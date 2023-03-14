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

#include <algorithm>
#include <list>

#include "gtest/gtest.h"

#include "core/components/box/box_component.h"
#include "core/components/box/box_element.h"
#include "core/components/list/list_component.h"
#include "core/components/list/list_element.h"
#include "core/components/list/list_item_component.h"
#include "core/components/list/list_item_element.h"
#include "core/components/list/list_item_group_component.h"
#include "core/components/list/list_item_group_element.h"
#include "core/components/list/render_list_item_group.h"
#include "core/components/scroll/render_multi_child_scroll.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/test/unittest/tabbar/tab_test_utils.h"
#include "core/pipeline/base/composed_element.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t ITEM_COUNT = 4;

bool CheckItemFocus(const std::list<RefPtr<Element>>& children, int32_t index)
{
    int32_t count = 0;
    bool flag = true;
    for (auto pos = children.begin(); pos != children.end(); ++pos) {
        auto composed = AceType::DynamicCast<ComposedElement>(*pos);
        if (composed) {
            auto listItem = AceType::DynamicCast<ListItemElement>(composed->GetFirstChild());
            if (listItem) {
                auto focus = AceType::DynamicCast<FocusNode>(listItem);
                if (count == index) {
                    if (!focus->IsCurrentFocus()) {
                        flag = false;
                    }
                } else {
                    if (focus->IsCurrentFocus()) {
                        flag = false;
                    }
                }
            }
        }
        count++;
    }
    return flag;
}

} // namespace

class ListElementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void InitFocusTree(FlexDirection direction);
    std::vector<RefPtr<ComposedComponent>> MakeListChildArray(double itemHeight);
    RefPtr<ComposedComponent> MakeListChild(std::string id, std::string name, std::string type = std::string("default"),
        double width = 0.0, double height = 0.0);
    RefPtr<ComposedComponent> MakeListItemGroupChild(std::string id, std::string name,
        std::string type = std::string("default"), double width = 0.0, double height = 0.0);
    bool CheckInsertAndDelete(const RefPtr<RenderNode>& multiChildNode, double itemHeight,
        int32_t size, int32_t firstIndex, std::string firstType);
    void SetListDirection(FlexDirection direction);
    void DisableAddDeleteEffect(const RefPtr<RenderNode>& multiChildNode);
    RefPtr<Element> BuildElementTree(const RefPtr<Component>& listComponent);

protected:
    static constexpr double ITEM_HEIGHT = 50.0;
    static constexpr double LIST_WIDTH = 60.0;
    static constexpr double LIST_HEIGHT = 100.0;
    static constexpr double CACHE_EXTENT = 0.0;

    RefPtr<PipelineContext> context_;
    RefPtr<ListElement> listZero_ = AceType::MakeRefPtr<ListElement>();
    RefPtr<ListElement> listOne_ = AceType::MakeRefPtr<ListElement>();
    RefPtr<ListElement> listTwo_ = AceType::MakeRefPtr<ListElement>();
    RefPtr<ContainerElement> rootContainer_ = AceType::MakeRefPtr<ContainerElement>();
};

void ListElementTest::SetUpTestCase() {}

void ListElementTest::TearDownTestCase() {}

void ListElementTest::SetUp()
{
    rootContainer_->Create();
    context_ = MockRenderCommon::GetMockContext();
    context_->SetupRootElement();
    rootContainer_->SetPipelineContext(context_);
}

void ListElementTest::TearDown()
{
    context_ = nullptr;
}

RefPtr<ComposedComponent> ListElementTest::MakeListChild(
    std::string id, std::string name, std::string type, double width, double height)
{
    auto child = AceType::MakeRefPtr<FocusComponet>();
    child->SetWidth(width);
    child->SetHeight(height);
    auto listItem = AceType::MakeRefPtr<ListItemComponent>(type, child);
    auto composed = AceType::MakeRefPtr<ComposedComponent>(id, name, listItem);
    return composed;
}

RefPtr<ComposedComponent> ListElementTest::MakeListItemGroupChild(
    std::string id, std::string name, std::string type, double width, double height)
{
    auto listItemGroup = AceType::MakeRefPtr<ListItemGroupComponent>("group");
    for (int32_t i = 0; i < 5; ++i) {
        listItemGroup->AppendChild(MakeListChild("list1_id", "list1_name", "focus", 200, ITEM_HEIGHT));
    }
    auto composed = AceType::MakeRefPtr<ComposedComponent>(id, name, listItemGroup);
    return composed;
}

void ListElementTest::InitFocusTree(FlexDirection direction)
{
    // create list 0
    auto listComponentZero = AceType::MakeRefPtr<ListComponent>();
    listComponentZero->AppendChild(MakeListChild("list0_id", "list0_name"));
    listComponentZero->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listZero_->SetNewComponent(listComponentZero);

    // create list 1
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < ITEM_COUNT; ++i) {
        listComponent->AppendChild(MakeListChild("list1_id", "list1_name"));
    }
    listComponent->SetDirection(direction);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);

    // create list 2
    auto listComponentTwo = AceType::MakeRefPtr<ListComponent>();
    listComponentTwo->AppendChild(MakeListChild("list2_id", "list2_name"));
    listComponentTwo->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listTwo_->SetNewComponent(listComponentTwo);

    listZero_->Mount(rootContainer_);
    auto renderNodeZero = listZero_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderNodeZero->SetLayoutParam(layoutParam);
    renderNodeZero->PerformLayout();

    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();

    listTwo_->Mount(rootContainer_);
    auto renderNodeTwo = listTwo_->GetRenderNode();
    renderNodeTwo->SetLayoutParam(layoutParam);
    renderNodeTwo->PerformLayout();

    rootContainer_->RequestFocusImmediately();
}

void ListElementTest::SetListDirection(FlexDirection direction)
{
    if (listOne_) {
        printf("SetListDirection direction\n");
    }
}

void ListElementTest::DisableAddDeleteEffect(const RefPtr<RenderNode>& multiChildNode)
{
    auto children = multiChildNode->GetChildren();
    if (!children.empty()) {
        auto renderList = AceType::DynamicCast<RenderList>(children.front());
        if (renderList) {
            renderList->SetAddDeleteEffect(false);
        }
    }
}

/**
 * @tc.name: ListElementTest001
 * @tc.desc: Verify the basic function of request focus and release focus, while the direction is column.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5O
 * @tc.author: xuliangjun
 */
HWTEST_F(ListElementTest, ListElementTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with three lists, the direction is column.
     * @tc.expected: step1. RootContainer and listZero acquire the focus.
     */
    InitFocusTree(FlexDirection::COLUMN);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listZero_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listZero_->GetChildren(), 0));

    /**
     * @tc.steps: step2. Animation to the down
     * @tc.expected: step2. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
     * @tc.steps: step3. Animation to the down
     * @tc.expected: step3. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));

    /**
     * @tc.steps: step4. Animation to the up
     * @tc.expected: step4. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
     * @tc.steps: step5. Animation to the left
     * @tc.expected: step5. RootContainer and listZero acquire the focus. listZero's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listZero_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listZero_->GetChildren(), 0));

    /**
     * @tc.steps: step6. Animation to the right
     * @tc.expected: step6. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
}

/**
 * @tc.name: ListElementTest002
 * @tc.desc: Verify the basic function of request focus and release focus, while the direction is row.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5O
 * @tc.author: xuliangjun
 */
HWTEST_F(ListElementTest, ListElementTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with three lists, the direction is row.
     * @tc.expected: step1. RootContainer and listZero acquire the focus.
     */
    InitFocusTree(FlexDirection::ROW);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listZero_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listZero_->GetChildren(), 0));

    /**
     * @tc.steps: step2. Animation to the right
     * @tc.expected: step2. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
     * @tc.steps: step3. Animation to the right
     * @tc.expected: step3. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));

    /**
     * @tc.steps: step4. Animation to the left
     * @tc.expected: step4. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
     * @tc.steps: step5. Animation to the up
     * @tc.expected: step5. RootContainer and listZero acquire the focus. listZero's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listZero_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listZero_->GetChildren(), 0));

    /**
     * @tc.steps: step6. Animation to the down
     * @tc.expected: step6. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
}

/**
 * @tc.name: ListElementTest003
 * @tc.desc: Verify the basic function of moving focus in list, while the direction is column.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5O
 * @tc.author: xuliangjun
 */
HWTEST_F(ListElementTest, ListElementTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with three lists, the direction is column.
     * @tc.expected: step1. RootContainer and listZero acquire the focus.
     */
    InitFocusTree(FlexDirection::COLUMN);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listZero_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listZero_->GetChildren(), 0));

    /**
     * @tc.steps: step2. Animation to the down
     * @tc.expected: step2. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
     * @tc.steps: step3. Animation to the down
     * @tc.expected: step3. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));

    /**
     * @tc.steps: step4. Animation to the down
     * @tc.expected: step4. RootContainer and listOne acquire the focus. listOne's third child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 2));

    /**
     * @tc.steps: step5. Animation to the down
     * @tc.expected: step5. RootContainer and listOne acquire the focus. listOne's fourth child Get the focus.
     */
    rootContainer_->HandleKeyEvent(DOWN_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 3));

    /**
     * @tc.steps: step6. Animation to the up
     * @tc.expected: step6. RootContainer and listOne acquire the focus. listOne's third child Get the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 2));

    /**
     * @tc.steps: step7. Animation to the up
     * @tc.expected: step7. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));

    /**
     * @tc.steps: step8. Animation to the up
     * @tc.expected: step8. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(UP_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
}

/**
 * @tc.name: ListElementTest004
 * @tc.desc: Verify the basic function of moving focus in list, while the direction is row.
 * @tc.type: FUNC
 * @tc.require: AR000DHJ5O
 * @tc.author: xuliangjun
 */
HWTEST_F(ListElementTest, ListElementTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the RootContainer with three lists, the direction is row.
     * @tc.expected: step1. RootContainer and listZero acquire the focus.
     */
    InitFocusTree(FlexDirection::ROW);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listZero_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listZero_->GetChildren(), 0));

    /**
     * @tc.steps: step2. Animation to the right
     * @tc.expected: step2. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
     * @tc.steps: step3. Animation to the right
     * @tc.expected: step3. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));

    /**
     * @tc.steps: step4. Animation to the right
     * @tc.expected: step4. RootContainer and listOne acquire the focus. listOne's third child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 2));

    /**
     * @tc.steps: step5. Animation to the right
     * @tc.expected: step5. RootContainer and listOne acquire the focus. listOne's fourth child Get the focus.
     */
    rootContainer_->HandleKeyEvent(RIGHT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 3));

    /**
     * @tc.steps: step6. Animation to the left
     * @tc.expected: step6. RootContainer and listOne acquire the focus. listOne's third child Get the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 2));

    /**
     * @tc.steps: step7. Animation to the left
     * @tc.expected: step7. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));

    /**
     * @tc.steps: step8. Animation to the left
     * @tc.expected: step8. RootContainer and listOne acquire the focus. listOne's first child Get the focus.
     */
    rootContainer_->HandleKeyEvent(LEFT_KEY);
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
}

RefPtr<Element> ListElementTest::BuildElementTree(const RefPtr<Component>& listComponent)
{
    RefPtr<ListElement> listElement = AceType::MakeRefPtr<ListElement>();
    listElement->SetNewComponent(listComponent);
    listElement->Mount(rootContainer_);
    return listElement;
}

/**
 * @tc.name: InsertionAndDeletion001
 * @tc.desc: Verify insertion when there is no item.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SV
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion001, TestSize.Level1)
{
    double listHeight = 1000.0;
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listComponent->InsertChild(0, MakeListChild("id0", "name0", "common", 0.0, ITEM_HEIGHT));

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, listHeight));
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());

    auto renderList = multiChildNode->GetChildren().front();

    auto children = renderList->GetChildren();
    int32_t childrenSize = children.size();
    EXPECT_EQ(childrenSize, 1);

    double position = 0.0;
    for (const auto& child : children) {
        auto listItem = RenderListItem::GetRenderListItem(child);
        EXPECT_TRUE(listItem);
        EXPECT_TRUE(NearEqual(listItem->GetPositionInList(), position));

        Size listItemSize = child->GetLayoutSize();
        EXPECT_TRUE(NearEqual(listItemSize.Height(), ITEM_HEIGHT));
        position += ITEM_HEIGHT;
    }
}

std::vector<RefPtr<ComposedComponent>> ListElementTest::MakeListChildArray(double itemHeight)
{
    std::vector<RefPtr<ComposedComponent>> children;
    children.push_back(MakeListChild("id0", "name0", "type0", 0.0, itemHeight));
    children.push_back(MakeListChild("id1", "name1", "type1", 0.0, itemHeight));
    children.push_back(MakeListChild("id2", "name2", "type2", 0.0, itemHeight));
    children.push_back(MakeListChild("id3", "name3", "type3", 0.0, itemHeight));
    children.push_back(MakeListChild("id4", "name4", "type4", 0.0, itemHeight));
    return children;
}

bool ListElementTest::CheckInsertAndDelete(const RefPtr<RenderNode>& multiChildNode, double itemHeight,
    int32_t size, int32_t firstIndex, std::string firstType)
{
    if (multiChildNode->GetChildren().empty()) {
        GTEST_LOG_(INFO) << "multiChildNode has no child";
        return false;
    }

    auto listNode = multiChildNode->GetChildren().front();
    int32_t childrenSize = listNode->GetChildren().size();
    if (childrenSize != size) {
        GTEST_LOG_(INFO) << "size: expect: " << size << ", actual: " << childrenSize;
        return false;
    }

    int32_t index = firstIndex;
    double position = itemHeight * firstIndex;
    auto renderList = AceType::DynamicCast<RenderList>(listNode);
    for (const auto& item : renderList->GetItems()) {
        auto listItem = RenderListItem::GetRenderListItem(item.second);
        if (!listItem) {
            return false;
        }
        if (!NearEqual(listItem->GetPositionInList(), position)) {
            GTEST_LOG_(INFO) << "position: expect: " << position << ", actual: " <<
                listItem->GetPositionInList();
            return false;
        }

        if (listItem->GetIndex() != index) {
            GTEST_LOG_(INFO) << "index: expect: " << index << ", actual: " << listItem->GetIndex();
            return false;
        }

        if (index == firstIndex) {
            if (listItem->GetType() != firstType) {
                GTEST_LOG_(INFO) << "first type: expect: " << firstType << ", actual: " << listItem->GetType();
                return false;
            }
        }
        index++;

        Size listItemSize = item.second->GetLayoutSize();
        if (!NearEqual(listItemSize.Height(), itemHeight)) {
            GTEST_LOG_(INFO) << "height: expect: " << itemHeight << ", actual: " << listItemSize.Height();
            return false;
        }
        position += itemHeight;
    }

    return true;
}

/**
 * @tc.name: InsertionAndDeletion002
 * @tc.desc: Verify insertion and deletion before display range.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SV
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion002, TestSize.Level1)
{
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
    listComponent->SetPositionController(positionController);

    // 0. prepare environment
    auto listItems = MakeListChildArray(ITEM_HEIGHT);
    int32_t listItemsSize = listItems.size();
    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->InsertChild(i, listItems[i]);
    }

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, LIST_HEIGHT));
    auto multiChild = AceType::DynamicCast<RenderMultiChildScroll>(multiChildNode);
    multiChild->SetCacheExtent(CACHE_EXTENT);
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    positionController->JumpTo(ITEM_HEIGHT);
    multiChildNode->PerformLayout();
    int32_t childrenSize = listNode->GetChildren().size();
    EXPECT_EQ(childrenSize, 2);

    // 1. same position
    auto newChild0 = MakeListChild("newid0", "newname0", "newtype0", 0.0, ITEM_HEIGHT);
    listComponent->SetPageReady(true);
    listComponent->InsertChild(0, newChild0);
    listComponent->RemoveChild(newChild0);
    listComponent->InsertChild(0, newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 3, 0, "newtype0"));

    // 2. different position
    positionController->JumpTo(ITEM_HEIGHT * 2);
    multiChildNode->PerformLayout();
    auto newChild1 = MakeListChild("newid1", "newname1", "newtype1", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(1, newChild1);
    listComponent->RemoveChild(newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 4, 0, "newtype1"));

    positionController->JumpTo(0.0);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 0, "newtype1"));
}

/**
 * @tc.name: InsertionAndDeletion003
 * @tc.desc: Verify insertion and deletion at beginning of the display range.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SV
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion003, TestSize.Level1)
{
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
    listComponent->SetPositionController(positionController);

    // 0. prepare environment
    auto listItems = MakeListChildArray(ITEM_HEIGHT);
    int32_t listItemsSize = listItems.size();
    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->InsertChild(i, listItems[i]);
    }

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, LIST_HEIGHT));
    auto multiChild = AceType::DynamicCast<RenderMultiChildScroll>(multiChildNode);
    multiChild->SetCacheExtent(CACHE_EXTENT);
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    positionController->JumpTo(ITEM_HEIGHT);
    multiChildNode->PerformLayout();
    int32_t childrenSize = listNode->GetChildren().size();
    // viewPort is [50.0, 150.0], CACHE_EXTENT is 0.0, so it's 2.
    EXPECT_EQ(childrenSize, 2);

    // 1. same position
    auto newChild0 = MakeListChild("newid0", "newname0", "newtype0", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(1, newChild0);
    listComponent->RemoveChild(newChild0);
    listComponent->InsertChild(1, newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 3, 0, "type0"));

    // 2. different position
    auto newChild1 = MakeListChild("newid1", "newname1", "newtype1", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(2, newChild1);
    listComponent->RemoveChild(newChild1);
    listComponent->InsertChild(2, newChild1);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 0, "type0"));

    positionController->JumpTo(ITEM_HEIGHT * 2);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "newtype0"));
}

/**
 * @tc.name: InsertionAndDeletion004
 * @tc.desc: Verify insertion and deletion at middle of the display range.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SV
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion004, TestSize.Level1)
{
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
    listComponent->SetPositionController(positionController);

    // 0. prepare environment
    auto listItems = MakeListChildArray(ITEM_HEIGHT);
    int32_t listItemsSize = listItems.size();
    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->InsertChild(i, listItems[i]);
    }

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, LIST_HEIGHT));
    auto multiChild = AceType::DynamicCast<RenderMultiChildScroll>(multiChildNode);
    multiChild->SetCacheExtent(CACHE_EXTENT);
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    positionController->JumpTo(ITEM_HEIGHT);
    multiChildNode->PerformLayout();
    int32_t childrenSize = listNode->GetChildren().size();
    // viewPort is [50.0, 150.0], CACHE_EXTENT is 0.0, so it's 2.
    EXPECT_EQ(childrenSize, 2);

    // 1. same position
    auto newChild0 = MakeListChild("newid0", "newname0", "newtype0", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(2, newChild0);
    listComponent->RemoveChild(newChild0);
    listComponent->InsertChild(2, newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "type1"));

    // 2. different position
    auto newChild1 = MakeListChild("newid1", "newname1", "newtype1", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(3, newChild1);
    listComponent->RemoveChild(newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "type1"));

    positionController->JumpTo(ITEM_HEIGHT * 2);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 2, "newtype1"));
}

/**
 * @tc.name: InsertionAndDeletion005
 * @tc.desc: Verify insertion and deletion at end of the display range.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T0
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion005, TestSize.Level1)
{
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
    listComponent->SetPositionController(positionController);

    // 0. prepare environment
    auto listItems = MakeListChildArray(ITEM_HEIGHT);
    int32_t listItemsSize = listItems.size();
    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->InsertChild(i, listItems[i]);
    }

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, LIST_HEIGHT));
    auto multiChild = AceType::DynamicCast<RenderMultiChildScroll>(multiChildNode);
    multiChild->SetCacheExtent(CACHE_EXTENT);
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    positionController->JumpTo(ITEM_HEIGHT);
    multiChildNode->PerformLayout();
    int32_t childrenSize = listNode->GetChildren().size();
    // viewPort is [50.0, 150.0], CACHE_EXTENT is 0.0, so it's 2.
    EXPECT_EQ(childrenSize, 2);

    // 1. same position
    auto newChild0 = MakeListChild("newid0", "newname0", "newtype0", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(3, newChild0);
    listComponent->RemoveChild(newChild0);
    listComponent->InsertChild(3, newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "type1"));

    // 2. different position
    auto newChild1 = MakeListChild("newid1", "newname1", "newtype1", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(3, newChild1);
    listComponent->RemoveChild(newChild1);
    listComponent->InsertChild(3, newChild1);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "type1"));

    // Long jump is not support now, so jump one by one.
    positionController->JumpTo(ITEM_HEIGHT * 2);
    multiChildNode->PerformLayout();
    positionController->JumpTo(ITEM_HEIGHT * 3);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 3, "newtype1"));

    positionController->JumpTo(ITEM_HEIGHT * 4);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 4, "newtype0"));
}

/**
 * @tc.name: InsertionAndDeletion006
 * @tc.desc: Verify insertion and deletion after the display range.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T0
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion006, TestSize.Level1)
{
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
    listComponent->SetPositionController(positionController);

    // 0. prepare environment
    auto listItems = MakeListChildArray(ITEM_HEIGHT);
    int32_t listItemsSize = listItems.size();
    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->InsertChild(i, listItems[i]);
    }

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, LIST_HEIGHT));
    auto multiChild = AceType::DynamicCast<RenderMultiChildScroll>(multiChildNode);
    multiChild->SetCacheExtent(CACHE_EXTENT);
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    positionController->JumpTo(ITEM_HEIGHT);
    multiChildNode->PerformLayout();
    int32_t childrenSize = listNode->GetChildren().size();
    // viewPort is [50.0, 150.0], CACHE_EXTENT is 0.0, so it's 2.
    EXPECT_EQ(childrenSize, 2);

    // 1. same position
    auto newChild0 = MakeListChild("newid0", "newname0", "newtype0", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(4, newChild0);
    listComponent->RemoveChild(newChild0);
    listComponent->InsertChild(4, newChild0);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "type1"));

    // 2. different position
    auto newChild1 = MakeListChild("newid1", "newname1", "newtype1", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(6, newChild1);
    listComponent->RemoveChild(listItems[4]);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 1, "type1"));

    // Long jump is not support now, so jump one by one.
    positionController->JumpTo(ITEM_HEIGHT * 2);
    multiChildNode->PerformLayout();
    positionController->JumpTo(ITEM_HEIGHT * 3);
    multiChildNode->PerformLayout();
    positionController->JumpTo(ITEM_HEIGHT * 4);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 4, "newtype0"));

    auto newChild2 = MakeListChild("newid2", "newname2", "newtype2", 0.0, ITEM_HEIGHT);
    listComponent->AppendChild(newChild2);
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    positionController->JumpTo(ITEM_HEIGHT * 5);
    multiChildNode->PerformLayout();
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 2, 5, "newtype1"));
}

/**
 * @tc.name: InsertionAndDeletion007
 * @tc.desc: Verify deletion when there is no item.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T0
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion007, TestSize.Level1)
{
    double listHeight = 1000.0;
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listComponent->RemoveChild(MakeListChild("id0", "name0", "common", 0.0, ITEM_HEIGHT));

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, listHeight));
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());

    auto renderList = multiChildNode->GetChildren().front();

    auto children = renderList->GetChildren();
    int32_t childrenSize = children.size();
    EXPECT_EQ(childrenSize, 0);
}

/**
 * @tc.name: InsertionAndDeletion008
 * @tc.desc: Verify delete all.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T0
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion008, TestSize.Level1)
{
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    auto positionController = AceType::MakeRefPtr<ScrollPositionController>();
    listComponent->SetPositionController(positionController);

    // 0. prepare environment
    auto listItems = MakeListChildArray(ITEM_HEIGHT);
    int32_t listItemsSize = listItems.size();
    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->InsertChild(i, listItems[i]);
    }

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, LIST_HEIGHT));
    auto multiChild = AceType::DynamicCast<RenderMultiChildScroll>(multiChildNode);
    multiChild->SetCacheExtent(CACHE_EXTENT);
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    positionController->JumpTo(ITEM_HEIGHT);
    multiChildNode->PerformLayout();
    int32_t childrenSize = listNode->GetChildren().size();
    // viewPort is [50.0, 150.0], CACHE_EXTENT is 0.0, so it's 2.
    EXPECT_EQ(childrenSize, 2);

    for (int32_t i = 0; i < listItemsSize; i++) {
        listComponent->RemoveChild(listItems[i]);
    }
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();
    childrenSize = listNode->GetChildren().size();
    EXPECT_EQ(childrenSize, 0);
    EXPECT_TRUE(CheckInsertAndDelete(multiChildNode, ITEM_HEIGHT, 0, 0, ""));
}

/**
 * @tc.name: InsertionAndDeletion009
 * @tc.desc: Verify insertion and deletion randomly.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T1
 * @tc.author: zhanghaibo
 */
HWTEST_F(ListElementTest, InsertionAndDeletion009, TestSize.Level1)
{
    double listHeight = 1000.0;
    auto listComponent = AceType::MakeRefPtr<ListComponent>();
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    auto child0 = MakeListChild("id0", "name0", "common", 0.0, ITEM_HEIGHT);
    auto child1 = MakeListChild("id1", "name1", "common", 0.0, ITEM_HEIGHT);
    auto child2 = MakeListChild("id2", "name2", "common", 0.0, ITEM_HEIGHT);
    listComponent->InsertChild(0, child0);
    listComponent->InsertChild(1, child1);
    listComponent->InsertChild(2, child2);

    RefPtr<Element> listElement = BuildElementTree(listComponent);
    auto multiChildNode = listElement->GetRenderNode();
    DisableAddDeleteEffect(multiChildNode);

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(LIST_WIDTH, listHeight));
    multiChildNode->SetLayoutParam(layoutParam);
    multiChildNode->PerformLayout();

    ASSERT_FALSE(multiChildNode->GetChildren().empty());
    auto listNode = multiChildNode->GetChildren().front();

    // insert
    listComponent->InsertChild(0, MakeListChild("newid0", "newname0", "common", 0.0, ITEM_HEIGHT));
    listComponent->InsertChild(1, MakeListChild("newid1", "newname1", "common", 0.0, ITEM_HEIGHT));
    rootContainer_->UpdateChild(listElement, listComponent);
    multiChildNode->PerformLayout();

    listNode = multiChildNode->GetChildren().front();
    auto children = listNode->GetChildren();
    int32_t childrenSize = children.size();
    EXPECT_EQ(childrenSize, 5);

    int32_t index = 0;
    double position = 0.0;
    auto renderList = AceType::DynamicCast<RenderList>(listNode);
    for (const auto& item : renderList->GetItems()) {
        auto listItem = RenderListItem::GetRenderListItem(item.second);
        EXPECT_TRUE(listItem);
        EXPECT_TRUE(NearEqual(listItem->GetPositionInList(), position));
        EXPECT_EQ(listItem->GetIndex(), index);
        index++;

        Size listItemSize = item.second->GetLayoutSize();
        EXPECT_TRUE(NearEqual(listItemSize.Height(), ITEM_HEIGHT));
        position += ITEM_HEIGHT;
    }
}

/**
 * @tc.name: ListElementItemFocusTest001
 * @tc.desc: Verify list item by render sequence.
 * @tc.type: FUNC
 * @tc.require: AR000DR6DF
 * @tc.author: jinwuwen
 */
HWTEST_F(ListElementTest, ListElementItemFocusTest001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create list.
    * @tc.expected: step1. create list correctly and item 0 on focus.
    */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < ITEM_COUNT; ++i) {
        listComponent->AppendChild(MakeListChild("list1_id", "list1_name"));
    }
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);
    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 1000.0));
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();
    rootContainer_->RequestFocusImmediately();

    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
    * @tc.steps: step2. Animation to the down
    * @tc.expected: step2. RootContainer and listOne acquire the focus. listOne's second child Get the focus.
    */
    listOne_->RequestNextFocus(true, false, Rect());
    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 1));
}

/**
 * @tc.name: ListElementItemFocusTest002
 * @tc.desc: Verify list item support long jump.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1T9
 * @tc.author: jinwuwen
 */
HWTEST_F(ListElementTest, ListElementItemFocusTest002, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create list with 50 item.
    * @tc.expected: step1. create list correctly and item 0 on focus.
    */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < 50; ++i) {
        listComponent->AppendChild(MakeListChild("list1_id", "list1_name"));
    }
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);
    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 5000.0));
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();
    rootContainer_->RequestFocusImmediately();

    /**
    * @tc.steps: step2. set item focusable false partly.
    * @tc.expected: step2. set item focusable false correctly.
    */
    for (auto focusNode : listOne_->GetChildrenList()) {
        auto listItem = AceType::DynamicCast<ListItemElement>(focusNode);
        if (listItem->GetIndex() > 0 && listItem->GetIndex() < 48) {
            focusNode->SetFocusable(false);
        }
    }

    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
    * @tc.steps: step3. Animation to the down
    * @tc.expected: step3. RootContainer and listOne acquire the focus. item 48 Get the focus.
    */
    listOne_->RequestNextFocus(true, false, Rect());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 48));

    /**
    * @tc.steps: step4. Animation to the down
    * @tc.expected: step4. RootContainer and listOne acquire the focus. item 49 Get the focus.
    */
    listOne_->RequestNextFocus(true, false, Rect());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 49));
}

/**
 * @tc.name: ListElementItemFocusTest003
 * @tc.desc: Verify list item support long jump.
 * @tc.type: FUNC
 * @tc.require: AR000DRS0L
 * @tc.author: jinwuwen
 */
HWTEST_F(ListElementTest, ListElementItemFocusTest003, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create list with 50 item.
    * @tc.expected: step1. create list correctly and item 0 on focus.
    */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < 50; ++i) {
        listComponent->AppendChild(MakeListChild("list1_id", "list1_name"));
    }
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);
    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 5000.0));
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();
    rootContainer_->RequestFocusImmediately();

    /**
    * @tc.steps: step2. set item focusable false partly.
    * @tc.expected: step2. set item focusable false correctly.
    */
    for (auto focusNode : listOne_->GetChildrenList()) {
        auto listItem = AceType::DynamicCast<ListItemElement>(focusNode);
        if (listItem->GetIndex() > 0 && listItem->GetIndex() < 48) {
            focusNode->SetFocusable(false);
        }
    }

    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
    * @tc.steps: step3. Animation to the down
    * @tc.expected: step3. RootContainer and listOne acquire the focus. item 48 Get the focus.
    */
    listOne_->RequestNextFocus(true, false, Rect());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 48));

    /**
    * @tc.steps: step4. Animation to the up
    * @tc.expected: step4. RootContainer and listOne acquire the focus. item 0 Get the focus.
    */
    listOne_->RequestNextFocus(true, true, Rect());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
}

/**
 * @tc.name: ListItemFocusTest004
 * @tc.desc: Verify list item support long jump.
 * @tc.type: FUNC
 * @tc.require: AR000DR6DF
 * @tc.author: jinwuwen
 */
HWTEST_F(ListElementTest, ListElementItemFocusTest004, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create list with 50 item.
    * @tc.expected: step1. create list correctly and item 0 on focus.
    */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < 50; ++i) {
        listComponent->AppendChild(MakeListChild("list1_id", "list1_name", "focus", 200, ITEM_HEIGHT));
    }
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);
    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 5000.0));
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();
    rootContainer_->RequestFocusImmediately();

    /**
    * @tc.steps: step2. set item focusable false partly.
    * @tc.expected: step2. set item focusable false correctly.
    */
    for (auto focusNode : listOne_->GetChildrenList()) {
        auto listItem = AceType::DynamicCast<ListItemElement>(focusNode);
        if (listItem->GetIndex() > 0 && listItem->GetIndex() < 48) {
            focusNode->SetFocusable(false);
        }
    }

    EXPECT_TRUE(rootContainer_->IsCurrentFocus());
    EXPECT_TRUE(listOne_->IsCurrentFocus());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));

    /**
    * @tc.steps: step3. Animation to the down
    * @tc.expected: step3. RootContainer and listOne acquire the focus. item 48 Get the focus.
    */
    listOne_->RequestNextFocus(true, false, Rect());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 48));

    /**
    * @tc.steps: step4. Get the focus item position in list
    * @tc.expected: step4. position in list may not be zero when item index is not zero.
    */
    auto renderList = AceType::DynamicCast<RenderList>(renderNodeOne->GetChildren().front());
    ASSERT_TRUE(renderList);
    auto itemChild = renderList->GetChildByIndex(48);
    ASSERT_TRUE(itemChild);
    auto listItem = RenderListItem::GetRenderListItem(itemChild);
    ASSERT_TRUE(listItem);
    double focusPostion = listItem->GetPositionInList();
    EXPECT_TRUE(NearEqual(focusPostion, ITEM_HEIGHT * 48));
}

/**
 * @tc.name: ListItemGroupFocusTest001
 * @tc.desc: Verify list item group nextFocusNode when item group is collapse.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TB
 * @tc.author: chenlien
 */
HWTEST_F(ListElementTest, ListItemGroupFocusTest001, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create list with 50 item.
    * @tc.expected: step1. create list correctly and item 0 on focus.
    */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < 50; ++i) {
        listComponent->AppendChild(MakeListItemGroupChild("list1_id", "list1_name", "focus", 200, ITEM_HEIGHT));
    }
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);
    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 5000.0));
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();
    rootContainer_->RequestFocusImmediately();

    /**
    * @tc.steps: step3. Animation to the down
    * @tc.expected: step3. expand the first item group, and then focus the first and second item
    */
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
    auto listItemElement = ListItemElement::GetListItem(listOne_->GetFirstChild());
    if (listItemElement) {
        auto listItemGroupElement = AceType::DynamicCast<ListItemGroupElement>(listItemElement);
        EXPECT_TRUE(listItemGroupElement->GetItemGroupFocusIndex() == 0);
        listItemGroupElement->RequestNextFocus(true, false, Rect());
        EXPECT_TRUE(listItemGroupElement->GetItemGroupFocusIndex() == 0);
    }
}

/**
 * @tc.name: ListItemGroupFocusTest002
 * @tc.desc: Verify list item group nextFocusNode when item group is expanded.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TB
 * @tc.author: chenlien
 */
HWTEST_F(ListElementTest, ListItemGroupFocusTest002, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create list with 50 item.
    * @tc.expected: step1. create list correctly and item 0 on focus.
    */
    RefPtr<ListComponent> listComponent = AceType::MakeRefPtr<ListComponent>();
    for (int32_t i = 0; i < 50; ++i) {
        listComponent->AppendChild(MakeListItemGroupChild("list1_id", "list1_name", "focus", 200, ITEM_HEIGHT));
    }
    listComponent->SetDirection(FlexDirection::COLUMN);
    listComponent->SetPositionController(AceType::MakeRefPtr<ScrollPositionController>());
    listOne_->SetNewComponent(listComponent);
    listOne_->Mount(rootContainer_);
    auto renderNodeOne = listOne_->GetRenderNode();

    LayoutParam layoutParam;
    layoutParam.SetMinSize(Size(0.0, 0.0));
    layoutParam.SetMaxSize(Size(1000.0, 5000.0));
    renderNodeOne->SetLayoutParam(layoutParam);
    renderNodeOne->PerformLayout();
    rootContainer_->RequestFocusImmediately();

    /**
    * @tc.steps: step3. Animation to the down
    * @tc.expected: step3. expand the first item group, and then focus the first and second item，
    * the list item group focus index will be 2
    */
    auto listItemElement = ListItemElement::GetListItem(listOne_->GetFirstChild());
    EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
    if (listItemElement) {
        auto listItemGroupElement = AceType::DynamicCast<ListItemGroupElement>(listItemElement);
        auto renderNode = listItemGroupElement->GetRenderNode();
        auto itemGroupRender = AceType::DynamicCast<RenderListItemGroup>(renderNode);
        itemGroupRender->SetExpand(true);
        EXPECT_TRUE(listItemGroupElement->GetItemGroupFocusIndex() == 0);
        listItemGroupElement->RequestNextFocus(true, false, Rect());
        EXPECT_TRUE(listItemGroupElement->GetItemGroupFocusIndex() == 1);
        listItemGroupElement->RequestNextFocus(true, false, Rect());
        EXPECT_TRUE(listItemGroupElement->GetItemGroupFocusIndex() == 2);
        EXPECT_TRUE(CheckItemFocus(listOne_->GetChildren(), 0));
    }
}

} // namespace OHOS::Ace
