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

#ifndef HISTREAMER_PLUGIN_INTF_AUDIO_SINK_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_AUDIO_SINK_PLUGIN_H

#include "common/plugin_buffer.h"
#include "common/plugin_caps.h"
#include "plugin_base.h"
#include "plugin_definition.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief Audio Sink Plugin.
 *
 * Component that receives media streams.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioSinkPlugin : public PluginBase {
    /// constructor
    explicit AudioSinkPlugin(std::string name): PluginBase(std::move(name)) {}
    /**
     * @brief Get the mute operation set for the audio.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param mute  Indicates the mute operation set for the audio.
     *              Value true means that the audio is muted, and false means the opposite.
     * @return   Execution status return
     *  @retval OK: Plugin GetMute succeeded.
     */
    virtual Status GetMute(bool& mute) = 0;

    /**
     * @brief Set the mute operation for the audio.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param mute  Indicates the mute operation set for the audio.
     *              Value true means that the audio is muted, and false means the opposite.
     * @return  Execution status return
     *  @retval OK: Plugin SetMute succeeded.
     */
    virtual Status SetMute(bool mute) = 0;

    /**
     * @brief Get the audio volume.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param volume    Indicates the volume to set. The value ranges from 0.0 to 1.0.
     * @return  Execution status return
     *  @retval OK: Plugin GetVolume succeeded.
     */
    virtual Status GetVolume(float& volume) = 0;

    /**
     * @brief Set the audio volume.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param volume    Indicates the volume to set. The value ranges from 0.0 to 1.0.
     * @return  Execution status return
     *  @retval OK: Plugin SetVolume succeeded.
     *  @retval ERROR_INVALID_DATA: The value is not in the valid range.
     */
    virtual Status SetVolume(float volume) = 0;

    /**
     * @brief Get the current audio rendering speed.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param speed Indicates the pointer to the current rendering speed to obtain.
     * @return  Execution status return
     *  @retval OK: Plugin GetSpeed succeeded.
     */
    virtual Status GetSpeed(float& speed) = 0;

    /**
     * @brief Set the audio rendering speed.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param speed speed Indicates the pointer to the current rendering speed to obtain.
     * @return  Execution status return
     *  @retval OK: Plugin SetSpeed succeeded.
     *  @retval ERROR_INVALID_DATA: The value is not in the valid range.
     */
    virtual Status SetSpeed(float speed) = 0;

    /**
     * @brief Pauses audio rendering
     *
     * The function is valid only in the RUNNING state. If the pause is successful,
     * the plugin enters the PAUSED state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Pause succeeded.
     */
    virtual Status Pause() = 0;

    /**
     * @brief Resumes audio rendering
     *
     * The function is valid only in the PAUSED state. If the resume is successful,
     * the plugin enters the RUNNING state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Resume succeeded.
     */
    virtual Status Resume() = 0;

    /**
     * @brief Get the estimated latency of the audio device driver.
     *
     * The function is valid only in the after PREPARED state.
     *
     * @param hstTime  latency times based on {@link HST_TIME_BASE}.
     * @return  Execution status return
     *  @retval OK: Plugin GetLatency succeeded.
     */
    virtual Status GetLatency(uint64_t& hstTime) = 0;

    /**
     * @brief Get the audio frame size, that is, the length (in bytes) of a frame.
     *
     * The function is valid only in the RUNNING state.
     *
     * @param size  size Indicates the pointer to the audio frame size (in bytes).
     * @return  Execution status return
     *  @retval OK: Plugin GetFrameSize succeeded.
     */
    virtual Status GetFrameSize(size_t& size) = 0;

    /**
     * @brief Get the number of audio frames in the audio buffer.
     *
     * The function is valid only in the RUNNING state.
     *
     * @param count Indicates the pointer to the number of audio frames in the audio buffer.
     * @return  Execution status return
     *  @retval OK: Plugin GetFrameCount succeeded.
     */
    virtual Status GetFrameCount(uint32_t& count) = 0;

    /**
     * @brief Writes a frame of output data into the audio driver for rendering.
     *
     * The function is valid only in the RUNNING state.
     *
     * @param input Indicates the pointer to the frame to write.
     * @return  Execution status return
     *  @retval OK: Plugin Write succeeded.
     */
    virtual Status Write(const std::shared_ptr<Buffer>& input) = 0;

    /**
     * @brief Flushes data in the audio buffer.
     *
     * The function is valid only in after RUNNING state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Flush succeeded.
     */
    virtual Status Flush() = 0;

    /**
     * @brief Drain data to make sure all the data processed.
     *
     * The function is valid only in RUNNING state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Drain succeeded.
     */
    virtual Status Drain() = 0;
};

/// Audio sink plugin api major number.
#define AUDIO_SINK_API_VERSION_MAJOR (1)

/// Audio sink plugin api minor number
#define AUDIO_SINK_API_VERSION_MINOR (0)

/// Audio sink plugin version
#define AUDIO_SINK_API_VERSION MAKE_VERSION(AUDIO_SINK_API_VERSION_MAJOR, AUDIO_SINK_API_VERSION_MINOR)

/**
 * @brief Describes the audio sink plugin information.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioSinkPluginDef : public PluginDefBase {
    CapabilitySet inCaps {};                       ///< Plug-in input capability, For details, @see Capability.
    PluginCreatorFunc<AudioSinkPlugin> creator {nullptr}; ///< Audio sink plugin create function.
    AudioSinkPluginDef()
    {
        apiVersion = AUDIO_SINK_API_VERSION; ///< Audio sink plugin version.
        pluginType = PluginType::AUDIO_SINK; ///< Plugin type, MUST be AUDIO_SINK.
    }
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INTF_AUDIO_SINK_PLUGIN_H
