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

#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_rating.h"
#include "frameworks/bridge/common/dom/dom_swiper.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

namespace {

const std::string EVENT_TEST_001 = ""
                                   "{                                        "
                                   "  \"tag\": \"swiper\",                   "
                                   "  \"commonEvent\": [                     "
                                   "                     \"touchstart\",     "
                                   "                     \"touchmove\",      "
                                   "                     \"touchcancel\",    "
                                   "                     \"touchend\"        "
                                   "                   ]                     "
                                   "}";

const std::string EVENT_TEST_002 = ""
                                   "{                                     "
                                   "  \"tag\": \"text\",                  "
                                   "  \"commonEvent\": [                  "
                                   "                     \"click\",       "
                                   "                     \"longpress\"    "
                                   "                   ]                  "
                                   "}";

const std::string EVENT_TEST_003 = ""
                                   "{                                        "
                                   "  \"tag\": \"text\",                     "
                                   "  \"commonEvent\": [                     "
                                   "                     \"touchstart\",     "
                                   "                     \"touchmove\",      "
                                   "                     \"touchcancel\",    "
                                   "                     \"touchend\",       "
                                   "                     \"click\",          "
                                   "                     \"longpress\"       "
                                   "                   ]                     "
                                   "}";

#ifndef WEARABLE_PRODUCT
const std::string EVENT_TEST_004 = ""
                                   "{                                    "
                                   "  \"tag\": \"rating\",               "
                                   "  \"event\": [                       "
                                   "               \"change\"            "
                                   "             ],                      "
                                   "  \"commonEvent\": [                 "
                                   "                     \"touchstart\", "
                                   "                     \"click\"       "
                                   "                   ]                 "
                                   "}";
#endif

const std::string EVENT_TEST_005 = ""
                                   "{                         "
                                   "  \"tag\": \"swiper\"     "
                                   "}";

const std::string EVENT_TEST_006 = ""
                                   "{                                              "
                                   "  \"tag\": \"swiper\",                         "
                                   "  \"commonEvent\": [                           "
                                   "                     \"focus\",                "
                                   "                     \"blur\",                 "
                                   "                     \"key\"                   "
                                   "                   ],                          "
                                   "  \"commonStyle\": [{                          "
                                   "                      \"focusable\" : \"true\" "
                                   "                    }                          "
                                   "                   ]                           "
                                   "}";

} // namespace

class CommonEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CommonEventTest::SetUpTestCase() {}
void CommonEventTest::TearDownTestCase() {}
void CommonEventTest::SetUp() {}
void CommonEventTest::TearDown() {}

/**
 * @tc.name: DomCommonEventTest001
 * @tc.desc: Test add touch events to dom swiper successfully.
 * @tc.type: FUNC
 */
