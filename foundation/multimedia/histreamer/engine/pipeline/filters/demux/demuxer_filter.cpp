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

#define HST_LOG_TAG "DemuxerFilter"

#include "demuxer_filter.h"
#include <algorithm>
#include "compatible_check.h"
#include "factory/filter_factory.h"
#include "foundation/log.h"
#include "pipeline/filters/common/plugin_utils.h"
#include "plugin/common/plugin_time.h"
#include "utils/constants.h"
#include "utils/steady_clock.h"
#include "filters/common/dump_buffer.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
static AutoRegisterFilter<DemuxerFilter> g_registerFilterHelper("builtin.player.demuxer");

class DemuxerFilter::DataSourceImpl : public Plugin::DataSourceHelper {
public:
    explicit DataSourceImpl(const DemuxerFilter& filter);
    ~DataSourceImpl() override = default;
    Plugin::Status ReadAt(int64_t offset, std::shared_ptr<Plugin::Buffer>& buffer, size_t expectedLen) override;
    Plugin::Status GetSize(size_t& size) override;
    Plugin::Seekable GetSeekable() override;

private:
    const DemuxerFilter& filter;
};

DemuxerFilter::DataSourceImpl::DataSourceImpl(const DemuxerFilter& filter) : filter(filter)
{
}

/**
 * ReadAt Plugin::DataSource::ReadAt implementation.
 * @param offset offset in media stream.
 * @param buffer caller allocate real buffer.
 * @param expectedLen buffer size wanted to read.
 * @return read result.
 */
Plugin::Status DemuxerFilter::DataSourceImpl::ReadAt(int64_t offset, std::shared_ptr<Plugin::Buffer>& buffer,
                                                     size_t expectedLen)
{
    if (!buffer || buffer->IsEmpty() || expectedLen == 0 || !filter.IsOffsetValid(offset)) {
        MEDIA_LOG_E("ReadAt failed, buffer empty: " PUBLIC_LOG_D32 ", expectedLen: " PUBLIC_LOG_D32
                    ", offset: " PUBLIC_LOG_D64, !buffer, static_cast<int>(expectedLen), offset);
        return Plugin::Status::ERROR_UNKNOWN;
    }
    Plugin::Status rtv = Plugin::Status::OK;
    switch (filter.pluginState_.load()) {
        case DemuxerState::DEMUXER_STATE_NULL:
            rtv = Plugin::Status::ERROR_WRONG_STATE;
            MEDIA_LOG_E("ReadAt error due to DEMUXER_STATE_NULL");
            break;
        case DemuxerState::DEMUXER_STATE_PARSE_HEADER: {
            if (filter.getRange_(static_cast<uint64_t>(offset), expectedLen, buffer)) {
                DUMP_BUFFER2FILE("demuxer_input_peek.data", buffer);
            } else {
                rtv = Plugin::Status::ERROR_NOT_ENOUGH_DATA;
            }
            break;
        }
        case DemuxerState::DEMUXER_STATE_PARSE_FRAME: {
            if (filter.getRange_(static_cast<uint64_t>(offset), expectedLen, buffer)) {
                DUMP_BUFFER2LOG("Demuxer GetRange", buffer, offset);
                DUMP_BUFFER2FILE("demuxer_input_get.data", buffer);
            } else {
                rtv = Plugin::Status::END_OF_STREAM;
            }
            break;
        }
        default:
            break;
    }
    return rtv;
}

Plugin::Status DemuxerFilter::DataSourceImpl::GetSize(size_t& size)
{
    size = filter.mediaDataSize_;
    return (filter.mediaDataSize_ > 0) ? Plugin::Status::OK : Plugin::Status::ERROR_WRONG_STATE;
}

Plugin::Seekable DemuxerFilter::DataSourceImpl::GetSeekable()
{
    return filter.seekable_;
}

DemuxerFilter::DemuxerFilter(std::string name)
    : FilterBase(std::move(name)),
      seekable_(Plugin::Seekable::INVALID),
      uriSuffix_(),
      mediaDataSize_(0),
      task_(nullptr),
      typeFinder_(nullptr),
      dataPacker_(nullptr),
      pluginName_(),
      plugin_(nullptr),
      pluginState_(DemuxerState::DEMUXER_STATE_NULL),
      pluginAllocator_(nullptr),
      dataSource_(std::make_shared<DataSourceImpl>(*this)),
      mediaMetaData_()
{
    filterType_ = FilterType::DEMUXER;
    dataPacker_ = std::make_shared<DataPacker>();
    task_ = std::make_shared<OSAL::Task>("DemuxerFilter");
    MEDIA_LOG_D("ctor called");
}

DemuxerFilter::~DemuxerFilter()
{
    MEDIA_LOG_I("dtor called");
    if (task_) {
        task_->Stop();
    }
    if (plugin_) {
        plugin_->Deinit();
    }
}

void DemuxerFilter::Init(EventReceiver* receiver, FilterCallback* callback)
{
    this->eventReceiver_ = receiver;
    this->callback_ = callback;
    inPorts_.clear();
    outPorts_.clear();
    inPorts_.push_back(std::make_shared<Pipeline::InPort>(this, PORT_NAME_DEFAULT));
    state_ = FilterState::INITIALIZED;
}

ErrorCode DemuxerFilter::Start()
{
    MEDIA_LOG_I("Start called.");
    if (task_) {
        task_->Start();
    }
    return FilterBase::Start();
}

ErrorCode DemuxerFilter::Stop()
{
    MEDIA_LOG_I("Stop called.");
    dataPacker_->Stop();
    if (task_) {
        task_->Stop();
    }
    Reset();
    if (!outPorts_.empty()) {
        PortInfo portInfo;
        portInfo.type = PortType::OUT;
        portInfo.ports.reserve(outPorts_.size());
        for (const auto& outPort : outPorts_) {
            portInfo.ports.push_back({outPort->GetName(), false});
        }
        if (callback_) {
            callback_->OnCallback(FilterCallbackType::PORT_REMOVE, static_cast<Filter*>(this), portInfo);
        }
    }
    return FilterBase::Stop();
}

ErrorCode DemuxerFilter::Pause()
{
    MEDIA_LOG_I("Pause called");
    return FilterBase::Pause();
}

void DemuxerFilter::FlushStart()
{
    MEDIA_LOG_I("FlushStart entered");
    task_->Pause();
    dataPacker_->Flush();
}

void DemuxerFilter::FlushEnd()
{
    MEDIA_LOG_I("FlushEnd entered");
}

ErrorCode DemuxerFilter::Prepare()
{
    MEDIA_LOG_I("Prepare called");
    DUMP_BUFFER2FILE_PREPARE();
    dataPacker_->Start();
    pluginState_ = DemuxerState::DEMUXER_STATE_NULL;
    task_->RegisterHandler([this] { DemuxerLoop(); });
    Pipeline::WorkMode mode;
    GetInPort(PORT_NAME_DEFAULT)->Activate({Pipeline::WorkMode::PULL, Pipeline::WorkMode::PUSH}, mode);
    if (mode == Pipeline::WorkMode::PULL) {
        dataPacker_->Flush();
        ActivatePullMode();
    } else {
        ActivatePushMode();
    }
    state_ = FilterState::PREPARING;
    return ErrorCode::SUCCESS;
}

ErrorCode DemuxerFilter::PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset)
{
    MEDIA_LOG_D("PushData for port: " PUBLIC_LOG_S, inPort.c_str());
    if (buffer->flag & BUFFER_FLAG_EOS) {
        dataPacker_->SetEos();
    } else {
        dataPacker_->PushData(std::move(buffer), offset);
    }
    return ErrorCode::SUCCESS;
}

bool DemuxerFilter::Negotiate(const std::string& inPort,
                              const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                              Plugin::Capability& negotiatedCap,
                              const Plugin::TagMap& upstreamParams,
                              Plugin::TagMap& downstreamParams)
{
    (void)inPort;
    (void)upstreamCap;
    (void)negotiatedCap;
    (void)upstreamParams;
    (void)downstreamParams;
    return true;
}

bool DemuxerFilter::Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    (void)upstreamMeta->GetUint64(Plugin::MetaID::MEDIA_FILE_SIZE, mediaDataSize_);
    int32_t seekable = static_cast<int32_t>(seekable_);
    if (upstreamMeta->GetInt32(Plugin::MetaID::MEDIA_SEEKABLE, seekable)) {
        seekable_ = static_cast<Plugin::Seekable>(seekable);
    }
    return upstreamMeta->GetString(Plugin::MetaID::MEDIA_FILE_EXTENSION, uriSuffix_);
}

