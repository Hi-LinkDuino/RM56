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
#ifdef RECORDER_SUPPORT

#define HST_LOG_TAG "MuxerFilter"

#include "muxer_filter.h"

#include "data_spliter.h"
#include "factory/filter_factory.h"
#include "foundation/log.h"
#include "common/plugin_settings.h"
#include "common/plugin_utils.h"
#include "pipeline/core/plugin_attr_desc.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
namespace {
std::vector<std::shared_ptr<Plugin::PluginInfo>> Intersections(
    const std::vector<std::shared_ptr<Plugin::PluginInfo>>& caps1,
    const std::vector<std::pair<std::shared_ptr<Plugin::PluginInfo>, Plugin::Capability>>& caps2)
{
    std::vector<std::shared_ptr<Plugin::PluginInfo>> intersections;
    for (const auto& cap1 : caps1) {
        for (const auto& cap2 : caps2) {
            if (cap1->name == cap2.first->name) {
                intersections.emplace_back(cap1);
            }
        }
    }
    return intersections;
}
}
static AutoRegisterFilter<MuxerFilter> g_registerFilterHelper("builtin.recorder.muxer");

MuxerFilter::MuxerFilter(std::string name) : FilterBase(std::move(name)),
    muxerDataSink_(std::make_shared<MuxerDataSink>())
{
    filterType_ = FilterType::MUXER;
}

MuxerFilter::~MuxerFilter() {}
void MuxerFilter::Init(EventReceiver* receiver, FilterCallback* callback)
{
    this->eventReceiver_ = receiver;
    this->callback_ = callback;
    inPorts_.clear();
    outPorts_.clear();
    outPorts_.emplace_back(std::make_shared<Pipeline::OutPort>(this, PORT_NAME_DEFAULT));
    muxerDataSink_->muxerFilter_ = this;
    state_ = FilterState::INITIALIZED;
}
bool MuxerFilter::UpdateAndInitPluginByInfo(const std::shared_ptr<Plugin::PluginInfo>& selectedPluginInfo)
{
    if (selectedPluginInfo == nullptr) {
        MEDIA_LOG_W("no available info to update plugin");
        return false;
    }
    if (plugin_ != nullptr) {
        if (targetPluginInfo_ != nullptr && targetPluginInfo_->name == selectedPluginInfo->name) {
            if (plugin_->Reset() == Plugin::Status::OK) {
                return true;
            }
            MEDIA_LOG_W("reuse previous plugin " PUBLIC_LOG_S " failed, will create new plugin",
                        targetPluginInfo_->name.c_str());
        }
        plugin_->Deinit();
    }

    plugin_ = Plugin::PluginManager::Instance().CreateMuxerPlugin(selectedPluginInfo->name);
    if (plugin_ == nullptr) {
        MEDIA_LOG_E("cannot create plugin " PUBLIC_LOG_S, selectedPluginInfo->name.c_str());
        return false;
    }
    auto err = TranslatePluginStatus(plugin_->Init());
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("muxer plugin init error");
        return false;
    }
    plugin_->SetCallback(this);
    targetPluginInfo_ = selectedPluginInfo;
    return true;
}

