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

#include "base/i18n/localization.h"
#include "core/animation/curves.h"
#include "core/components/arc/arc_component.h"
#include "core/components/arc/render_arc.h"
#include "core/components/indexer/indexer_list_component.h"
#include "core/components/indexer/render_indexer_circle.h"
#include "core/components/list/list_item_component.h"
#include "core/components/test/unittest/indexer/indexer_test_utils.h"
#include "core/components/test/unittest/mock/mock_render_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class RenderIndexerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    // check item visible status exclude collapse item;
    bool CheckItemVisible(RefPtr<RenderIndexer> renderIndexer) const;
    RefPtr<MockRenderText> GetBubbleText(RefPtr<RenderIndexer> renderIndexer) const;
    RefPtr<RenderNode> CreateRenderIndexer(RefPtr<IndexerComponent> indexerComponent, bool isCircle, bool isBubble)
        const;

    RefPtr<PipelineContext> mockContext_;
    RefPtr<MockRenderIndexList> indexerList_;

    int32_t index_ = 0;
};

RefPtr<MockRenderText> RenderIndexerTest::GetBubbleText(RefPtr<RenderIndexer> renderIndexer) const
{
    auto children = renderIndexer->GetChildren();
    for (auto iter = children.begin(); iter != children.end(); ++iter) {
        if (AceType::InstanceOf<MockRenderDisplay>(*iter)) {
            RefPtr<MockRenderBox> renderBox = AceType::DynamicCast<MockRenderBox>((*iter)->GetChildren().front());
            if (!renderBox) {
                return nullptr;
            }
            RefPtr<MockRenderText> renderText = AceType::DynamicCast<MockRenderText>(renderBox->GetChildren().front());
            return renderText;
        }
    }
    return nullptr;
}

bool RenderIndexerTest::CheckItemVisible(RefPtr<RenderIndexer> renderIndexer) const
{
    auto renderCircleIndexer = AceType::DynamicCast<MockRenderIndexerCircle>(renderIndexer);
    if (!renderCircleIndexer) {
        return false;
    }
    auto arc = AceType::DynamicCast<MockRenderArc>(renderIndexer->GetChildren().front());
    if (!arc) {
        return false;
    }

    IndexerItemStatus curStatus = renderCircleIndexer->GetIndexerStatus();
    auto iter = renderCircleIndexer->GetChildren().begin();
    while (!AceType::InstanceOf<RenderIndexerItem>(*iter)) {
        ++iter;
    }
    if (curStatus == IndexerItemStatus::EXPAND) {
        for (; iter != renderCircleIndexer->GetChildren().end(); ++iter) {
            auto item = AceType::DynamicCast<RenderIndexerItem>(*iter);
            // skip ">"
            if (item->GetSectionText() == ">") {
                return true;
            }
            if (renderCircleIndexer->NeedProcess(*iter) != (*iter)->GetVisible()) {
                return false;
            }
        }
    } else if (curStatus == IndexerItemStatus::COLLAPSE) {
        int32_t index = 0;
        int32_t maxCount = renderCircleIndexer->GetMaxShowItemCount();
        double itemExtent = 2 * M_PI / maxCount;
        double arcHead = renderCircleIndexer->GetArcHeadOffset() + INDEXER_ARC_BEGIN; // actual arc head position
        for (; iter != renderCircleIndexer->GetChildren().end(); ++iter) {
            auto item = AceType::DynamicCast<RenderIndexerItem>(*iter);
            // skip ">"
            if (item->GetSectionText() == ">") {
                return true;
            }
            if (renderCircleIndexer->NeedProcess(*iter) && index < INDEXER_COLLAPSE_ITEM_COUNT) {
                auto position = arcHead - itemExtent / 2.0;
                arcHead +=  itemExtent;
                if (arc->IsInRange(position, itemExtent / 2.0)) {
                    ++index;
                }
                if (arc->IsInRange(position, itemExtent / 2.0) != (*iter)->GetVisible()) {
                    return false;
                }
            } else {
                if ((*iter)->GetVisible()) {
                    return false;
                }
            }
        }
    }
    return true;
}

