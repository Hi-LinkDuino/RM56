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

#ifndef HISTREAMER_PLUGIN_CORE_OUTPUT_SINK_H
#define HISTREAMER_PLUGIN_CORE_OUTPUT_SINK_H

#include "base.h"
#include "common/plugin_buffer.h"
#include "plugin/common/media_sink.h"
#include "plugin/interface/output_sink_plugin.h"

namespace OHOS {
namespace Media {
namespace Plugin {
class OutputSink : public Base {
public:
    OutputSink(const OutputSink &) = delete;
    OutputSink operator = (const OutputSink &) = delete;
    ~OutputSink() override = default;

    Status SetSink(const MediaSink& sink);
    Seekable GetSeekable();
    Status SeekTo(uint64_t offset);
    Status Write(const std::shared_ptr<Buffer>& buffer);
    Status Flush();
private:
    friend class PluginManager;

    OutputSink(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<OutputSinkPlugin> plugin);

private:
    std::shared_ptr<OutputSinkPlugin> outputSink_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_OUTPUT_SINK_H
