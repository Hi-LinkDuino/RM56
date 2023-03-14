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

#ifndef HISTREAMER_PLUGIN_INTF_OUTPUT_SINK_PLUGIN_H
#define HISTREAMER_PLUGIN_INTF_OUTPUT_SINK_PLUGIN_H

#include "foundation/pre_defines.h"
#include "plugin_base.h"
#include "plugin_definition.h"
#include "plugin/common/media_sink.h"
#include "plugin/common/plugin_caps.h"
#include "plugin/common/plugin_tags.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct OutputSinkPlugin : public Plugin::PluginBase {
    explicit OutputSinkPlugin(std::string name): PluginBase(std::move(name)) {}
    std::shared_ptr<Allocator> GetAllocator() override
    {
        return nullptr;
    }
    Status SetCallback(Callback* cb) override
    {
        UNUSED_VARIABLE(cb);
        return Status::ERROR_UNIMPLEMENTED;
    }
    virtual Status SetSink(const MediaSink& sink) = 0;
    virtual Seekable GetSeekable() = 0;
    virtual Status SeekTo(uint64_t offset) = 0;
    virtual Status Write(const std::shared_ptr<Buffer>& buffer) = 0;
    virtual Status Flush() = 0;
};

/// Output Sink plugin api major number.
#define OUTPUT_SINK_API_VERSION_MAJOR (1)

/// Output Sink plugin api minor number
#define OUTPUT_SINK_API_VERSION_MINOR (0)

/// Output Sink plugin version
#define OUTPUT_SINK_API_VERSION MAKE_VERSION(OUTPUT_SINK_API_VERSION_MAJOR, OUTPUT_SINK_API_VERSION_MINOR)

struct OutputSinkPluginDef : public PluginDefBase {
    ProtocolType protocolType;
    CapabilitySet inCaps;
    PluginCreatorFunc<OutputSinkPlugin> creator {nullptr}; ///< Output sink plugin create function.
    OutputSinkPluginDef()
    {
        apiVersion = OUTPUT_SINK_API_VERSION; ///< Output sink plugin version.
        pluginType = PluginType::OUTPUT_SINK; ///< Plugin type, MUST be OUTPUT_SINK.
        inCaps.emplace_back(Capability("*")); ///< Output sink sink can accept any data
    }
};
} // Plugin
} // Media
} // OHOS
#endif // HISTREAMER_PLUGIN_INTF_OUTPUT_SINK_PLUGIN_H
