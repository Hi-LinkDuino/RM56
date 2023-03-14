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

#include "media_source.h"

namespace OHOS {
namespace Media {
namespace Plugin {
MediaSource::MediaSource(std::string uri)
    : uri_(std::move(uri)), type_(SourceType::SOURCE_TYPE_URI)
{
}

MediaSource::MediaSource(std::shared_ptr<DataConsumer> dataStream)
    : type_(SourceType::SOURCE_TYPE_STREAM), dataConsumer_(std::move(dataStream))
{
}

MediaSource::MediaSource(std::string uri, std::map<std::string, std::string> header)
    : uri_(std::move(uri)), header_(std::move(header))
{
}

SourceType MediaSource::GetSourceType() const
{
    return type_;
}

const std::string &MediaSource::GetSourceUri() const
{
    return uri_;
}

const std::map<std::string, std::string> &MediaSource::GetSourceHeader() const
{
    return header_;
}

std::shared_ptr<DataConsumer> MediaSource::GetDataConsumer() const
{
    return dataConsumer_;
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS

