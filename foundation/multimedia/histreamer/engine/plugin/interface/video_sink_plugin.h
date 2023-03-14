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

#ifndef HISTREAMER_PLUGIN_INTF_VIDEO_SINK_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_VIDEO_SINK_PLUGIN_H

#include "common/plugin_buffer.h"
#include "common/plugin_caps.h"
#include "plugin_base.h"
#include "plugin_definition.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief Video Sink Plugin.
 *
 * Component that receives media streams.
 *
 * @since 1.0
 * @version 1.0
 */
struct VideoSinkPlugin : public PluginBase {
    /// constructor
    explicit VideoSinkPlugin(std::string name): PluginBase(std::move(name)) {}
    /**
     * @brief Pauses video rendering
     *
     * The function is valid only in the RUNNING state. If the pause is successful,
     * the plugin enters the PAUSED state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Pause succeeded.
     */
    virtual Status Pause() = 0;

    /**
     * @brief Resumes video rendering
     *
     * The function is valid only in the PAUSED state. If the resume is successful,
     * the plugin enters the RUNNING state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Resume succeeded.
     */
    virtual Status Resume() = 0;

    /**
     * @brief Writes a frame of output data into the video display device for rendering.
     *
     * The function is valid only in the RUNNING state.
     *
     * @param input Indicates the pointer to the frame to write.
     * @return  Execution status return
     *  @retval OK: Plugin Write succeeded.
     */
    virtual Status Write(const std::shared_ptr<Buffer>& input) = 0;

    /**
     * @brief Flushes data in the video buffer.
     *
     * The function is valid only in after RUNNING state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Flush succeeded.
     */
    virtual Status Flush() = 0;

    /**
      * @brief Get the estimated latency of the video device driver.
      *
      * The function is valid only in the after PREPARED state.
      *
      * @param nanoSec    Indicates the pointer to the latency (in nanoseconds) to be obtained.
      * @return  Execution status return
      *  @retval OK: Plugin GetLatency succeeded.
      */
    virtual Status GetLatency(uint64_t& nanoSec) = 0;
};

/// Video sink plugin api major number.
#define VIDEO_SINK_API_VERSION_MAJOR (1)

/// Video sink plugin api minor number
#define VIDEO_SINK_API_VERSION_MINOR (0)

/// Video sink plugin version
#define VIDEO_SINK_API_VERSION MAKE_VERSION(VIDEO_SINK_API_VERSION_MAJOR, VIDEO_SINK_API_VERSION_MINOR)

/**
 * @brief Describes the video sink plugin information.
 *
 * @since 1.0
 * @version 1.0
 */
struct VideoSinkPluginDef : public PluginDefBase {
    CapabilitySet inCaps;                       ///< Plug-in input capability, For details, @see Capability.
    PluginCreatorFunc<VideoSinkPlugin> creator; ///< Video sink plugin create function.
    VideoSinkPluginDef()
    {
        apiVersion = VIDEO_SINK_API_VERSION; ///< Video sink plugin version.
        pluginType = PluginType::VIDEO_SINK; ///< Plugin type, MUST be VIDEO_SINK.
    }
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INTF_VIDEO_SINK_PLUGIN_H
