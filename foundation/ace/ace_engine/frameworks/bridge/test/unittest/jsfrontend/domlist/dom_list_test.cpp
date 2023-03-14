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

#include "core/components/common/layout/constants.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_list_item_group.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string LIST_MOTION_TEST_001 = ""
                                         "{                   "
                                         "  \"tag\": \"list\" "
                                         "}";

const std::string LIST_MOTION_TEST_002 = ""
                                         "{                                             "
                                         "  \"tag\": \"list\" ,                         "
                                         "  \"attr\": [{                                "
                                         "                \"scrolleffect\" : \"fade\"   "
                                         "             }]                               "
                                         "}";

const std::string LIST_MOTION_TEST_003 = ""
                                         "{                                                 "
                                         "  \"tag\": \"list\" ,                             "
                                         "  \"attr\": [{                                    "
                                         "                \"scrolleffect\" : \"fade\"       "
                                         "             }],"
                                         "  \"style\": [{"
                                         "                \"fadeColor\" : \"#FF0000\"      "
                                         "             }]                                   "
                                         "}";

const std::string LIST_MOTION_TEST_004 = ""
                                         "{                                         "
                                         "  \"tag\": \"list\" ,                     "
                                         "  \"attr\": [{                            "
                                         "                \"scrolleffect\" : \"no\" "
                                         "             }]                           "
                                         "}";

const std::string LIST_MOTION_TEST_005 = ""
                                         "{                                         "
                                         "  \"tag\": \"list\" ,                     "
                                         "  \"attr\": [{                            "
                                         "                \"scrollbar\" : \"off\"   "
                                         "             }]                           "
                                         "}";

const std::string LIST_MOTION_TEST_006 = ""
                                         "{                                         "
                                         "  \"tag\": \"list\" ,                     "
                                         "  \"attr\": [{                            "
                                         "                \"scrollbar\" : \"auto\"  "
                                         "             }]                           "
                                         "}";

const std::string LIST_MOTION_TEST_007 = ""
                                         "{                                         "
                                         "  \"tag\": \"list\" ,                     "
                                         "  \"attr\": [{                            "
                                         "                \"scrollbar\" : \"on\"    "
                                         "             },                           "
                                         "             {                            "
                                         "                \"shapemode\" : \"round\" "
                                         "             }]                           "
                                         "}";

const std::string LIST_MOTION_TEST_008 = ""
                                         "{                                         "
                                         "  \"tag\": \"list\" ,                     "
                                         "  \"style\": [{                           "
                                         "                \"itemExtent\" : \"60px\" "
                                         "             }]                           "
                                         "}";

const std::string LIST_MOTION_TEST_009 = ""
                                         "{                                         "
                                         "  \"tag\": \"list\" ,                     "
                                         "  \"style\": [{                           "
                                         "                \"itemExtent\" : \"20%\"  "
                                         "             }]                           "
                                         "}";

const std::string ITEM_GROUP_TEST_010 = ""
                                         "{                                         "
                                         "  \"tag\": \"list-item-group\"            "
                                         "}";


const std::string LIST_UPDATE_TEST_001 = ""
                                         "{                                             "
                                         "  \"tag\": \"list\" ,                         "
                                         "  \"attr\": [{                                "
                                         "                \"updateeffect\" : \"false\"  "
                                         "             }]                               "
                                         "}";

const std::string LIST_UPDATE_TEST_002 = ""
                                         "{                                             "
                                         "  \"tag\": \"list\" ,                         "
                                         "  \"attr\": [{                                "
                                         "                \"updateeffect\" : \"true\"   "
                                         "             }]                               "
                                         "}";

} // namespace

class DomListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomListTest::SetUpTestCase() {}
void DomListTest::TearDownTestCase() {}
void DomListTest::SetUp() {}
void DomListTest::TearDown() {}

