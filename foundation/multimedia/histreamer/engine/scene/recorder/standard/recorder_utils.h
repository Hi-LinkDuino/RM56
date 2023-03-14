/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_SCENE_RECORDER_STD_REC_UTILS_H
#define HISTREAMER_SCENE_RECORDER_STD_REC_UTILS_H

#include "i_recorder_engine.h"
#include "hst_recorder_param.h"
#include "pipeline/core/error_code.h"
#include "plugin/common/plugin_source_tags.h"
#include "plugin/core/plugin_meta.h"
#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Record {
/**
* @brief Max number of sources supported for multi-source concurrent recording.
*/
constexpr uint8_t VIDEO_SOURCE_MAX_COUNT = 1;
constexpr uint8_t AUDIO_SOURCE_MAX_COUNT = 1;

/**
* @brief Invalid source id, represent that the source set fail result
*/
constexpr int32_t INVALID_SOURCE_ID = -1;

struct SourceIdGenerator {
    static const uint32_t SOURCE_MASK = 0xF00;
    static const uint32_t VIDEO_MASK = 0x100;
    static const uint32_t AUDIO_MASK = 0x200;
    static const uint32_t INDEX_MASK = 0xFF;
    /**
      SourceId is currently represented as int32_t, and internal descripted as source kind mask + index :
      high 20bits(reserverd) + 4bits(source kind mask) + 8bits(index).
      The handle can uniquely identify the recorder source.
    */
    static int32_t GenerateAudioSourceId(uint32_t index)
    {
        return static_cast<int32_t>(AUDIO_MASK + (INDEX_MASK & index));
    }

    static int32_t GenerateVideoSourceId(uint32_t index)
    {
        return static_cast<int32_t>(VIDEO_MASK + (INDEX_MASK & index));
    }

    static int32_t IsAudio(int32_t sourceId)
    {
        return ((sourceId > 0) &&
                ((static_cast<uint32_t>(sourceId) & SOURCE_MASK) == AUDIO_MASK));
    }

    static int32_t IsVideo(int32_t sourceId)
    {
        return ((sourceId > 0) &&
                ((static_cast<uint32_t>(sourceId) & SOURCE_MASK) == VIDEO_MASK));
    }
};

const std::map<OutputFormatType, std::string> g_outputFormatToMimeMap = {
    {OutputFormatType::FORMAT_DEFAULT, MEDIA_MIME_CONTAINER_MP4},
    {OutputFormatType::FORMAT_MPEG_4, MEDIA_MIME_CONTAINER_MP4},
    {OutputFormatType::FORMAT_M4A, MEDIA_MIME_CONTAINER_MP4},
};

int TransErrorCode(ErrorCode errorCode);
Plugin::SrcInputType TransAudioInputType(OHOS::Media::AudioSourceType sourceType);
Plugin::SrcInputType TransVideoInputType(OHOS::Media::VideoSourceType sourceType);
bool TransAudioEncoderFmt(OHOS::Media::AudioCodecFormat aFormat, Plugin::Meta& encoderMeta);
bool TransVideoEncoderFmt(OHOS::Media::VideoCodecFormat vFormat, Plugin::Meta& encoderMeta);
bool GenerateFilePath(const std::string& dirPath, OutputFormatType outputFormatType, std::string& filePath);
bool CastRecorderParam(int32_t sourceId, const RecorderParam& param, HstRecParam& out);
}  // namespace Record
}  // namespace Media
}  // namespace OHOS

#endif  // HISTREAMER_SCENE_RECORDER_STD_REC_UTILS_H
