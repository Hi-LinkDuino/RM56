/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_AUDIO_ENCODER_INCLUDE_AUDIO_ENCODER_H_
#define FRAMEWORKS_AUDIO_ENCODER_INCLUDE_AUDIO_ENCODER_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <time.h>
#include <vector>

#include "codec_interface.h"
#include "format.h"
#include "media_errors.h"
#include "media_info.h"

namespace OHOS {
namespace Audio {
constexpr int32_t AUDIO_ENC_PARAM_NUM = 8;
/* count of audio frame in Buffer */
constexpr uint32_t AUDIO_FRAME_NUM_IN_BUF = 30;
/* sample per frame for all encoder(aacplus:2048) */
constexpr uint32_t AUDIO_POINT_NUM = 1024;

struct AudioEncodeConfig {
    AudioCodecFormat audioFormat;
    uint32_t bitRate = 0;
    uint32_t sampleRate = 0;
    uint32_t channelCount = 0;
    AudioBitWidth bitWidth = BIT_WIDTH_16;
};

struct AudioStream {
    uint8_t *buffer = nullptr;    /* the virtual address of stream */
    uint32_t bufferLen = 0;   /* stream length, by bytes */
    int64_t timeStamp = 0;
};

class AudioEncoder {
public:
    AudioEncoder();
    ~AudioEncoder();

    /**
    * Initailizes the audio source according to a specific configuration.
    *
    * @param config a configuration of audio source.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t Initialize(const AudioEncodeConfig &config);

    /**
    * Binds audio source to a specific device.
    *
    * @param deviceId specifies the identity of device to bind.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t BindSource(uint32_t deviceId);

    /**
    * Gets mute status of current encoder, dummy implemented currently.
    *
    * @param muted holds mute status if success.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t GetMute(bool &muted);

    /**
    * Sets mute status of current encoder, dummy implemented currently.
    *
    * @param muted mute status to set.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t SetMute(bool muted);

    /**
    * Starts audio source.
    *
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t Start();

    /**
    *
    * Reads stream from a audio source.
    *
    * @param stream source stream to read from.
    * @param isBlockingRead reading mode.
    * @return Returns size of data actually read.
    */
    int32_t ReadStream(AudioStream &stream, bool isBlockingRead);

    /**
    * Stops audio source.
    *
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t Stop();

    /**
    * release.
    */
    int32_t Release();

private:
    int32_t InitAudioEncoderAttr(const AudioEncodeConfig &config);

private:
    bool initialized_;
    CODEC_HANDLETYPE encHandle_;
    CodecType domainKind_ = AUDIO_ENCODER;
    AvCodecMime codecMime_ = MEDIA_MIMETYPE_AUDIO_AAC;
    Profile profile_ = INVALID_PROFILE;
    AudioSampleRate sampleRate_ = AUD_SAMPLE_RATE_INVALID;
    uint32_t bitRate_ = 0;
    AudioSoundMode soundMode_ = AUD_SOUND_MODE_INVALID;
    uint32_t ptNumPerFrm_ = AUDIO_POINT_NUM;
    uint32_t bufSize_ = AUDIO_FRAME_NUM_IN_BUF;
    Param encAttr_[AUDIO_ENC_PARAM_NUM];
    bool started_ = false;
};
}  // namespace Audio
}  // namespace OHOS

#endif  // FRAMEWORKS_AUDIO_ENCODER_INCLUDE_AUDIO_ENCODER_H_
