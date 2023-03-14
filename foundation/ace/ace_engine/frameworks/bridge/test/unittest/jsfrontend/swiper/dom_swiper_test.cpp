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
#include "frameworks/bridge/common/dom/dom_swiper.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t TEST_INDEX_VALUE = 0;
constexpr double TEST_INTERVAL_VALUE = 2000.0;
constexpr double TEST_DURATION_VALUE = 500.0;
constexpr double TEST_INDICATOR_SIZE = 10.0;
constexpr double TEST_INDICATOR_TOP = 20.0;
constexpr double TEST_INDICATOR_LEFT = 20.0;
constexpr double TEST_INDICATOR_RIGHT = 20.0;
constexpr double TEST_INDICATOR_BOTTOM = 20.0;
const std::string TEST_INDICATOR_COLOR = "#12345678";
const std::string TEST_INDICATOR_SELECT_COLOR = "#87654321";
constexpr int32_t TEST_AXIS_VALUE = 0;
constexpr int32_t TEST_CHILDREN_SIZE = 2;
constexpr uint32_t TEST_INDEX_VALUE_004 = 3;
constexpr uint32_t DEFAULT_INDEX_VALUE = 0;
constexpr double DEFAULT_INTERVAL_VALUE = 3000.0;
constexpr int32_t DEFAULT_AXIS_VALUE = 1;

const std::string SWIPER_TEST_001 = ""
                                    "{                                                          "
                                    "  \"tag\": \"swiper\",                                     "
                                    "  \"attr\": [{                                             "
                                    "                \"index\" : \"0\"                          "
                                    "              },                                           "
                                    "              {"
                                    "                \"duration\" : \"500\"                     "
                                    "              },                                           "
                                    "              {"
                                    "                \"loop\" : \"false\"                       "
                                    "              },                                           "
                                    "              {"
                                    "                \"autoplay\" : \"true\"                    "
                                    "              },                                           "
                                    "              {"
                                    "                \"interval\" : \"2000\"                    "
                                    "              },                                           "
                                    "              {"
                                    "                \"vertical\" : \"true\"                    "
                                    "              },                                           "
                                    "              {"
                                    "                \"indicator\" : \"true\"                   "
                                    "              }],                                          "
                                    "  \"style\":  [{ "
                                    "                \"indicatorColor\" : \"#12345678\"         "
                                    "              },                                           "
                                    "              { "
                                    "                \"indicatorSelectedColor\" : \"#87654321\" "
                                    "              },                                           "
                                    "              {"
                                    "                \"indicatorSize\" : \"10\"                 "
                                    "              },                                           "
                                    "              {"
                                    "                \"indicatorTop\" : \"20\"                  "
                                    "              },                                           "
                                    "              {"
                                    "                \"indicatorLeft\" : \"20\"                 "
                                    "              },                                           "
                                    "              {"
                                    "                \"indicatorRight\" : \"20\"                "
                                    "              },                                           "
                                    "              {"
                                    "                \"indicatorBottom\" : \"20\"               "
                                    "              }]"
                                    "}";

const std::string SWIPER_TEST_002 = ""
                                    "{                                         "
                                    "  \"tag\": \"swiper\",                    "
                                    "  \"attr\" : [{"
                                    "               \"indicator\" : \"false\"  "
                                    "             }]                           "
                                    "}";

const std::string SWIPER_TEST_003 = ""
                                    "{                                 "
                                    "  \"tag\": \"swiper\",            "
                                    "  \"child\": [{                   "
                                    "               \"tag\":\"text\"   "
                                    "              },"
                                    "              {                   "
                                    "               \"tag\":\"text\"   "
                                    "              }]                  "
                                    "}";

const std::string SWIPER_TEST_004 = ""
                                    "{                                 "
                                    "  \"tag\": \"swiper\",            "
                                    "  \"attr\" : [{                   "
                                    "               \"index\" : \"3\"  "
                                    "             }],                  "
                                    "  \"child\": [{                   "
                                    "               \"tag\":\"text\"   "
                                    "              },"
                                    "              {                   "
                                    "               \"tag\":\"text\"   "
                                    "              },"
                                    "              {                   "
                                    "               \"tag\":\"text\"   "
                                    "              }]                  "
                                    "}";

