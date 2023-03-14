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

#ifndef HISTREAMER_PIPELINE_VIDEO_ENCODER_FILTER_H
#define HISTREAMER_PIPELINE_VIDEO_ENCODER_FILTER_H

#if defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#include "filters/codec/codec_filter_base.h"
#include "pipeline/core/type_define.h"
#include "plugin/common/plugin_tags.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class VideoEncoderFilter : public CodecFilterBase {
public:
    explicit VideoEncoderFilter(const std::string& name);
    ~VideoEncoderFilter() override;

    virtual ErrorCode SetVideoEncoder(int32_t sourceId, std::shared_ptr<Plugin::Meta> encoderMeta);

    ErrorCode Prepare() override;

    ErrorCode Start() override;

    ErrorCode Stop() override;

    void FlushStart() override;

    void FlushEnd() override;

    bool Negotiate(const std::string& inPort,
                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                   Plugin::Capability& negotiatedCap,
                   const Plugin::TagMap& upstreamParams,
                   Plugin::TagMap& downstreamParams) override;

    bool Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta) override;

    /**
     *
     * @param inPort
     * @param buffer
     * @param offset always ignore this parameter
     * @return
     */
    ErrorCode PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset) override;

    void OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& buffer) override;

    void OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& buffer) override;

private:
    class DataCallbackImpl;

    struct VideoEncoderFormat {
        std::string mime;
        uint32_t width;
        uint32_t height;
        int64_t bitRate;
        uint32_t frameRate;
        Plugin::VideoPixelFormat format;
        Plugin::VideoH264Profile profile;
        uint32_t level;
        Plugin::CodecConfig codecConfig;
    };

    ErrorCode SetVideoEncoderFormat(const std::shared_ptr<const Plugin::Meta>& meta);

    ErrorCode AllocateOutputBuffers();

    ErrorCode ConfigurePluginOutputBuffers();

    ErrorCode ConfigurePluginParams();

    ErrorCode ConfigurePlugin();

    ErrorCode ConfigureNoLocked(const std::shared_ptr<const Plugin::Meta>& meta);

    void HandleFrame();

    void HandleOneFrame(const std::shared_ptr<AVBuffer>& data);

    void FinishFrame();

    bool isFlushing_ {false};
    bool isStop_ {false};
    Capability capNegWithDownstream_;
    Capability capNegWithUpstream_;
    VideoEncoderFormat vencFormat_;
    std::shared_ptr<Plugin::Meta> codecMeta_ {nullptr};
    DataCallbackImpl* dataCallback_ {nullptr};

    std::shared_ptr<OHOS::Media::OSAL::Task> handleFrameTask_ {nullptr};
    std::shared_ptr<OHOS::Media::OSAL::Task> pushTask_ {nullptr};
    std::shared_ptr<BufferPool<AVBuffer>> outBufPool_ {nullptr};
    std::shared_ptr<OHOS::Media::BlockingQueue<AVBufferPtr>> inBufQue_ {nullptr};
    std::shared_ptr<OHOS::Media::BlockingQueue<AVBufferPtr>> outBufQue_ {nullptr};
};
}
}
}
#endif
#endif // HISTREAMER_PIPELINE_VIDEO_ENCODER_FILTER_H
