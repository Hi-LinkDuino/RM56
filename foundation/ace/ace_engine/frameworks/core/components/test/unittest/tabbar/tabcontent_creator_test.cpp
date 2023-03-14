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

#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_content_component.h"
#include "core/components/test/json/tab_bar_creator.h"
#include "core/components/test/json/tab_content_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

RefPtr<TabContentComponent> CreateTabContentComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<TabContentComponent>(component);
}

void CheckTabContentDefaultValue(const RefPtr<TabContentComponent>& tabContent)
{
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->GetController()->GetIndex(), 0);
}

RefPtr<TabBarComponent> CreateTabBarComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<TabBarComponent>(component);
}

} // namespace

class TabContentCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TabContentCreatorTest::SetUpTestCase() {}

void TabContentCreatorTest::TearDownTestCase() {}

void TabContentCreatorTest::SetUp() {}

void TabContentCreatorTest::TearDown() {}

/**
 * @tc.name: TabContentCreator001
 * @tc.desc: Test parse TabContent's field successfully from no field dsl json of tabcontent component.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with no fields, then create tabcontent component with the json.
     * @tc.expected: step1. TabContent component successfully created, and default fields.
     */
    std::string dslJson = "{ \"className\": \"Tabcontent\" }";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(dslJson);
    CheckTabContentDefaultValue(tabContent);
}

/**
 * @tc.name: TabContentCreator002
 * @tc.desc: Test tabContent's field successfully from fields.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32 AR000DAR33
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tab json string, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and correct fields.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 1,"
                              "    \"scrollable\": false"
                              "}";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->IsScrollable(), false);
    EXPECT_EQ(tabContent->GetController()->GetIndex(), 0);
}

/**
 * @tc.name: TabContentCreator003
 * @tc.desc: Test parse TabContent's field successfully from invalid index of tabbar component.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string with invalid fields, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0,"
                              "    \"scrollable\": true"
                              "}";

    std::string barJson = "{"
                          "    \"className\": \"Tabbar\","
                          "    \"controllerId\": 0,"
                          "    \"index\": -1,"
                          "    \"tabs\": ["
                          "        {"
                          "            \"className\": \"Tab\","
                          "            \"text\": \"Tab1\""
                          "        },"
                          "        {"
                          "            \"className\": \"Tab\","
                          "            \"text\": \"Tab1\""
                          "        }"
                          "    ]"
                          "}";

    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(barJson);
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->GetController()->GetIndex(), 0);

    /**
     * @tc.steps: step2. set the index = 1, check the index of controller
     * @tc.expected: step2. the new index = 1
     */
    tabBar->SetIndex(1);

    /**
     * @tc.steps: step3. set the index = 5, 5 is more than the size, check the index of controller
     * @tc.expected: step3. the new index = 0
     */
    tabBar->SetIndex(5);
    EXPECT_EQ(tabContent->GetController()->GetIndex(), 0);
}

/**
 * @tc.name: TabContentCreator004
 * @tc.desc: Test tabContent's field successfully from tabbar component without index.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string without index, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0,"
                              "    \"scrollable\": true"
                              "}";

    std::string barJson = "{"
                          "    \"className\": \"Tabbar\","
                          "    \"controllerId\": 0,"
                          "    \"tabs\": ["
                          "        {"
                          "            \"className\": \"Tab\","
                          "            \"text\": \"Tab1\""
                          "        },"
                          "        {"
                          "            \"className\": \"Tab\","
                          "            \"text\": \"Tab1\""
                          "        }"
                          "    ]"
                          "}";

    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(barJson);
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->GetController()->GetIndex(), 0);
}

/**
 * @tc.name: TabContentCreator005
 * @tc.desc: Test tabContent's field successfully from tabbar component with index.
 * @tc.type: FUNC
 * @tc.require: AR000DAR32
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string with index, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has correct value.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0,"
                              "    \"scrollable\": true"
                              "}";

    std::string barJson = "{"
                          "    \"className\": \"Tabbar\","
                          "    \"controllerId\": 0,"
                          "    \"index\": 1,"
                          "    \"tabs\": ["
                          "        {"
                          "            \"className\": \"Tab\","
                          "            \"text\": \"Tab1\""
                          "        },"
                          "        {"
                          "            \"className\": \"Tab\","
                          "            \"text\": \"Tab1\""
                          "        }"
                          "    ]"
                          "}";

    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(barJson);
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
}

/**
 * @tc.name: TabContentCreator006
 * @tc.desc: Test tabContent's field successfully from tabbar component without scrollable.
 * @tc.type: FUNC
 * @tc.require: AR000DAR33
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string without index, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0"
                              "}";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->IsScrollable(), true);
}

/**
 * @tc.name: TabContentCreator007
 * @tc.desc: Test tabContent's field successfully from tabbar component with scrollable.
 * @tc.type: FUNC
 * @tc.require: AR000DAR33
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string with index, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0,"
                              "    \"scrollable\": false"
                              "}";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->IsScrollable(), false);
}

/**
 * @tc.name: TabContentCreator008
 * @tc.desc: Test tabContent's field successfully.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V3
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tab json string without vertical, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0"
                              "}";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->IsVertical(), false);
}

/**
 * @tc.name: TabContentCreator009
 * @tc.desc: Test tabContent's field successfully.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V3
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string with index, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0,"
                              "    \"vertical\": true"
                              "}";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->IsVertical(), true);
}

/**
 * @tc.name: TabContentCreator010
 * @tc.desc: Test tabContent's field successfully.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V3
 * @tc.author: liujinwei
 */
HWTEST_F(TabContentCreatorTest, TabContentCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the tabBar json string with index, then create tabContent component.
     * @tc.expected: step1. TabContent component successfully created, and the fields has default values.
     */
    std::string contentJson = "{"
                              "    \"child\": ["
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"0000Good news!\""
                              "        },"
                              "        {"
                              "            \"className\": \"Text\","
                              "            \"value\": \"1111Good news!\""
                              "        }"
                              "    ],"
                              "    \"className\": \"Tabcontent\","
                              "    \"controllerId\": 0,"
                              "    \"vertical\": false"
                              "}";
    RefPtr<TabContentComponent> tabContent = CreateTabContentComponent(contentJson);
    EXPECT_TRUE(tabContent);
    EXPECT_EQ(tabContent->IsVertical(), false);
}

} // namespace OHOS::Ace
