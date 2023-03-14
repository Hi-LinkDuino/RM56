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

#ifndef HISTREAMER_PLUGIN_INTF_SOURCE_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_SOURCE_PLUGIN_H

#include <map>
#include <string>
#include "common/media_source.h"
#include "common/plugin_caps.h"
#include "common/plugin_source_tags.h"
#include "plugin_base.h"
#include "plugin_definition.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * @brief Source Plugin Interface.
 *
 * The data source may be network push or active read.
 *
 * @since 1.0
 * @version 1.0
 */
struct SourcePlugin : public PluginBase {
    /// constructor
    explicit SourcePlugin(std::string name): PluginBase(std::move(name)) {}
    /**
     * @brief Set the data source to source plugin.
     *
     * The function is valid only in the CREATED state.
     *
     * @param source data source, uri or stream source
     * @return  Execution status return
     *  @retval OK: Plugin SetSource succeeded.
     *  @retval ERROR_WRONG_STATE: Call this function in non wrong state
     *  @retval ERROR_NOT_EXISTED: Uri is not existed.
     *  @retval ERROR_UNSUPPORTED_FORMAT: Uri is not supported.
     *  @retval ERROR_INVALID_PARAMETER: Uri is invalid.
     */
    virtual OHOS::Media::Plugin::Status SetSource(std::shared_ptr<MediaSource> source) = 0;

    /**
     * @brief Read data from data source.
     *
     * The function is valid only after RUNNING state.
     *
     * @param buffer Buffer to store the data, it can be nullptr or empty to get the buffer from plugin.
     * @param expectedLen   Expected data size to be read
     * @return  Execution status return
     *  @retval OK: Plugin Read succeeded.
     *  @retval ERROR_NOT_ENOUGH_DATA: Data not enough
     *  @retval END_OF_STREAM: End of stream
     */
    virtual Status Read(std::shared_ptr<Buffer>& buffer, size_t expectedLen) = 0;

    /**
     * @brief Get data source size.
     *
     * The function is valid only after INITIALIZED state.
     *
     * @param size data source size.
     * @return  Execution status return.
     *  @retval OK: Plugin GetSize succeeded.
     */
    virtual Status GetSize(size_t& size) = 0;

    /**
     * @brief Indicates that the current source can be seek.
     *
     * The function is valid only after INITIALIZED state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin GetSeekable succeeded.
     */
    virtual Seekable GetSeekable() = 0;

    /**
     * @brief Seeks for a specified position for the source.
     *
     * After being started, the source seeks for a specified position to read data frames.
     *
     * The function is valid only after RUNNING state.
     *
     * @param offset position to read data frames
     * @return  Execution status return
     *  @retval OK: Plugin SeekTo succeeded.
     *  @retval ERROR_INVALID_DATA: The offset is invalid.
     */
    virtual Status SeekTo(uint64_t offset) = 0;
};

/// Source plugin api major number.
#define SOURCE_API_VERSION_MAJOR (1)

/// Source plugin api minor number
#define SOURCE_API_VERSION_MINOR (0)

/// Source plugin version
#define SOURCE_API_VERSION MAKE_VERSION(SOURCE_API_VERSION_MAJOR, SOURCE_API_VERSION_MINOR)

/**
 * @brief Describes the source plugin information.
 *
 * @since 1.0
 * @version 1.0
 */
struct SourcePluginDef : public PluginDefBase {
    std::vector<ProtocolType> protocol;      ///< Protocols supported by playback source
    SrcInputType inputType;                   ///< Input type supported by record source
    CapabilitySet outCaps;                   ///< Plug-in output capability, For details, @see Capability.
    PluginCreatorFunc<SourcePlugin> creator {nullptr}; ///< Source plugin create function.
    SourcePluginDef()
    {
        apiVersion = SOURCE_API_VERSION; ///< Source plugin version.
        pluginType = PluginType::SOURCE; ///< Plugin type, MUST be SOURCE.
    }
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INTF_SOURCE_PLUGIN_H