RefPtr<RenderNode> RenderIndexerTest::CreateRenderIndexer(RefPtr<IndexerComponent> indexerComponent, bool isCircle,
    bool isBubble) const
{
    RefPtr<RenderNode> renderIndexer;
    if (isCircle) {
        renderIndexer = AceType::MakeRefPtr<MockRenderIndexerCircle>();
    } else {
        renderIndexer = AceType::MakeRefPtr<MockRenderIndexer>();
    }
    renderIndexer->Attach(mockContext_);

    auto children = indexerComponent->GetChildren();
    auto iter = children.begin();
    if (isCircle) {
        // add arc
        if (AceType::InstanceOf<ArcComponent>(*iter)) {
            RefPtr<MockRenderArc> renderArc = AceType::MakeRefPtr<MockRenderArc>();
            renderArc->Attach(mockContext_);
            renderArc->Update(*iter);
            renderIndexer->AddChild(renderArc);
        }
        ++iter;
        // add indicatorBox
        if (AceType::InstanceOf<BoxComponent>(*iter)) {
            RefPtr<MockRenderBox> renderIndicator = AceType::MakeRefPtr<MockRenderBox>();
            renderIndicator->Attach(mockContext_);
            renderIndicator->Update(*iter);
            renderIndexer->AddChild(renderIndicator);
        }
        ++iter;
    }
    if (isBubble) {
        // add bubbleIndex
        auto display = AceType::DynamicCast<DisplayComponent>(*iter);
        auto box = AceType::DynamicCast<BoxComponent>(display->GetChild());
        auto text = AceType::DynamicCast<TextComponent>(box->GetChild());

        RefPtr<MockRenderText> renderText = AceType::MakeRefPtr<MockRenderText>();
        renderText->Attach(mockContext_);
        renderText->Update(text);
        RefPtr<MockRenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
        renderBox->Attach(mockContext_);
        renderBox->AddChild(renderText);
        renderBox->Update(box);
        RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();
        renderDisplay->Attach(mockContext_);
        renderDisplay->AddChild(renderBox);
        renderDisplay->Update(display);
        renderIndexer->AddChild(renderDisplay);
    }

    std::list<RefPtr<IndexerItemComponent>> itemList = indexerComponent->GetIndexerItemsComponents();
    for (auto item : itemList) {
        RefPtr<RenderIndexerItem> renderIndexerItem = AceType::MakeRefPtr<RenderIndexerItem>();
        renderIndexerItem->Attach(mockContext_);
        renderIndexerItem->Update(item);
        renderIndexer->AddChild(renderIndexerItem);
    }
    renderIndexer->Update(indexerComponent);
    return renderIndexer;
}

void RenderIndexerTest::SetUpTestCase()
{
    Localization::GetInstance()->SetLocale("zh", "CN", "", "", "");
    GTEST_LOG_(INFO) << "IndexerTest SetUpTestCase";
}

void RenderIndexerTest::TearDownTestCase() {}

void RenderIndexerTest::SetUp()
{
    mockContext_ = MockRenderCommon::GetMockContext();
    indexerList_ = AceType::MakeRefPtr<MockRenderIndexList>();
    indexerList_->Attach(mockContext_);
    index_ = 0;
}

void RenderIndexerTest::TearDown()
{
    mockContext_ = nullptr;
    indexerList_ = nullptr;
}