bool MuxerFilter::Negotiate(const std::string& inPort,
                            const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                            Plugin::Capability& negotiatedCap,
                            const Plugin::TagMap& upstreamParams,
                            Plugin::TagMap& downstreamParams)
{
    if (state_ != FilterState::PREPARING) {
        MEDIA_LOG_W("muxer filter is not in preparing when negotiate");
        return false;
    }
    hasWriteHeader_ = false;
    capabilityCache_.emplace_back(std::make_pair(inPort, *upstreamCap));
    if (capabilityCache_.size() < inPorts_.size()) {
        return true;
    }
    MEDIA_LOG_I("all track caps has been received, start negotiating downstream");
    auto candidate = FindAvailablePluginsByOutputMime(containerMime_, Plugin::PluginType::MUXER);
    for (const auto& cache: capabilityCache_) {
        auto tmp = FindAvailablePlugins(cache.second, Plugin::PluginType::MUXER);
        candidate = Intersections(candidate, tmp);
        if (candidate.empty()) {
            break;
        }
    }
    if (candidate.empty()) {
        MEDIA_LOG_E("cannot find any available plugins");
        return false;
    }
    auto muxerCap = std::make_shared<Capability>(containerMime_);
    Capability downCap;
    if (!outPorts_[0]->Negotiate(muxerCap, downCap, upstreamParams, downstreamParams)) {
        MEDIA_LOG_E("downstream of muxer filter negotiate failed");
        return false;
    }
    // always use the first candidate plugin info
    return UpdateAndInitPluginByInfo(candidate[0]);
}
ErrorCode MuxerFilter::AddTrackThenConfigure(const std::pair<std::string, Plugin::Meta>& metaPair)
{
    uint32_t trackId = 0;
    ErrorCode ret = TranslatePluginStatus(plugin_->AddTrack(trackId));
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("muxer plugin add track failed");
        return ret;
    }
    trackInfos_.emplace_back(TrackInfo{static_cast<int32_t>(trackId), metaPair.first, false});
    auto parameterMap = PluginParameterTable::FindAllowedParameterMap(filterType_);
    for (const auto& keyPair : parameterMap) {
        auto outValue = metaPair.second.GetData(static_cast<Plugin::MetaID>(keyPair.first));
        if (outValue &&
            (keyPair.second.second & PARAM_SET) &&
            keyPair.second.first(keyPair.first, *outValue)) {
            plugin_->SetTrackParameter(trackId, keyPair.first, *outValue);
        } else {
            if (!HasTagInfo(keyPair.first)) {
                MEDIA_LOG_W("tag " PUBLIC_LOG_D32 " is not in map, may be update it?", keyPair.first);
            } else {
                MEDIA_LOG_W("parameter " PUBLIC_LOG_S " in meta is not found or type mismatch",
                    GetTagStrName(keyPair.first));
            }
        }
    }
    return ErrorCode::SUCCESS;
}

ErrorCode MuxerFilter::ConfigureToStart()
{
    ErrorCode ret;
    for (const auto& cache: metaCache_) {
        ret = AddTrackThenConfigure(cache);
        if (ret != ErrorCode::SUCCESS) {
            MEDIA_LOG_E("add and configure for track from inPort " PUBLIC_LOG_S " failed", cache.first.c_str());
            return ret;
        }
    }
    // todo add other global meta

    ret = TranslatePluginStatus(plugin_->Prepare());
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("muxer plugin prepare failed");
        return ret;
    }
    ret = TranslatePluginStatus(plugin_->Start());
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("muxer plugin start failed");
    }
    return ret;
}
bool MuxerFilter::Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    std::string tmp;
    if (!upstreamMeta->GetString(Plugin::MetaID::MIME, tmp)) {
        MEDIA_LOG_E("stream meta must contain mime, which is not found in current stream from port " PUBLIC_LOG_S,
                    inPort.c_str());
        return false;
    }
    metaCache_.emplace_back(std::make_pair(inPort, *upstreamMeta));
    if (metaCache_.size() < inPorts_.size()) {
        return true;
    }
    if (plugin_ == nullptr) {
        MEDIA_LOG_E("cannot configure when no plugin available");
        return false;
    }

    auto meta = std::make_shared<Plugin::Meta>();
    meta->SetString(Plugin::MetaID::MIME, containerMime_);
    if (!outPorts_[0]->Configure(meta)) {
        MEDIA_LOG_E("downstream of muxer filter configure failed");
        return false;
    }
    plugin_->SetDataSink(muxerDataSink_);
    auto ret = ConfigureToStart();
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("muxer filter configure and start error");
        OnEvent({name_, EventType::EVENT_ERROR, ret});
        return false;
    }
    state_ = FilterState::READY;
    OnEvent({name_, EventType::EVENT_READY});
    MEDIA_LOG_I("muxer send EVENT_READY");
    return true;
}

