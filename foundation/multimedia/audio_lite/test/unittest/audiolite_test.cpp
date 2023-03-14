/*
 * Copyright (C) 2020-2021 Huawei Device Co., Ltd.
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
#include "audiolite_test.h"
#include "codec_type.h"
#include "media_errors.h"

using namespace std;
using namespace OHOS::Audio;
using namespace testing::ext;


namespace OHOS {
const int32_t BUFFER_SIZE = 1024;
const int32_t SAMPLE_RATE = 24000;
const int32_t BIT_RATE = 1024;
const int32_t CHANNEL_COUNT = 2;

void AudioliteTest::SetUpTestCase(void) {}

void AudioliteTest::TearDownTestCase(void) {}

void AudioliteTest::SetUp()
{
    audioCapInfo.channelCount = CHANNEL_COUNT;
    audioCapInfo.sampleRate = SAMPLE_RATE;
    audioCapInfo.bitRate = BIT_RATE;
    audioCapInfo.inputSource = AUDIO_SOURCE_DEFAULT;
    audioCapInfo.bitWidth = BIT_WIDTH_16;
}

void AudioliteTest::TearDown() {}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_buffer_001, TestSize.Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();

    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;

    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_buffer_002, TestSize.Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audioCapturer_003, TestSize.Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audioCapturer_004, TestSize.Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audioCapturer_005, TestSize.Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_006, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_007, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_008, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_009, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 24000;
    info.channelCount = 2;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_010, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 48000;
    info.channelCount = 2;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_011, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_012, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_013, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 8; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_014, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 24000;
    info.channelCount = 2;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 2; Bit_width 8; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_015, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 48000;
    info.channelCount = 2;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_016, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_017, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_018, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 8; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_019, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format DEFAULT; Channel 1; Bit_width 8; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_020, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_021, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_022, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_023, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_024, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_025, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_026, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_027, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_028, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_029, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 24000;
    info.channelCount = 2;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_030, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 48000;
    info.channelCount = 2;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_031, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_032, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_033, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 8; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_034, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 24000;
    info.channelCount = 2;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 2; Bit_width 8; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_035, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 48000;
    info.channelCount = 2;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_036, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_037, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_038, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 8; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_039, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LC; Channel 1; Bit_width 8; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_040, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LC;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_041, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_042, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_043, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_044, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_045, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_046, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_047, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_048, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_049, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 24000;
    info.channelCount = 2;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_050, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 48000;
    info.channelCount = 2;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_051, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_052, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_053, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 8; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_054, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 24000;
    info.channelCount = 2;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 2; Bit_width 8; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_055, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 48000;
    info.channelCount = 2;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_056, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_057, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_058, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 8; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_059, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_HE_V1; Channel 1; Bit_width 8; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_060, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_HE_V1;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}


/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 16; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_061, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 16; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_062, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 16; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_063, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 16; Sample Rate 24000; Bit Rate 24000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_064, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 24000;
    info.channelCount = 1;
    info.bitRate = 24000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 16; Sample Rate 48000; Bit Rate 48000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_065, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 48000;
    info.channelCount = 1;
    info.bitRate = 48000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 24; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_066, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 24; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_067, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 24; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_068, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 24; Sample Rate 64000; Bit Rate 64000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_069, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 64000;
    info.channelCount = 2;
    info.bitRate = 64000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 16; Sample Rate 96000; Bit Rate 96000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_070, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 96000;
    info.channelCount = 2;
    info.bitRate = 96000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 8; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_071, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 8000;
    info.channelCount = 2;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 8; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_072, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 16000;
    info.channelCount = 2;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 8; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_073, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 32000;
    info.channelCount = 2;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 8; Sample Rate 64000; Bit Rate 64000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_074, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 64000;
    info.channelCount = 2;
    info.bitRate = 64000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 2; Bit_width 8; Sample Rate 96000; Bit Rate 96000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_075, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 96000;
    info.channelCount = 2;
    info.bitRate = 96000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_8;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 24; Sample Rate 8000; Bit Rate 8000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_076, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 8000;
    info.channelCount = 1;
    info.bitRate = 8000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 24; Sample Rate 16000; Bit Rate 16000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_077, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 16000;
    info.channelCount = 1;
    info.bitRate = 16000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 24; Sample Rate 32000; Bit Rate 32000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_078, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 32000;
    info.channelCount = 1;
    info.bitRate = 32000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 24; Sample Rate 64000; Bit Rate 64000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_079, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 64000;
    info.channelCount = 1;
    info.bitRate = 64000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}

/*
 * Feature: Audiolite
 * Function: audioCapturer
 * SubFunction: NA
 * FunctionPoints: NA
 * EnvConditions: NA
 * CaseDescription: Audio Capture Test-Format AAC_LD; Channel 1; Bit_width 24; Sample Rate 96000; Bit Rate 96000
 */
HWTEST_F(AudioliteTest, audio_lite_audioCapturer_test_080, Level1)
{
    AudioCapturer *audioCapturer = new AudioCapturer();
    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AAC_LD;
    info.sampleRate = 96000;
    info.channelCount = 1;
    info.bitRate = 96000;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_24;
    uint64_t frameCnt = audioCapturer->GetFrameCount();
    uint32_t framesize = static_cast<uint32_t>((frameCnt * info.channelCount * info.bitWidth) / sizeof(uint8_t));
    uint8_t *buffer;
    audioCapturer->SetCapturerInfo(info);
    audioCapturer->Start();
    audioCapturer->Read(buffer, framesize, false);
    EXPECT_TRUE(sizeof(buffer) > 0);
    audioCapturer->Release();
    delete audioCapturer;
}
} // namespace OHOS