/**
 * @tc.name: RenderIndexerTest001
 * @tc.desc: Verify IndexList can create with list mode, the input is sorted.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TJ
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        indexer->SetCircleMode(false);
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 5; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 8);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[4]);
    ASSERT_TRUE(index == 6);
}

/**
 * @tc.name: RenderIndexerTest002
 * @tc.desc: Verify IndexList can create with list mode, the input is not sorted.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TK
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        indexer->SetCircleMode(false);
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1" };
    for (int32_t i = 0; i < 5; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 8);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[4]);
    ASSERT_TRUE(index == 0);
}

/**
 * @tc.name: RenderIndexerTest003
 * @tc.desc: Verify IndexList can return section index by item index.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TM
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        indexer->SetCircleMode(false);
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1" };
    for (int32_t i = 0; i < 5; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 8);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInIndexer(4);
    ASSERT_TRUE(index == 2);
}

/**
 * @tc.name: RenderIndexerTest004
 * @tc.desc: Verify IndexList can create with circle mode, the input is not sorted.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = false;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1" };
    for (int32_t i = 0; i < 5; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 5);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[1]);
    ASSERT_TRUE(index == 2);
}

/**
 * @tc.name: RenderIndexerTest005
 * @tc.desc: Verify IndexList can return section index by item index.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = false;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1" };
    for (int32_t i = 0; i < 5; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 5);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInIndexer(3);
    ASSERT_TRUE(index == 3);
}

/**
 * @tc.name: RenderIndexerTest006
 * @tc.desc: Verify IndexList can create with list mode, the input is not sorted, and the indexkey is NULL.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TK
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        indexer->SetCircleMode(false);
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[4]);
    ASSERT_TRUE(index == 2);
}

/**
 * @tc.name: RenderIndexerTest007
 * @tc.desc: Verify IndexList can index with circle mode, the input is not sorted, and the indexkey is NULL.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TK
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with circle mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = false;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[4]);
    ASSERT_TRUE(index == 1);
}

/**
 * @tc.name: RenderIndexerTest008
 * @tc.desc: Verify IndexList can create with list mode, return section index by item index, and the indexkey is Null.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TM
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        indexer->SetCircleMode(false);
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInIndexer(6);
    ASSERT_TRUE(index == 2);
}

/**
 * @tc.name: RenderIndexerTest009
 * @tc.desc: Verify IndexList can create with circle mode, return section index by item index, and the indexkey is
 * number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TM
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent circle list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = false;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInIndexer(4);
    ASSERT_TRUE(index == 3);
}

/**
 * @tc.name: RenderIndexerTest010
 * @tc.desc: Verify IndexList can index with list mode, the input is not sorted, and the indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "123" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[1]);
    ASSERT_TRUE(index == 5);
}

/**
 * @tc.name: RenderIndexerTest011
 * @tc.desc: Verify IndexList can return section index by item index with list mode, while indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "123" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInIndexer(3);
    ASSERT_TRUE(index == 1);
}

/**
 * @tc.name: RenderIndexerTest012
 * @tc.desc: Verify IndexList can index with circle mode, the input is not sorted, and the indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = false;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "123" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section head is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInList(indexKey[1]);
    ASSERT_TRUE(index == 3);
}

/**
 * @tc.name: RenderIndexerTest013
 * @tc.desc: Verify IndexList can return section index by item index with circle mode, while indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = false;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "123" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    int32_t index = indexerList->GetIndexer()->GetSectionIndexInIndexer(3);
    ASSERT_TRUE(index == 3);
}

/**
 * @tc.name: RenderIndexerTest014
 * @tc.desc: Verify IndexList can return section index by item index with list mode, while indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "123" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    std::string itemIndexKey;
    RefPtr<ListItemComponent> firstItem = ListItemComponent::GetListItem(list->GetChildren().front());
    if (firstItem) {
        itemIndexKey = firstItem->GetIndexKey();
    }
    ASSERT_TRUE(itemIndexKey == "#");
}

/**
 * @tc.name: RenderIndexerTest015
 * @tc.desc: Verify IndexList can return section index by item index with list mode, while indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "D1", "B2", "B1", "A2", "A1", "123" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    bool sticky = false;
    RefPtr<ListItemComponent> firstItem = ListItemComponent::GetListItem(list->GetChildren().front());
    if (firstItem) {
        sticky = firstItem->GetSticky();
    }
    ASSERT_TRUE(sticky);
}

/**
 * @tc.name: RenderIndexerTest016
 * @tc.desc: Verify IndexList can return section index by item index with list mode, while indexkey is number.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1TP
 * @tc.author: xuliangjun
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    std::string itemIndexKey;
    RefPtr<ListItemComponent> lastItem = ListItemComponent::GetListItem(list->GetChildren().back());
    if (lastItem) {
        itemIndexKey = lastItem->GetIndexKey();
    }
    ASSERT_TRUE(itemIndexKey == "D1");
}

/**
 * @tc.name: RenderIndexerTest017
 * @tc.desc: Verify IndexList has a bubble component with default setting.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCD
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList = AceType::MakeRefPtr<IndexerListComponent>(list, false, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 10);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    EXPECT_TRUE(indexer->GetChildren().size() == 28);
    auto child = indexer->GetChildren().front();
    auto display = AceType::DynamicCast<DisplayComponent>(child);
    ASSERT_TRUE(display);
}

/**
 * @tc.name: RenderIndexerTest018
 * @tc.desc: Verify IndexList children count with circle mode、 non-bubble、multi-language.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCD
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, false, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    auto itemList = indexer->GetIndexerItemsComponents();
    EXPECT_TRUE(itemList.size() == 30);
    EXPECT_TRUE(indexer->GetChildren().size() == 32);
    auto iter = indexer->GetChildren().begin();
    auto arc = AceType::DynamicCast<ArcComponent>(*iter);
    ASSERT_TRUE(arc);
    ++iter;
    auto box = AceType::DynamicCast<BoxComponent>(*iter);
    ASSERT_TRUE(box);
    ++iter;
    auto item = AceType::DynamicCast<IndexerItemComponent>(*iter);
    ASSERT_TRUE(item);
}

/**
 * @tc.name: RenderIndexerTest019
 * @tc.desc: Verify IndexList can shown correctly with circle-mode、bubble、multi-language.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCD
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    ASSERT_TRUE(AceType::InstanceOf<RenderIndexerCircle>(renderNode));
    RefPtr<MockRenderIndexerCircle> renderIndex019 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    ASSERT_TRUE(renderIndex019);
    ASSERT_TRUE(renderIndex019->GetChildren().size() == indexer->GetChildren().size());
    ASSERT_TRUE(renderIndex019->GetCircleMode());
    ASSERT_TRUE(renderIndex019->GetIndexerStatus() == IndexerItemStatus::EXPAND);
    ASSERT_TRUE(renderIndex019->GetFocusItemIndex() == -1);
    renderIndex019->PerformLayout();
    EXPECT_TRUE(renderIndex019->GetFocusItemIndex() == 1);
    EXPECT_TRUE(renderIndex019->GetSpecificItem(1)->GetKeyCount() == 1); // "1"
    EXPECT_TRUE(renderIndex019->GetSpecificItem(5)->GetKeyCount() == 2); // "A"
    EXPECT_TRUE(renderIndex019->GetSpecificItem(6)->GetKeyCount() == 2); // "B"
    EXPECT_TRUE(renderIndex019->GetSpecificItem(7)->GetKeyCount() == 0); // "C"
    EXPECT_TRUE(renderIndex019->GetSpecificItem(8)->GetKeyCount() == 1); // "D"
    EXPECT_TRUE(CheckItemVisible(renderIndex019));
}

/**
 * @tc.name: RenderIndexerTest020
 * @tc.desc: Verify IndexList can change language when touch specific item when on expand status.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCD
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex020 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex020->PerformLayout();
    ASSERT_TRUE(renderIndex020->GetCurrentItemType() == 0);
    ASSERT_TRUE(renderIndex020->GetIndexerStatus() == IndexerItemStatus::EXPAND);
    ASSERT_TRUE(!renderIndex020->NeedChangeLanguage(1)); // "1"
    ASSERT_TRUE(renderIndex020->NeedChangeLanguage(5));  // "A"
    ASSERT_TRUE(renderIndex020->GetSpecificItem(5)->GetVisible());
    ASSERT_TRUE(!renderIndex020->GetSpecificItem(6)->GetVisible()); // "B" is not visible
    ASSERT_TRUE(renderIndex020->GetActualItemIndex(3) == 3);  // position 3 is "3"
    EXPECT_TRUE(CheckItemVisible(renderIndex020));
}

/**
 * @tc.name: RenderIndexerTest021
 * @tc.desc: Verify IndexList can change language when touch specific item when on collapse status.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCD
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex021 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex021->PerformLayout();

    renderIndex021->CollapseItems();
    ASSERT_TRUE(renderIndex021->GetIndexerStatus() == IndexerItemStatus::ANIMATION);
    ASSERT_TRUE(renderIndex021->GetIndexerNextStatus() == IndexerItemStatus::COLLAPSE);

    renderIndex021->FinishCollapseAnimation();
    ASSERT_TRUE(renderIndex021->GetIndexerStatus() == IndexerItemStatus::COLLAPSE);
    ASSERT_TRUE(renderIndex021->GetCurrentItemType() == 0);
    ASSERT_TRUE(!renderIndex021->NeedChangeLanguage(1)); // "1"
    ASSERT_TRUE(!renderIndex021->NeedChangeLanguage(5));  // "A"
}

/**
 * @tc.name: RenderIndexerTest022
 * @tc.desc: Verify IndexList can shown correctly after language changed.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex022 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex022->PerformLayout();
    EXPECT_TRUE(renderIndex022->GetSpecificItem(0)->GetItemType() == 0);
    EXPECT_TRUE(!renderIndex022->ChangeLanguage(0)); // "#"
    EXPECT_TRUE(!renderIndex022->ChangeLanguage(1));
    ASSERT_TRUE(renderIndex022->ChangeLanguage(5));
    EXPECT_TRUE(renderIndex022->GetCurrentItemType() == 1);
    EXPECT_TRUE(renderIndex022->GetIndexerStatus() == IndexerItemStatus::EXPAND);
    EXPECT_TRUE(renderIndex022->NeedChangeLanguage(1)); // "1"
    EXPECT_TRUE(!renderIndex022->GetSpecificItem(2)->GetVisible()); // "2" is not visible
    EXPECT_TRUE(!renderIndex022->NeedChangeLanguage(5));  // "A"
    EXPECT_TRUE(renderIndex022->GetSpecificItem(5)->GetVisible());
    EXPECT_TRUE(renderIndex022->GetSpecificItem(6)->GetVisible()); // "B" is visible
    EXPECT_TRUE(renderIndex022->GetActualItemIndex(3) == 6); // position 3 is "B"
    EXPECT_TRUE(CheckItemVisible(renderIndex022));
}

/**
 * @tc.name: RenderIndexerTest023
 * @tc.desc: Verify IndexList can change index correctly.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex023 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex023->PerformLayout();
    ASSERT_TRUE(renderIndex023->ChangeLanguage(5));
    EXPECT_TRUE(renderIndex023->GetFocusItemIndex() == 5);

    renderIndex023->SetItemsFocused(7);
    EXPECT_TRUE(!renderIndex023->GetSpecificItem(7)->GetClicked()); // "C" can not clicked
    EXPECT_TRUE(renderIndex023->GetFocusItemIndex() == 5);

    renderIndex023->SetItemsFocused(8);
    renderIndex023->FinishFocusAnimation();
    EXPECT_TRUE(renderIndex023->GetSpecificItem(8)->GetClicked()); // "D" clicked
    EXPECT_TRUE(renderIndex023->GetFocusItemIndex() == 8);

    renderIndex023->CollapseItems();
    renderIndex023->FinishCollapseAnimation();
    ASSERT_TRUE(renderIndex023->GetIndexerStatus() == IndexerItemStatus::COLLAPSE);
    EXPECT_TRUE(renderIndex023->GetSpecificItem(31)->GetVisible()); // ">"
    ASSERT_TRUE(CheckItemVisible(renderIndex023));

    renderIndex023->ExpandItems();
    renderIndex023->FinishCollapseAnimation();
    EXPECT_TRUE(renderIndex023->GetSpecificItem(31)->GetVisible()); // ">"
    EXPECT_TRUE(CheckItemVisible(renderIndex023));
}

/**
 * @tc.name: RenderIndexerTest024
 * @tc.desc: Verify bubble text when focus change on circle mode.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list, labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex024 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex024->PerformLayout();
    ASSERT_TRUE(renderIndex024->ChangeLanguage(5));

    renderIndex024->SetItemsFocused(8);
    renderIndex024->FinishFocusAnimation();
    EXPECT_TRUE(renderIndex024->GetSpecificItem(8)->GetClicked()); // "D" clicked

    RefPtr<MockRenderText> renderText = GetBubbleText(renderIndex024);
    ASSERT_TRUE(renderText);
    EXPECT_TRUE(renderText->GetTextData() == "D");
    renderIndex024->SetItemsFocused(7);
    EXPECT_TRUE(renderText->GetTextData() == "D");
    renderIndex024->SetItemsFocused(6);
    EXPECT_TRUE(renderText->GetTextData() == "B");
    ASSERT_TRUE(renderIndex024->ChangeLanguage(1));
    EXPECT_TRUE(renderText->GetTextData() == "1");
}

/**
 * @tc.name: RenderIndexerTest025
 * @tc.desc: Verify indexer shown correctly when section letter than four.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex025 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex025->PerformLayout();
    RefPtr<RenderIndexerItem> collapse = AceType::DynamicCast<RenderIndexerItem>(renderIndex025->GetChildren().back());
    ASSERT_TRUE(collapse);
    EXPECT_TRUE(collapse->GetSectionText() != ">");
    EXPECT_TRUE(CheckItemVisible(renderIndex025));
}

/**
 * @tc.name: RenderIndexerTest026
 * @tc.desc: Verify indexer shown correctly when section letter than four with multi-language
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest026, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    std::string indexKey[] = { "123", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 6);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex026 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex026->PerformLayout();
    RefPtr<MockRenderText> renderText = GetBubbleText(renderIndex026);

    RefPtr<RenderIndexerItem> collapse = AceType::DynamicCast<RenderIndexerItem>(renderIndex026->GetChildren().back());
    ASSERT_TRUE(collapse);
    EXPECT_TRUE(collapse->GetSectionText() == ">");
    EXPECT_TRUE(!collapse->GetVisible());
    EXPECT_TRUE(CheckItemVisible(renderIndex026));
    EXPECT_TRUE(renderIndex026->GetFocusItemIndex() == 1);
    EXPECT_TRUE(renderIndex026->GetIndexerStatus() == IndexerItemStatus::EXPAND);

    renderIndex026->MoveSection(4);
    EXPECT_TRUE(renderIndex026->GetIndexerStatus() == IndexerItemStatus::EXPAND);
    EXPECT_TRUE(renderIndex026->GetFocusItemIndex() == 1);

    renderIndex026->MoveSection(2);
    EXPECT_TRUE(renderIndex026->GetIndexerStatus() == IndexerItemStatus::EXPAND);
    EXPECT_TRUE(renderIndex026->GetFocusItemIndex() == 3);

    ASSERT_TRUE(renderIndex026->ChangeLanguage(3)); // "A"
    EXPECT_TRUE(collapse->GetVisible());
    EXPECT_TRUE(CheckItemVisible(renderIndex026));

    renderIndex026->CollapseItems();
    renderIndex026->FinishCollapseAnimation();
    ASSERT_TRUE(renderIndex026->GetIndexerStatus() == IndexerItemStatus::COLLAPSE);
    EXPECT_TRUE(CheckItemVisible(renderIndex026));
    EXPECT_TRUE(collapse->GetVisible());
}

/**
 * @tc.name: RenderIndexerTest027
 * @tc.desc: Verify indexer can change focus when list scroll on circle mode.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest027, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    // list is {"9", "123", "19", "31", "32", "A1", "A2", "B1", "B2", "D1", "H1", "H2", "Z1"}
    std::string indexKey[] = { "123", "9", "Z1", "31", "19", "32", "H1", "H2", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 13; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 13);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex027 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex027->PerformLayout();
    EXPECT_TRUE(renderIndex027->GetFocusItemIndex() == 0);
    EXPECT_TRUE(renderIndex027->GetIndexerStatus() == IndexerItemStatus::EXPAND);

    renderIndex027->MoveSection(1);
    if (renderIndex027->GetIndexerStatus() == IndexerItemStatus::ANIMATION) {
        renderIndex027->FinishCollapseAnimation();
    }
    EXPECT_TRUE(renderIndex027->GetIndexerStatus() == IndexerItemStatus::COLLAPSE);
    EXPECT_TRUE(renderIndex027->GetFocusItemIndex() == 1);

    renderIndex027->MoveSection(4);
    EXPECT_TRUE(renderIndex027->GetFocusItemIndex() == 3);
    renderIndex027->MoveSection(2);
    EXPECT_TRUE(renderIndex027->GetFocusItemIndex() == 1);
}

/**
 * @tc.name: RenderIndexerTest028
 * @tc.desc: Verify indexer handle rotation event correctly on circle mode.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCC
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest028, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */

    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    // list is {"9", "123", "19", "31", "32", "A1", "A2", "B1", "B2", "D1", "H1", "H2", "Z1"}
    std::string indexKey[] = { "123", "9", "Z1", "31", "19", "32", "H1", "H2", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 13; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 13);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex028 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex028->PerformLayout();
    EXPECT_TRUE(renderIndex028->GetFocusItemIndex() == 0);
    EXPECT_TRUE(renderIndex028->GetIndexerStatus() == IndexerItemStatus::EXPAND);

    renderIndex028->SetItemsFocused(3);
    EXPECT_TRUE(renderIndex028->GetFocusItemIndex() == 3);

    renderIndex028->HandleRotation(ROTATION_THRESHOLD - 1.0); // can not change
    EXPECT_TRUE(renderIndex028->GetFocusItemIndex() == 3);

    renderIndex028->HandleRotation(ROTATION_THRESHOLD + 1.0); // show previous
    EXPECT_TRUE(renderIndex028->GetFocusItemIndex() == 1);

    renderIndex028->HandleRotation(-(ROTATION_THRESHOLD + 1.0)); // show next
    EXPECT_TRUE(renderIndex028->GetFocusItemIndex() == 3);
}

