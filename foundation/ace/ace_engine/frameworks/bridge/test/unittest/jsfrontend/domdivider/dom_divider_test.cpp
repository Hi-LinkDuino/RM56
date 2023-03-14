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

#include "core/components/divider/divider_component.h"
#include "core/components/divider/divider_theme.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const Dimension DEFAULT_STROKE_WIDTH = Dimension(1.0);
const Dimension STROKE_WIDTH = Dimension(1.0);
const Color DIVIDER_COLOR = Color(0xffff0000);

const std::string DIVIDER_TEST_001 = ""
                                     "{                     "
                                     " \"tag\": \"divider\" "
                                     "}";
const std::string DIVIDER_TEST_002 = ""
                                     "{                                                "
                                     " \"tag\": \"divider\",                           "
                                     " \"attr\": [{                                    "
                                     "                  \"vertical\": \"true\"         "
                                     "              }],                                "
                                     " \"style\": [{                                   "
                                     "                  \"strokeWidth\": \"1px\"       "
                                     "              },                                 "
                                     "             {                                   "
                                     "                  \"lineCap\": \"round\"         "
                                     "              },                                 "
                                     "             {                                   "
                                     "                  \"color\": \"#ff0000\"         "
                                     "              }]                                 "
                                     "}";

} // namespace

class DomDividerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomDividerTest::SetUpTestCase() {}
void DomDividerTest::TearDownTestCase() {}
void DomDividerTest::SetUp() {}
void DomDividerTest::TearDown() {}

/**
 * @tc.name: DomDividerCreatorTest001
 * @tc.desc: Test divider node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomDividerTest, DomDividerCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create divider node with it.
     * @tc.expected: step1. divider node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(DIVIDER_TEST_001);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    auto divider = AceType::DynamicCast<DividerComponent>(boxChild->GetChild());
    ASSERT_TRUE(divider);

    /**
     * @tc.steps: step2. Check styles and attributes of created divider node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto theme = themeManager->GetTheme<DividerTheme>();
    ASSERT_TRUE(theme);
    EXPECT_EQ(divider->GetStrokeWidth(), DEFAULT_STROKE_WIDTH);
    ASSERT_FALSE(divider->IsVertical());
    EXPECT_EQ(divider->GetDividerColor(), theme->GetColor());
    EXPECT_EQ(divider->GetLineCap(), LineCap::BUTT);
}

/**
 * @tc.name: DomDividerCreatorTest002
 * @tc.desc: Test divider node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomDividerTest, DomDividerCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create divider node with it.
     * @tc.expected: step1. divider node are created successfully.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(DIVIDER_TEST_002);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    auto divider = AceType::DynamicCast<DividerComponent>(boxChild->GetChild());
    ASSERT_TRUE(divider);

    /**
     * @tc.steps: step2. Check styles and attributes of created divider node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(divider->GetStrokeWidth(), STROKE_WIDTH);
    ASSERT_TRUE(divider->IsVertical());
    EXPECT_EQ(divider->GetDividerColor(), DIVIDER_COLOR);
    EXPECT_EQ(divider->GetLineCap(), LineCap::ROUND);
}

} // namespace OHOS::Ace::Framework