ErrorCode MuxerFilter::SetOutputFormat(std::string containerMime)
{
    containerMime_ = std::move(containerMime);
    return ErrorCode::SUCCESS;
}

ErrorCode MuxerFilter::AddTrack(std::shared_ptr<InPort> &trackPort)
{
    if (state_ != FilterState::INITIALIZED) {
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    trackPort = std::make_shared<InPort>(this, std::string(PORT_NAME_DEFAULT) + std::to_string(inPorts_.size()));
    inPorts_.emplace_back(trackPort);
    return ErrorCode::SUCCESS;
}

ErrorCode MuxerFilter::SetMaxDuration(uint64_t maxDuration)
{
    return ErrorCode::SUCCESS;
}

ErrorCode MuxerFilter::SetMaxSize(uint64_t maxSize)
{
    return ErrorCode::SUCCESS;
}

ErrorCode MuxerFilter::StartNextSegment()
{
    return ErrorCode::SUCCESS;
}

ErrorCode MuxerFilter::SendEos()
{
    OSAL::ScopedLock lock(pushDataMutex_);
    MEDIA_LOG_I("SendEos entered.");
    eos_ = true;
    if (hasWriteHeader_ && plugin_) {
        plugin_->WriteTrailer();
    }
    hasWriteHeader_ = false;
    auto buf = std::make_shared<AVBuffer>();
    buf->flag |= BUFFER_FLAG_EOS;
    outPorts_[0]->PushData(buf, -1);
    metaCache_.clear();
    return ErrorCode::SUCCESS;
}

bool MuxerFilter::AllTracksEos()
{
    return static_cast<size_t>(eosTrackCnt.load()) == trackInfos_.size();
}
void MuxerFilter::UpdateEosState(const std::string& inPort)
{
    int32_t eosCnt = 0;
    for (auto& item : trackInfos_) {
        if (item.inPort == inPort) {
            item.eos = true;
        }
        if (item.eos) {
            eosCnt++;
        }
    }
    eosTrackCnt = eosCnt;
}

ErrorCode MuxerFilter::PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset)
{
    {
        OSAL::ScopedLock lock(pushDataMutex_);
        if (state_ != FilterState::READY && state_ != FilterState::PAUSED && state_ != FilterState::RUNNING) {
            MEDIA_LOG_W("pushing data to muxer when state is " PUBLIC_LOG_D32, static_cast<int>(state_.load()));
            return ErrorCode::ERROR_INVALID_OPERATION;
        }
        if (eos_) {
            MEDIA_LOG_D("SendEos exit");
            return ErrorCode::SUCCESS;
        }
        // todo we should consider more tracks
        if (!hasWriteHeader_) {
            plugin_->WriteHeader();
            hasWriteHeader_ = true;
        }
        if (buffer->GetMemory()->GetSize() != 0) {
            plugin_->WriteFrame(buffer);
        }

        if (buffer->flag & BUFFER_FLAG_EOS) {
            MEDIA_LOG_I("It is EOS buffer");
            UpdateEosState(inPort);
        }
    }
    if (AllTracksEos()) {
        SendEos();
    }
    return ErrorCode::SUCCESS;
}

Plugin::Status MuxerFilter::MuxerDataSink::WriteAt(int64_t offset, const std::shared_ptr<Plugin::Buffer> &buffer)
{
    if (muxerFilter_ != nullptr) {
        muxerFilter_->outPorts_[0]->PushData(buffer, offset);
    }
    return Plugin::Status::OK;
}

ErrorCode MuxerFilter::Start()
{
    eos_ = false;
    return FilterBase::Start();
}
} // Pipeline
} // Media
} // OHOS
#endif // RECORDER_SUPPORT