/**
 * @tc.name: DomListMotionTest001
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListMotionTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_001);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollEdgeEffect> scrollEffect = listComponent->GetScrollEffect();
    ASSERT_TRUE(scrollEffect != nullptr);
    ASSERT_TRUE(scrollEffect->IsSpringEffect());
}

/**
 * @tc.name: DomListMotionTest002
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListMotionTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_002);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollEdgeEffect> scrollEffect = listComponent->GetScrollEffect();
    RefPtr<ScrollFadeEffect> fadeEffect = AceType::DynamicCast<ScrollFadeEffect>(scrollEffect);
    ASSERT_TRUE(fadeEffect != nullptr);
    ASSERT_TRUE(fadeEffect->IsFadeEffect());
    ASSERT_TRUE(fadeEffect->GetFadeColor() == Color::GRAY);
}

/**
 * @tc.name: DomListMotionTest003
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListMotionTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_003);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollEdgeEffect> scrollEffect = listComponent->GetScrollEffect();
    RefPtr<ScrollFadeEffect> fadeEffect = AceType::DynamicCast<ScrollFadeEffect>(scrollEffect);
    ASSERT_TRUE(fadeEffect != nullptr);
    ASSERT_TRUE(fadeEffect->IsFadeEffect());
    ASSERT_TRUE(fadeEffect->GetFadeColor() == Color::RED);
}

/**
 * @tc.name: DomListMotionTest004
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListMotionTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_004);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollEdgeEffect> scrollEffect = listComponent->GetScrollEffect();
    ASSERT_TRUE(scrollEffect != nullptr);
    ASSERT_TRUE(scrollEffect->GetEdgeEffect() == EdgeEffect::NONE);
}

/**
 * @tc.name: DomListScrollBarTest001
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListScrollBarTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_005);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollBar> scrollBar = listComponent->GetScrollBar();
    ASSERT_TRUE(scrollBar != nullptr);
    ASSERT_TRUE(scrollBar->GetDisplayMode() == DisplayMode::OFF);
}


/**
 * @tc.name: DomListScrollBarTest002
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListScrollBarTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_006);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollBar> scrollBar = listComponent->GetScrollBar();
    ASSERT_TRUE(scrollBar != nullptr);
    ASSERT_TRUE(scrollBar->GetDisplayMode() == DisplayMode::AUTO);
}

/**
 * @tc.name: DomListScrollBarPolymorphismTest001
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListScrollBarPolymorphismTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_007);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    RefPtr<ScrollBar> scrollBar = listComponent->GetScrollBar();
    ASSERT_TRUE(scrollBar != nullptr);
    ASSERT_TRUE(scrollBar->GetDisplayMode() == DisplayMode::ON);
    ASSERT_TRUE(scrollBar->GetShapeMode() == ShapeMode::ROUND);
}

/**
 * @tc.name: DomListItemExtentTest001
 * @tc.desc: Test list set item extent with length.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListItemExtentTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. list node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_008);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created list node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    Dimension itemExtent = listComponent->GetItemExtent();
    ASSERT_TRUE(itemExtent.IsValid());
    ASSERT_TRUE(itemExtent.Unit() == DimensionUnit::PX);
    ASSERT_TRUE(NearEqual(itemExtent.Value(), 60.0));
}

/**
 * @tc.name: DomListItemExtentTest002
 * @tc.desc: Test list set item extent with percentage.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListItemExtentTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. list node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_009);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created list node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    Dimension itemExtent = listComponent->GetItemExtent();
    ASSERT_TRUE(itemExtent.IsValid());
    ASSERT_TRUE(itemExtent.Unit() == DimensionUnit::PERCENT);
    ASSERT_TRUE(NearEqual(itemExtent.Value(), 0.2));
}

/**
 * @tc.name: DomListScrollerTest001
 * @tc.desc: Test list node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, DomListScrollerTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_MOTION_TEST_005);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto positionController = listComponent->GetPositionController();
    ASSERT_TRUE(positionController != nullptr);
    ASSERT_TRUE(NearZero(positionController->GetCurrentPosition()));
}

/**
 * @tc.name: AceListExpansion009
 * @tc.desc: Test list item group node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, AceListExpansion009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. list item group node are created successfully.
     */
    auto itemGroup = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(ITEM_GROUP_TEST_010);
    ASSERT_TRUE(itemGroup != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(itemGroup);
    RefPtr<ListItemGroupComponent> itemGroupComponent =
        AceType::DynamicCast<ListItemGroupComponent>(boxChild->GetChild());
    ASSERT_TRUE(itemGroupComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created list node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(itemGroupComponent->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(!itemGroupComponent->GetExpand());
}

/**
 * @tc.name: AceListUpdate001
 * @tc.desc: Test list item attr updateeffect can be set false.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, AceListUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. list node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_UPDATE_TEST_001);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of update effect.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(listComponent->GetUpdateEffect() == false);
}

/**
 * @tc.name: AceListUpdate002
 * @tc.desc: Test list item attr updateeffect can be set true.
 * @tc.type: FUNC
 */
HWTEST_F(DomListTest, AceListUpdate002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create list node with it.
     * @tc.expected: step1. list node are created successfully.
     */
    auto list = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(LIST_UPDATE_TEST_002);
    ASSERT_TRUE(list != nullptr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(list);
    RefPtr<ListComponent> listComponent = AceType::DynamicCast<ListComponent>(boxChild->GetChild());
    ASSERT_TRUE(listComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of update effect.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    ASSERT_TRUE(listComponent->GetUpdateEffect() == true);
}

} // namespace OHOS::Ace::Framework
