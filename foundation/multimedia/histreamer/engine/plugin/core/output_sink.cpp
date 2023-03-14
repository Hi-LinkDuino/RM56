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

#include "output_sink.h"
namespace OHOS {
namespace Media {
namespace Plugin {
OutputSink::OutputSink(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<OutputSinkPlugin> plugin)
    : Base(pkgVer, apiVer, plugin), outputSink_(std::move(plugin)) {}
Status OutputSink::SetSink(const MediaSink &sink)
{
    return outputSink_->SetSink(sink);
}

Seekable OutputSink::GetSeekable()
{
    return outputSink_->GetSeekable();
}

Status OutputSink::SeekTo(uint64_t offset)
{
    return outputSink_->SeekTo(offset);
}

Status OutputSink::Write(const std::shared_ptr<Buffer>& buffer)
{
    return outputSink_->Write(buffer);
}

Status OutputSink::Flush()
{
    return outputSink_->Flush();
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS