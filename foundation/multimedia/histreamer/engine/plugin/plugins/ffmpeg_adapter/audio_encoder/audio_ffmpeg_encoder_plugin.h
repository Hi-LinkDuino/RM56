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
#ifndef HISTREAMER_AUDIO_FFMPEG_ENCODER_PLUGIN_H
#define HISTREAMER_AUDIO_FFMPEG_ENCODER_PLUGIN_H

#include <functional>
#include <map>
#include "utils/blocking_queue.h"
#include "plugin/interface/codec_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
class AudioFfmpegEncoderPlugin : public CodecPlugin {
public:
    explicit AudioFfmpegEncoderPlugin(std::string name);

    ~AudioFfmpegEncoderPlugin() override;

    Status Init() override;

    Status Deinit() override;

    Status Prepare() override;

    Status Reset() override;

    Status Start() override;

    Status Stop() override;

    Status GetParameter(Tag tag, ValueType& value) override;

    Status SetParameter(Tag tag, const ValueType& value) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override
    {
        return Status::OK;
    }

    Status QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs) override;

    Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffer, int32_t timeoutMs) override;

    Status Flush() override;

    Status SetDataCallback(DataCallback* dataCallback) override
    {
        dataCallback_ = dataCallback;
        return Status::OK;
    }

private:
    Status ResetLocked();

    Status DeInitLocked();

    Status SendBufferLocked(const std::shared_ptr<Buffer>& inputBuffer);

    Status ReceiveFrameSucc(const std::shared_ptr<Buffer>& ioInfo, const std::shared_ptr<AVPacket>& packet);

    Status ReceiveBuffer();

    Status ReceiveBufferLocked(const std::shared_ptr<Buffer>& ioInfo);

    bool CheckReformat();

    void FillInFrameCache(const std::shared_ptr<Memory>& mem);

    Status SendOutputBuffer();

    mutable OSAL::Mutex parameterMutex_ {};
    std::map<Tag, ValueType> audioParameter_ {};

    mutable OSAL::Mutex avMutex_ {};
    std::shared_ptr<const AVCodec> avCodec_ {nullptr};
    std::shared_ptr<AVCodecContext> avCodecContext_ {nullptr};
    std::shared_ptr<AVFrame> cachedFrame_ {nullptr};
    uint32_t fullInputFrameSize_ {0};
    std::shared_ptr<Buffer> outBuffer_ {nullptr};
    uint64_t prev_pts_;
    bool needReformat_ {false};
    AVSampleFormat srcFmt_ {AVSampleFormat::AV_SAMPLE_FMT_NONE};
    uint32_t srcBytesPerSample_ {0};
    std::shared_ptr<SwrContext> swrCtx_ {nullptr};
    std::vector<uint8_t> resampleCache_ {};
    std::vector<uint8_t*> resampleChannelAddr_ {};
    DataCallback* dataCallback_ {nullptr};
};
} // Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_AUDIO_FFMPEG_ENCODER_PLUGIN_H
