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

#ifndef HISTREAMER_PLUGIN_INTF_PLUGIN_BASE_H
#define HISTREAMER_PLUGIN_INTF_PLUGIN_BASE_H

#include <memory>
#include <common/plugin_event.h>
#include "common/plugin_tags.h"
#include "common/plugin_types.h"
#include "common/plugin_buffer.h"

namespace OHOS {
namespace Media {
namespace Plugin {
enum class ErrorType {
    PLUGIN_ERROR,
    ALGO_ERROR,
    CLIENT_ERROR,
    SERVER_ERROR,
};
/**
 * @brief Plugin status callback interface.
 *
 * @since 1.0
 * @version 1.0
 */
struct Callback {
    /// Destructor
    virtual ~Callback() = default;

    /**
     * @brief When asynchronous time occurs during plugin running,
     * the plugin implementer invokes this interface to notify the plugin user.
     *
     * @note Reserved Interface, Not used yet.
     *
     * @param event Event ID.
     */
    virtual void OnEvent(const PluginEvent &event) = 0;
};

/**
 * @brief Base class of a plugin. All plugins of different types inherit this interface.
 *
 * @details The base class contains only common operation methods and defines basic operation processes.
 * Different operations are valid only in the corresponding states. The timing of calls is guaranteed by
 * the plugin framework. Some operations also change the plugin status.
 * For details, see the description of each function.
 *
 * @since 1.0
 * @version 1.0
 */
struct PluginBase {
    /// Constructor
    explicit PluginBase(std::string name): pluginName_(std::move(name)) {}

    /// Destructor
    virtual ~PluginBase() = default;

    /**
     * @brief Get plugin name
     *
     * @return plugin name
     */
    std::string GetName() const
    {
        return pluginName_;
    }

    /**
     * @brief Plugin initialization, which is used to load external resources or plugin common resources.
     *
     * The function is valid only in the CREATED state. If the initialization is successful,
     * the plugin enters the INITIALIZED state.
     *
     * @return  Execution status return
     *  @retval OK: Plugin Init succeeded.
     *  @retval ERROR_NO_MEMORY: Memory allocation or external resource loading error caused by insufficient memory.
     */
    virtual Status Init()
    {
        return Status::OK;
    }

    /**
     * @brief Plugin deinitialize to release resources.
     *
     * This function can be invoked in any state.
     * After the function is invoked, the plugin will no longer be available.
     *
     * @return Execution status return
     *  @retval OK: Plugin Deinit succeeded.
     */
    virtual Status Deinit()
    {
        return Status::OK;
    }

    /**
     * @brief Preparing parameters required or allocate the memory for plugin running.
     *
     * The function is valid only in the INITIALIZED state. If the prepare is successful,
     * the plugin enters the PREPARED state.
     *
     * @return Execution status return
     *  @retval OK: Plugin Prepare succeeded.
     *  @retval ERROR_NO_MEMORY: Memory allocation error caused by insufficient memory.
     */
    virtual Status Prepare()
    {
        return Status::OK;
    }

    /**
     * @brief Reset the plugin, reset the plugin running status and parameters before Prepare.
     *
     * The function is valid only in the PREPARED/RUNNING/PAUSED state. If the reset is successful,
     * the plugin enters the INITIALIZED state.
     *
     * @return Execution status return
     *  @retval OK: Plugin Reset succeeded.
     *  @retval ERROR_UNIMPLEMENTED: This method is not implemented and cannot respond to reset.
     */
    virtual Status Reset()
    {
        return Status::OK;
    }

    /**
     * @brief The plugin enters the running state and can process data.
     *
     * The function is valid only in the PREPARED state. If the start is successful,
     * the plugin enters the RUNNING state. If an error occurs during the running,
     * the plu-in status can be changed through asynchronous callback.
     *
     * @return Execution status return
     *  @retval OK: Plugin Start succeeded.
     */
    virtual Status Start()
    {
        return Status::OK;
    }

    /**
     * @brief The plugin enters the stopped state and stops processing data.
     *
     * The function is valid only in the RUNNING state. If the stop is successful,
     * the plugin enters the PREPARED state. Temporary data generated during the operation will be cleared.
     *
     * @return Execution status return
     *  @retval OK: Plugin Stop succeeded.
     */
    virtual Status Stop()
    {
        return Status::OK;
    }

    /**
     * @brief Get the value of a specified parameter.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param tag   Plugin parameter type, which is described by tag.
     * @param value Plugin parameter value. which is described by Any type. Need check the real type in tag.
     * @return Execution status return
     *  @retval OK: Plugin GetParameter succeeded.
     *  @retval ERROR_INVALID_PARAMETER: The plugin does not support this parameter.
     */
    virtual Status GetParameter(Tag tag, ValueType &value)
    {
        return Status::ERROR_UNIMPLEMENTED;
    }

    /**
     * @brief Set the specified parameter. The value must be within the valid range of the parameter.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param tag   Plugin parameter type, which is described by tag.
     * @param value Plugin parameter value. which is described by Any type. Need check the real type in tag.
     * @return Execution status return
     *  @retval OK: Plugin SetParameter succeeded.
     *  @retval ERROR_INVALID_PARAMETER: The plugin does not support this parameter.
     *  @retval ERROR_INVALID_DATA: The value is not in the valid range.
     *  @retval ERROR_MISMATCHED_TYPE: The data type is mismatched.
     */
    virtual Status SetParameter(Tag tag, const ValueType &value)
    {
        return Status::ERROR_UNIMPLEMENTED;
    }

    /**
     * @brief Get the allocator specified by the plugin.
     * The allocator can allocate memory types that meet the plugin requirements.
     *
     * @return Obtains the allocator object or NULL if the plugin does not have requirements for memory.
     */
    virtual std::shared_ptr<Allocator> GetAllocator()
    {
        return nullptr;
    }

    /**
     * @brief Sets the plugin callback message to notify the plugin user.
     *
     * This function can be called in any state except DESTROYED and INVALID.
     *
     * @param cb   Message callback, NULL callback listening is canceled.
     * @return Execution status return
     *  @retval OK: Plugin SetCallback succeeded.
     */
    virtual Status SetCallback(Callback* cb) = 0;

protected:
    const std::string pluginName_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INTF_PLUGIN_BASE_H
