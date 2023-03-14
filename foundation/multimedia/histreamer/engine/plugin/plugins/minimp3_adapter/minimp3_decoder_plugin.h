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

#ifndef HISTREAMER_MINIMP3_DECODER_PLUGIN_H
#define HISTREAMER_MINIMP3_DECODER_PLUGIN_H

#include <functional>
#include <map>

#include "minimp3_wrapper.h"
#include "plugin/interface/codec_plugin.h"
#include "plugin/common/plugin_types.h"
#include "utils/blocking_queue.h"

using Mp3DecoderHandle = Minimp3WrapperMp3dec;
using Mp3DecoderSample = Minimp3WrapperMp3dSample;

struct AudioDecoderRst {
    uint32_t usedInputLength;
    uint8_t *packetBuffer;
    uint32_t packetLength;
};

struct AudioDecoderMp3Attr {
    Mp3DecoderHandle  mp3DecoderHandle;
    Mp3DecoderSample *pcm;
    AudioDecoderRst  *rst;
};

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Minimp3 {
class Minimp3DecoderPlugin : public CodecPlugin {
public:
    explicit Minimp3DecoderPlugin(std::string name);

    ~Minimp3DecoderPlugin() override;

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

    Status GetPcmDataProcess(const std::shared_ptr<Buffer>& inputBuffer, std::shared_ptr<Buffer>& outputBuffer);

    Status QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs) override;

    Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs) override;

    Status Flush() override;

    Status SetDataCallback(DataCallback* dataCallback) override;

private:

    void AudioDecoderMp3Open();

    int  AudioDecoderMp3Close();

    Status AudioDecoderMp3Process(std::shared_ptr<Buffer> inBuffer, std::shared_ptr<Buffer> outBuffer);

    int  AudioDecoderMp3FreePacket(uint8_t *packet);

    Status SendOutputBuffer();

    mutable OSAL::Mutex         ioMutex_{};
    uint32_t                    samplesPerFrame_;
    uint32_t                    channels_;
    State                       state_{State::CREATED};
    Minimp3DemuxerOp            minimp3DecoderImpl_{};
    AudioDecoderMp3Attr         mp3DecoderAttr_{};
    std::map<Tag, ValueType>    mp3Parameter_ {};
    std::shared_ptr<Buffer>     inputBuffer_ {nullptr};
    std::shared_ptr<Buffer>     outputBuffer_ {nullptr};
    DataCallback* dataCb_ {};
};
} // namespace Minimp3
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_MINIMP3_DECODER_PLUGIN_H