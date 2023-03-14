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
#ifndef MEDIA_PIPELINE_VIDEO_CAPTURE_FILTER_H
#define MEDIA_PIPELINE_VIDEO_CAPTURE_FILTER_H

#if defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#include <memory>
#include <string>

#include "osal/thread/task.h"
#include "osal/utils/util.h"
#include "utils/constants.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/filter_base.h"
#include "plugin/common/plugin_video_tags.h"
#include "plugin/core/plugin_manager.h"
#include "plugin/interface/source_plugin.h"
#include "pipeline/core/type_define.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class VideoCaptureFilter : public FilterBase {
public:
    explicit VideoCaptureFilter(const std::string& name);
    ~VideoCaptureFilter() override;

    std::vector<WorkMode> GetWorkModes() override;
    ErrorCode SetParameter(int32_t key, const Plugin::Any& value) override;
    ErrorCode GetParameter(int32_t key, Plugin::Any& value) override;
    ErrorCode Prepare() override;
    ErrorCode Start() override;
    ErrorCode Stop() override;
    ErrorCode Pause() override;
    ErrorCode Resume() override;
    ErrorCode SendEos();
private:
    void InitPorts() override;
    ErrorCode InitAndConfigPlugin(const std::shared_ptr<Plugin::Meta>& videoMeta);
    void ReadLoop();
    ErrorCode CreatePlugin(const std::shared_ptr<Plugin::PluginInfo>& info, const std::string& name,
                           Plugin::PluginManager& manager);
    ErrorCode FindPlugin();
    bool DoNegotiate(const CapabilitySet& outCaps);
    ErrorCode DoConfigure();
    void SendBuffer(const std::shared_ptr<AVBuffer>& buffer);

    std::shared_ptr<OSAL::Task> taskPtr_ {nullptr};
    std::shared_ptr<Plugin::Source> plugin_ {nullptr};
    std::shared_ptr<Allocator> pluginAllocator_ {nullptr};
    std::shared_ptr<Plugin::PluginInfo> pluginInfo_ {nullptr};
    Plugin::SrcInputType inputType_ {};
    bool inputTypeSpecified_ {false};
    std::string mime_ {"video/raw"};
    uint32_t videoWidth_ {0};
    uint32_t videoHeight_ {0};
    double captureRate_ {0};
    int64_t bitRate_ {0};
    uint32_t frameRate_ {0};
    Plugin::VideoPixelFormat pixelFormat_ {Plugin::VideoPixelFormat::YUV420P};
    Capability capNegWithDownstream_ {};
    std::atomic<bool> isEos_ {false};
    OSAL::Mutex pushMutex_ {};
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif
#endif // MEDIA_PIPELINE_VIDEO_CAPTURE_FILTER_H

