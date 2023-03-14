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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t SINGLE_CHILD_COUNT = 2;
constexpr int32_t TEST_CROSS_ALIGN = 2;
constexpr int32_t TEST_DIRECTION_FLEX = 1;
constexpr int32_t TEST_MAIN_ALIGN = 3;

} // namespace

class DomDivTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomDivTest::SetUpTestCase() {}
void DomDivTest::TearDownTestCase() {}
void DomDivTest::SetUp() {}
void DomDivTest::TearDown() {}

/**
 * @tc.name: DomDivTest001
 * @tc.desc: Verify that DomDiv can be created and appended children.
 * @tc.type: FUNC
 */
HWTEST_F(DomDivTest, DomDivTest001, TestSize.Level1)
{
    const std::string jsonDivStr = ""
                                   "{                                 "
                                   "  \"tag\": \"div\",               "
                                   "  \"child\": [{                   "
                                   "           \"tag\":\"text\"       "
                                   "          },"
                                   "          {"
                                   "           \"tag\":\"text\" "
                                   "          }]                      "
                                   "}";

    /**
     * @tc.steps: step1. call JsonUtil interface and create DomDiv and its children.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonDivStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<FlexComponent> flexChild = AceType::DynamicCast<FlexComponent>(boxChild->GetChild());

    /**
     * @tc.steps: step2. Verify whether the DomDiv and its children are created.
     * @tc.expected: step2. DomDiv is not null and children size is 2.
     */
    ASSERT_TRUE(domNodeRoot != nullptr);
    ASSERT_TRUE(flexChild->GetChildren().size() == SINGLE_CHILD_COUNT);
}

/**
 * @tc.name: DomDivTest002
 * @tc.desc: Verify that DomDiv can be set styles.
 * @tc.type: FUNC
 */
HWTEST_F(DomDivTest, DomDivTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomDiv with three attributes:
     * alignItems, flexDirection, justify-content.
     */
    const std::string jsonDivStr = ""
                                   "{                                           "
                                   "  \"tag\": \"div\",                         "
                                   "  \"style\": [{                             "
                                   "           \"alignItems\":\"center\"        "
                                   "          },"
                                   "          {"
                                   "           \"flexDirection\":\"column\"     "
                                   "          },                                "
                                   "          {"
                                   "           \"justifyContent\":\"flex-end\" "
                                   "           }]"
                                   "}";

    /**
     * @tc.steps: step1. call JsonUtil interface, create DomDiv and set its style.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonDivStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<FlexComponent> flexChild = AceType::DynamicCast<FlexComponent>(boxChild->GetChild());
    int32_t crossAxisAlign = static_cast<int32_t>(flexChild->GetCrossAxisAlign());
    int32_t flexDirectionValue = static_cast<int32_t>(flexChild->GetDirection());
    int32_t mainAxisAlign = static_cast<int32_t>(flexChild->GetMainAxisAlign());

    /**
     * @tc.steps: step2. Verify the DomDiv's style is set correctly.
     * @tc.expected: step2. crossAxisAlign is 2,flexDirectionValue is 1, mainAxisAlign is 3.
     */
    EXPECT_TRUE(crossAxisAlign == TEST_CROSS_ALIGN);
    EXPECT_TRUE(flexDirectionValue == TEST_DIRECTION_FLEX);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
}

/**
 * @tc.name: DomDivTest003
 * @tc.desc: Verify that DomDiv can be set styles as grid.
 * @tc.type: FUNC
 */
HWTEST_F(DomDivTest, DomDivTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of DomDiv with three attributes:
     * columns, rows, gap.
     */
    const std::string jsonDivStr = ""
                                   "{                                                           "
                                   "  \"tag\": \"div\",                                         "
                                   "  \"style\": [{                                             "
                                   "           \"display\":\"grid\"                             "
                                   "          },                                                "
                                   "          {                                                 "
                                   "           \"gridTemplateColumns\":\"100px 200px 300px\"    "
                                   "          },                                                "
                                   "          {                                                 "
                                   "           \"gridTemplateRows\":\"20% 30% 30% 20%\"         "
                                   "          },                                                "
                                   "          {                                                 "
                                   "           \"gridColumnsGap\":\"10px\"                      "
                                   "          },                                                "
                                   "          {                                                 "
                                   "           \"gridRowsGap\":\"20px\"                         "
                                   "          },                                                "
                                   "          {                                                 "
                                   "           \"flexDirection\":\"row\"                        "
                                   "          },                                                "
                                   "          {                                                 "
                                   "           \"justifyContent\":\"flex-end\"                  "
                                   "           }]                                               "
                                   "}";


    /**
     * @tc.steps: step1. call JsonUtil interface, create DomDiv and set its style.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonDivStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<FlexComponent> flexChild = AceType::DynamicCast<FlexComponent>(boxChild->GetChild());
    RefPtr<GridLayoutComponent> grid = AceType::DynamicCast<GridLayoutComponent>(flexChild->GetChildren().front());
    EXPECT_TRUE(grid != nullptr);
    int32_t flexDirectionValue = static_cast<int32_t>(flexChild->GetDirection());
    int32_t mainAxisAlign = static_cast<int32_t>(flexChild->GetMainAxisAlign());

    /**
     * @tc.steps: step2. Verify the DomDiv's style is set correctly.
     * @tc.expected: step2. columns, rows, gap is correctly.
     */
    EXPECT_TRUE(flexDirectionValue == 0);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    EXPECT_TRUE(grid->GetColumnsArgs() == "100px 200px 300px");
    EXPECT_TRUE(grid->GetRowsArgs() == "20% 30% 30% 20%");
    EXPECT_TRUE(static_cast<int32_t>(grid->GetColumnGap().Value()) == 10);
    EXPECT_TRUE(static_cast<int32_t>(grid->GetRowGap().Value()) == 20);
}

} // namespace OHOS::Ace::Framework
