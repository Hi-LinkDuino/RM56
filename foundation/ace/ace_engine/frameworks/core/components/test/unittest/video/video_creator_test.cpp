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

#include <string>

#include "gtest/gtest.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/video/video_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string JSON_SRC = "file:///data/data/hiace/files/data/test.mp4";
constexpr bool JSON_AUTOPLAY = true;
const std::string JSON_POSTER = "file:///data/data/hiace/files/data/test.png";
constexpr bool JSON_CONTROLS = true;
constexpr bool JSON_MUTED = true;
constexpr ImageFit JSON_FIT = ImageFit::COVER;
const std::string JSON_PREPARED_EVENT_ID = "onPreparedId1";
const std::string JSON_START_EVENT_ID = "onStartId1";
const std::string JSON_PAUSE_EVENT_ID = "onPauseId1";
const std::string JSON_FINISH_EVENT_ID = "onFinishId1";
const std::string JSON_ERROR_EVENT_ID = "onErrorId1";
const std::string JSON_SEEKING_EVENT_ID = "onSeekingId1";
const std::string JSON_SEEKED_EVENT_ID = "onSeekedId1";
const std::string JSON_TIMEUPDATE_EVENT_ID = "onTimeUpdateId1";
const std::string JSON_FULLSCREENCHANGE_EVENT_ID = "onFullScreenChangeId1";

} // namespace

class VideoCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<VideoComponent> CreateComponent(const std::string& testJson);
    void CompareAllProperty(const RefPtr<VideoComponent>& slider);
    void CompareAllDefault(const RefPtr<VideoComponent>& slider);
};

void VideoCreatorTest::SetUpTestCase() {}

void VideoCreatorTest::TearDownTestCase() {}

void VideoCreatorTest::SetUp() {}

void VideoCreatorTest::TearDown() {}

RefPtr<VideoComponent> VideoCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (component == nullptr) {
        return nullptr;
    }
    return AceType::DynamicCast<VideoComponent>(component);
}

void VideoCreatorTest::CompareAllProperty(const RefPtr<VideoComponent>& video)
{
    EXPECT_TRUE(video->GetSrc() == JSON_SRC);
    EXPECT_TRUE(video->IsAutoPlay() == JSON_AUTOPLAY);
    EXPECT_TRUE(video->GetPoster() == JSON_POSTER);
    EXPECT_TRUE(video->NeedControls() == JSON_CONTROLS);
    EXPECT_TRUE(video->IsMute() == JSON_MUTED);
    EXPECT_TRUE(video->GetFit() == JSON_FIT);
    EXPECT_TRUE(video->GetPreparedEventId() == JSON_PREPARED_EVENT_ID);
    EXPECT_TRUE(video->GetStartEventId() == JSON_START_EVENT_ID);
    EXPECT_TRUE(video->GetPauseEventId() == JSON_PAUSE_EVENT_ID);
    EXPECT_TRUE(video->GetFinishEventId() == JSON_FINISH_EVENT_ID);
    EXPECT_TRUE(video->GetErrorEventId() == JSON_ERROR_EVENT_ID);
    EXPECT_TRUE(video->GetSeekingEventId() == JSON_SEEKING_EVENT_ID);
    EXPECT_TRUE(video->GetSeekedEventId() == JSON_SEEKED_EVENT_ID);
    EXPECT_TRUE(video->GetTimeUpdateEventId() == JSON_TIMEUPDATE_EVENT_ID);
    EXPECT_TRUE(video->GetFullscreenChangeEventId() == JSON_FULLSCREENCHANGE_EVENT_ID);
}

void VideoCreatorTest::CompareAllDefault(const RefPtr<VideoComponent>& video)
{
    EXPECT_TRUE(video->GetSrc() == JSON_SRC);
    EXPECT_TRUE(video->IsAutoPlay() == false);
    EXPECT_TRUE(video->GetPoster().empty());
    EXPECT_TRUE(video->NeedControls() == true);
    EXPECT_TRUE(video->IsMute() == false);
    EXPECT_TRUE(video->GetFit() == ImageFit::CONTAIN);
    EXPECT_TRUE(video->GetPreparedEventId().IsEmpty());
    EXPECT_TRUE(video->GetStartEventId().IsEmpty());
    EXPECT_TRUE(video->GetPauseEventId().IsEmpty());
    EXPECT_TRUE(video->GetFinishEventId().IsEmpty());
    EXPECT_TRUE(video->GetErrorEventId().IsEmpty());
    EXPECT_TRUE(video->GetSeekingEventId().IsEmpty());
    EXPECT_TRUE(video->GetSeekedEventId().IsEmpty());
    EXPECT_TRUE(video->GetTimeUpdateEventId().IsEmpty());
    EXPECT_TRUE(video->GetFullscreenChangeEventId().IsEmpty());
}

