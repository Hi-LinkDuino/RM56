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

#include "player_source.h"
#include "player_demux.h"
#include "format_type.h"
#include "hi_liteplayer.h"
#include <memory>
#include <vector>
#include "media_log.h"

using namespace std;

namespace OHOS {
namespace Media {
#define CHECK_FAILED_RETURN(value, target, ret, printfString) \
do { \
    if (value != target) { \
        MEDIA_ERR_LOG(" %s ", printfString ? printfString : " "); \
        return ret; \
    } \
} while (0)

PlayerSource::PlayerSource()
    : inited_(false),
      prepared_(false),
      started_(false),
      demuxer_(nullptr)
{
}

PlayerSource::~PlayerSource()
{
    inited_ = false;
}

int32_t PlayerSource::Init()
{
    CHECK_FAILED_RETURN(inited_, false, 0, "have inited before");
    demuxer_ = std::make_shared<PlayerDemuxer>();
    if (demuxer_ == nullptr) {
        MEDIA_ERR_LOG("new PlayerDemuxer failed");
        return -1;
    }
    if (demuxer_->Init() != 0) {
        MEDIA_ERR_LOG("demuxer init failed");
        return -1;
    }
    inited_ = true;
    return 0;
}

int32_t PlayerSource::SetSource(int32_t fd)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->SetSource(fd);
}


int32_t PlayerSource::SetSource(const char *url)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->SetSource(url);
}

int32_t PlayerSource::SetSource(BufferStream &stream)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->SetSource(stream);
}

int32_t PlayerSource::SetCallBack(PlayEventCallback &callBack)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->SetCallBack(callBack);
}

int32_t PlayerSource::Prepare(void)
{
    int ret;
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    ret = demuxer_->Prepare();
    if (ret == 0) {
        prepared_ = true;
    }
    return ret;
}

int32_t PlayerSource::GetFileInfo(FormatFileInfo &fileInfo)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return demuxer_->GetFileInfo(fileInfo);
}

int32_t PlayerSource::SelectTrack(int32_t programId, int32_t trackId)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return demuxer_->SelectTrack(programId, trackId);
}

int32_t PlayerSource::UnselectTrack(int32_t programId, int32_t trackId)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return demuxer_->UnselectTrack(programId, trackId);
}

int32_t PlayerSource::GetSelectedTrack(int32_t &programId, int32_t trackId[], int32_t &nums)
{
    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    return demuxer_->GetSelectedTrack(programId, trackId, nums);
}

int32_t PlayerSource::Start()
{
    int ret;

    CHECK_FAILED_RETURN(prepared_, true, -1, "not prepared");
    ret = demuxer_->Start();
    if (ret == 0) {
        started_ = true;
    }
    return ret;
}

int32_t PlayerSource::ReadFrame(FormatFrame &frame)
{
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    return demuxer_->ReadFrame(frame);
}

int32_t PlayerSource::FreeFrame(FormatFrame &frame)
{
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    return demuxer_->FreeFrame(frame);
}

int32_t PlayerSource::Seek(int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode)
{
    CHECK_FAILED_RETURN(started_, true, -1, "not started");
    return demuxer_->Seek(streamIndex, timeStampUs, mode);
}

int32_t PlayerSource::Stop()
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->Stop();
}

int32_t PlayerSource::SetParam(int32_t trackId, const ParameterItem *metaData, int32_t metaDataCnt)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->SetParam(trackId, metaData, metaDataCnt);
}

int32_t PlayerSource::GetParam(int32_t trackId, ParameterItem &metaData)
{
    CHECK_FAILED_RETURN(inited_, true, -1, "not inited");
    return demuxer_->GetParam(trackId, metaData);
}
}  // namespace Media
}  // namespace OHOS

