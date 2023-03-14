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

#ifndef HISTREAMER_AUDIO_AAC_DECODER_PLUGIN_H
#define HISTREAMER_AUDIO_AAC_DECODER_PLUGIN_H

#include <functional>
#include <map>
#include "plugin/interface/codec_plugin.h"
#include "plugin/common/plugin_types.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "aac_wrapper.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace LiteAacPlugin {
class LiteAACDecoderPlugin : public CodecPlugin {
public:
    explicit LiteAACDecoderPlugin(std::string name);

    ~LiteAACDecoderPlugin() override;

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

    Status QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs) override;

    Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs) override;

    Status Flush() override;

    Status SetDataCallback(DataCallback* dataCallback) override;

    Status SendOutputBuffer();

private:
    Status ReceiveBuffer();

    Status ReceiveBufferLocked(const std::shared_ptr<Buffer> &ioInfo);

    Status AudioDecoderAACMp4Open();

    void  AudioDecoderAACMp4Close();

    Status  AudioDecoderAACMp4Process(std::shared_ptr<Buffer> inBuffer, std::shared_ptr<Buffer> outBuffer);

    std::map<Tag, ValueType> audioParameter_ {};
    mutable OSAL::Mutex avMutex_ {};
    DataCallback* dataCb_ {};
    std::shared_ptr<Buffer> inBuffer_ {nullptr};
    std::shared_ptr<Buffer> outBuffer_ {nullptr};
};
} // LiteAacPlugin
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_MINIMP4_AAC_DECODER_PLUGIN_H

