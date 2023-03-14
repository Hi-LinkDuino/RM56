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

#ifndef HISTREAMER_FOUNDATION_CONSTANTS_H
#define HISTREAMER_FOUNDATION_CONSTANTS_H

#include <string>

namespace OHOS {
namespace Media {

constexpr size_t DEFAULT_QUEUE_SIZE = 10;
constexpr int DEFAULT_FRAME_SIZE = 2 * 1024; // 2kb
constexpr size_t DEFAULT_POOL_SIZE = 10;
constexpr uint32_t MAX_PORT_NUMBER = 1;  // max in/out port in filter
constexpr uint32_t MAX_ROUTE_NUMBER = 1; // max route in filter
extern const char* const PORT_NAME_DEFAULT;

// audio mime
extern const char* const MEDIA_MIME_AUDIO_MPEG;
extern const char* const MEDIA_MIME_AUDIO_FLAC;
extern const char* const MEDIA_MIME_AUDIO_RAW;
extern const char* const MEDIA_MIME_AUDIO_APE;
extern const char* const MEDIA_MIME_AUDIO_WAV;
extern const char* const MEDIA_MIME_AUDIO_AAC;
extern const char* const MEDIA_MIME_AUDIO_AAC_ADTS;
extern const char* const MEDIA_MIME_AUDIO_AAC_LATM;
extern const char* const MEDIA_MIME_AUDIO_VORBIS;
extern const char* const MEDIA_MIME_AUDIO_OPUS;
extern const char* const MEDIA_MIME_AUDIO_AC3;
extern const char* const MEDIA_MIME_AUDIO_EAC3;
extern const char* const MEDIA_MIME_AUDIO_EAC3_JOC;
extern const char* const MEDIA_MIME_AUDIO_AC4;
extern const char* const MEDIA_MIME_AUDIO_WMA;
extern const char* const MEDIA_MIME_AUDIO_AMR_NB;
extern const char* const MEDIA_MIME_AUDIO_AMR_WB;
extern const char* const MEDIA_MIME_AUDIO_AVS3DA;

// video mime
extern const char* const MEDIA_MIME_VIDEO_RAW;
extern const char* const MEDIA_MIME_VIDEO_H264;
extern const char* const MEDIA_MIME_VIDEO_MPEG4;

// container mime
extern const char* const MEDIA_MIME_CONTAINER_MP4;

bool IsAudioMime(const std::string& mime);
bool IsVideoMime(const std::string& mime);
bool IsRawAudio(const std::string& mime);
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_CONSTANTS_H
