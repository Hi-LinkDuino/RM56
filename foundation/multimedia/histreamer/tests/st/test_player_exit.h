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

// Run these http tests, should run hfs.exe and add resource folder to it.

// @fixture(tags=audio_play_fast)
FIXTURE(TestPlayerExit)
{
    bool StartPlayer(std::string url)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        return true;
    }

    TEST(Test play local source two times)
    {
        ASSERT_TRUE(StartPlayer(RESOURCE_DIR "/MP3/MP3_48000_32_SHORT.mp3"));
        ASSERT_TRUE(StartPlayer(RESOURCE_DIR "/MP3/MP3_48000_32_SHORT.mp3"));
    }

    TEST(Test play http non live source two times)
    {
        ASSERT_TRUE(StartPlayer("http://localhost/resource-src/media/MP3/MP3_48000_32_SHORT.mp3"));
        ASSERT_TRUE(StartPlayer("http://localhost/resource-src/media/MP3/MP3_48000_32_SHORT.mp3"));
    }
};