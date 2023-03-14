/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "source.h"
#include "media_log.h"

namespace OHOS {
namespace Media {
Source::Source(const std::string &uri)
    : uri_(uri),
      sourceType_(SourceType::SOURCE_TYPE_URI)
{}

Source::Source(const std::string &uri, const std::map<std::string, std::string> &header)
    : uri_(uri),
      sourceType_(SourceType::SOURCE_TYPE_URI),
      header_(header)
{}

Source::Source(const std::shared_ptr<StreamSource> &stream, const Format &formats)
    : sourceType_(SourceType::SOURCE_TYPE_STREAM),
      stream_(stream)
{
    format_.CopyFrom(formats);
}

Source::Source(const std::shared_ptr<DataConsumer> &dataConsumer)
    : sourceType_(SourceType::SOURCE_TYPE_STREAM),
      dataConsumer_(dataConsumer)
{
}

SourceType Source::GetSourceType() const
{
    return sourceType_;
}

const std::string &Source::GetSourceUri() const
{
    return uri_;
}

const std::map<std::string, std::string> &Source::GetSourceHeader() const
{
    return header_;
}

const std::shared_ptr<StreamSource> &Source::GetSourceStream() const
{
    return stream_;
}
const Format &Source::GetSourceStreamFormat() const
{
    return format_;
}

const std::shared_ptr<DataConsumer> &Source::GetDataConsumer() const
{
    return dataConsumer_;
}

StreamSource::StreamSource()
#ifndef SURFACE_DISABLED
    : surface_(nullptr),
      curBuffer_(nullptr)
#endif
{}

StreamSource::~StreamSource()
{
    MEDIA_ERR_LOG("[%s,%d] in", __func__, __LINE__);
#ifndef SURFACE_DISABLED
    if (surface_ != nullptr) {
        delete surface_;
    }
#endif
}

#ifndef SURFACE_DISABLED
void StreamSource::SetSurface(Surface* surface)
{
    surface_ = surface;
}

Surface* StreamSource::GetSurface(void)
{
    return surface_;
}
#endif

uint8_t* StreamSource::GetSharedBuffer(size_t& size)
{
#ifndef SURFACE_DISABLED
    if ((surface_ == nullptr) || (curBuffer_ != nullptr)) {
        return nullptr;
    }
    SurfaceBuffer* surfaceBuffer = surface_->RequestBuffer();
    if (surfaceBuffer != nullptr) {
        curBuffer_ = surfaceBuffer;
        size = surface_->GetSize();
        return static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    } else {
        return nullptr;
    }
#else
  return nullptr;
#endif
}

int StreamSource::QueueSharedBuffer(void* buffer, size_t size)
{
#ifndef SURFACE_DISABLED
    if ((surface_ == nullptr) || (buffer == nullptr) || (curBuffer_ == nullptr)) {
        return -1;
    }

    if(buffer != curBuffer_->GetVirAddr()) {
        return -1;
    }
    curBuffer_->SetInt32(0, size);
    if (surface_->FlushBuffer(curBuffer_) != 0) {
        surface_->CancelBuffer(curBuffer_);
        curBuffer_ = nullptr;
        return -1;
    }
    curBuffer_ = nullptr;
#endif
    return 0;
}
}  // namespace Media
}  // namespace OHOS
