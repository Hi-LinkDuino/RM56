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

#ifndef HISTREAMER_UTCODECTEST1_H
#define HISTREAMER_UTCODECTEST1_H

#include "plugin/interface/codec_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
class UtCodecTest1 : public CodecPlugin {
public:
    explicit UtCodecTest1(std::string name) : CodecPlugin(std::move(name))
    {
    }

    ~UtCodecTest1() override = default;

    Status QueueInputBuffer(const std::shared_ptr<Buffer> &inputBuffer, int32_t timeoutMs) override;

    Status QueueOutputBuffer(const std::shared_ptr<Buffer> &outputBuffers, int32_t timeoutMs) override;

    Status Flush() override;

    Status SetDataCallback(DataCallback* dataCallback) override;

    std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_UTCODECTEST1_H
