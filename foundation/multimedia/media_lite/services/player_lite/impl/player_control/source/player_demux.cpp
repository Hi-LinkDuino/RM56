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
#include "player_demux.h"
#include <memory>
#include "format_type.h"
#include "media_log.h"
extern "C"
{
#include "format_interface.h"
}

using namespace std;

namespace OHOS {
namespace Media {
#define READ_FRAME_TIMEOUT 20

#define CHECK_NULL_RETURN(value, printfString) \
do { \
    if (value == nullptr) { \
        MEDIA_ERR_LOG(" %s ", printfString ? printfString : " "); \
        return -1; \
    } \
} while (0)

#define CHECK_FAILED_RETURN(value, target, ret, printfString) \
do { \
    if (value != target) { \
        MEDIA_ERR_LOG(" %s ", printfString ? printfString : " "); \
        return ret; \
    } \
} while (0)

PlayerDemuxer::PlayerDemuxer()
    : inited_(false),
      prepared_(false),
      started_(false),
      demuxer_(nullptr),
      sourceType_(SOURCE_TYPE_BUT),
      fd_(-1)
{
    stream_.handle = nullptr;
    stream_.ReadData = nullptr;
    stream_.GetReadableSize = nullptr;
    formatListener_.OnError = nullptr;
    formatListener_.OnInfo = nullptr;
    formatListener_.privateDataHandle = nullptr;
    callBack_.onEventCallback = nullptr;
    callBack_.priv = nullptr;
}

PlayerDemuxer::~PlayerDemuxer()
{
    if (demuxer_ != nullptr) {
        FormatDemuxerDestroy(demuxer_);
        demuxer_ = nullptr;
    }
}

int32_t PlayerDemuxer::Init(void)
{
    formatListener_.OnError = ErrorEventPro;
    formatListener_.OnInfo = InfoEventPro;
    FormatInit();
    inited_ = true;
    return 0;
}

int32_t PlayerDemuxer::ErrorEventPro(void *handle, int32_t errorType, int32_t errorCode)
{
    return 0;
}

int32_t PlayerDemuxer::InfoEventPro(void *handle, int32_t type, int32_t extra)
{
    return 0;
}

int32_t PlayerDemuxer::GetFormatDemuxer(void)
{
    int ret;
    FormatSource source;

    source.type = sourceType_;
    if (sourceType_ == SOURCE_TYPE_FD) {
        source.fd = fd_;
    } else if (sourceType_ == SOURCE_TYPE_URI) {
        if (strncpy_s(source.url, URL_LEN, filePath_.c_str(), filePath_.size()) != 0) {
            return -1;
        }
    } else {
        source.stream = &stream_;
    }
    ret = FormatDemuxerCreate(&source, &demuxer_);
    if (ret != 0 || demuxer_ == nullptr) {
        MEDIA_ERR_LOG("FormatDemuxerCreate failed");
        return -1;
    }
    return 0;
}

int32_t PlayerDemuxer::SetSource(int fd)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    fd_ = fd;
    sourceType_ = SOURCE_TYPE_FD;
    CHECK_FAILED_RETURN(GetFormatDemuxer(), 0, -1, " ");
    return 0;
}

int32_t PlayerDemuxer::SetSource(const char *url)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    filePath_ = std::string(url);
    sourceType_ = SOURCE_TYPE_URI;
    CHECK_FAILED_RETURN(GetFormatDemuxer(), 0, -1, " ");
    return 0;
}

int32_t PlayerDemuxer::SetSource(BufferStream &stream)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    stream_ = stream;
    sourceType_ = SOURCE_TYPE_STREAM;
    CHECK_FAILED_RETURN(GetFormatDemuxer(), 0, -1, " ");
    return 0;
}

int32_t PlayerDemuxer::SetCallBack(PlayEventCallback &callBack)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    callBack_ = callBack;
    return 0;
}

int32_t PlayerDemuxer::Prepare(void)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    CHECK_FAILED_RETURN(FormatDemuxerSetCallBack(demuxer_, &formatListener_), 0, -1, "set callback failed");
    CHECK_FAILED_RETURN(FormatDemuxerPrepare(demuxer_), 0, -1, "prepare failed");
    prepared_ = true;
    return 0;
}