/**
 * @tc.name: VideoComponentCreator001
 * @tc.desc: Test all the property created properly
 * @tc.type: FUNC
 * @tc.require: AR000DAR0T AR000DACKK
 * @tc.author: HeSu
 */
HWTEST_F(VideoCreatorTest, VideoComponentCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with all property.
     */
    std::string testJson = "{                                           "
                           "  \"className\": \"Video\",                 "
                           "  \"src\": \"file:///data/data/hiace/files/data/test.mp4\", "
                           "  \"autoplay\": true,                       "
                           "  \"poster\": \"file:///data/data/hiace/files/data/test.png\", "
                           "  \"controls\": true,                       "
                           "  \"muted\": true,                          "
                           "  \"videofit\": \"cover\",                "
                           "  \"onPreparedId\": \"onPreparedId1\",      "
                           "  \"onStartId\": \"onStartId1\",            "
                           "  \"onPauseId\": \"onPauseId1\",            "
                           "  \"onFinishId\": \"onFinishId1\",           "
                           "  \"onErrorId\": \"onErrorId1\",             "
                           "  \"onSeekingId\": \"onSeekingId1\",         "
                           "  \"onSeekedId\": \"onSeekedId1\",           "
                           "  \"onTimeUpdateId\": \"onTimeUpdateId1\",      "
                           "  \"onFullScreenChangeId\": \"onFullScreenChangeId1\"      "
                           "}";
    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property match
     */
    RefPtr<VideoComponent> video = CreateComponent(testJson);
    ASSERT_TRUE(video != nullptr);
    CompareAllProperty(video);
}

/**
 * @tc.name: VideoComponentCreator002
 * @tc.desc: Test value validation. If src is invalid, create component failed.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0T AR000DACKK
 * @tc.author: HeSu
 */
HWTEST_F(VideoCreatorTest, VideoComponentCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string without src.
     */
    std::string testJson = "{                                           "
                           "  \"className\": \"Video\",                 "
                           "  \"autoplay\": true,                       "
                           "  \"poster\": \"file:///data/data/hiace/files/data/test.png\", "
                           "  \"controls\": true,                       "
                           "  \"muted\": true,                          "
                           "  \"videofit\": \"contain\",                "
                           "  \"onPreparedId\": \"onPreparedId1\",      "
                           "  \"onStartId\": \"onStartId1\",            "
                           "  \"onPauseId\": \"onPauseId1\",            "
                           "  \"onFinishId\": \"onFinishId\",           "
                           "  \"onErrorId\": \"onErrorId\",             "
                           "  \"onSeekingId\": \"onSeekingId\",         "
                           "  \"onSeekedId\": \"onSeekedId\",           "
                           "  \"onTimeUpdateId\": \"onTimeUpdateId\",      "
                           "  \"onFullScreenChangeId\": \"onFullScreenChangeId\"      "
                           "}";
    /**
     * @tc.steps: step2. check component is null or not
     * @tc.expected: step2. component is nullptr
     */
    RefPtr<VideoComponent> video = CreateComponent(testJson);
    ASSERT_TRUE(video == nullptr);
}

/**
 * @tc.name: VideoComponentCreator003
 * @tc.desc: Test the VideoComponent initialized by default value.
 * @tc.type: FUNC
 * @tc.require: AR000DAR0T AR000DACKK
 * @tc.author: YaoYuchi
 */
HWTEST_F(VideoCreatorTest, VideoComponentCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string with value, minValue and maxValue. And choose min > value
     */
    std::string testJson = "{                                 "
                           "  \"className\": \"Video\",      "
                           "  \"src\": \"file:///data/data/hiace/files/data/test.mp4\" "
                           "}";
    /**
     * @tc.steps: step2. check all property matched
     * @tc.expected: step2. all property matched
     */
    RefPtr<VideoComponent> video = CreateComponent(testJson);
    EXPECT_TRUE(video != nullptr);
    CompareAllDefault(video);
}

} // namespace OHOS::Ace