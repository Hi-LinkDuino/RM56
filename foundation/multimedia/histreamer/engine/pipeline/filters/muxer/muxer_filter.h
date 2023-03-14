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

#ifndef HISTREAMER_PIPELINE_MUXER_FILTER_H
#define HISTREAMER_PIPELINE_MUXER_FILTER_H
#ifdef RECORDER_SUPPORT
#include "filter_base.h"
#include "plugin/core/muxer.h"
#include "plugin/core/plugin_info.h"
#include "plugin/interface/muxer_plugin.h"
#include "plugin/core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class DataSpliter;
class MuxerFilter : public FilterBase {
public:
    explicit MuxerFilter(std::string name);
    ~MuxerFilter() override;

    void Init(EventReceiver* receiver, FilterCallback* callback) override;

    bool Negotiate(const std::string& inPort,
                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::TagMap& upstreamParams,
                   Plugin::TagMap& downstreamParams) override;

    bool Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta) override;

    ErrorCode SetOutputFormat(std::string containerMime);
    ErrorCode AddTrack(std::shared_ptr<InPort>& trackPort);
    ErrorCode SetMaxDuration(uint64_t maxDuration);
    ErrorCode SetMaxSize(uint64_t maxSize);
    ErrorCode StartNextSegment();
    ErrorCode SendEos();
    ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) override;
    ErrorCode Start() override;
private:
    class MuxerDataSink : public Plugin::DataSinkHelper {
    public:
        Plugin::Status WriteAt(int64_t offset, const std::shared_ptr<Plugin::Buffer>& buffer) override;
        MuxerFilter* muxerFilter_;
    };

    struct TrackInfo {
        int32_t trackId;
        std::string inPort;
        bool eos;
    };

    int32_t GetTrackIdByInPort(const std::shared_ptr<InPort>& inPort);
    int32_t UpdateTrackIdOfInPort(const std::shared_ptr<InPort>& inPort, int32_t trackId);

    bool UpdateAndInitPluginByInfo(const std::shared_ptr<Plugin::PluginInfo>& selectedPluginInfo);

    ErrorCode ConfigureToStart();
    ErrorCode AddTrackThenConfigure(const std::pair<std::string, Plugin::Meta>& meta);

    bool AllTracksEos();
    void UpdateEosState(const std::string& inPort);

    std::string containerMime_ {};
    std::vector<TrackInfo> trackInfos_ {};
    std::shared_ptr<Plugin::Muxer> plugin_ {};
    std::shared_ptr<Plugin::PluginInfo> targetPluginInfo_ {nullptr};
    std::shared_ptr<DataSpliter> dataSpliter_{};
    std::vector<std::pair<std::string, Capability>> capabilityCache_ {};
    std::vector<std::pair<std::string, Plugin::Meta>> metaCache_ {};
    bool hasWriteHeader_ {false};
    std::shared_ptr<MuxerDataSink> muxerDataSink_;

    OSAL::Mutex pushDataMutex_;
    bool eos_ {false};
    std::atomic<int> eosTrackCnt {0};
};
} // Pipeline
} // Media
} // OHOS
#endif
#endif // HISTREAMER_PIPELINE_MUXER_FILTER_H