HWTEST_F(CommonEventTest, DomCommonEventTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create swiper node with it.
     * @tc.expected: step1. Swiper node is created successfully.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(EVENT_TEST_001);
    RefPtr<TouchListenerComponent> touchEventComponent = swiper->GetTouchListenerComponent();
    ASSERT_TRUE(touchEventComponent != nullptr);

    /**
     * @tc.steps: step2. Check touch eventId of created swiper node.
     * @tc.expected: step2. The value of touch eventId is as expected.
     */
    EXPECT_TRUE(touchEventComponent->GetOnTouchDownId() == std::to_string(swiper->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchDownId().GetData().eventType == DOM_TOUCH_START);
    EXPECT_TRUE(touchEventComponent->GetOnTouchMoveId() == std::to_string(swiper->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchMoveId().GetData().eventType == DOM_TOUCH_MOVE);
    EXPECT_TRUE(touchEventComponent->GetOnTouchUpId() == std::to_string(swiper->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchUpId().GetData().eventType == DOM_TOUCH_END);
    EXPECT_TRUE(touchEventComponent->GetOnTouchCancelId() == std::to_string(swiper->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchCancelId().GetData().eventType == DOM_TOUCH_CANCEL);
}

/**
 * @tc.name: DomCommonEventTest002
 * @tc.desc: Test add click and longpress event to dom text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(CommonEventTest, DomCommonEventTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create text node with it.
     * @tc.expected: step1. Text node is created successfully.
     */
    auto text = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(EVENT_TEST_002);
    RefPtr<GestureListenerComponent> gestureEventComponent = text->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent != nullptr);

    /**
     * @tc.steps: step2. Check click and longpress eventId of created text node.
     * @tc.expected: step2. The value of click and longpress eventId is as expected.
     */
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId() == std::to_string(text->GetNodeId()));
    EXPECT_TRUE(gestureEventComponent->GetOnLongPressId().GetData().eventType == DOM_LONG_PRESS);
    EXPECT_TRUE(gestureEventComponent->GetOnLongPressId() == std::to_string(text->GetNodeId()));
}

/**
 * @tc.name: DomCommonEventTest003
 * @tc.desc: Test add touch events and click/longpress event to dom text successfully.
 * @tc.type: FUNC
 */
HWTEST_F(CommonEventTest, DomCommonEventTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create text node with it.
     * @tc.expected: step1. Text node is created successfully.
     */
    auto text = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(EVENT_TEST_003);
    RefPtr<TouchListenerComponent> touchEventComponent = text->GetTouchListenerComponent();
    RefPtr<GestureListenerComponent> gestureEventComponent = text->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent != nullptr);
    ASSERT_TRUE(touchEventComponent != nullptr);

    /**
     * @tc.steps: step2. Check all event eventId of created text node.
     * @tc.expected: step2. The value of eventId about all events is as expected.
     */
    EXPECT_TRUE(touchEventComponent->GetOnTouchDownId() == std::to_string(text->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchDownId().GetData().eventType == DOM_TOUCH_START);
    EXPECT_TRUE(touchEventComponent->GetOnTouchMoveId() == std::to_string(text->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchMoveId().GetData().eventType == DOM_TOUCH_MOVE);
    EXPECT_TRUE(touchEventComponent->GetOnTouchUpId() == std::to_string(text->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchUpId().GetData().eventType == DOM_TOUCH_END);
    EXPECT_TRUE(touchEventComponent->GetOnTouchCancelId() == std::to_string(text->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchCancelId().GetData().eventType == DOM_TOUCH_CANCEL);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId() == std::to_string(text->GetNodeId()));
    EXPECT_TRUE(gestureEventComponent->GetOnLongPressId().GetData().eventType == DOM_LONG_PRESS);
    EXPECT_TRUE(gestureEventComponent->GetOnLongPressId() == std::to_string(text->GetNodeId()));
}

#ifndef WEARABLE_PRODUCT
/**
 * @tc.name: DomCommonEventTest004
 * @tc.desc: Test private change event and common gesture event to dom rating successfully.
 * @tc.type: FUNC
 */
HWTEST_F(CommonEventTest, DomCommonEventTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create rating node with it.
     * @tc.expected: step1. Swiper node is created successfully.
     */
    auto rating = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(EVENT_TEST_004);
    RefPtr<TouchListenerComponent> touchEventComponent = rating->GetTouchListenerComponent();
    RefPtr<GestureListenerComponent> gestureEventComponent = rating->GetGestureListenerComponent();
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(rating);
    RefPtr<RatingComponent> ratingComponent = AceType::DynamicCast<RatingComponent>(boxChild->GetChild());
    ASSERT_TRUE(gestureEventComponent != nullptr);
    ASSERT_TRUE(touchEventComponent != nullptr);
    ASSERT_TRUE(ratingComponent != nullptr);

    /**
     * @tc.steps: step2. Check all event eventId of created rating node.
     * @tc.expected: step2. The value of eventId about all events is as expected.
     */
    EXPECT_TRUE(touchEventComponent->GetOnTouchDownId() == std::to_string(rating->GetNodeId()));
    EXPECT_TRUE(touchEventComponent->GetOnTouchDownId().GetData().eventType == DOM_TOUCH_START);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId() == std::to_string(rating->GetNodeId()));
    EXPECT_TRUE(ratingComponent->GetChangeEventId() == std::to_string(rating->GetNodeId()));
    EXPECT_TRUE(ratingComponent->GetChangeEventId().GetData().eventType == DOM_CHANGE);
}
#endif

/**
 * @tc.name: DomCommonEventTest005
 * @tc.desc: Test default event value without add event to swiper node.
 * @tc.type: FUNC
 */
HWTEST_F(CommonEventTest, DomCommonEventTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string with right fields, then create swiper node with it.
     * @tc.expected: step1. Swiper node is created successfully and has no event.
     */
    auto swiper = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(EVENT_TEST_005);
    RefPtr<TouchListenerComponent> touchEventComponent = swiper->GetTouchListenerComponent();
    RefPtr<GestureListenerComponent> gestureEventComponent = swiper->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent == nullptr);
    ASSERT_TRUE(touchEventComponent == nullptr);
}

} // namespace OHOS::Ace::Framework