ErrorCode DemuxerFilter::SeekTo(int64_t pos, Plugin::SeekMode mode)
{
    if (!plugin_) {
        MEDIA_LOG_E("SeekTo failed due to no valid plugin");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    auto rtv = TranslatePluginStatus(plugin_->SeekTo(-1, pos, mode));
    if (rtv == ErrorCode::SUCCESS) {
        if (task_) {
            task_->Start();
        }
    } else {
        MEDIA_LOG_E("SeekTo failed with return value: " PUBLIC_LOG_D32, static_cast<int>(rtv));
    }
    return rtv;
}

std::vector<std::shared_ptr<Plugin::Meta>> DemuxerFilter::GetStreamMetaInfo() const
{
    return mediaMetaData_.trackMetas;
}

std::shared_ptr<Plugin::Meta> DemuxerFilter::GetGlobalMetaInfo() const
{
    return mediaMetaData_.globalMeta;
}

void DemuxerFilter::Reset()
{
    mediaMetaData_.globalMeta.reset();
    mediaMetaData_.trackMetas.clear();
    mediaMetaData_.trackInfos.clear();
}

void DemuxerFilter::InitTypeFinder()
{
    if (!typeFinder_) {
        typeFinder_ = std::make_shared<TypeFinder>();
    }
}

bool DemuxerFilter::CreatePlugin(std::string pluginName)
{
    if (plugin_) {
        plugin_->Deinit();
    }
    plugin_ = Plugin::PluginManager::Instance().CreateDemuxerPlugin(pluginName);
    if (!plugin_ || plugin_->Init() != Plugin::Status::OK) {
        MEDIA_LOG_E("CreatePlugin " PUBLIC_LOG_S " failed.", pluginName.c_str());
        return false;
    }
    plugin_->SetCallback(this);
    pluginAllocator_ = plugin_->GetAllocator();
    pluginName_.swap(pluginName);
    return true;
}

bool DemuxerFilter::InitPlugin(std::string pluginName)
{
    if (pluginName.empty()) {
        return false;
    }
    if (pluginName_ != pluginName) {
        FALSE_RETURN_V(CreatePlugin(std::move(pluginName)), false);
    } else {
        if (plugin_->Reset() != Plugin::Status::OK) {
            FALSE_RETURN_V(CreatePlugin(std::move(pluginName)), false);
        }
    }
    MEDIA_LOG_I("InitPlugin, " PUBLIC_LOG_S " used.", pluginName_.c_str());
    (void)plugin_->SetDataSource(std::dynamic_pointer_cast<Plugin::DataSourceHelper>(dataSource_));
    pluginState_ = DemuxerState::DEMUXER_STATE_PARSE_HEADER;
    return plugin_->Prepare() == Plugin::Status::OK;
}

void DemuxerFilter::ActivatePullMode()
{
    MEDIA_LOG_D("ActivatePullMode called");
    InitTypeFinder();
    checkRange_ = [this](uint64_t offset, uint32_t size) {
        uint64_t curOffset = offset;
        if (dataPacker_->IsDataAvailable(offset, size, curOffset)) {
            return true;
        }
        MEDIA_LOG_DD("IsDataAvailable false, require offset " PUBLIC_LOG_D64 ", DataPacker data offset end - curOffset "
                     PUBLIC_LOG_D64, offset, curOffset);
        AVBufferPtr bufferPtr = std::make_shared<AVBuffer>();
        bufferPtr->AllocMemory(pluginAllocator_, size);
        auto ret = inPorts_.front()->PullData(curOffset, size, bufferPtr);
        if (ret == ErrorCode::SUCCESS) {
            dataPacker_->PushData(std::move(bufferPtr), curOffset);
            return true;
        } else if (ret == ErrorCode::END_OF_STREAM) {
            // hasDataToRead is ture if there is some data in data packer can be read.
            bool hasDataToRead = offset < curOffset && (!dataPacker_->IsEmpty());
            if (hasDataToRead) {
                dataPacker_->SetEos();
            } else {
                dataPacker_->Flush();
            }
            return hasDataToRead;
        } else {
            MEDIA_LOG_E("PullData from source filter failed " PUBLIC_LOG_D32, ret);
        }
        return false;
    };
    peekRange_ = [this](uint64_t offset, size_t size, AVBufferPtr& bufferPtr) -> bool {
        if (checkRange_(offset, size)) {
            return dataPacker_->PeekRange(offset, size, bufferPtr);
        }
        return false;
    };
    getRange_ = [this](uint64_t offset, size_t size, AVBufferPtr& bufferPtr) -> bool {
        if (checkRange_(offset, size)) {
            auto ret = dataPacker_->GetRange(offset, size, bufferPtr);
            return ret;
        }
        return false;
    };
    typeFinder_->Init(uriSuffix_, mediaDataSize_, checkRange_, peekRange_);
    std::string type = typeFinder_->FindMediaType();
    MEDIA_LOG_I("FindMediaType result : type : " PUBLIC_LOG_S ", uriSuffix_ : " PUBLIC_LOG_S ", mediaDataSize_ : "
        PUBLIC_LOG_D64, type.c_str(), uriSuffix_.c_str(), mediaDataSize_);
    MediaTypeFound(std::move(type));
}

void DemuxerFilter::ActivatePushMode()
{
    MEDIA_LOG_D("ActivatePushMode called");
    InitTypeFinder();
    checkRange_ = [this](uint64_t offset, uint32_t size) {
        return !dataPacker_->IsEmpty(); // True if there is some data
    };
    peekRange_ = [this](uint64_t offset, size_t size, AVBufferPtr& bufferPtr) -> bool {
        return dataPacker_->PeekRange(offset, size, bufferPtr);
    };
    getRange_ = [this](uint64_t offset, size_t size, AVBufferPtr& bufferPtr) -> bool {
        // In push mode, ignore offset, always get data from the start of the data packer.
        return dataPacker_->GetRange(size, bufferPtr);
    };
    typeFinder_->Init(uriSuffix_, mediaDataSize_, checkRange_, peekRange_);
    typeFinder_->FindMediaTypeAsync([this](std::string pluginName) { MediaTypeFound(std::move(pluginName)); });
}

void DemuxerFilter::MediaTypeFound(std::string pluginName)
{
    if (InitPlugin(std::move(pluginName))) {
        task_->Start();
    } else {
        MEDIA_LOG_E("MediaTypeFound init plugin error.");
        OnEvent({name_, EventType::EVENT_ERROR, ErrorCode::ERROR_UNSUPPORTED_FORMAT});
    }
}

void DemuxerFilter::InitMediaMetaData(const Plugin::MediaInfoHelper& mediaInfo)
{
    mediaMetaData_.globalMeta = std::make_shared<Plugin::Meta>(mediaInfo.globalMeta);
    mediaMetaData_.trackMetas.clear();
    int trackCnt = 0;
    for (auto& trackMeta : mediaInfo.trackMeta) {
        mediaMetaData_.trackMetas.push_back(std::make_shared<Plugin::Meta>(trackMeta));
        if (!trackMeta.Empty()) {
            ++trackCnt;
        }
    }
    mediaMetaData_.trackInfos.reserve(trackCnt);
}

bool DemuxerFilter::IsOffsetValid(int64_t offset) const
{
    return mediaDataSize_ == 0 || offset <= static_cast<int64_t>(mediaDataSize_);
}

bool DemuxerFilter::PrepareStreams(const Plugin::MediaInfoHelper& mediaInfo)
{
    MEDIA_LOG_I("PrepareStreams called");
    InitMediaMetaData(mediaInfo);
    outPorts_.clear();
    int streamCnt = mediaInfo.trackMeta.size();
    PortInfo portInfo;
    portInfo.type = PortType::OUT;
    portInfo.ports.reserve(streamCnt);
    int audioTrackCnt = 0;
    for (int i = 0; i < streamCnt; ++i) {
        if (mediaInfo.trackMeta[i].Empty()) {
            MEDIA_LOG_E("PrepareStreams, unsupported stream with trackId = " PUBLIC_LOG_D32, i);
            continue;
        }
        std::string mime;
        uint32_t trackId = 0;
        if (!mediaInfo.trackMeta[i].GetString(Plugin::MetaID::MIME, mime) ||
            !mediaInfo.trackMeta[i].GetUint32(Plugin::MetaID::TRACK_ID, trackId)) {
            MEDIA_LOG_E("PrepareStreams failed to extract mime or trackId.");
            continue;
        }
        if (IsAudioMime(mime)) {
            MEDIA_LOG_D("PrepareStreams, audio stream with trackId = " PUBLIC_LOG_U32 ".", trackId);
            if (audioTrackCnt == 1) {
                MEDIA_LOG_E("PrepareStreams, discard audio track: " PUBLIC_LOG_D32 ".", trackId);
                continue;
            }
            ++audioTrackCnt;
        }
        auto port = std::make_shared<OutPort>(this, NamePort(mime));
        MEDIA_LOG_I("PrepareStreams, trackId: " PUBLIC_LOG_D32 ", portName: " PUBLIC_LOG_S,
                    i, port->GetName().c_str());
        outPorts_.push_back(port);
        portInfo.ports.push_back({port->GetName(), IsRawAudio(mime)});
        mediaMetaData_.trackInfos.emplace_back(trackId, std::move(port), true);
    }
    if (portInfo.ports.empty()) {
        MEDIA_LOG_E("PrepareStreams failed due to no valid port.");
        return false;
    }
    ErrorCode ret = ErrorCode::SUCCESS;
    if (callback_) {
        ret = callback_->OnCallback(FilterCallbackType::PORT_ADDED, static_cast<Filter*>(this), portInfo);
    }
    return ret == ErrorCode::SUCCESS;
}

ErrorCode DemuxerFilter::ReadFrame(AVBuffer& buffer, uint32_t& trackId)
{
    MEDIA_LOG_DD("ReadFrame called");
    ErrorCode result = ErrorCode::ERROR_UNKNOWN;
    auto rtv = plugin_->ReadFrame(buffer, 0);
    if (rtv == Plugin::Status::OK) {
        trackId = buffer.trackID;
        result = ErrorCode::SUCCESS;
    }
    MEDIA_LOG_DD("ReadFrame return with rtv = " PUBLIC_LOG_D32, static_cast<int32_t>(rtv));
    return (rtv != Plugin::Status::END_OF_STREAM) ? result : ErrorCode::END_OF_STREAM;
}

std::shared_ptr<Plugin::Meta> DemuxerFilter::GetTrackMeta(uint32_t trackId)
{
    return (trackId < mediaMetaData_.trackMetas.size()) ? mediaMetaData_.trackMetas[trackId] : nullptr;
}

void DemuxerFilter::SendEventEos()
{
    MEDIA_LOG_I("SendEventEos called");
    AVBufferPtr bufferPtr = std::make_shared<AVBuffer>();
    bufferPtr->flag = BUFFER_FLAG_EOS;
    for (const auto& stream : mediaMetaData_.trackInfos) {
        stream.port->PushData(bufferPtr, -1);
    }
}

void DemuxerFilter::HandleFrame(const AVBufferPtr& bufferPtr, uint32_t trackId)
{
    for (auto& stream : mediaMetaData_.trackInfos) {
        if (stream.trackId != trackId) {
            continue;
        }
        stream.port->PushData(bufferPtr, -1);
        break;
    }
}

void DemuxerFilter::NegotiateDownstream()
{
    PROFILE_BEGIN("NegotiateDownstream profile begins.");
    for (auto& stream : mediaMetaData_.trackInfos) {
        if (stream.needNegoCaps) {
            Capability caps;
            MEDIA_LOG_I("demuxer negotiate with trackId: " PUBLIC_LOG_U32, stream.trackId);
            auto streamMeta = GetTrackMeta(stream.trackId);
            auto tmpCap = MetaToCapability(*streamMeta);
            Plugin::TagMap upstreamParams;
            Plugin::TagMap downstreamParams;
            if (stream.port->Negotiate(tmpCap, caps, upstreamParams, downstreamParams) &&
                stream.port->Configure(streamMeta)) {
                stream.needNegoCaps = false;
            } else {
                task_->PauseAsync();
                MEDIA_LOG_E("NegotiateDownstream failed error.");
                OnEvent({name_, EventType::EVENT_ERROR, ErrorCode::ERROR_UNSUPPORTED_FORMAT});
            }
        }
    }
    PROFILE_END("NegotiateDownstream end.");
}

void DemuxerFilter::DemuxerLoop()
{
    if (pluginState_.load() == DemuxerState::DEMUXER_STATE_PARSE_FRAME) {
        AVBufferPtr bufferPtr = std::make_shared<AVBuffer>();
        uint32_t streamIndex = 0;
        auto rtv = ReadFrame(*bufferPtr, streamIndex);
        if (rtv == ErrorCode::SUCCESS) {
            DUMP_BUFFER2LOG("Demuxer Output", bufferPtr, 0);
            DUMP_BUFFER2FILE("demuxer_output.data", bufferPtr);
            HandleFrame(bufferPtr, streamIndex);
        } else {
            SendEventEos();
            task_->PauseAsync();
            if (rtv != ErrorCode::END_OF_STREAM) {
                MEDIA_LOG_E("ReadFrame failed with rtv = " PUBLIC_LOG_D32, CppExt::to_underlying(rtv));
            }
        }
    } else {
        Plugin::MediaInfoHelper mediaInfo;
        PROFILE_BEGIN();
        if (plugin_->GetMediaInfo(mediaInfo) == Plugin::Status::OK && PrepareStreams(mediaInfo)) {
            PROFILE_END("Succeed to extract mediainfo.");
            NegotiateDownstream();
            pluginState_ = DemuxerState::DEMUXER_STATE_PARSE_FRAME;
            state_ = FilterState::READY;
            OnEvent({name_, EventType::EVENT_READY, {}});
        } else {
            task_->PauseAsync();
            MEDIA_LOG_E("demuxer filter parse meta failed");
            OnEvent({name_, EventType::EVENT_ERROR, ErrorCode::ERROR_UNKNOWN});
        }
    }
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
