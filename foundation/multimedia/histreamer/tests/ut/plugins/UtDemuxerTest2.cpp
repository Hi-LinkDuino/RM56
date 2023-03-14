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

#include "UtDemuxerTest2.h"

using namespace OHOS::Media::Plugin;

static int Sniff(const std::string& name, std::shared_ptr<DataSource> dataSource)
{
    return 0;
}

static Status RegisterPlugins(const std::shared_ptr<Register>& reg)
{
    DemuxerPluginDef regInfo;
    regInfo.name = "UtDemuxerTest2";
    regInfo.description = "unit test demuxer test2";
    regInfo.rank = 100; // 100
    regInfo.creator = [](const std::string& name) -> std::shared_ptr<DemuxerPlugin> {
        return std::make_shared<UtDemuxerTest2>(name);
    };
    regInfo.sniffer = Sniff;
    return reg->AddPlugin(regInfo);
}

PLUGIN_DEFINITION(UtDemuxerTest2, LicenseType::LGPL, RegisterPlugins, [] {});

Status UtDemuxerTest2::SetDataSource(const std::shared_ptr<DataSource> &source)
{
    return Status::OK;
}

Status UtDemuxerTest2::GetMediaInfo(MediaInfo &mediaInfo)
{
    return Status::OK;
}

size_t UtDemuxerTest2::GetTrackCount()
{
    return 0;
}

Status UtDemuxerTest2::SelectTrack(int32_t trackId)
{
    return Status::OK;
}

Status UtDemuxerTest2::UnselectTrack(int32_t trackId)
{
    return Status::OK;
}

Status UtDemuxerTest2::GetSelectedTracks(std::vector<int32_t> &trackIds)
{
    return Status::OK;
}

Status UtDemuxerTest2::ReadFrame(Buffer &buffer, int32_t timeOutMs)
{
    return Status::OK;
}

Status UtDemuxerTest2::SeekTo(int32_t trackId, int64_t timeStampUs, SeekMode mode)
{
    return Status::OK;
}

std::shared_ptr<Allocator> UtDemuxerTest2::GetAllocator()
{
    return std::shared_ptr<Allocator>();
}

Status UtDemuxerTest2::SetCallback(Callback* cb)
{
    return Status::OK;
}
