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
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_tab_bar.h"
#include "frameworks/bridge/common/dom/dom_tab_content.h"
#include "frameworks/bridge/common/dom/dom_tabs.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string TABS_TEST_001 = ""
                                  "{                                                          "
                                  "  \"tag\": \"tabs\"                                        "
                                  "}";

const std::string TABS_TEST_002 = ""
                                  "{                                                          "
                                  "  \"tag\": \"tabs\",                                       "
                                  "  \"attr\": [{                                             "
                                  "                \"index\" : \"2\"                          "
                                  "             }]                                            "
                                  "}";

const std::string TABS_TEST_003 = ""
                                  "{                                                          "
                                  "  \"tag\": \"tabs\",                                       "
                                  "  \"attr\": [{                                             "
                                  "                \"vertical\" : \"true\"                    "
                                  "             }]                                            "
                                  "}";

const std::string TABS_TEST_004 = ""
                                  "{                                                          "
                                  "  \"tag\": \"tabs\",                                       "
                                  "  \"attr\": [{                                             "
                                  "                \"vertical\" : \"false\"                   "
                                  "             }]                                            "
                                  "}";

const std::string TABS_TEST_005 = ""
                                  "{                                                          "
                                  "  \"tag\": \"tabs\",                                       "
                                  "  \"attr\": [{                                             "
                                  "                \"index\" : \"2\"                          "
                                  "             }]                                            "
                                  "}";

const std::string TABBAR_TEST_001 = ""
                                    "{                                                        "
                                    "  \"tag\": \"tab-bar\"                                   "
                                    "}";

const std::string TABBAR_TEST_002 = ""
                                    "{                                                        "
                                    "  \"tag\": \"tab-bar\" ,                                 "
                                    "  \"attr\": [{                                           "
                                    "                \"mode\" : \"fixed\"                     "
                                    "             }]                                          "
                                    "}";

const std::string TABCONTENT_TEST_001 = ""
                                        "{                                                    "
                                        "  \"tag\": \"tab-content\"                           "
                                        "}";

const std::string TABCONTENT_TEST_002 = ""
                                        "{                                                    "
                                        "  \"tag\": \"tab-content\" ,                         "
                                        "  \"attr\": [{                                       "
                                        "                \"scrollable\" : \"false\"           "
                                        "             }]                                      "
                                        "}";

const std::string TABCONTENT_TEST_003 = "                                                     "
                                        "{                                                    "
                                        "  \"tag\": \"tab-content\" ,                         "
                                        "    \"commonStyle\": [{                              "
                                        "        \"height\": \"1000px\"                       "
                                        "    }],                                              "
                                        "  \"attr\": [{                                       "
                                        "                \"scrollable\" : \"false\"           "
                                        "             }],                                     "
                                        "   \"child\":                                        "
                                        "{                                                    "
                                        "    \"tag\":\"div\",                                 "
                                        "    \"attr\": {},                                    "
                                        "    \"commonStyle\": [{                              "
                                        "        \"height\": \"2000px\"                       "
                                        "    }]                                               "
                                        "}                                                    "
                                        "}";

} // namespace

class DomTabTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomTabTest::SetUpTestCase() {}
void DomTabTest::TearDownTestCase() {}
void DomTabTest::SetUp() {}
void DomTabTest::TearDown() {}

/**
 * @tc.name: DomTabsCreatorTest001
 * @tc.desc: Test tabs node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabsCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tabs node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto tabs = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABS_TEST_001);
    ASSERT_TRUE(tabs != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->GetTabIndex(), 0U);
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->GetTabControllerId(), 1U);
}

/**
 * @tc.name: DomTabsCreatorTest002
 * @tc.desc: Test tabs node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabsCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tabs node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto tabs = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABS_TEST_002);
    ASSERT_TRUE(tabs != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->GetTabIndex(), 2U);
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->GetTabControllerId(), 2U);
}

/**
 * @tc.name: DomTabsCreatorTest003
 * @tc.desc: Test tabs node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabsCreatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tabs node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto tabs = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABS_TEST_003);
    ASSERT_TRUE(tabs != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->IsVertical(), true);
}

/**
 * @tc.name: DomTabsCreatorTest004
 * @tc.desc: Test tabs node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabsCreatorTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tabs node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto tabs = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABS_TEST_004);
    ASSERT_TRUE(tabs != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->IsVertical(), false);
}

/**
 * @tc.name: DomTabsCreatorTest005
 * @tc.desc: Test tabs node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabsCreatorTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tabs node with it.
     * @tc.expected: step1. tabs node are created successfully.
     */
    auto tabs = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABS_TEST_005);
    ASSERT_TRUE(tabs != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tabs node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(AceType::DynamicCast<DOMTabs>(tabs)->IsVertical(), false);
}

/**
 * @tc.name: DomTabBarCreatorTest001
 * @tc.desc: Test tab-bar node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabBarCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tab-bar node with it.
     * @tc.expected: step1. tab-bar node and its component are created successfully.
     */
    auto tabBarNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABBAR_TEST_001);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(tabBarNode);
    RefPtr<TabBarComponent> tabBarComponent = AceType::DynamicCast<TabBarComponent>(boxChild->GetChild());

    ASSERT_TRUE(tabBarComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tab-bar node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(tabBarComponent->GetMode(), TabBarMode::SCROLLABEL);
}

/**
 * @tc.name: DomTabBarCreatorTest002
 * @tc.desc: Test tab-bar node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabBarCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tab-bar node with it.
     * @tc.expected: step1. tab-bar node and its component are created successfully.
     */
    auto tabBarNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABBAR_TEST_002);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(tabBarNode);
    RefPtr<TabBarComponent> tabBarComponent = AceType::DynamicCast<TabBarComponent>(boxChild->GetChild());

    ASSERT_TRUE(tabBarComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tab-bar node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(tabBarComponent->GetMode(), TabBarMode::FIXED);
}

/**
 * @tc.name: DomTabContentCreatorTest001
 * @tc.desc: Test tab-content node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabContentCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tab-content node with it.
     * @tc.expected: step1. tab-content node and its component are created successfully.
     */
    auto tabContentNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABCONTENT_TEST_001);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(tabContentNode);
    RefPtr<TabContentComponent> tabContentComponent = AceType::DynamicCast<TabContentComponent>(boxChild->GetChild());

    ASSERT_TRUE(tabContentComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tab-content node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(tabContentComponent->IsScrollable(), true);
}

/**
 * @tc.name: DomTabContentCreatorTest002
 * @tc.desc: Test tab-content node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabContentCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tab-content node with it.
     * @tc.expected: step1. tab-content node and its component are created successfully.
     */
    auto tabContentNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABCONTENT_TEST_002);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(tabContentNode);
    RefPtr<TabContentComponent> tabContentComponent = AceType::DynamicCast<TabContentComponent>(boxChild->GetChild());

    ASSERT_TRUE(tabContentComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created tab-content node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(tabContentComponent->IsScrollable(), false);
}

/**
 * @tc.name: DomTabContentCreatorTest003
 * @tc.desc: Test tab-content node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomTabTest, DomTabContentCreatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create tab-content node with it.
     * @tc.expected: step1. tab-content node and its component are created successfully.
     */
    auto tabContentNode = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(TABCONTENT_TEST_003);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(tabContentNode);
    RefPtr<TabContentComponent> tabContentComponent = AceType::DynamicCast<TabContentComponent>(boxChild->GetChild());

    ASSERT_TRUE(tabContentComponent != nullptr);
}

} // namespace OHOS::Ace::Framework
