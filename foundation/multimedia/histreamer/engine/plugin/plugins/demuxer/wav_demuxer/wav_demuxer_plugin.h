/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef WAV_DEMUXER_PLUGIN_H
#define WAV_DEMUXER_PLUGIN_H

#include <memory>
#include <string>
#include <vector>

#include "core/plugin_register.h"
#include "plugin/interface/demuxer_plugin.h"
#include "foundation/osal/thread/mutex.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace WavPlugin {
struct WavHeadAttr {
    uint8_t  chunkID[4];     // 4 byte RIFF Chunk descriptor
    uint32_t chunkSize;      // 4 byte RIFF Chunk Size
    uint8_t  format[4];      // 4 byte The format concern here is "WAVE",
    // which requires two or three sub-chunks:"fmt", "fact"(optional),and "data"

    uint8_t  subChunk1ID[4]; // 4 byte The "fmt" sub-chunk describes the sound information in the data sub-chunk
    uint32_t subChunk1Size;  // 4 byte Size of the fmt chunk  16 or 18  18 stand for has auxiliary information
    uint16_t audioFormat;    // 2 byte Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t numChannels;    // 2 byte Number of channels 1=Mono 2=Sterio
    uint32_t sampleRate;     // 4 byte Sampling Frequency in Hz
    uint32_t byteRate;       // 4 byte bytes per second
    uint16_t blockAlign;     // 2 byte 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample;  // 2 byte Number of bits per sample

    uint8_t  subChunk2ID[4]; // 4 byte (optional) The "fact" sub-chunk exists in non-PCM format
    uint32_t subChunk2Size;  // 4 byte (optional)
    uint32_t dataFactSize;   // 4 byte (optional)

    uint8_t  subChunk3ID[4]; // 4 byte The "data" sub-chunk indicates the size of the sound information
    uint32_t subChunk3Size;  // 4 byte Sampled data length
}; // 根据wav协议构建的结构体，轻易勿动

class WavDemuxerPlugin : public DemuxerPlugin {
public:
    explicit WavDemuxerPlugin(std::string name);
    ~WavDemuxerPlugin() override;

    Status SetDataSource(const std::shared_ptr<DataSource>& source) override;
    Status GetMediaInfo(MediaInfo& mediaInfo) override;
    Status ReadFrame(Buffer& outBuffer, int32_t timeOutMs) override;
    Status SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode) override;
    Status Reset() override;
    Status GetParameter(Tag tag, ValueType &value) override;
    Status SetParameter(Tag tag, const ValueType &value) override;
    std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;
    size_t GetTrackCount() override;
    Status SelectTrack(int32_t trackId) override;
    Status UnselectTrack(int32_t trackId) override;
    Status GetSelectedTracks(std::vector<int32_t>& trackIds) override;
    Status GetDataFromSource();
private:
    struct IOContext {
        std::shared_ptr<DataSource> dataSource {nullptr};
        int64_t offset {0};
        bool eos {false};
    };
    size_t              fileSize_;
    IOContext           ioContext_;
    uint32_t            dataOffset_;
    Seekable            seekable_;
    uint32_t            wavHeadLength_;
    WavHeadAttr         wavHeader_;
};
} // namespace WavPlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // WAV_DEMUXER_PLUGIN_H
