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

#include "constants.h"
#include <string>

namespace OHOS {
namespace Media {
const char* const PORT_NAME_DEFAULT = "default";

const char* const MEDIA_MIME_AUDIO_MPEG = "audio/mpeg";
const char* const MEDIA_MIME_AUDIO_FLAC = "audio/flac";
const char* const MEDIA_MIME_AUDIO_RAW = "audio/raw";
const char* const MEDIA_MIME_AUDIO_APE = "audio/ape";
const char* const MEDIA_MIME_AUDIO_WAV = "audio/wav";
const char* const MEDIA_MIME_AUDIO_AAC = "audio/aac";
const char* const MEDIA_MIME_AUDIO_AAC_ADTS = "audio/aac-adts";
const char* const MEDIA_MIME_AUDIO_AAC_LATM = "audio/aac-latm";
const char* const MEDIA_MIME_AUDIO_VORBIS = "audio/vorbis";
const char* const MEDIA_MIME_AUDIO_OPUS = "audio/opus";
const char* const MEDIA_MIME_AUDIO_AC3 = "audio/ac3";
const char* const MEDIA_MIME_AUDIO_EAC3 = "audio/eac3";
const char* const MEDIA_MIME_AUDIO_EAC3_JOC = "audio/eac3-joc";
const char* const MEDIA_MIME_AUDIO_AC4 = "audio/ac4";
const char* const MEDIA_MIME_AUDIO_WMA = "audio/x-ms-wma";
const char* const MEDIA_MIME_AUDIO_AMR_NB = "audio/3gpp";
const char* const MEDIA_MIME_AUDIO_AMR_WB = "audio/amr-wb";
const char* const MEDIA_MIME_AUDIO_AVS3DA = "audio/avs-3da";

const char* const MEDIA_MIME_VIDEO_RAW = "video/raw";
const char* const MEDIA_MIME_VIDEO_H264 = "video/h264";
const char* const MEDIA_MIME_VIDEO_MPEG4 = "video/mpeg4";

const char* const MEDIA_MIME_CONTAINER_MP4 = "video/mp4";

bool IsAudioMime(const std::string& mime)
{
    return mime.find("audio/") == 0;
}
bool IsVideoMime(const std::string& mime)
{
    return mime.find("video/") == 0;
}

bool IsRawAudio(const std::string& mime)
{
    return mime == MEDIA_MIME_AUDIO_RAW;
}
} // namespace Media
} // namespace OHOS