int32_t PlayerDemuxer::GetFileInfo(FormatFileInfo &fileInfo)
{
    uint32_t i;
    FileInfo info;
    int programId = -1;
    int trackId[0x2] = {-1, -1};
    int trackNum = 0x2;
    ProgramInfo *programInfo = nullptr;
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    CHECK_FAILED_RETURN(FormatDemuxerGetFileInfo(demuxer_, &info), 0, -1, "");
    CHECK_FAILED_RETURN(FormatDemuxerGetSelectedTrack(demuxer_, &programId, trackId, &trackNum), 0, -1, "");

    if (programId == -1 || trackNum == 0 || trackId[0] == -1) {
        MEDIA_ERR_LOG("FormatDemuxerGetSelectedTrack failed");
        return -1;
    }

    fileInfo.s64FileSize = -1;
    fileInfo.s64StartTime = 0;
    fileInfo.u32Bitrate = info.bitrate;
    if (info.programNum == 1) {
        programInfo = &info.programInfo[0];
    } else {
        for (i = 0; i < info.programNum; i++) {
            if (info.programInfo[i].programId == programId) {
                programInfo = &info.programInfo[i];
                break;
            }
        }
    }
    if (programInfo == nullptr) {
        MEDIA_ERR_LOG("can not find the program");
        return -1;
    }
    fileInfo.s64Duration = programInfo->durationMs;
    fileInfo.s32UsedVideoStreamIndex = -1;
    fileInfo.s32UsedAudioStreamIndex = -1;
    for (i = 0; i < programInfo->trackNum; i++) {
        if (programInfo->track[i].trackId == trackId[0] || programInfo->track[i].trackId == trackId[1]) {
            if (programInfo->track[i].trackType == TRACK_TYPE_VIDEO) {
                fileInfo.s32UsedVideoStreamIndex = programInfo->track[i].trackId;
                fileInfo.u32Width = programInfo->track[i].vidTrack.width;
                fileInfo.u32Height = programInfo->track[i].vidTrack.height;
                fileInfo.enVideoType = programInfo->track[i].vidTrack.format;
            } else if (programInfo->track[i].trackType == TRACK_TYPE_AUDIO) {
                fileInfo.s32UsedAudioStreamIndex = programInfo->track[i].trackId;
                fileInfo.u32AudioChannelCnt = programInfo->track[i].audTrack.channels;
                fileInfo.u32SampleRate = programInfo->track[i].audTrack.sampleRate;
                fileInfo.enAudioType = programInfo->track[i].audTrack.format;
            }
        }
    }
    int index = 0;
    uint32_t j;
    for (i = 0; i < info.programNum; i++) {
        programInfo = &info.programInfo[i];
        for (j = 0; j < programInfo->trackNum; j++) {
            if (programInfo->track[j].trackType == TRACK_TYPE_VIDEO && index < HI_DEMUXER_RESOLUTION_CNT) {
                fileInfo.stSteamResolution[index].s32VideoStreamIndex = programInfo->track[j].trackId;
                fileInfo.stSteamResolution[index].u32Width = programInfo->track[j].vidTrack.width;
                fileInfo.stSteamResolution[index].u32Height = programInfo->track[j].vidTrack.height;
                fileInfo.stSteamResolution[index].enVideoType = programInfo->track[j].vidTrack.format;
                index++;
            }
        }
    }
    for (; index < HI_DEMUXER_RESOLUTION_CNT; index++) {
        fileInfo.stSteamResolution[index].s32VideoStreamIndex = -1;
        fileInfo.stSteamResolution[index].u32Width = 0;
        fileInfo.stSteamResolution[index].u32Height = 0;
    }

    fileInfo.formatName = info.formatName;
    return 0;
}

int32_t PlayerDemuxer::SelectTrack(int32_t programId, int32_t trackId)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return FormatDemuxerSelectTrack(demuxer_, programId, trackId);
}

int32_t PlayerDemuxer::UnselectTrack(int32_t programId, int32_t trackId)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return FormatDemuxerUnselectTrack(demuxer_, programId, trackId);
}

int32_t PlayerDemuxer::GetSelectedTrack(int32_t &programId, int32_t trackId[], int32_t &nums)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return FormatDemuxerGetSelectedTrack(demuxer_, &programId, trackId, &nums);
}

int32_t PlayerDemuxer::Start()
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    CHECK_FAILED_RETURN(FormatDemuxerStart(demuxer_), 0, -1, "");
    started_ = true;
    return 0;
}

int32_t PlayerDemuxer::ReadFrame(FormatFrame &frame)
{
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    return FormatDemuxerReadFrame(demuxer_, &frame, READ_FRAME_TIMEOUT);
}

int32_t PlayerDemuxer::FreeFrame(FormatFrame &frame)
{
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    return FormatDemuxerFreeFrame(demuxer_, &frame);
}

int32_t PlayerDemuxer::Seek(int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode)
{
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    return FormatDemuxerSeek(demuxer_, streamIndex, timeStampUs, mode);
}

int32_t PlayerDemuxer::Stop()
{
    if (demuxer_ == nullptr) {
        return 0;
    }
    return FormatDemuxerStop(demuxer_);
}

int32_t PlayerDemuxer::SetParam(int32_t trackId, const ParameterItem *metaData, int32_t metaDataCnt)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return FormatDemuxerSetParameter(demuxer_, trackId, metaData, metaDataCnt);
}

int32_t PlayerDemuxer::GetParam(int32_t trackId, ParameterItem &metaData)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return FormatDemuxerGetParameter(demuxer_, trackId, &metaData);
}
}  // namespace Media
}  // namespace OHOS