const std::string SWIPER_TEST_005 = ""
                                    "{                                 "
                                    "  \"tag\": \"swiper\"             "
                                    "}";

const std::string SWIPER_TEST_006 = ""
                                    "{                                    "
                                    "  \"tag\": \"swiper\",               "
                                    "  \"event\": [ \"change\" ]          "
                                    "}";

} // namespace

class DomSwiperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomSwiperTest::SetUpTestCase() {}
void DomSwiperTest::TearDownTestCase() {}
void DomSwiperTest::SetUp() {}
void DomSwiperTest::TearDown() {}

/**
 * @tc.name: DomSwiperCreatorTest001
 * @tc.desc: Test swiper node and child swiper component are created as expected.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwiperTest, DomSwiperCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create swiper node with it.
     * @tc.expected: step1. Swiper node and child swiper component are created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SWIPER_TEST_001);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(swiper);
    RefPtr<SwiperComponent> swiperComponent = AceType::DynamicCast<SwiperComponent>(boxChild->GetChild());
    ASSERT_TRUE(swiperComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created swiper node.
     * @tc.expected: step2. The styles and attributes are as expected.
     */
    EXPECT_EQ(swiperComponent->GetIndex(), TEST_INDEX_VALUE);
    EXPECT_TRUE(swiperComponent->IsAutoPlay());
    EXPECT_TRUE(NearEqual(swiperComponent->GetAutoPlayInterval(), TEST_INTERVAL_VALUE));
    EXPECT_FALSE(swiperComponent->IsLoop());
    EXPECT_TRUE(NearEqual(swiperComponent->GetDuration(), TEST_DURATION_VALUE));
    auto indicator = swiperComponent->GetIndicator();
    EXPECT_TRUE(indicator);
    if (indicator) {
        EXPECT_TRUE(NearEqual(indicator->GetSize().Value(), TEST_INDICATOR_SIZE));
        EXPECT_TRUE(NearEqual(indicator->GetTop().Value(), TEST_INDICATOR_TOP));
        EXPECT_TRUE(NearEqual(indicator->GetLeft().Value(), TEST_INDICATOR_LEFT));
        EXPECT_TRUE(NearEqual(indicator->GetBottom().Value(), TEST_INDICATOR_BOTTOM));
        EXPECT_TRUE(NearEqual(indicator->GetRight().Value(), TEST_INDICATOR_RIGHT));
        EXPECT_EQ(indicator->GetColor(), Color::FromString(TEST_INDICATOR_COLOR));
        EXPECT_EQ(indicator->GetSelectedColor(), Color::FromString(TEST_INDICATOR_SELECT_COLOR));
    }
    EXPECT_EQ(static_cast<int32_t>(swiperComponent->GetAxis()), TEST_AXIS_VALUE);
}

/**
 * @tc.name: DomSwiperCreatorTest002
 * @tc.desc: Test set indicator attribute to swiper component successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwiperTest, DomSwiperCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create swiper node with it.
     * @tc.expected: step1. Swiper node and child swiper component are created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SWIPER_TEST_002);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(swiper);
    RefPtr<SwiperComponent> swiperComponent = AceType::DynamicCast<SwiperComponent>(boxChild->GetChild());
    ASSERT_TRUE(swiperComponent != nullptr);

    /**
     * @tc.steps: step2. Check indicator value of swiper component.
     * @tc.expected: step2. The indicator value of swiper component is as expected.
     */
    EXPECT_TRUE(swiperComponent->GetIndicator() == nullptr);
}

