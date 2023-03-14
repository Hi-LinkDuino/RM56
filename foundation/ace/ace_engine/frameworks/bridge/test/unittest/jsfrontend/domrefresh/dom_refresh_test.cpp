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
#include "frameworks/bridge/common/dom/dom_refresh.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr double LOADING_DISTANCE = 16.0;
constexpr double REFRESH_DISTANCE = 64.0;

const std::string REFRESH_TEST_001 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\"                                     "
                                     "}";

const std::string REFRESH_TEST_002 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\",                                    "
                                     "  \"attr\": [{                                             "
                                     "                \"offset\" : \"500px\"                     "
                                     "             }]                                            "
                                     "}";

const std::string REFRESH_TEST_003 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\",                                    "
                                     "  \"attr\": [{                                             "
                                     "                \"refreshing\" : \"true\"                  "
                                     "             }]                                            "
                                     "}";

const std::string REFRESH_TEST_004 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\",                                    "
                                     "  \"attr\": [{                                             "
                                     "                \"lasttime\" : \"true\"                    "
                                     "             }]                                            "
                                     "}";

const std::string REFRESH_TEST_005 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\",                                    "
                                     "  \"attr\": [{                                             "
                                     "                \"type\" : \"pulldown\"                    "
                                     "             }]                                            "
                                     "}";

const std::string REFRESH_TEST_006 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\" ,                                   "
                                     "  \"style\": [{                                            "
                                     "                \"backgroundColor\" : \"#bbbbbb\"          "
                                     "             }]                                            "
                                     "}";

const std::string REFRESH_TEST_007 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\" ,                                   "
                                     "  \"style\": [{                                             "
                                     "                \"progressColor\" : \"#bbbbbb\"            "
                                     "             }]                                            "
                                     "}";

const std::string REFRESH_TEST_008 = ""
                                     "{                                                          "
                                     "  \"tag\": \"refresh\" ,                                   "
                                     "  \"event\": [                                            "
                                     "                \"refresh\"            "
                                     "             ]                                            "
                                     "}";

const std::string REFRESH_TEST_009 = ""
                                     "{"
                                     "  \"tag\": \"refresh\" ,"
                                     "   \"child\":"
                                     "[{"
                                     "    \"tag\":\"div\""
                                     "}]"
                                     "}";
} // namespace

class DomRefreshTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomRefreshTest::SetUpTestCase() {}
void DomRefreshTest::TearDownTestCase() {}
void DomRefreshTest::SetUp() {}
void DomRefreshTest::TearDown() {}

/**
 * @tc.name: DomRefreshTest001
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_001);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();

    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetRefreshType() == RefreshType::AUTO);
    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->IsRefreshing() == false);
}

/**
 * @tc.name: DomRefreshTest002
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_002);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();

    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetIndicatorOffset() ==
                Dimension(500.0, DimensionUnit::PX));
}

/**
 * @tc.name: DomRefreshTest003
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_003);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();

    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->IsRefreshing() == true);
}

/**
 * @tc.name: DomRefreshTest004
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_004);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();
    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->IsShowLastTime() == true);
}

/**
 * @tc.name: DomRefreshTest005
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_005);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();
    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetRefreshType() == RefreshType::PULL_DOWN);
}

/**
 * @tc.name: DomRefreshTest006
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_006);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();
    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetBackgroundColor() == Color(0xffbbbbbb));
}

/**
 * @tc.name: DomRefreshTest007
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_007);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();
    ASSERT_TRUE(AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetProgressColor() == Color(0xffbbbbbb));
}

/**
 * @tc.name: DomRefreshTest008
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_008);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();
    EXPECT_EQ(AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetRefreshEventId(),
        std::to_string(refresh->GetNodeId()));
}

/**
 * @tc.name: DomRefreshTest009
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_009);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();
    auto column = AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetChild();
    ASSERT_TRUE(column != nullptr);
    ASSERT_EQ(AceType::DynamicCast<ColumnComponent>(column)->GetChildren().size(), 1U);
}

/**
 * @tc.name: DomRefreshTest010
 * @tc.desc: Test refresh node are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomRefreshTest, DomRefreshTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create node with it.
     * @tc.expected: step1. node are created successfully.
     */
    auto refresh = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(REFRESH_TEST_001);
    ASSERT_TRUE(refresh != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created dom node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    auto refreshComponent = AceType::DynamicCast<DOMRefresh>(refresh)->GetSpecializedComponent();

    ASSERT_TRUE(
        AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetLoadingDistance().Value() == LOADING_DISTANCE);
    ASSERT_TRUE(
        AceType::DynamicCast<RefreshComponent>(refreshComponent)->GetRefreshDistance().Value() == REFRESH_DISTANCE);
}

} // namespace OHOS::Ace::Framework
