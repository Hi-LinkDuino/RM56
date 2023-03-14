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

#ifndef HISTREAMER_PLUGIN_CORE_VIDEO_SINK_H
#define HISTREAMER_PLUGIN_CORE_VIDEO_SINK_H

#include <cstddef>
#include "base.h"
#include "common/plugin_buffer.h"
#include "common/plugin_tags.h"
#include "common/plugin_types.h"
#include "memory"

namespace OHOS {
namespace Media {
namespace Plugin {
struct VideoSinkPlugin;

class VideoSink : public Base {
public:
    VideoSink(const VideoSink&) = delete;
    VideoSink operator=(const VideoSink&) = delete;
    ~VideoSink() override = default;

    Status Pause();

    Status Resume();

    Status Flush();

    Status Write(const std::shared_ptr<Buffer>& input);

    Status GetLatency(uint64_t& hstTime);
private:
    friend class PluginManager;

    VideoSink(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<VideoSinkPlugin> plugin);

private:
    std::shared_ptr<VideoSinkPlugin> videoSink;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_VIDEO_SINK_H
