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

#ifndef HISTREAMER_PLUGIN_CORE_CODEC_H
#define HISTREAMER_PLUGIN_CORE_CODEC_H

#include <memory>

#include "base.h"
#include "common/plugin_types.h"
#include "common/plugin_tags.h"
#include "common/plugin_buffer.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct DataCallbackHelper {
    virtual ~DataCallbackHelper() = default;
    virtual void OnInputBufferDone(const std::shared_ptr<Buffer> &input) = 0;
    virtual void OnOutputBufferDone(const std::shared_ptr<Buffer> &output) = 0;
};

struct CodecPlugin;

struct DataCallback;

class Codec : public Base {
public:
    Codec(const Codec &) = delete;

    Codec operator=(const Codec &) = delete;

    ~Codec() override = default;

    Status QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs);

    Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs);

    Status Flush();

    Status SetDataCallback(DataCallbackHelper* dataCallback);

private:
    friend class PluginManager;

    Codec(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<CodecPlugin> plugin);

private:
    std::shared_ptr<CodecPlugin> codec_;

    std::shared_ptr<DataCallback> dataCallback_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_CODEC_H
