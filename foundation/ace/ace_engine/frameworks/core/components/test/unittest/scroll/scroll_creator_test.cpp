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
#include "core/components/scroll/scroll_component.h"
#include "core/components/test/json/component_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string TEXT_VALUE = "Hi Ace";
constexpr double SCROLL_INIT_POSITION = 50.0;
const Edge SCROLL_TEST_PADDING = Edge(15.0);

} // namespace

class ScrollCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScrollCreatorTest::SetUpTestCase() {}
void ScrollCreatorTest::TearDownTestCase() {}
void ScrollCreatorTest::SetUp() {}
void ScrollCreatorTest::TearDown() {}

/**
 * @tc.name: ScrollCreator001
 * @tc.desc: Verify TransformCreator can create scroll component without child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR08
 * @tc.author: jinwuwen
 */
HWTEST_F(ScrollCreatorTest, ScrollCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of scroll component without child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Scroll\",     "
                                "  \"direction\": 0,              "
                                "  \"scrollPosition\": 50,        "
                                "  \"scrollPadding\": {           "
                                "    \"className\": \"Edge\",     "
                                "    \"value\": 15                "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get scroll component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(component);
    ASSERT_TRUE(scroll != nullptr);
    ASSERT_TRUE(NearEqual(scroll->GetScrollPositionController()->GetInitialOffset(), SCROLL_INIT_POSITION));
    ASSERT_TRUE(scroll->GetAxisDirection() == Axis::VERTICAL);
    ASSERT_TRUE(scroll->GetPadding() == SCROLL_TEST_PADDING);
}

/**
 * @tc.name: ScrollCreator002
 * @tc.desc: Verify TransformCreator can create scroll component with a box child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR08
 * @tc.author: jinwuwen
 */
HWTEST_F(ScrollCreatorTest, ScrollCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of scroll component with a box child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Scroll\",     "
                                "  \"direction\": 0,              "
                                "  \"scrollPosition\": 50,        "
                                "  \"scrollPadding\": {           "
                                "    \"className\": \"Edge\",     "
                                "    \"value\": 15                "
                                "  },                             "
                                "  \"child\":{                    "
                                "    \"className\": \"Box\"       "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get scroll component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(component);
    ASSERT_TRUE(scroll != nullptr);
    RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(scroll->GetChild());
    ASSERT_TRUE(box != nullptr);
    ASSERT_TRUE(NearEqual(scroll->GetScrollPositionController()->GetInitialOffset(), SCROLL_INIT_POSITION));
    ASSERT_TRUE(scroll->GetAxisDirection() == Axis::VERTICAL);
    ASSERT_TRUE(scroll->GetPadding() == SCROLL_TEST_PADDING);
}

/**
 * @tc.name: ScrollCreator003
 * @tc.desc: Verify TransformCreator can create scroll component with a box child when direction is missing.
 * @tc.type: FUNC
 * @tc.require: AR000DAR08
 * @tc.author: jinwuwen
 */
HWTEST_F(ScrollCreatorTest, ScrollCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of scroll component with a box child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Scroll\",     "
                                "  \"scrollPosition\": 50,        "
                                "  \"child\":{                    "
                                "    \"className\": \"Box\"       "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get scroll component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ScrollComponent> scroll = AceType::DynamicCast<ScrollComponent>(component);
    ASSERT_TRUE(scroll->GetAxisDirection() == Axis::VERTICAL);
}

} // namespace OHOS::Ace