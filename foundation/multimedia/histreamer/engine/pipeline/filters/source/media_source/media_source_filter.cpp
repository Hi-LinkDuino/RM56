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

#define HST_LOG_TAG "MediaSourceFilter"

#include "media_source_filter.h"
#include "common/plugin_utils.h"
#include "compatible_check.h"
#include "factory/filter_factory.h"
#include "filters/common/dump_buffer.h"
#include "pipeline/core/type_define.h"
#include "plugin/interface/source_plugin.h"
#include "plugin/core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
using namespace Plugin;

static constexpr size_t DEFAULT_READ_SIZE = 4096;  // default read size for push mode

static AutoRegisterFilter<MediaSourceFilter> g_registerFilterHelper("builtin.player.mediasource");

static std::map<std::string, ProtocolType> g_protocolStringToType = {
    {"http", ProtocolType::HTTP},
    {"https", ProtocolType::HTTPS},
    {"file", ProtocolType::FILE},
    {"stream", ProtocolType::STREAM},
    {"fd", ProtocolType::FD}
};

MediaSourceFilter::MediaSourceFilter(const std::string& name)
    : FilterBase(name),
      taskPtr_(nullptr),
      protocol_(),
      uri_(),
      seekable_(Seekable::INVALID),
      position_(0),
      bufferSize_(DEFAULT_FRAME_SIZE),
      plugin_(nullptr),
      pluginAllocator_(nullptr)
{
    filterType_ = FilterType::MEDIA_SOURCE;
    MEDIA_LOG_D("ctor called");
}

MediaSourceFilter::~MediaSourceFilter()
{
    MEDIA_LOG_D("dtor called");
    if (plugin_) {
        plugin_->Deinit();
    }
    if (taskPtr_) {
        taskPtr_->Stop();
    }
}

void MediaSourceFilter::ClearData()
{
    protocol_.clear();
    uri_.clear();
    seekable_ = Seekable::INVALID;
    position_ = 0;
    mediaOffset_ = 0;
    isPluginReady_ = false;
    isAboveWaterline_ = false;
}

