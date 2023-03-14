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

#ifndef HISTREAMER_PIPELINE_FILTER_AUDIO_ENCODER_H
#define HISTREAMER_PIPELINE_FILTER_AUDIO_ENCODER_H

#ifdef RECORDER_SUPPORT

#include "pipeline/filters/codec/codec_filter_base.h"
#include "plugin/common/plugin_tags.h"
#include "utils/ring_buffer.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class AudioEncoderFilter : public CodecFilterBase {
public:
    explicit AudioEncoderFilter(const std::string& name);
    ~AudioEncoderFilter() override;

    virtual ErrorCode SetAudioEncoder(int32_t sourceId, std::shared_ptr<Plugin::Meta> encoderMeta);

    ErrorCode Start() override;

    ErrorCode Stop() override;

    bool Negotiate(const std::string& inPort,
                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::TagMap& upstreamParams,
                   Plugin::TagMap& downstreamParams) override;

    uint32_t CalculateBufferSize(const std::shared_ptr<const Plugin::Meta>& meta) override;

    bool Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta) override;

    /**
     *
     * @param inPort
     * @param buffer
     * @param offset always ignore this parameter
     * @return
     */
    ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) override;

private:
    ErrorCode ConfigureToStartPluginLocked(const std::shared_ptr<const Plugin::Meta>& meta) override;

    ErrorCode HandleFrame(const std::shared_ptr<AVBuffer>& buffer);

    ErrorCode FinishFrame();

    ErrorCode Release();

    void OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& input) override;

    void OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& output) override;

private:
    std::shared_ptr<BufferPool<AVBuffer>> outBufferPool_ {};
    Capability capNegWithDownstream_;
    Capability capNegWithUpstream_;
    size_t frameSize_ {0};
    std::string mime_;
    std::shared_ptr<Plugin::Meta> encoderMeta_ {};
    std::unique_ptr<RingBuffer> rb_ {};
    AVBufferPtr cahceBuffer_ {nullptr};
};
} // OHOS
} // Media
} // Pipeline
#endif
#endif // HISTREAMER_PIPELINE_FILTER_AUDIO_ENCODER_H
