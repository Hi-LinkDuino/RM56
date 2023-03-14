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

#ifndef AAC_DEMUXER_PLUGIN_H
#define AAC_DEMUXER_PLUGIN_H

#include <memory>
#include <string>
#include <vector>

#include "interface/demuxer_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace AacDemuxer {
struct AudioDemuxerUserArg {
    uint32_t fileSize;
    void *priv;
};

struct AACDemuxerRst {
    uint64_t usedInputLength;
    uint8_t  *frameBuffer;
    uint32_t frameLength;
    uint64_t inputNeedOffsetSize;

    uint32_t frameBitrateKbps;
    uint32_t frameSampleRate;
    uint8_t  frameChannels;
    uint8_t  mpegVersion;
    uint8_t  audioLayer;
    uint32_t samplesPerFrame;
};

class AACDemuxerPlugin : public DemuxerPlugin {
public:
    explicit AACDemuxerPlugin(std::string name);
    ~AACDemuxerPlugin() override;
    Status Init()    override;
    Status Deinit()  override;
    Status Prepare() override;
    Status Reset()   override;
    Status Start()   override;
    Status Stop()    override;
    Status GetParameter(Tag tag, ValueType& value) override;
    Status SetParameter(Tag tag, const ValueType& value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;

    Status SetDataSource(const std::shared_ptr<DataSource>& source) override;
    Status GetMediaInfo(MediaInfo& mediaInfo) override;
    Status ReadFrame(Buffer& outBuffer, int32_t timeOutMs) override;
    Status SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode) override;
    size_t GetTrackCount() override;
    Status SelectTrack(int32_t trackId) override;
    Status UnselectTrack(int32_t trackId) override;
    Status GetSelectedTracks(std::vector<int32_t>& trackIds) override;

private:
    struct IOContext {
        std::shared_ptr<DataSource> dataSource {nullptr};
        int64_t offset {0};
        bool eos {false};
    };
    Status DoReadFromSource(uint32_t readSize);
    Status GetDataFromSource();
    int GetFrameLength(const uint8_t *data);
    int AudioDemuxerAACOpen(AudioDemuxerUserArg *userArg);
    int AudioDemuxerAACClose();
    int AudioDemuxerAACPrepare(const uint8_t *buf, uint32_t len, AACDemuxerRst *rst);
    int AudioDemuxerAACProcess(const uint8_t *buffer, uint32_t bufferLen, AACDemuxerRst *rst);
    int AudioDemuxerAACFreeFrame(uint8_t *frame);

    AACDemuxerRst aacDemuxerRst_;
    IOContext ioContext_;
    size_t fileSize_;
    bool isSeekable_;
    unsigned char *inIoBuffer_;
    unsigned int ioDataRemainSize_;
    int inIoBufferSize_;
};
} // namespace AacDemuxer
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // AAC_DEMUXER_PLUGIN_H