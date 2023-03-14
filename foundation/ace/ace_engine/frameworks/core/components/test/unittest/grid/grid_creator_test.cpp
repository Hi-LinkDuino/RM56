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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/grid/grid_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/test/json/grid_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class GridCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GridCreatorTest::SetUpTestCase() {}
void GridCreatorTest::TearDownTestCase() {}
void GridCreatorTest::SetUp() {}
void GridCreatorTest::TearDown() {}

/**
 * @tc.name: GridCreator001
 * @tc.desc: Verify GridCreator can create grid component without child by row direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7I
 * @tc.author: zhongjianfei
 */
HWTEST_F(GridCreatorTest, GridCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of grid component with grid and child.
     */
    const std::string content = "{"
                                "  \"className\": \"Grid\", "
                                "  \"scrollPage\": true,    "
                                "  \"direction\": 0,        "
                                "  \"align\": 1,            "
                                "  \"columnCount\": 3,      "
                                "  \"columnExtent\": 540,   "
                                "  \"itemCount\": 20,       "
                                "  \"cachedCount\": 5,      "
                                "  \"width\": 800,          "
                                "  \"height\": 1000         "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get grid component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    ASSERT_TRUE(grid != nullptr);
    ASSERT_TRUE(grid->GetScrollPage());
    ASSERT_TRUE(grid->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(grid->GetFlexAlign() == FlexAlign::FLEX_START);
    ASSERT_TRUE(grid->GetColumnCount() == 3);
    ASSERT_TRUE(grid->GetColumnExtent() == 540);
    ASSERT_TRUE(grid->GetTotalCount() == 20);
    ASSERT_TRUE(grid->GetCachedCount() == 5);
    ASSERT_TRUE(NearEqual(grid->GetWidth(), 800.0));
    ASSERT_TRUE(NearEqual(grid->GetHeight(), 1000.0));
    ASSERT_TRUE(grid->GetGridController() != nullptr);
    ASSERT_TRUE(grid->GetGridController()->GetInitialIndex() == 0);
    std::list<RefPtr<Component>> children = grid->GetChildren();
    ASSERT_TRUE(children.empty());
}

/**
 * @tc.name: GridCreator002
 * @tc.desc: Verify GridCreator can create grid component with one child by column direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7I
 * @tc.author: zhongjianfei
 */
HWTEST_F(GridCreatorTest, GridCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of grid component with grid and child.
     */
    const std::string content = "{"
                                "  \"className\": \"Grid\",                                                "
                                "  \"child\": [                                                            "
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90}"
                                "  ],                                                                      "
                                "  \"direction\": 1,                                                       "
                                "  \"columnCount\": 2,                                                     "
                                "  \"initialItem\": 2                                                      "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get grid component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    ASSERT_TRUE(grid != nullptr);
    ASSERT_TRUE(!grid->GetScrollPage());
    ASSERT_TRUE(grid->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(grid->GetColumnCount() == 2);
    ASSERT_TRUE(grid->GetColumnExtent() == 0);
    ASSERT_TRUE(grid->GetTotalCount() == 0);
    ASSERT_TRUE(grid->GetCachedCount() == 10);
    ASSERT_TRUE(NearEqual(grid->GetWidth(), -1.0));
    ASSERT_TRUE(NearEqual(grid->GetHeight(), -1.0));
    ASSERT_TRUE(grid->GetGridController() != nullptr);
    ASSERT_TRUE(grid->GetGridController()->GetInitialIndex() == 2);
    std::list<RefPtr<Component>> children = grid->GetChildren();
    ASSERT_TRUE(children.size() == 1);
}

/**
 * @tc.name: GridCreator003
 * @tc.desc: Verify GridCreator can create grid component with five children by row reverse direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7I
 * @tc.author: zhongjianfei
 */
HWTEST_F(GridCreatorTest, GridCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of grid component with grid and child.
     */
    const std::string content = "{"
                                "  \"className\": \"Grid\",                                                 "
                                "  \"child\": [                                                             "
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90} "
                                "  ],                                                                       "
                                "  \"direction\": 2,                                                        "
                                "  \"columnCount\": 3                                                       "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get grid component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    ASSERT_TRUE(grid != nullptr);
    ASSERT_TRUE(grid->GetDirection() == FlexDirection::ROW_REVERSE);
    ASSERT_TRUE(grid->GetColumnCount() == 3);
    ASSERT_TRUE(NearEqual(grid->GetWidth(), -1.0));
    ASSERT_TRUE(NearEqual(grid->GetHeight(), -1.0));
    std::list<RefPtr<Component>> children = grid->GetChildren();
    ASSERT_TRUE(children.size() == 5);
}

