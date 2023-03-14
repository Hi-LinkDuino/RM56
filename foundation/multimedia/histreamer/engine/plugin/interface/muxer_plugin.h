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

#ifndef HISTREAMER_PLUGIN_INTF_MUXER_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_MUXER_PLUGIN_H

#include "common/plugin_caps.h"
#include "plugin_base.h"
#include "plugin_definition.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct DataSink {
    virtual ~DataSink() = default;
    virtual Status WriteAt(int64_t offset, const std::shared_ptr<Buffer>& buffer) = 0;
};

struct MuxerPlugin : public PluginBase {
    explicit MuxerPlugin(std::string&& name) : PluginBase(std::move(name)) {}
    virtual Status SetTrackParameter(uint32_t trackId, Plugin::Tag tag, const Plugin::ValueType& value) = 0;
    virtual Status GetTrackParameter(uint32_t trackId, Plugin::Tag tag, Plugin::ValueType& value) = 0;
    virtual Status AddTrack(uint32_t& trackId) = 0;
    virtual Status SetDataSink(const std::shared_ptr<DataSink>& dataSink) = 0;
    virtual Status WriteHeader() = 0;
    virtual Status WriteFrame(const std::shared_ptr<Buffer>& buffer) = 0;
    virtual Status WriteTrailer() = 0;
};

/// Muxer plugin api major number.
#define MUXER_API_VERSION_MAJOR (1)

/// Muxer plugin api minor number
#define MUXER_API_VERSION_MINOR (0)

/// Muxer plugin version
#define MUXER_API_VERSION MAKE_VERSION(MUXER_API_VERSION_MAJOR, MUXER_API_VERSION_MINOR)


struct MuxerPluginDef : public PluginDefBase {
    CapabilitySet inCaps;                ///< Plug-in input capability, For details, @see Capability.
    CapabilitySet outCaps;                    ///< Plug-in output capability, For details, @see Capability.
    PluginCreatorFunc<MuxerPlugin> creator {nullptr}; ///< Muxer plugin create function.
    MuxerPluginDef()
    {
        apiVersion = MUXER_API_VERSION; ///< Muxer plugin version.
        pluginType = PluginType::MUXER; ///< Plugin type, MUST be MUXER.
    }
};
} // Plugin
} // Media
} // OHOS
#endif // HISTREAMER_PLUGIN_INTF_MUXER_PLUGIN_H
