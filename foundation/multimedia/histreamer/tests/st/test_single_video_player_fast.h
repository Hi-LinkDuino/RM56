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
#include <fcntl.h>
#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#define O_BINARY 0 // which is not defined for Linux
#endif
#include <math.h>
#include <thread>
#include "foundation/log.h"
#include "helper/test_player.hpp"
#include "testngpp/testngpp.hpp"

using namespace OHOS::Media::Test;

// @fixture(tags=video_play_fast)
FIXTURE(DataDrivenSingleVideoPlayerTestFast)
{
    DATA_PROVIDER(myurls, 1,
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/9_AVC_1280x720_59.940fps_AAC_128Kbps_2channels.mp4")));

    DATA_PROVIDER(myfdurl, 2,
    DATA_GROUP(std::string(RESOURCE_DIR "/MP4/9_AVC_1280x720_59.940fps_AAC_128Kbps_2channels.mp4"), 34373632));

    std::string FilePathToFd(std::string url, int32_t fileSize)
    {
        std::string uri = "fd://?offset=0&size=";
        uri += std::to_string(fileSize);
        int32_t fd = open(url.c_str(), O_RDONLY|O_BINARY);
        std::string fdStr = std::to_string(fd);
        uri.insert(5, fdStr); // 5 ---fd:://
        return uri;
    }

    bool CheckTimeEquality(int32_t expectValue, int32_t currentValue)
    {
        MEDIA_LOG_I("expectValue : %d, currentValue : %d", expectValue, currentValue);
        return fabs(expectValue - currentValue) < 100; // if open debug log, should use value >= 1000
    }

    // @test(data="myurls", tags=video_play_fast)
    PTEST((std::string url), Test single player play url video, and finished automatically)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test single player play fd source, and finished automatically)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    // SUB_MEDIA_VIDEO_PLAYER_fdSrc_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test single player wrong fd)
    {
        std::string uri = "fd://?offset=0&size=";
        uri += std::to_string(fileSize);
        uri.insert(5, "-1"); // 5 ---fd:://
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_fdSrc_CALLBACK_0200
    // @test(data="myurls", tags=video_play_fast)
    PTEST((std::string url), Test fdsource prepare, play, pause, release)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_fdSrc_CALLBACK_0300
    // @test(data="myurls", tags=video_play_fast)
    PTEST((std::string url), Test fdsource prepare, play, pause, release)
    {
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_fdSrc_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare then release)
    {
        std::string uri = "fd://?offset=0&size=";
        uri += std::to_string(fileSize);
        uri.insert(5, "-123456789"); // 5 ---fd:://
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare then release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, prepare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_NE(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, prepare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_NE(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, prpeare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, reset, setsource, prepare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0600/0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, Seek, prepare, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_NE(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_0800
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, prepare, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_NE(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1000
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PREPARE_CALLBACK_1200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, prepare, prepare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->Play());
        ASSERT_EQ(0, player->Release());
    }

    // // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_NE(0, player->Play());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Reset());
        ASSERT_NE(0, player->Play());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0600 /0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0800
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1000
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource  play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Play());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_1200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, play, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare,  pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_NE(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, relese)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_NE(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, reset, pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Reset());
        ASSERT_NE(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0600/0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, pause, relase)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_0800
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, pause, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1000
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, setsource, pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PAUSE_CALLBACK_1200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, pause, pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_020
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // 'SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, reset, stop, relese)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Reset());
        ASSERT_NE(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0600/0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, stop, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0800
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, prepare, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_0900
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, prepare, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1000
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, setsource, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_STOP_CALLBACK_1200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, stop, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0600/0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, reset, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_0800
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1000
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RESET_CALLBACK_1200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, reset, reset, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, reset)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0600/0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_0800
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1000
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource setSource, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_RELEASE_CALLBACK_1200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, reset, release)
    {
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource create, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_NE(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_NE(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0600
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Reset());
        ASSERT_NE(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, seek, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        int64_t  seekPos {5000};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_0900
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_NE(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, seek, seek, release)
    {
        int64_t seekPos {5000};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        seekPos = 5000;
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        seekPos = 5000;
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, -1, release)
    {
        int64_t seekPos {-1};
        int64_t currentMS {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_NE(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SEEK_CALLBACK_1400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, durationTime + 1000, release)
    {
        int64_t seekPos {0};
        int64_t currentMS {0};
        int64_t durationMs {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->GetDuration(durationMs));
        seekPos = durationMs + 1000;
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->GetCurrentTime(currentMS));
        EXPECT_TRUE(CheckTimeEquality(seekPos, currentMS));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0200
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_PLAY_CALLBACK_0300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, pause, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0500
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, stop, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0600
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, reset, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->Reset());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0700
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, seek, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        int64_t seekPos {5000};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(0, player->Seek(seekPos));
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_0900
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource setsourse, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1100
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, setvolume, setvolume, setvolume, release)
    {
        float leftVolume {1};
        float rightVolume {1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1300
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setVolume, -1, release)
    {
        float leftVolume {-1};
        float rightVolume {-1};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_NE(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_SETVOLUME_CALLBACK_1400
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, play, setVolume, 2, release)
    {
        float leftVolume {2};
        float rightVolume {2};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_NE(0, player->SetVolume(leftVolume, rightVolume));
        ASSERT_EQ(0, player->Release());
    }

    //  SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_LOOP
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, setsingleloop true, play, seek, durationtime
        3 times, setsingleloop flase, release)
    {
        int64_t durationMs {0};
        std::string uri = FilePathToFd(url, fileSize);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->SetSingleLoop(true));
        ASSERT_EQ(0, player->Play());
        ASSERT_EQ(0, player->GetDuration(durationMs));
        ASSERT_EQ(0, player->Seek(durationMs));
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->GetDuration(durationMs));
        ASSERT_EQ(0, player->Seek(durationMs));
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        ASSERT_EQ(0, player->GetDuration(durationMs));
        ASSERT_EQ(0, player->Seek(durationMs));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(0, player->GetDuration(durationMs));
        ASSERT_EQ(0, player->Seek(durationMs));
        std::this_thread::sleep_for(std::chrono::milliseconds(8000));
        ASSERT_EQ(0, player->SetSingleLoop(false));
        ASSERT_EQ(0, player->Release());
    }

    // SUB_MEDIA_VIDEO_PLAYER_FUNCTION_CALLBACK_BASE
    // @test(data="myfdurl", tags=video_play_fast)
    PTEST((std::string url, int32_t fileSize), Test fdsource prepare, setsingleloop true, play, seek, set fd, seek
        2 times, setsingleloop false, release)
    {
        int64_t durationMs {0};
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(url)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Pause());
        ASSERT_EQ(0, player->Play());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->Stop());
        ASSERT_EQ(0, player->Reset());
        std::string uri = FilePathToFd(url, fileSize);
        ASSERT_EQ(0, player->SetSource(TestSource(uri)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->SetSingleLoop(true));
        ASSERT_EQ(0, player->Play());
        ASSERT_TRUE(player->IsPlaying());
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        ASSERT_EQ(0, player->GetDuration(durationMs));
        ASSERT_EQ(0, player->Seek(durationMs/2));
        ASSERT_EQ(0, player->Seek(0));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ASSERT_EQ(0, player->GetDuration(durationMs));
        ASSERT_EQ(0, player->Seek(durationMs));
        std::this_thread::sleep_for(std::chrono::milliseconds(8000));
        ASSERT_EQ(0, player->SetSingleLoop(false));
        ASSERT_EQ(0, player->Release());
    }
};
