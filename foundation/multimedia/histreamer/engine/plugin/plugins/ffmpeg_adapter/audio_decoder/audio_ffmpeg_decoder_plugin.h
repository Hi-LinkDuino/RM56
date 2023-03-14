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

#ifndef HISTREAMER_AUDIO_FFMPEG_DECODER_PLUGIN_H
#define HISTREAMER_AUDIO_FFMPEG_DECODER_PLUGIN_H

#include <functional>
#include <map>
#include "foundation/osal/thread/task.h"
#include "utils/blocking_queue.h"
#include "plugin/interface/codec_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
class AudioFfmpegDecoderPlugin : public CodecPlugin {
public:
    explicit AudioFfmpegDecoderPlugin(std::string name);

    ~AudioFfmpegDecoderPlugin() override;

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
    Status AssignExtraDataIfExistsLocked(const std::shared_ptr<AVCodecContext>& ctx);

    Status OpenCtxLocked();

    Status CloseCtxLocked();

    Status StopLocked();

    Status ResetLocked();

    Status DeInitLocked();

    template <typename T>
    Status FindInParameterMapThenAssignLocked(Tag tag, T& assign);

    Status SendBufferLocked(const std::shared_ptr<Buffer>& inputBuffer);

    Status ReceiveFrameSucc(const std::shared_ptr<Buffer>& ioInfo);

    Status ReceiveBuffer();

    Status ReceiveBufferLocked(const std::shared_ptr<Buffer>& ioInfo);

    Status SendOutputBuffer();

    void NotifyInputBufferDone(const std::shared_ptr<Buffer>& input);

    void NotifyOutputBufferDone(const std::shared_ptr<Buffer>& output);

    mutable OSAL::Mutex parameterMutex_ {};
    std::map<Tag, ValueType> audioParameter_ {};

    mutable OSAL::Mutex avMutex_ {};
    std::shared_ptr<const AVCodec> avCodec_ {};
    std::shared_ptr<AVCodecContext> avCodecContext_ {};
    std::shared_ptr<AVFrame> cachedFrame_ {};
    std::shared_ptr<AVPacket> avPacket_ {};

    std::vector<uint8_t> paddedBuffer_ {};
    size_t paddedBufferSize_ {0};
    std::shared_ptr<Buffer> outBuffer_ {nullptr};
    DataCallback* dataCallback_ {nullptr};
    int64_t preBufferGroupPts_ {0};
    int64_t curBufferGroupPts_ {0};
    int32_t bufferNum_ {1};
    int32_t bufferIndex_ {1};
    int64_t bufferGroupPtsDistance {0};
};
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_AUDIO_FFMPEG_DECODER_PLUGIN_H