ErrorCode MediaSourceFilter::SetSource(const std::shared_ptr<MediaSource>& source)
{
    MEDIA_LOG_I("SetSource entered.");
    if (source == nullptr) {
        MEDIA_LOG_E("Invalid source");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    ClearData();
    ErrorCode err = FindPlugin(source);
    if (err != ErrorCode::SUCCESS) {
        return err;
    }
    err = InitPlugin(source);
    if (err != ErrorCode::SUCCESS) {
        return err;
    }
    ActivateMode();
    return DoNegotiate(source);
}

ErrorCode MediaSourceFilter::InitPlugin(const std::shared_ptr<MediaSource>& source)
{
    MEDIA_LOG_D("IN");
    ErrorCode err = TranslatePluginStatus(plugin_->Init());
    if (err != ErrorCode::SUCCESS) {
        return err;
    }
    plugin_->SetCallback(this);
    pluginAllocator_ = plugin_->GetAllocator();
    return TranslatePluginStatus(plugin_->SetSource(source));
}

ErrorCode MediaSourceFilter::SetBufferSize(size_t size)
{
    MEDIA_LOG_I("SetBufferSize, size: " PUBLIC_LOG_ZU, size);
    bufferSize_ = size;
    return ErrorCode::SUCCESS;
}

std::vector<WorkMode> MediaSourceFilter::GetWorkModes()
{
    MEDIA_LOG_DD("IN, isSeekable_: " PUBLIC_LOG_D32, static_cast<int32_t>(seekable_));
    if (seekable_ == Seekable::SEEKABLE) {
        return {WorkMode::PUSH, WorkMode::PULL};
    } else {
        return {WorkMode::PUSH};
    }
}

ErrorCode MediaSourceFilter::Prepare()
{
    MEDIA_LOG_I("Prepare entered.");
    if (plugin_ == nullptr) {
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    Status ret = plugin_->Prepare();
    if (ret == Status::OK) {
        MEDIA_LOG_D("media source send EVENT_READY");
        FilterBase::OnEvent(Event{name_, EventType::EVENT_READY, {}});
    } else if (ret == Status::ERROR_DELAY_READY) {
        isPluginReady_ = true;
        if (isAboveWaterline_ && isPluginReady_) {
            MEDIA_LOG_D("media source send EVENT_READY");
            FilterBase::OnEvent(Event{name_, EventType::EVENT_READY, {}});
            isPluginReady_= false;
            isAboveWaterline_= false;
        }
    }
    auto err = TranslatePluginStatus(ret);
    return err;
}

ErrorCode MediaSourceFilter::Start()
{
    MEDIA_LOG_I("Start entered.");
    return plugin_ ? TranslatePluginStatus(plugin_->Start()) : ErrorCode::ERROR_INVALID_OPERATION;
}

ErrorCode MediaSourceFilter::PullData(const std::string& outPort, uint64_t offset, size_t size, AVBufferPtr& data)
{
    MEDIA_LOG_DD("IN, offset: " PUBLIC_LOG_U64 ", size: " PUBLIC_LOG_ZU ", outPort: " PUBLIC_LOG_S,
                 offset, size, outPort.c_str());
    if (!plugin_) {
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    ErrorCode err;
    auto readSize = size;
    if (seekable_ == Seekable::SEEKABLE) {
        size_t totalSize = 0;
        if ((plugin_->GetSize(totalSize) == Status::OK) && (totalSize != 0)) {
            if (offset >= totalSize) {
                MEDIA_LOG_W("offset: " PUBLIC_LOG_U64 " is larger than totalSize: " PUBLIC_LOG_ZU,
                            offset, totalSize);
                return ErrorCode::END_OF_STREAM;
            }
            if ((offset + readSize) > totalSize) {
                readSize = totalSize - offset;
            }
            if (data != nullptr && data->GetMemory() != nullptr) {
                auto realSize = data->GetMemory()->GetCapacity();
                readSize = (readSize > realSize) ? realSize : readSize;
            }
            MEDIA_LOG_DD("totalSize_: " PUBLIC_LOG_ZU, totalSize);
        }
        if (position_ != offset) {
            err = TranslatePluginStatus(plugin_->SeekTo(offset));
            if (err != ErrorCode::SUCCESS) {
                MEDIA_LOG_E("Seek to " PUBLIC_LOG_U64 " fail", offset);
                return err;
            }
            position_ = offset;
        }
    }
    err = TranslatePluginStatus(plugin_->Read(data, readSize));
    DUMP_BUFFER2LOG("Source PullData", data, offset);
    if (err == ErrorCode::SUCCESS) {
        position_ += data->GetMemory()->GetSize();
    }
    return err;
}

ErrorCode MediaSourceFilter::Stop()
{
    MEDIA_LOG_I("Stop entered.");
    ErrorCode ret = ErrorCode::SUCCESS;
    if (plugin_) {
        ret = TranslatePluginStatus(plugin_->Stop()); // stop plugin first, then stop thread
    }
    if (taskPtr_) {
        taskPtr_->Stop();
    }
    mediaOffset_ = 0;
    seekable_ = Seekable::INVALID;
    protocol_.clear();
    uri_.clear();
    return ret;
}

void MediaSourceFilter::FlushStart()
{
    MEDIA_LOG_I("FlushStart entered.");
}

void MediaSourceFilter::FlushEnd()
{
    MEDIA_LOG_I("FlushEnd entered.");
}

void MediaSourceFilter::InitPorts()
{
    MEDIA_LOG_D("IN");
    auto outPort = std::make_shared<OutPort>(this);
    outPorts_.push_back(outPort);
}

void MediaSourceFilter::ActivateMode()
{
    MEDIA_LOG_D("IN");
    do {
        if (plugin_) {
            seekable_ = plugin_->GetSeekable();
        }
        OSAL::SleepFor(10); // 10
    } while (seekable_ == Seekable::INVALID);
    FALSE_LOG(seekable_ != Seekable::INVALID);
    if (seekable_ == Seekable::UNSEEKABLE) {
        if (taskPtr_ == nullptr) {
            taskPtr_ = std::make_shared<OSAL::Task>("DataReader");
            taskPtr_->RegisterHandler(std::bind(&MediaSourceFilter::ReadLoop, this));
        }
        taskPtr_->Start();
    }
}

Plugin::Seekable MediaSourceFilter::GetSeekable() const
{
    return seekable_;
}

ErrorCode MediaSourceFilter::DoNegotiate(const std::shared_ptr<MediaSource>& source)
{
    MEDIA_LOG_D("IN");
    SourceType sourceType = source->GetSourceType();
    if (sourceType == SourceType::SOURCE_TYPE_URI) {
        std::shared_ptr<Plugin::Meta> suffixMeta = std::make_shared<Plugin::Meta>();
        std::string suffix = GetUriSuffix(source->GetSourceUri());
        suffixMeta->SetString(Media::Plugin::MetaID::MEDIA_FILE_EXTENSION, suffix);
        size_t fileSize = 0;
        if ((plugin_->GetSize(fileSize) == Status::OK) && (fileSize != 0)) {
            suffixMeta->SetUint64(Media::Plugin::MetaID::MEDIA_FILE_SIZE, fileSize);
        }
        Seekable seekable = plugin_->GetSeekable();
        suffixMeta->SetInt32(Media::Plugin::MetaID::MEDIA_SEEKABLE, static_cast<int32_t>(seekable));
        Capability peerCap;
        auto tmpCap = MetaToCapability(*suffixMeta);
        Plugin::TagMap upstreamParams;
        Plugin::TagMap downstreamParams;
        if (!GetOutPort(PORT_NAME_DEFAULT)->Negotiate(tmpCap, peerCap, upstreamParams, downstreamParams) ||
            !GetOutPort(PORT_NAME_DEFAULT)->Configure(suffixMeta)) {
            MEDIA_LOG_E("Negotiate fail!");
            return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
        }
    }
    return ErrorCode::SUCCESS;
}

std::string MediaSourceFilter::GetUriSuffix(const std::string& uri)
{
    MEDIA_LOG_D("IN");
    std::string suffix;
    auto const pos = uri.find_last_of('.');
    if (pos != std::string::npos) {
        suffix = uri.substr(pos + 1);
    }
    return suffix;
}

void MediaSourceFilter::ReadLoop()
{
    MEDIA_LOG_D("IN");
    AVBufferPtr bufferPtr = nullptr;
    ErrorCode result = TranslatePluginStatus(plugin_->Read(bufferPtr, DEFAULT_READ_SIZE));
    if (result == ErrorCode::END_OF_STREAM) {
        if (taskPtr_) {
            taskPtr_->StopAsync();
        }
        bufferPtr->flag |= BUFFER_FLAG_EOS;
        outPorts_[0]->PushData(bufferPtr, mediaOffset_);
    } else if (result == ErrorCode::SUCCESS) {
        auto memory = bufferPtr->GetMemory();
        FALSE_RETURN(memory != nullptr);
        auto size = memory->GetSize();
        FALSE_RETURN_MSG(size > 0 || (bufferPtr->flag & BUFFER_FLAG_EOS), "Read data size zero.");
        if (bufferPtr->flag & BUFFER_FLAG_EOS) {
            if (taskPtr_) {
                taskPtr_->StopAsync();
            }
        }
        outPorts_[0]->PushData(bufferPtr, mediaOffset_);
        mediaOffset_ += size;
    } else {
        MEDIA_LOG_E("Read data failed (" PUBLIC_LOG_D32 ")", result);
    }
}

bool MediaSourceFilter::GetProtocolByUri()
{
    auto ret = true;
    auto const pos = uri_.find("://");
    if (pos != std::string::npos) {
        auto prefix = uri_.substr(0, pos);
        protocol_.append(prefix);
    } else {
        protocol_.append("file");
        std::string fullPath;
        ret = OSAL::ConvertFullPath(uri_, fullPath); // convert path to full path
        if (ret && !fullPath.empty()) {
            uri_ = fullPath;
        }
    }
    return ret;
}

bool MediaSourceFilter::ParseProtocol(const std::shared_ptr<MediaSource>& source)
{
    bool ret = true;
    SourceType srcType = source->GetSourceType();
    MEDIA_LOG_D("sourceType = " PUBLIC_LOG_D32, CppExt::to_underlying(srcType));
    if (srcType == SourceType::SOURCE_TYPE_URI) {
        uri_ = source->GetSourceUri();
        ret = GetProtocolByUri();
    } else if (srcType == SourceType::SOURCE_TYPE_FD) {
        protocol_.append("fd");
        uri_ = source->GetSourceUri();
    } else if (srcType == SourceType::SOURCE_TYPE_STREAM) {
        protocol_.append("stream");
        uri_.append("stream://");
    }
    MEDIA_LOG_I("protocol: " PUBLIC_LOG_S ", uri: " PUBLIC_LOG_S, protocol_.c_str(), uri_.c_str());
    return ret;
}

ErrorCode MediaSourceFilter::CreatePlugin(const std::shared_ptr<PluginInfo>& info, const std::string& name,
                                          PluginManager& manager)
{
    if ((plugin_ != nullptr) && (pluginInfo_ != nullptr)) {
        if (info->name == pluginInfo_->name && TranslatePluginStatus(plugin_->Reset()) == ErrorCode::SUCCESS) {
            MEDIA_LOG_I("Reuse last plugin: " PUBLIC_LOG_S, name.c_str());
            return ErrorCode::SUCCESS;
        }
        if (TranslatePluginStatus(plugin_->Deinit()) != ErrorCode::SUCCESS) {
            MEDIA_LOG_E("Deinit last plugin: " PUBLIC_LOG_S " error", pluginInfo_->name.c_str());
        }
    }
    plugin_ = manager.CreateSourcePlugin(name);
    if (plugin_ == nullptr) {
        MEDIA_LOG_E("PluginManager CreatePlugin " PUBLIC_LOG_S " fail", name.c_str());
        return ErrorCode::ERROR_UNKNOWN;
    }
    pluginInfo_ = info;
    MEDIA_LOG_I("Create new plugin: \"" PUBLIC_LOG_S "\" success", pluginInfo_->name.c_str());
    return ErrorCode::SUCCESS;
}

ErrorCode MediaSourceFilter::FindPlugin(const std::shared_ptr<MediaSource>& source)
{
    if (!ParseProtocol(source)) {
        MEDIA_LOG_E("Invalid source!");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    if (protocol_.empty()) {
        MEDIA_LOG_E("protocol_ is empty");
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    PluginManager& pluginManager = PluginManager::Instance();
    std::set<std::string> nameList = pluginManager.ListPlugins(PluginType::SOURCE);
    for (const std::string& name : nameList) {
        std::shared_ptr<PluginInfo> info = pluginManager.GetPluginInfo(PluginType::SOURCE, name);
        MEDIA_LOG_I("name: " PUBLIC_LOG_S ", info->name: " PUBLIC_LOG_S, name.c_str(), info->name.c_str());
        auto val = info->extra[PLUGIN_INFO_EXTRA_PROTOCOL];
        if (val.SameTypeWith(typeid(std::vector<ProtocolType>))) {
            auto supportProtocols = OHOS::Media::Plugin::AnyCast<std::vector<ProtocolType>>(val);
            for(auto supportProtocol : supportProtocols){
                if (g_protocolStringToType[protocol_] == supportProtocol &&
                    CreatePlugin(info, name, pluginManager) == ErrorCode::SUCCESS) {
                    MEDIA_LOG_I("supportProtocol:" PUBLIC_LOG_S " CreatePlugin " PUBLIC_LOG_S " success",
                                protocol_.c_str(), name_.c_str());
                    return ErrorCode::SUCCESS;
                }
            }
        }
    }
    MEDIA_LOG_I("Cannot find any plugin");
    return ErrorCode::ERROR_UNSUPPORTED_FORMAT;
}

void MediaSourceFilter::OnEvent(const Plugin::PluginEvent &event)
{
    if (event.type == PluginEventType::ABOVE_LOW_WATERLINE) {
        isAboveWaterline_ = true;
        if (isPluginReady_ && isAboveWaterline_) {
            FilterBase::OnEvent(Event{name_, EventType::EVENT_READY, {}});
            isAboveWaterline_ = false;
            isPluginReady_ = false;
        }
    }
    FilterBase::OnEvent(event);
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
