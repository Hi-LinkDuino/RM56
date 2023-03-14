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

#include "muxer.h"
#include "plugin/interface/muxer_plugin.h"
#include "plugin_wrapper.h"

namespace OHOS {
namespace Media {
namespace Plugin {
Muxer::Muxer (uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<MuxerPlugin> plugin)
    : Base(pkgVer, apiVer, plugin), muxer_(std::move(plugin)) {}

Status Muxer::SetTrackParameter(uint32_t trackId, Plugin::Tag tag, const Plugin::ValueType& value)
{
    return muxer_->SetTrackParameter(trackId, tag, value);
}
Status Muxer::GetTrackParameter(uint32_t trackId, Plugin::Tag tag, Plugin::ValueType& value)
{
    return muxer_->GetTrackParameter(trackId, tag, value);
}
Status Muxer::AddTrack(uint32_t& trackId)
{
    return muxer_->AddTrack(trackId);
}
Status Muxer::SetDataSink(const std::shared_ptr<DataSinkHelper>& dataSink)
{
    return muxer_->SetDataSink(std::make_shared<DataSinkWrapper>(pkgVersion_, dataSink));
}
Status Muxer::WriteHeader()
{
    return muxer_->WriteHeader();
}
Status Muxer::WriteFrame(const std::shared_ptr<Buffer>& buffer)
{
    return muxer_->WriteFrame(buffer);
}
Status Muxer::WriteTrailer()
{
    return muxer_->WriteTrailer();
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS