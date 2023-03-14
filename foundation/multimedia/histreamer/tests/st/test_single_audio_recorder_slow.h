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
#include "helper/test_recorder.hpp"
#include "helper/test_player.hpp"
#include "testngpp/testngpp.hpp"
#include "foundation/log.h"
#include "foundation/osal/filesystem/file_system.h"

using namespace OHOS::Media::Test;

// @fixture(tags=audio_record_slow)
FIXTURE(DataDrivenSingleAudioRecorderTestSlow)
{
    // file name: 44100_2_02.pcm,  44100 - sample rate, 2 - channel count, 02 - file index
    DATA_PROVIDER(pcmSources, 1,
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/8000_1_01.pcm"), 8000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/8000_2_01.pcm"), 8000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/11025_2_01.pcm"), 11025, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/12000_2_01.pcm"), 12000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/16000_1_01.pcm"), 16000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/16000_1_02.pcm"), 16000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/16000_2_01.pcm"), 16000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/22050_2_01.pcm"), 22050, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/24000_2_01.pcm"), 24000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/32000_1_01.pcm"), 32000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/44100_2_01.pcm"), 44100, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_1_01.pcm"), 48000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_1_02.pcm"), 48000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_1_03.pcm"), 48000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_2_01.pcm"), 48000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_2_02.pcm"), 48000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_2_03.pcm"), 48000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/48000_2_04.pcm"), 48000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/64000_1_01.pcm"), 64000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/96000_1_01.pcm"), 96000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/96000_1_02.pcm"), 96000, 1, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/96000_2_01.pcm"), 96000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/96000_2_02.pcm"), 96000, 2, 320000)),
    DATA_GROUP(AudioRecordSource(std::string(RESOURCE_DIR "/PCM/96000_2_03.pcm"), 96000, 2, 320000))
    );

    SETUP()
    {
        OHOS::Media::OSAL::FileSystem::RemoveFilesInDir(TestRecorder::GetOutputDir());
    }

    TEARDOWN()
    {
    }

    // @test(data="pcmSources", tags=recorderdebug)
    PTEST((AudioRecordSource recordSource), Test single audio recorder)
    {
        std::unique_ptr<TestRecorder> recorder = TestRecorder::CreateAudioRecorder();
        ASSERT_EQ(0, recorder->Configure(recordSource));
        ASSERT_EQ(0, recorder->Prepare());
        ASSERT_EQ(0, recorder->Start());
        std::this_thread::sleep_for(std::chrono::seconds(30));
        ASSERT_EQ(0, recorder->Stop());
        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::string filePath;
        ASSERT_TRUE(recorder->GetRecordedFile(filePath) > 0);
        std::unique_ptr<TestPlayer> player = TestPlayer::Create();
        ASSERT_EQ(0, player->SetSource(TestSource(filePath)));
        ASSERT_EQ(0, player->Prepare());
        ASSERT_EQ(0, player->Play());
        while (player->IsPlaying()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        OHOS::Media::OSAL::FileSystem::RemoveFilesInDir(filePath);
    }
};

