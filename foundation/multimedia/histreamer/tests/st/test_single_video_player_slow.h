/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include <chrono>
#include <thread>
#include "helper/test_player.hpp"
#include "testngpp/testngpp.hpp"

using namespace OHOS::Media::Test;

// @fixture(tags=video_play_slow)
FIXTURE(DataDrivenSingleVideoPlayerTestSlow)
{
    DATA_PROVIDER(myurls, 1,
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/9_AVC_1280x720_59.940fps_AAC_128Kbps_2channels.mp4")),
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/11_AVC_640x480_25.000fps.mp4")),
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/AVC_1000x526_25FPS.mp4")),
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/AVC_1024x526_25FPS.mp4")),
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/h264_aac_128x72_30r_voiced.mp4")),
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/h264_mute_audio_640x480_30r.mp4")));

    // @test(data="myurls", tags=video_play_slow)
    PTEST((std::string url), Test single player play all urls video, and finished automatically)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
};
