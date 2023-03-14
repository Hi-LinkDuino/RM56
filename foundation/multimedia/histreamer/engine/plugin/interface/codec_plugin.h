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

#ifndef HISTREAMER_PLUGIN_INTF_CODEC_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_CODEC_PLUGIN_H

#include <vector>
#include "common/plugin_caps.h"
#include "plugin_base.h"
#include "plugin_definition.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief Plugin data callback interface.
 *
 * @since 1.0
 * @version 1.0
 */
struct DataCallback {
    virtual ~DataCallback() = default;

    /**
     * @brief When the input buffer has been consumed inside the plugin.
     *
     * This function works with QueueInputBuffer to implement input data transmission.
     *
     * @param input Indicates the pointer to the input data.
     */
    virtual void OnInputBufferDone(const std::shared_ptr<Buffer>& input) = 0;

    /**
     * @brief When the out buffer has been produced inside the plugin.
     *
     * This function works with QueueOutputBuffer to implement out data transmission.
     *
     * @param output Indicates the pointer to the output data.
     */
    virtual void OnOutputBufferDone(const std::shared_ptr<Buffer>& output) = 0;
};

/**
 * @brief Codec Plugin Interface.
 *
 * Used for audio and video encoding and decoding.
 *
 * @since 1.0
 * @version 1.0
 */
struct CodecPlugin : public PluginBase {
    /// constructor
    explicit CodecPlugin(std::string name): PluginBase(std::move(name)) {}

    /**
     * @brief Queues input data
     *
     * This function works with DataCallback::OnInputBufferDone to implement input data transmission.
     *
     * The function is valid only in the RUNNING state.
     *
     * @param inputBuffer  Indicates the pointer to the input data.
     * @param timeoutMs    Indicates the timeout duration.
     * @return  Execution status return
     *  @retval OK: Plugin QueueInputBuffer succeeded.
     *  @retval ERROR_INVALID_DATA: The input buffer is invalid.
     *  @retval ERROR_TIMED_OUT: Operation timeout.
     */
    virtual Status QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs) = 0;

    /**
     * @brief Queues output data
     *
     * This function works with DataCallback::OnOutputBufferDone to implement output data transmission.
     *
     * The function is valid only in the RUNNING state.
     *
     * @param outputBuffers Indicates the pointer to the output data.
     * @param timeoutMs     Indicates the timeout duration.
     * @return  Execution status return
     *  @retval OK: Plugin QueueOutputBuffer succeeded.
     *  @retval ERROR_INVALID_DATA: The output buffer is invalid.
     *  @retval ERROR_TIMED_OUT: Operation timeout.
     */
    virtual Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffers, int32_t timeoutMs) = 0;

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
     * @brief Sets the plugin callback data to notify the plugin user.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param cb   Data callback, NULL callback listening is canceled.
     * @return Execution status return
     * @retval OK: Plugin SetDataCallback succeeded.
     */
    virtual Status SetDataCallback(DataCallback* dataCallback) = 0;
};

/// Codec plugin api major number.
#define CODEC_API_VERSION_MAJOR (1)

/// Codec plugin api minor number
#define CODEC_API_VERSION_MINOR (0)

/// Codec plugin version
#define CODEC_API_VERSION MAKE_VERSION(CODEC_API_VERSION_MAJOR, CODEC_API_VERSION_MINOR)

/**
 * @brief The Codec Type.
 *
 * @since 1.0
 * @version 1.0
 */
enum struct CodecType {
    AUDIO_DECODER, ///< Audio decoder
    AUDIO_ENCODER, ///< Audio encoder
    VIDEO_DECODER, ///< video decoder
    VIDEO_ENCODER, ///< video encoder
};

/**
 * @brief Describes the codec plugin information.
 *
 * @since 1.0
 * @version 1.0
 */
struct CodecPluginDef : public PluginDefBase {
    CodecType codecType {};
    CapabilitySet inCaps {};                   ///< Plug-in input capability, For details, @see Capability.
    CapabilitySet outCaps {};                  ///< Plug-in output capability, For details, @see Capability.
    PluginCreatorFunc<CodecPlugin> creator {nullptr}; ///< Codec plugin create function.
    CodecPluginDef()
    {
        apiVersion = CODEC_API_VERSION; ///< Codec plugin version
        pluginType = PluginType::CODEC; ///< Plugin type, MUST be CODEC.
    }
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INTF_CODEC_PLUGIN_H
