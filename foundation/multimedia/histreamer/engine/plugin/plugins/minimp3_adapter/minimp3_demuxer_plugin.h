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

#ifndef MINIMP3_DEMUXER_PLUGIN_H
#define MINIMP3_DEMUXER_PLUGIN_H

#include <memory>
#include <string>
#include <vector>

#include "core/plugin_register.h"
#include "minimp3_wrapper.h"
#include "plugin/interface/demuxer_plugin.h"
#include "foundation/osal/thread/mutex.h"

using Mp3DemuxerHandle     = Minimp3WrapperMp3dec;
using Mp3DemuxerSampleAttr = Minimp3WrapperMp3dSample;
using Mp3DemuxerFrameInfo  = Minimp3WrapperMp3decFrameInfo;

enum AudioDemuxerStatus {
    AUDIO_DEMUXER_PREPARE_UNMATCHED_FORMAT = -2,
    AUDIO_DEMUXER_ERROR = -1,
    AUDIO_DEMUXER_SUCCESS = 0,
    AUDIO_DEMUXER_PROCESS_NEED_MORE_DATA,
    AUDIO_DEMUXER_PREPARE_NEED_MORE_DATA,
    AUDIO_DEMUXER_PREPARE_NEED_SEEK,
    AUDIO_DEMUXER_SEEK_NEED_MORE_DATA,
};

struct AudioDemuxerUserArg {
    uint32_t fileSize;
    void *priv;
};

struct AudioDemuxerRst {
    uint64_t usedInputLength;
    uint8_t  *frameBuffer;
    uint32_t frameLength;
    uint64_t inputNeedOffsetSize;
    uint32_t frameBitrateKbps;
    uint32_t frameSampleRate;
    uint8_t  frameChannels;
    uint8_t  audioLayer;
    uint32_t samplesPerFrame;
};

struct AudioDemuxerMp3Attr {
    Mp3DemuxerHandle mp3DemuxerHandle;
    AudioDemuxerRst  *rst;
    void     *userArg;
    uint32_t internalRemainLen;
    uint32_t fileSize;
    uint32_t bitRate;
    uint32_t sampleRate;
    uint8_t  channelNum;

    uint8_t  id3v2SkipFlag;
    uint32_t id3v2Size;
    uint32_t id3v2Offset;

    uint32_t seekOffset;

    uint8_t mp3SeekFlag;
    uint8_t discardItemCount;

    Mp3DemuxerSampleAttr *probePcmBuf;
};

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Minimp3 {
class Minimp3DemuxerPlugin : public DemuxerPlugin {
public:
    explicit Minimp3DemuxerPlugin(std::string name);
    ~Minimp3DemuxerPlugin() override;
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
    Status GetDataFromSource();
    uint64_t GetCurrentPositionTimeS();
private:
    struct IOContext {
        std::shared_ptr<DataSource> dataSource {nullptr};
        int64_t offset {0};
        bool eos {false};
    };
    void AudioDemuxerMp3IgnoreTailZero(uint8_t *data, uint32_t *dataLen);
    static int  AudioDemuxerMp3IterateCallback(void *userData, const uint8_t *frame, int frameSize,
                                               int freeFormatBytes, size_t bufSize, uint64_t offset,
                                               Mp3DemuxerFrameInfo *info);
    static int  AudioDemuxerMp3IterateCallbackForPrepare(void *userData, const uint8_t *frame,
                                                         int frameSize, int freeFormatBytes, size_t bufSize,
                                                         uint64_t offset, Mp3DemuxerFrameInfo *info);
    void AudioDemuxerMp3Open();
    int  AudioDemuxerMp3Close();
    Status AudioDemuxerMp3Prepare(AudioDemuxerMp3Attr *mp3DemuxerAttr, uint8_t *inputBuffer,
                                  uint32_t inputLength, AudioDemuxerRst *mp3DemuxerRst);
    int AudioDemuxerMp3Process(uint8_t *buf, uint32_t len);
    int AudioDemuxerMp3FreeFrame(uint8_t *frame);
    int AudioDemuxerMp3Seek(uint32_t pos, uint8_t *buf, uint32_t len, AudioDemuxerRst *rst);
    int AudioDemuxerMp3GetSeekPosition(uint32_t targetTimeMs, uint64_t *pos);

    Status DoReadFromSource(uint32_t readSize);

    void FillInMediaInfo(MediaInfo& mediaInfo) const;

    Seekable            seekable_;
    int                 inIoBufferSize_;
    size_t              fileSize_;
    uint8_t             *inIoBuffer_;
    uint32_t            ioDataRemainSize_;
    uint64_t            currentDemuxerPos_;
    uint64_t            durationMs_;
    IOContext           ioContext_;
    AudioDemuxerRst     mp3DemuxerRst_ {};
    Minimp3DemuxerOp    minimp3DemuxerImpl_ {};
    AudioDemuxerMp3Attr mp3DemuxerAttr_ {};
};
} // namespace Minimp3
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // MINIMP3_DEMUXER_PLUGIN_H
