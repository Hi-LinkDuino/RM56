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

#ifndef HISTREAMER_FFMPEG_UTILS_H
#define HISTREAMER_FFMPEG_UTILS_H

#include <string>
#include <type_traits>
#include <vector>
#include "plugin/common/plugin_tags.h"
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_video_tags.h"
#include "plugin/common/tag_map.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/pixdesc.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
std::string AVStrError(int errnum);

/**
 * Convert time from ffmpeg to time in HST_TIME_BASE.
 * @param pts ffmpeg time
 * @param base ffmpeg time_base
 * @return time in HST_TIME_BASE
 */
int64_t ConvertTimeFromFFmpeg(int64_t pts, AVRational base);

/**
 * Convert time in HST_TIME_BASE to ffmpeg time.
 * @param time time in HST_TIME_BASE
 * @param base ffmpeg time_base
 * @return time in ffmpeg.
 */
int64_t ConvertTimeToFFmpeg(int64_t timestampUs, AVRational base);

/*
 * Fill in pointers in an AVFrame, aligned by 4 (required by X).
 */
int FillAVPicture(AVFrame* picture, uint8_t* ptr, enum AVPixelFormat pixFmt, int width, int height);

/*
 * Get the size of an picture
 */
int GetAVPictureSize(int pixFmt, int width, int height);

void RemoveDelimiter(char delimiter, std::string& str);

std::string RemoveDelimiter(const char* str, char delimiter);

void ReplaceDelimiter(const std::string& delmiters, char newDelimiter, std::string& str);

std::vector<std::string> SplitString(const char* str, char delimiter);

std::vector<std::string> SplitString(const std::string& string, char delimiter);

AudioSampleFormat ConvFf2PSampleFmt(AVSampleFormat sampleFormat);

AVSampleFormat ConvP2FfSampleFmt(AudioSampleFormat sampleFormat);

AudioChannelLayout ConvertChannelLayoutFromFFmpeg(int channels, uint64_t ffChannelLayout);

uint64_t ConvertChannelLayoutToFFmpeg(AudioChannelLayout channelLayout);

bool FindAvMetaNameByTag(Tag tag, std::string& metaName);

void InsertMediaTag(TagMap& meta, AVDictionaryEntry* tag);

AudioAacProfile ConvAacProfileFromFfmpeg (int32_t ffmpegProfile);

int32_t ConvAacProfileToFfmpeg (AudioAacProfile profile);

VideoPixelFormat ConvertPixelFormatFromFFmpeg(int32_t ffmpegPixelFormat);

AVPixelFormat ConvertPixelFormatToFFmpeg(VideoPixelFormat pixelFormat);

bool IsYuvFormat(AVPixelFormat format);

bool IsRgbFormat(AVPixelFormat format);

VideoH264Profile ConvH264ProfileFromFfmpeg (int32_t ffmpegProfile);

int32_t ConvH264ProfileToFfmpeg(VideoH264Profile profile);
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FFMPEG_UTILS_H
