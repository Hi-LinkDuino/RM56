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

#ifndef HISTREAMER_FFMPEG_MUXER_PLUGIN_H
#define HISTREAMER_FFMPEG_MUXER_PLUGIN_H

#include "plugin/interface/muxer_plugin.h"
#include "foundation/osal/thread/mutex.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
class FFmpegMuxerPlugin : public MuxerPlugin {
public:
    explicit FFmpegMuxerPlugin(std::string name);

    ~FFmpegMuxerPlugin() override;

    Status Init() override;

    Status Deinit() override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;

    Status GetParameter(Tag tag, ValueType& value) override;

    Status SetParameter(Tag tag, const ValueType& value) override;

    Status Prepare() override;

    Status Reset() override;

    Status AddTrack(uint32_t& trackId) override;

    Status SetTrackParameter(uint32_t trackId, Tag tag, const ValueType& value) override;

    Status GetTrackParameter(uint32_t trackId, Tag tag, ValueType& value) override;

    Status SetDataSink(const std::shared_ptr<DataSink>& dataSink) override;

    Status WriteHeader() override;

    Status WriteFrame(const std::shared_ptr<Buffer>& buffer) override;

    Status WriteTrailer() override;

private:
    struct IOContext {
        std::shared_ptr<DataSink> dataSink_ {};
        int64_t pos_ {0};
        int64_t end_ {0};
    };

    static int32_t IoRead(void* opaque, uint8_t* buf, int bufSize);

    static int32_t IoWrite(void* opaque, uint8_t* buf, int bufSize);

    static int64_t IoSeek(void* opaque, int64_t offset, int whence);

    AVIOContext* InitAvIoCtx();

    static void DeInitAvIoCtx(AVIOContext* ptr);

    static void ResetIoCtx(IOContext& ioContext);

    Status InitFormatCtxLocked();

    Status Release();

    std::shared_ptr<AVOutputFormat> outputFormat_{};

    std::map<uint32_t, TagMap> trackParameters_{};

    TagMap generalParameters_{};

    OSAL::Mutex fmtMutex_{};
    std::shared_ptr<AVFormatContext> formatContext_{};

    std::shared_ptr<AVPacket> cachePacket_ {};

    IOContext ioContext_;
};
} // Ffmpeg
} // Plugin
} // Media
} // OHOS
#endif // HISTREAMER_FFMPEG_MUXER_PLUGIN_H
