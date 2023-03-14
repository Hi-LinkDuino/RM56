/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "media_sink.h"
namespace OHOS {
namespace Media {
namespace Plugin {
MediaSink::MediaSink(ProtocolType protocolType) : protocolType_(std::move(protocolType))
{
}
std::string MediaSink::GetPath() const
{
    return path_;
}

int32_t MediaSink::GetFd() const
{
    return fd_;
}

void MediaSink::SetFd(const int32_t fd)
{
    fd_ = fd;
}
void MediaSink::SetPath(const std::string& path)
{
    path_ = path;
}

ProtocolType MediaSink::GetProtocolType() const
{
    return protocolType_;
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS
