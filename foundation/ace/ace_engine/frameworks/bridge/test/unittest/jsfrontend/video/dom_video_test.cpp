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
#include "frameworks/bridge/common/dom/dom_video.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomVideoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomVideoTest::SetUpTestCase() {}
void DomVideoTest::TearDownTestCase() {}
void DomVideoTest::SetUp() {}
void DomVideoTest::TearDown() {}

/**
 * @tc.name: DomVideoCreatorTest001
 * @tc.desc: Test create video node successfully and videocomponent create as desire.
 * @tc.type: FUNC
 */
HWTEST_F(DomVideoTest, DomVideoCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomVideo with all attributes and style.
     */
    const std::string videoTest001 = "{                                                  "
                                     "  \"tag\": \"video\",                              "
                                     "  \"attr\": [{                                     "
                                     "                \"muted\" : \"true\"               "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"src\" : \"/1.mp4\"               "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"autoplay\" : \"true\"            "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"poster\" : \"/1.png\"            "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"controls\" : \"false\"           "
                                     "              }],                                  "
                                     "  \"style\":  [{                                   "
                                     "                \"objectFit\" : \"cover \"        "
                                     "              }]"
                                     "}";
    /**
     * @tc.steps: step2. Create video node with the string.
     * @tc.expected: step2. Video node and child video component are created successfully.
     */
    auto video = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(videoTest001);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(video);
    RefPtr<VideoComponent> videoComponent = AceType::DynamicCast<VideoComponent>(boxChild->GetChild());
    ASSERT_TRUE(videoComponent != nullptr);

    /**
     * @tc.steps: step3. Check the style and attributes of created video node.
     * @tc.expected: step3. The style and attributes are as expected.
     */
    EXPECT_TRUE(videoComponent->GetSrc() == "/1.mp4");
    EXPECT_TRUE(videoComponent->GetPoster() == "/1.png");
    EXPECT_TRUE(videoComponent->IsMute());
    EXPECT_TRUE(videoComponent->IsAutoPlay());
    EXPECT_TRUE(!videoComponent->NeedControls());
}

/**
 * @tc.name: DomVideoCreatorTest002
 * @tc.desc: Test the VideoComponent initialized by default value.
 * @tc.type: FUNC
 */
HWTEST_F(DomVideoTest, DomVideoCreatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomVideo with necessary parameters.
     */
    const std::string videoTest002 = "{                                                  "
                                     "  \"tag\": \"video\",                              "
                                     "  \"attr\": [{                                     "
                                     "                \"src\" : \"/1.mp4\"               "
                                     "              }]                                   "
                                     "}";
    /**
     * @tc.steps: step2. Create video node with the string.
     * @tc.expected: step2. Video node and child video component are created successfully.
     */
    auto video = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(videoTest002);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(video);
    RefPtr<VideoComponent> videoComponent = AceType::DynamicCast<VideoComponent>(boxChild->GetChild());
    ASSERT_TRUE(videoComponent != nullptr);

    /**
     * @tc.steps: step3. Check the style and attributes of created video node.
     * @tc.expected: step3. The style and attributes are as expected.
     */
    EXPECT_TRUE(videoComponent->GetSrc() == "/1.mp4");
    EXPECT_TRUE(videoComponent->GetPoster().empty());
    EXPECT_TRUE(!videoComponent->IsMute());
    EXPECT_TRUE(!videoComponent->IsAutoPlay());
    EXPECT_TRUE(videoComponent->NeedControls());
}

/**
 * @tc.name: DomVideoCreatorTest003
 * @tc.desc: Test value validation. If src is invalid, create component failed.
 * @tc.type: FUNC
 */
HWTEST_F(DomVideoTest, DomVideoCreatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomVideo without src attributes.
     */
    const std::string videoTest003 = "{                                                  "
                                     "  \"tag\": \"video\",                              "
                                     "  \"attr\": [{                                     "
                                     "                \"muted\" : \"true\"               "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"autoplay\" : \"true\"            "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"poster\" : \"/1.png\"            "
                                     "              },                                   "
                                     "              {                                    "
                                     "                \"controls\" : \"false\"           "
                                     "              }],                                  "
                                     "  \"style\":  [{                                   "
                                     "                \"objectFit\" : \"cover \"        "
                                     "              }]"
                                     "}";
    /**
     * @tc.steps: step2. Create video node with the string.
     * @tc.expected: step2. Video node created successfully, but child is nullptr.
     */
    auto video = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(videoTest003);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(video);
    ASSERT_TRUE(boxChild->GetChild() == nullptr);
}

/**
 * @tc.name: DomVideoCreatorTest004
 * @tc.desc: Test add event to Video component.
 * @tc.type: FUNC
 */
HWTEST_F(DomVideoTest, DomVideoCreatorTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct string of DomVideo with all events.
     */
    const std::string videoTest004 = "{                                                     "
                                     "  \"tag\": \"video\",                                 "
                                     "  \"attr\": [{                                        "
                                     "                \"src\" : \"/1.mp4\"                  "
                                     "              }],                                     "
                                     "  \"event\": [                                        "
                                     "               \"prepared\",                          "
                                     "               \"start\",                             "
                                     "               \"pause\",                             "
                                     "               \"finish\",                            "
                                     "               \"error\",                             "
                                     "               \"seeking\",                           "
                                     "               \"seeked\",                            "
                                     "               \"timeupdate\",                        "
                                     "               \"fullscreenchange\"                   "
                                     "             ]                                        "
                                     "}";

    /**
     * @tc.steps: step2. Create video node with the string.
     * @tc.expected: step2. Progress node and child video component are created successfully.
     */
    auto video = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(videoTest004);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(video);
    RefPtr<VideoComponent> videoComponent = AceType::DynamicCast<VideoComponent>(boxChild->GetChild());
    ASSERT_TRUE(videoComponent != nullptr);

    /**
     * @tc.steps: step3. Check events of created video node.
     * @tc.expected: step3. The events are as expected.
     */
    ASSERT_TRUE(videoComponent->GetPreparedEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetStartEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetPauseEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetFinishEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetErrorEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetSeekingEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetSeekedEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetTimeUpdateEventId() == std::to_string(video->GetNodeId()));
    ASSERT_TRUE(videoComponent->GetFullscreenChangeEventId() == std::to_string(video->GetNodeId()));
}

} // namespace OHOS::Ace::Framework
