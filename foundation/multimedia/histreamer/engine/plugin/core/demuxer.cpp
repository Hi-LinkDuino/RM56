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

#include "demuxer.h"
#include "interface/demuxer_plugin.h"
#include "interface/plugin_definition.h"

#include "plugin_wrapper.h"

using namespace OHOS::Media::Plugin;

Demuxer::Demuxer(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<DemuxerPlugin> plugin)
    : Base(pkgVer, apiVer, plugin), demuxer_(std::move(plugin))
{
}

Status Demuxer::SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode)
{
    return demuxer_->SeekTo(trackId, hstTime, mode);
}

Status Demuxer::SetDataSource(const std::shared_ptr<DataSourceHelper>& source)
{
    return demuxer_->SetDataSource(std::make_shared<DataSourceWrapper>(pkgVersion_, source));
}

Status Demuxer::GetMediaInfo(MediaInfoHelper& mediaInfo)
{
    MediaInfo info;
    demuxer_->GetMediaInfo(info);
    ConvertToMediaInfoHelper(pkgVersion_, info, mediaInfo);
    return Status::OK;
}

Status Demuxer::ReadFrame(Buffer& info, int32_t timeOutMs)
{
    return demuxer_->ReadFrame(info, timeOutMs);
}