/**
 * @tc.name: DomSwiperCreatorTest003
 * @tc.desc: Test append child to swiper component successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwiperTest, DomSwiperCreatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create swiper node with it.
     * @tc.expected: step1. Swiper node and child swiper component are created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SWIPER_TEST_003);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(swiper);
    RefPtr<SwiperComponent> swiperComponent = AceType::DynamicCast<SwiperComponent>(boxChild->GetChild());
    ASSERT_TRUE(swiperComponent != nullptr);

    /**
     * @tc.steps: step2. Check child number of created swiper component.
     * @tc.expected: step2. The child number of swiper component is as expected.
     */
    EXPECT_TRUE(swiperComponent->GetChildren().size() == TEST_CHILDREN_SIZE);
}

/**
 * @tc.name: DomSwiperCreatorTest004
 * @tc.desc: Test set index attribute to swiper component successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwiperTest, DomSwiperCreatorTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string include index attribute, then create swiper node with it.
     * @tc.expected: step1. Swiper node and child swiper component are created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SWIPER_TEST_004);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(swiper);
    RefPtr<SwiperComponent> swiperComponent = AceType::DynamicCast<SwiperComponent>(boxChild->GetChild());
    ASSERT_TRUE(swiperComponent != nullptr);

    /**
     * @tc.steps: step2. Check index value of created swiper node.
     * @tc.expected: step2. The index value of swiper node is as expected.
     */
    EXPECT_TRUE(swiperComponent->GetIndex() == TEST_INDEX_VALUE_004);
}

/**
 * @tc.name: DomSwiperCreatorTest005
 * @tc.desc: Test swiper node and swiper component created successfully with empty input of style and attribute.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwiperTest, DomSwiperCreatorTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string without style and attribute, then create swiper node with it .
     * @tc.expected: step1. Swiper node and child swiper component are created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SWIPER_TEST_005);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(swiper);
    RefPtr<SwiperComponent> swiperComponent = AceType::DynamicCast<SwiperComponent>(boxChild->GetChild());
    ASSERT_TRUE(swiperComponent != nullptr);

    /**
     * @tc.steps: step2. Check styles and attributes of created swiper node.
     * @tc.expected: step2. Swiper node's attrs and styles are initialized with default value.
     */
    EXPECT_EQ(swiperComponent->GetIndex(), DEFAULT_INDEX_VALUE);
    EXPECT_FALSE(swiperComponent->IsAutoPlay());
    EXPECT_TRUE(NearEqual(swiperComponent->GetAutoPlayInterval(), DEFAULT_INTERVAL_VALUE));
    EXPECT_TRUE(swiperComponent->IsLoop());
    auto indicator = swiperComponent->GetIndicator();
    EXPECT_TRUE(indicator);
    if (indicator) {
        auto themeManager = AceType::MakeRefPtr<ThemeManager>();
        RefPtr<SwiperIndicatorTheme> theme = themeManager->GetTheme<SwiperIndicatorTheme>();
        EXPECT_TRUE(indicator->GetSize() == theme->GetSize());
        EXPECT_EQ(indicator->GetColor(), theme->GetColor());
        EXPECT_EQ(indicator->GetSelectedColor(), theme->GetSelectedColor());
    }
    EXPECT_EQ(static_cast<int32_t>(swiperComponent->GetAxis()), DEFAULT_AXIS_VALUE);
}

/**
 * @tc.name: DomSwiperCreatorTest006
 * @tc.desc: Test add event to swiper component successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomSwiperTest, DomSwiperCreatorTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create swiper node with it.
     * @tc.expected: step1. Swiper node and child swiper component are created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(SWIPER_TEST_006);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(swiper);
    RefPtr<SwiperComponent> swiperComponent = AceType::DynamicCast<SwiperComponent>(boxChild->GetChild());
    ASSERT_TRUE(swiperComponent != nullptr);

    /**
     * @tc.steps: step2. Check eventId of created swiper component.
     * @tc.expected: step2. The eventId value of swiper component is as expected.
     */
    EXPECT_TRUE(swiperComponent->GetChangeEventId() == std::to_string(swiper->GetNodeId()));
}

} // namespace OHOS::Ace::Framework