/**
 * @tc.name: RenderIndexerTest029
 * @tc.desc: Verify indexer can change focus when list scroll on list mode.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCF
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest029, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, false, false, true, false);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(!circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    // list is {"#", "9", "A1","1", "123", "19", "3", "31", "32",}
    std::string indexKey[] = { "123", "9", "31", "19", "32", "A1" };
    for (int32_t i = 0; i < 6; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 9);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, circleMode, true);
    RefPtr<MockRenderIndexer> renderIndex029 = AceType::DynamicCast<MockRenderIndexer>(renderNode);
    ASSERT_TRUE(renderIndex029);
    renderIndex029->PerformLayout();
    RefPtr<MockRenderText> renderText = GetBubbleText(renderIndex029);
    ASSERT_TRUE(renderText);

    EXPECT_TRUE(renderIndex029->GetFocusItemIndex() == 0);
    renderIndex029->MoveSection(3); // "1"
    EXPECT_TRUE(renderText->GetTextData() == "1");
    EXPECT_TRUE(renderIndex029->GetFocusItemIndex() == 1);

    renderIndex029->MoveSection(7); // "31"
    EXPECT_TRUE(renderText->GetTextData() == "3");
    EXPECT_TRUE(renderIndex029->GetFocusItemIndex() == 3);

    renderIndex029->MoveSection(5); // "19"
    EXPECT_TRUE(renderText->GetTextData() == "1");
    EXPECT_TRUE(renderIndex029->GetFocusItemIndex() == 1);
}

/**
 * @tc.name: RenderIndexerTest030
 * @tc.desc: Verify indexer can change focus when list scroll on circle mode with different status.
 * @tc.type: FUNC
 * @tc.require: AR000F3CCE
 * @tc.author: jinwuwen
 */
