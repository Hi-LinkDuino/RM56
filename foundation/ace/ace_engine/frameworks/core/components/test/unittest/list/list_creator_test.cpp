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
#include "core/components/box/box_component.h"
#include "core/components/list/list_component.h"
#include "core/components/test/json/component_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class ListCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ListCreatorTest::SetUpTestCase() {}
void ListCreatorTest::TearDownTestCase() {}
void ListCreatorTest::SetUp() {}
void ListCreatorTest::TearDown() {}

/**
 * @tc.name: ListCreator001
 * @tc.desc: Verify TransformCreator can create list component without child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0A
 * @tc.author: zhongjianfei
 */
HWTEST_F(ListCreatorTest, ListCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of list component without child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"List\",       "
                                "  \"direction\": 0,              "
                                "  \"count\": 15,                 "
                                "  \"cachedCount\": 8,            "
                                "  \"listPosition\": 100,         "
                                "  \"scrollPage\": true           "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get list component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ListComponent> list = AceType::DynamicCast<ListComponent>(component);
    ASSERT_TRUE(list != nullptr);
    ASSERT_TRUE(list->GetDirection() == FlexDirection::ROW);
    ASSERT_TRUE(list->GetTotalCount() == 15);
    ASSERT_TRUE(list->GetCachedCount() == 8);
    ASSERT_TRUE(list->GetScrollPage());
    ASSERT_TRUE(NearEqual(list->GetPositionController()->GetInitialOffset(), 100.0));
}

/**
 * @tc.name: ListCreatorTest001
 * @tc.desc: Verify list component can open scrollpage.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1SR
 * @tc.author: chenlien
 */
HWTEST_F(ListCreatorTest, ListCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of list component with scrollpage property.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"List\",       "
                                "  \"direction\": 0,              "
                                "  \"count\": 15,                 "
                                "  \"cachedCount\": 8,            "
                                "  \"listPosition\": 100,         "
                                "  \"scrollPage\": true           "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get list component.
     * @tc.expected: step2. properties are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ListComponent> list = AceType::DynamicCast<ListComponent>(component);
    ASSERT_TRUE(list != nullptr);
    ASSERT_TRUE(list->GetScrollPage());
}

/**
 * @tc.name: ListCreatorTest002
 * @tc.desc: Verify list component can close scrollpage.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0J
 * @tc.author: chenlien
 */
HWTEST_F(ListCreatorTest, ListCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of list component with scrollpage property.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"List\",       "
                                "  \"direction\": 0,              "
                                "  \"count\": 15,                 "
                                "  \"cachedCount\": 8,            "
                                "  \"listPosition\": 100,         "
                                "  \"scrollPage\": false          "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get list component.
     * @tc.expected: step2. properties are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ListComponent> list = AceType::DynamicCast<ListComponent>(component);
    ASSERT_TRUE(list != nullptr);
    ASSERT_TRUE(!list->GetScrollPage());
}

/**
 * @tc.name: ListCreatorScrollBarTest001
 * @tc.desc: Verify list component can close scrollbar.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1UI
 * @tc.author: chenlien
 */
HWTEST_F(ListCreatorTest, ListCreatorScrollBarTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of list component with scrollBar property.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"List\",       "
                                "  \"direction\": 0,              "
                                "  \"count\": 15,                 "
                                "  \"cachedCount\": 8,            "
                                "  \"listPosition\": 100,         "
                                "  \"scrollbar\": 1,              "
                                "  \"shapemode\": 1               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get list component.
     * @tc.expected: step2. properties are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ListComponent> list = AceType::DynamicCast<ListComponent>(component);
    ASSERT_TRUE(list != nullptr);
    ASSERT_TRUE(list->GetScrollBar() != nullptr);
    auto scrollBar = list->GetScrollBar();
    ASSERT_TRUE(scrollBar->GetDisplayMode() == DisplayMode::AUTO);
    ASSERT_TRUE(scrollBar->GetShapeMode() == ShapeMode::RECT);
}

} // namespace OHOS::Ace