/**
 * @tc.name: GridCreator004
 * @tc.desc: Verify GridCreator can create grid component with three children by column reverse direction.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7I
 * @tc.author: zhongjianfei
 */
HWTEST_F(GridCreatorTest, GridCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of grid component with grid and child.
     */
    const std::string content = "{"
                                "  \"className\": \"Grid\",                                                 "
                                "  \"child\": [                                                             "
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90} "
                                "  ],                                                                       "
                                "  \"direction\": 3,                                                        "
                                "  \"columnCount\": 4,                                                      "
                                "  \"scrollPage\": false                                                    "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get grid component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    ASSERT_TRUE(grid != nullptr);
    ASSERT_TRUE(!grid->GetScrollPage());
    ASSERT_TRUE(grid->GetDirection() == FlexDirection::COLUMN_REVERSE);
    ASSERT_TRUE(grid->GetColumnCount() == 4);
    ASSERT_TRUE(NearEqual(grid->GetWidth(), -1.0));
    ASSERT_TRUE(NearEqual(grid->GetHeight(), -1.0));
    std::list<RefPtr<Component>> children = grid->GetChildren();
    ASSERT_TRUE(children.size() == 3);
}

/**
 * @tc.name: GridCreator005
 * @tc.desc: Verify GridCreator can create grid component with three children by invalid attribute.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7I
 * @tc.author: zhongjianfei
 */
HWTEST_F(GridCreatorTest, GridCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of grid component with grid and child.
     */
    const std::string content = "{"
                                "  \"className\": \"Grid\",                                                 "
                                "  \"child\": [                                                             "
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90},"
                                "    {\"className\":\"Image\",\"src\":\"1.jpg\",\"width\":90,\"height\":90} "
                                "  ],                                                                       "
                                "  \"scrollPage\": 100,                                                     "
                                "  \"direction\": 100,                                                      "
                                "  \"align\": 5,                                                            "
                                "  \"columnCount\": -12,                                                    "
                                "  \"columnExtent\": -222,                                                  "
                                "  \"itemCount\": -333,                                                     "
                                "  \"initialItem\": -2,                                                     "
                                "  \"cachedCount\": -1000,                                                  "
                                "  \"width\": -8888,                                                        "
                                "  \"height\": -9999                                                        "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get grid component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    ASSERT_TRUE(grid != nullptr);
    ASSERT_TRUE(!grid->GetScrollPage());
    ASSERT_TRUE(grid->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(grid->GetFlexAlign() == FlexAlign::STRETCH);
    ASSERT_TRUE(grid->GetColumnCount() == 0);
    ASSERT_TRUE(grid->GetColumnExtent() == 0);
    ASSERT_TRUE(grid->GetTotalCount() == 0);
    ASSERT_TRUE(grid->GetCachedCount() == 10);
    ASSERT_TRUE(NearEqual(grid->GetWidth(), -1.0));
    ASSERT_TRUE(NearEqual(grid->GetHeight(), -1.0));
    ASSERT_TRUE(grid->GetGridController() != nullptr);
    ASSERT_TRUE(grid->GetGridController()->GetInitialIndex() == 0);
    std::list<RefPtr<Component>> children = grid->GetChildren();
    ASSERT_TRUE(children.size() == 3);
}

/**
 * @tc.name: GridCreator006
 * @tc.desc: Verify GridCreator can create grid component with no children by invalid attribute.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7I
 * @tc.author: zhongjianfei
 */
HWTEST_F(GridCreatorTest, GridCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of grid component with grid and child.
     */
    const std::string content = "{"
                                "  \"className\": \"Grid\",     "
                                "  \"direction\": 4,            "
                                "  \"columnCount\": 0,          "
                                "  \"columnExtent\": 0,         "
                                "  \"itemCount\": 0,            "
                                "  \"cachedCount\": 0,          "
                                "  \"width\": 0,                "
                                "  \"height\": 0                "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get grid component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component);
    ASSERT_TRUE(grid != nullptr);
    ASSERT_TRUE(grid->GetDirection() == FlexDirection::COLUMN);
    ASSERT_TRUE(grid->GetColumnCount() == 0);
    ASSERT_TRUE(grid->GetColumnExtent() == 0);
    ASSERT_TRUE(grid->GetTotalCount() == 0);
    ASSERT_TRUE(grid->GetCachedCount() == 10);
    ASSERT_TRUE(NearEqual(grid->GetWidth(), -1.0));
    ASSERT_TRUE(NearEqual(grid->GetHeight(), -1.0));
    std::list<RefPtr<Component>> children = grid->GetChildren();
    ASSERT_TRUE(children.size() == 0);
}

} // namespace OHOS::Ace