HWTEST_F(RenderIndexerTest, RenderIndexerTest030, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct IndexerListComponent with list mode.
     * @tc.expected: step1. properties are set correctly.
     */
    std::vector<std::string> labels = {"#", "1", "2", "3", "4"};
    RefPtr<ListComponent> list = AceType::MakeRefPtr<ListComponent>();
    list->SetDirection(FlexDirection::COLUMN);
    RefPtr<IndexerListComponent> indexerList =
        AceType::MakeRefPtr<IndexerListComponent>(list,  labels, true, false, true, true);
    RefPtr<IndexerComponent> indexer = indexerList->GetIndexer();
    ASSERT_TRUE(indexer != nullptr);
    bool circleMode = true;
    if (indexer) {
        circleMode = indexer->GetCircleMode();
    }
    ASSERT_TRUE(circleMode);

    /**
     * @tc.steps: step2. insert item with index key string, the input is not sorted.
     * @tc.expected: step2. the count of items are correctly.
     */
    // list is {"9", "123", "19", "31", "32", "A1", "A2", "B1", "B2", "D1", "H1", "H2", "Z1"}
    std::string indexKey[] = { "123", "9", "Z1", "31", "19", "32", "H1", "H2", "A1", "A2", "B1", "B2", "D1" };
    for (int32_t i = 0; i < 13; i++) {
        RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
        box->SetWidth(400.0);
        box->SetHeight(100.0);
        RefPtr<ListItemComponent> listItem = AceType::MakeRefPtr<ListItemComponent>("item", box);
        listItem->SetIndexKey(indexKey[i]);
        RefPtr<ComposedComponent> composedItem =
            AceType::MakeRefPtr<ComposedComponent>("section", "sectionComposed", listItem);
        indexerList->AppendChild(composedItem);
    }
    EXPECT_TRUE(indexerList->GetItemCountInList() == 13);

    /**
     * @tc.steps: step3. IndexList insert section head item to the list.
     * @tc.expected: step3. the index of section is correct.
     */
    RefPtr<RenderNode> renderNode = CreateRenderIndexer(indexer, true, true);
    RefPtr<MockRenderIndexerCircle> renderIndex030 = AceType::DynamicCast<MockRenderIndexerCircle>(renderNode);
    renderIndex030->PerformLayout();
    EXPECT_TRUE(renderIndex030->GetFocusItemIndex() == 0);
    EXPECT_TRUE(renderIndex030->GetIndexerStatus() == IndexerItemStatus::EXPAND);

    renderIndex030->MoveSection(1);
    renderIndex030->FinishCollapseAnimation();
    EXPECT_TRUE(renderIndex030->GetIndexerStatus() == IndexerItemStatus::COLLAPSE);

    renderIndex030->ExpandItems();
    renderIndex030->FinishCollapseAnimation();
    EXPECT_TRUE(renderIndex030->GetIndexerStatus() == IndexerItemStatus::EXPAND);

    renderIndex030->SetItemsFocused(1);
    EXPECT_TRUE(renderIndex030->GetIndexerStatus() == IndexerItemStatus::EXPAND);

    renderIndex030->MoveSection(1); // do not collapse item
    EXPECT_TRUE(renderIndex030->GetIndexerStatus() == IndexerItemStatus::EXPAND);
    renderIndex030->MoveSection(2);
    renderIndex030->FinishCollapseAnimation();
    EXPECT_TRUE(renderIndex030->GetIndexerStatus() == IndexerItemStatus::COLLAPSE);
}
} // namespace OHOS::Ace