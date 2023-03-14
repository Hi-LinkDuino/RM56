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

#ifndef HISTREAMER_PIPELINE_OUTPUT_SINK_FILTER_H
#define HISTREAMER_PIPELINE_OUTPUT_SINK_FILTER_H
#ifdef RECORDER_SUPPORT
#include "filter_base.h"
#include "plugin/common/media_sink.h"
#include "plugin/common/plugin_tags.h"
#include "plugin/core/output_sink.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
using MediaSink = OHOS::Media::Plugin::MediaSink;
class OutputSinkFilter : public FilterBase {
public:
    explicit OutputSinkFilter(std::string name);
    ~OutputSinkFilter() override;

    void Init(EventReceiver *receiver, FilterCallback *callback) override;

    bool Negotiate(const std::string &inPort,
                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::TagMap& upstreamParams,
                   Plugin::TagMap& downstreamParams) override;

    bool Configure(const std::string &inPort, const std::shared_ptr<const Plugin::Meta> &upstreamMeta) override;

    ErrorCode Prepare() override;
    ErrorCode Start() override;
    ErrorCode Stop() override;
    ErrorCode SetSink(const MediaSink& sink);
    ErrorCode PushData(const std::string &inPort, const AVBufferPtr& buffer, int64_t offset) override;

private:
    ErrorCode ConfigureToPreparePlugin();

    std::shared_ptr<Plugin::OutputSink> plugin_;
    Plugin::ProtocolType protocolType_ {Plugin::ProtocolType::UNKNOWN};
    int64_t currentPos_ {0};
    MediaSink sink_ {Plugin::ProtocolType::UNKNOWN};
    bool bufferEos_ {true};
};
} // Pipeline
} // Media
} // OHOS
#endif
#endif // HISTREAMER_PIPELINE_OUTPUT_SINK_FILTER_H

