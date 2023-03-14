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

#define HST_LOG_TAG "StreamSourcePlugin"

#include "stream_source_plugin.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_source_tags.h"
#include "plugin/core/plugin_manager.h"
#include "foundation/log.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace StreamSource {
std::shared_ptr<SourcePlugin> StreamSourcePluginCreator(const std::string& name)
{
    return std::make_shared<StreamSourcePlugin>(name);
}

const Status StreamSourceRegister(const std::shared_ptr<Register>& reg)
{
    SourcePluginDef definition;
    definition.name = "StreamSource";
    definition.description = "Stream source";
    definition.rank = 100; // 100: max rank
    definition.protocol.emplace_back(ProtocolType::STREAM);
    definition.creator = StreamSourcePluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(StreamSource, LicenseType::APACHE_V2, StreamSourceRegister, [] {});


StreamSourcePlugin::StreamSourcePlugin(std::string name)
    : SourcePlugin(std::move(name))
{
    MEDIA_LOG_D("ctor called");
}

StreamSourcePlugin::~StreamSourcePlugin()
{
    MEDIA_LOG_D("dtor called");
}

Status StreamSourcePlugin::Init()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::Deinit()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::Prepare()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::Reset()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::Start()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::Stop()
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::GetParameter(Tag tag, ValueType& value)
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::SetParameter(Tag tag, const ValueType& value)
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::SetCallback(Callback* cb)
{
    MEDIA_LOG_D("IN");
    return Status::OK;
}

Status StreamSourcePlugin::SetSource(std::shared_ptr<MediaSource> source)
{
    stream_ = source->GetDataConsumer();
    FALSE_RETURN_V(stream_ != nullptr, Status::ERROR_INVALID_PARAMETER);
    return Status::OK;
}

std::shared_ptr<Buffer> StreamSourcePlugin::WrapDataBuffer(const std::shared_ptr<DataBuffer>& dataBuffer)
{
    std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>();
    auto deleter = [this](uint8_t* ptr) {  FALSE_LOG(stream_->QueueEmptyBuffer(ptr));  };
    std::shared_ptr<uint8_t> address = std::shared_ptr<uint8_t>(dataBuffer->GetAddress(), deleter);
    buffer->WrapMemoryPtr(address, dataBuffer->GetCapacity(), dataBuffer->GetSize());
    if (dataBuffer->IsEos()) {
        buffer->flag |= BUFFER_FLAG_EOS;
    }
    return buffer;
}

// stream source is non-seekable
// so this read is called by MediaSourceFilter::ReadLoop
// ReadLoop always not provider buffer, so no need copy here, and no need to care about buffer count/length.
// We always process buffer count/length in DemuxerFitler's DataPacker.
Status StreamSourcePlugin::Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen)
{
    std::shared_ptr<DataBuffer> dataBuffer;
    FALSE_RETURN_V(stream_->GetDataBuffer(dataBuffer), Status::ERROR_TIMED_OUT);
    buffer = WrapDataBuffer(dataBuffer);
    return Status::OK;
}

Status StreamSourcePlugin::GetSize(size_t& size)
{
    MEDIA_LOG_D("IN");
    size = 0;
    return Status::ERROR_WRONG_STATE;
}

Seekable StreamSourcePlugin::GetSeekable()
{
    MEDIA_LOG_D("IN");
    return seekable_;
}

Status StreamSourcePlugin::SeekTo(uint64_t offset)
{
    MEDIA_LOG_D("IN");
    return Status::ERROR_UNIMPLEMENTED;
}
} // namespace StreamSource
} // namespace Plugin
} // namespace Media
} // namespace OHOS
