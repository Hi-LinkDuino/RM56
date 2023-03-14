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
#include "core/components/test/json/component_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const int32_t CONTROLLER_ID = 12345;
const Edge LABEL_PADDING = Edge(100.0, 50.0, 50.0, 0.0);

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

class TabBarCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TabBarCreatorTest::SetUpTestCase() {}

void TabBarCreatorTest::TearDownTestCase() {}

void TabBarCreatorTest::SetUp() {}

void TabBarCreatorTest::TearDown() {}

/**
 * @tc.name: TabBarCreator001
 * @tc.desc: Verify BoxCreator can create TabBar with controllerId and tabs only.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with controllerId and tabs.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12345,
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. tabs and controller are set correctly and other properties set default.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
    EXPECT_EQ(tabBar->GetChildren().size(), std::size_t(1));
    EXPECT_EQ(tabBar->GetController()->GetId(), CONTROLLER_ID);
    EXPECT_EQ(tabBar->GetIndex(), 0);
    EXPECT_EQ(tabBar->GetMode(), TabBarMode::FIXED);
    EXPECT_EQ(tabBar->GetIndicatorSize(), TabBarIndicatorType::LABEL);
    EXPECT_EQ(tabBar->GetLabelPadding(), Edge());
}

/**
 * @tc.name: TabBarCreator002
 * @tc.desc: Verify BoxCreator can not create TabBar with tabs only.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with tabs only.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is null
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(!tabBar);
}

/**
 * @tc.name: TabBarCreator003
 * @tc.desc: Verify BoxCreator can not create TabBar with invalid type of controllerId.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with invalid type of controllerId.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": "0",
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is null
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(!tabBar);
}

/**
 * @tc.name: TabBarCreator004
 * @tc.desc: Verify BoxCreator can not create TabBar without tabs.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component without tabs.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 0
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is null
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(!tabBar);
}

/**
 * @tc.name: TabBarCreator005
 * @tc.desc: Verify BoxCreator can not create TabBar with empty tabs.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with empty tabs.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 0,
            "tabs": []
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is null
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(!tabBar);
}

/**
 * @tc.name: TabBarCreator006
 * @tc.desc: Verify BoxCreator can not create TabBar with invalid value of index.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V AR000DAR30
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with invalid value of index.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12345,
            "index": -1,
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component create success and index is default 0.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar != nullptr);
    EXPECT_TRUE(tabBar->GetIndex() == 0);
}

/**
 * @tc.name: TabBarCreator007
 * @tc.desc: Verify BoxCreator can not create TabBar with invalid value of index.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V AR000DAR30
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with invalid value of index.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12345,
            "index": 1,
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component create success and index is default 0.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar != nullptr);
    EXPECT_TRUE(tabBar->GetIndex() == 0);
}

/**
 * @tc.name: TabBarCreator008
 * @tc.desc: Verify the default value of labelColor and unselectedLabelColor.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with labelColor.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 1,
            "labelColor": {
                "className": "Color",
                "alpha": 255,
                "red": 0,
                "green": 0,
                "blue": 255
            },
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. unselectedLabelColor's alpha is 70% of labelColor.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
}

/**
 * @tc.name: TabBarCreator009
 * @tc.desc: Verify the default value of labelStyle and unselectedLabelStyle.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with labelStyle.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 1,
            "labelStyle": {
                "className": "TextStyle",
                "fontSize": 123
            },
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. unselectedLabelStyle is same as labelStyle.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
}

/**
 * @tc.name: TabBarCreator010
 * @tc.desc: Verify TabBarCreator can create TabBar component with all properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with all properties.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12345,
            "index": 1,
            "indicatorSize": "label",
            "mode": "scrollable",
            "labelPadding": {
                "className": "Edge",
                "left": 100.0,
                "top": 50.0,
                "right": 50.0
            },
            "labelStyle": {
                "className": "TextStyle",
                "fontSize": 123
            },
            "unselectedLabelStyle": {
                "className": "TextStyle",
                "fontSize": 100
            },
            "labelColor": {
                "className": "Color",
                "alpha": 255,
                "red": 0,
                "green": 0,
                "blue": 255
            },
            "unselectedLabelColor": {
                "className": "Color",
                "alpha": 255,
                "red": 0,
                "green": 255,
                "blue": 0
            },
            "tabs": [
                {
                    "className": "Tab",
                    "text": "Tab1"
                },
                {
                    "className": "Tab",
                    "text": "Tab2"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. all properties are set correctly.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);

    EXPECT_TRUE(tabBar);
    EXPECT_EQ(tabBar->GetChildren().size(), std::size_t(2));
    EXPECT_EQ(tabBar->GetController()->GetId(), CONTROLLER_ID);
    EXPECT_EQ(tabBar->GetIndex(), 1);
    EXPECT_EQ(tabBar->GetMode(), TabBarMode::SCROLLABEL);
    EXPECT_EQ(tabBar->GetIndicatorSize(), TabBarIndicatorType::LABEL);
    EXPECT_EQ(tabBar->GetLabelPadding(), LABEL_PADDING);
}

/**
 * @tc.name: TabBarCreator011
 * @tc.desc: Verify TabBarCreator can not create TabBarItem component with className only.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with className only.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12345,
            "tabs": [
                {
                    "className": "Tab"
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component create success.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar != nullptr);
}

/**
 * @tc.name: TabBarCreator012
 * @tc.desc: Verify TabBarCreator can create TabBarItem component with different property set.
 * @tc.type: FUNC
 * @tc.require: AR000DAR2V
 * @tc.author: jiangtao
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component with different property set.
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12345,
            "tabs": [
                {
                    "className": "Tab"
                },
                {
                    "className": "Tab",
                    "text": "Tab1"
                },
                {
                    "className": "Tab",
                    "icon": {
                        "className": "Image",
                        "src": "pictures/480x480/001.jpg",
                        "width": 100,
                        "height": 100
                    }
                },
                {
                    "className": "Tab",
                    "text": "Tab3",
                    "icon": {
                        "className": "Image",
                        "src": "pictures/480x480/001.jpg",
                        "width": 100,
                        "height": 100
                    }
                },
                {
                    "className": "Tab",
                    "child": {
                        "className": "Text",
                        "value": "Tab4",
                        "textStyle": {
                            "className": "TextStyle",
                            "fontSize": 50
                        }
                    }
                },
                {
                    "className": "Tab",
                    "text": "Tab5",
                    "icon": {
                        "className": "Image",
                        "src": "pictures/480x480/001.jpg",
                        "width": 100,
                        "height": 100
                    },
                    "child": {
                        "className": "Text",
                        "value": "Tab5",
                        "textStyle": {
                            "className": "TextStyle",
                            "fontSize": 50
                        }
                    }
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar's size is 5.
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
    const std::list<RefPtr<Component>>& tabs = tabBar->GetChildren();
    EXPECT_EQ(tabs.size(), size_t(5));
}

/**
 * @tc.name: TabBarCreator013
 * @tc.desc: Verify vertical of TabBarCreator.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V2
 * @tc.author: liujinwei
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component .
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12346,
            "tabs": [
                {
                    "className": "Tab",
                    "child": {
                        "className": "Text",
                        "value": "Tab4",
                        "textStyle": {
                            "className": "TextStyle",
                            "fontSize": 50
                        }
                    }
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is right
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
    EXPECT_EQ(tabBar->IsVertical(), false);
}

/**
 * @tc.name: TabBarCreator014
 * @tc.desc: Verify vertical of TabBarCreator.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V2
 * @tc.author: liujinwei
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component .
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12347,
            "vertical":false,
            "tabs": [
                {
                    "className": "Tab",
                    "child": {
                        "className": "Text",
                        "value": "Tab4",
                        "textStyle": {
                            "className": "TextStyle",
                            "fontSize": 50
                        }
                    }
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is right
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
    EXPECT_EQ(tabBar->IsVertical(), false);
}

/**
 * @tc.name: TabBarCreator015
 * @tc.desc: Verify vertical of TabBarCreator.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1V2
 * @tc.author: liujinwei
 */
HWTEST_F(TabBarCreatorTest, TabBarCreator015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of TabBar component .
     */
    std::string dslJson = R"(
        {
            "className": "Tabbar",
            "controllerId": 12348,
            "vertical":true,
            "tabs": [
                {
                    "className": "Tab",
                    "child": {
                        "className": "Text",
                        "value": "Tab4",
                        "textStyle": {
                            "className": "TextStyle",
                            "fontSize": 50
                        }
                    }
                }
            ]
        })";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get TabBar component.
     * @tc.expected: step2. TabBar component is right
     */
    RefPtr<TabBarComponent> tabBar = CreateTabBarComponent(dslJson);
    EXPECT_TRUE(tabBar);
    EXPECT_EQ(tabBar->IsVertical(), true);
}
} // namespace OHOS::Ace