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

#ifndef HISTREAMER_VIDEO_CAPTURE_PLUGIN_H
#define HISTREAMER_VIDEO_CAPTURE_PLUGIN_H

#if !defined(OHOS_LITE) && defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#include <atomic>
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"
#include "plugin/common/plugin_types.h"
#include "plugin/interface/source_plugin.h"
#include "refbase.h"
#include "surface/surface.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace VideoCapture {
class VideoCapturePlugin : public SourcePlugin {
public:
    explicit VideoCapturePlugin(std::string name);
    ~VideoCapturePlugin() override;

    Status Init() override;
    Status Deinit() override;
    Status Prepare() override;
    Status Reset() override;
    Status Start() override;
    Status Stop() override;
    Status GetParameter(Tag tag, ValueType& value) override;
    Status SetParameter(Tag tag, const ValueType& value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;
    Status SetSource(std::shared_ptr<MediaSource> source) override;
    Status Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen) override;
    Status GetSize(size_t& size) override;
    Seekable GetSeekable() override;
    Status SeekTo(uint64_t offset) override;

protected:
    class SurfaceConsumerListener : public IBufferConsumerListener {
    public:
        explicit SurfaceConsumerListener(VideoCapturePlugin &owner) : owner_(owner) {}
        ~SurfaceConsumerListener() = default;
        void OnBufferAvailable() override;
    private:
        VideoCapturePlugin &owner_;
    };

private:
    void ConfigSurfaceConsumer();
    Status AcquireSurfaceBuffer();
    void OnBufferAvailable();
    void SetVideoBufferMeta(std::shared_ptr<BufferMeta>& bufferMeta);

    OSAL::Mutex mutex_ {};
    OSAL::ConditionVariable readCond_;
    sptr<Surface> surfaceConsumer_ {nullptr};
    sptr<Surface> surfaceProducer_ {nullptr};
    std::atomic<bool> isStop_ {false};
    uint32_t width_ {0};
    uint32_t height_ {0};
    double captureRate_ {0.0};
    VideoPixelFormat pixelFormat_;

    uint32_t bufferCnt_ {0};
    uint64_t curTimestampNs_ {0};
    uint64_t stopTimestampNs_ {0};
    uint64_t totalPauseTimeNs_ {0};

    sptr<SurfaceBuffer> surfaceBuffer_ {nullptr};
    int32_t fence_ {-1};
    int64_t timestamp_ {0};
    int32_t bufferSize_ {0};
    Rect damage_;
    int32_t isKeyFrame_ {0};
};
} // namespace VideoCapture
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
#endif // HISTREAMER_VIDEO_CAPTURE_PLUGIN_H

