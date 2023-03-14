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

#ifndef HISTREAMER_VIDEO_FFMPEG_DECODER_PLUGIN_H
#define HISTREAMER_VIDEO_FFMPEG_DECODER_PLUGIN_H

#ifdef VIDEO_SUPPORT

#include <functional>
#include <map>
#include "osal/thread/task.h"
#include "utils/blocking_queue.h"
#include "plugin/interface/codec_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
};
#endif

#ifdef DUMP_RAW_DATA
#include <cstdio>
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
class VideoFfmpegDecoderPlugin : public CodecPlugin {
public:
    explicit VideoFfmpegDecoderPlugin(std::string name);
    ~VideoFfmpegDecoderPlugin() override = default;

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

    Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs) override;

    Status Flush() override;

    Status SetDataCallback(DataCallback* dataCallback) override
    {
        dataCb_ = dataCallback;
        return Status::OK;
    }

private:
    Status CreateCodecContext();

    void InitCodecContext();

    void DeinitCodecContext();

    void SetCodecExtraData();

    Status OpenCodecContext();

    Status CloseCodecContext();

    Status ResetLocked();

    template<typename T>
    void FindInParameterMapThenAssignLocked(Tag tag, T& assign);

    Status SendBufferLocked(const std::shared_ptr<Buffer>& inputBuffer);

    Status CreateSwsContext();

    Status ScaleVideoFrame();

    Status WriteYuvData(const std::shared_ptr<Buffer>& frameBuffer);

    Status WriteRgbData(const std::shared_ptr<Buffer>& frameBuffer);

#ifndef OHOS_LITE
    Status WriteYuvDataStride(const std::shared_ptr<Buffer>& frameBuffer, int32_t stride);

    Status WriteRgbDataStride(const std::shared_ptr<Buffer>& frameBuffer, int32_t stride);
#endif

    Status FillFrameBuffer(const std::shared_ptr<Buffer>& frameBuffer);

    Status ReceiveBufferLocked(const std::shared_ptr<Buffer>& frameBuffer);

    void ReceiveFrameBuffer();

#ifdef DUMP_RAW_DATA
    std::FILE* dumpFd_;
    void DumpVideoRawOutData();
#endif

    void NotifyInputBufferDone(const std::shared_ptr<Buffer>& input);

    void NotifyOutputBufferDone(const std::shared_ptr<Buffer>& output);

    std::shared_ptr<const AVCodec> avCodec_;
    std::shared_ptr<struct SwsContext> swsCtx_ {nullptr};
    std::map<Tag, ValueType> videoDecParams_ {};
    std::vector<uint8_t> paddedBuffer_;
    size_t paddedBufferSize_ {0};
    std::shared_ptr<AVFrame> cachedFrame_ {nullptr};
    std::shared_ptr<AVPacket> avPacket_ {};

    uint8_t* scaleData_[AV_NUM_DATA_POINTERS];
    int32_t scaleLineSize_[AV_NUM_DATA_POINTERS];
    bool isAllocScaleData_ {false};
    DataCallback* dataCb_ {};

    uint32_t width_;
    uint32_t height_;
    VideoPixelFormat pixelFormat_;

    mutable OSAL::Mutex avMutex_ {};
    State state_ {State::CREATED};
    std::shared_ptr<AVCodecContext> avCodecContext_ {};
    OHOS::Media::BlockingQueue<std::shared_ptr<Buffer>> outBufferQ_;
    std::shared_ptr<OHOS::Media::OSAL::Task> decodeTask_;
};
}
}
}
}
#endif
#endif // HISTREAMER_VIDEO_FFMPEG_DECODER_PLUGIN_H
