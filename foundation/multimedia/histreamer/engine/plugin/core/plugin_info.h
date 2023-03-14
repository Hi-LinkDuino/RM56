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

#ifndef HISTREAMER_PLUGIN_INFO_H
#define HISTREAMER_PLUGIN_INFO_H

#include "interface/plugin_definition.h"
#include "common/plugin_tags.h"
#include "common/plugin_caps.h"

namespace OHOS {
namespace Media {
namespace Plugin {
/**
 * PluginInfo, which describes static information for a plugin, including basic plugin information,
 * such as the type, name, rank, and input and output capabilities.
 *
 * Different types of plugins have their own extra information,
 * which is described in the "extra" field in the form of key-value.
 *
 * Note that the type, name, rating, and extra information describes the plugin as a whole;
 *
 * Typically, plugin have inputs and outputs, those capabilities are described by inCaps and outCaps.
 * (The Source plugin has only output capability, and the Sink plugin has only input capability.)
 * The input/output capability describes the data processing capability by mime-type.
 * To describe mime-type more closely, a detailed tag may be attached to the mime-type.
 *
 */
struct PluginInfo {
    uint32_t apiVersion;
    PluginType pluginType;
    std::string name;
    std::string description;
    uint32_t rank;
    CapabilitySet inCaps;
    CapabilitySet outCaps;
    std::map<std::string, ValueType> extra;
};

/**
 * Extra information about the plugin.
 * Describes the protocol types supported by the Source plugin for playback.
 */
#define PLUGIN_INFO_EXTRA_PROTOCOL          "protocol"  // NOLINT: macro constant

/**
 * Extra information about the plugin.
 * Describes the input source types supported by the Source plugin for record.
 */
#define PLUGIN_INFO_EXTRA_INPUT_TYPE        "inputType"  // NOLINT: macro constant

/**
 * Extra information about the plugin.
 * Describes the output types supported by the OutputSink plugin.
 */
#define PLUGIN_INFO_EXTRA_OUTPUT_TYPE        "outputType"  // NOLINT: macro constant

/**
 * Extra information about the plugin.
 * Describes the extensions supported by the Demuxer plugin.
 */
#define PLUGIN_INFO_EXTRA_EXTENSIONS        "extensions" // NOLINT: macro constant

/**
 * Extra information about the plugin.
 * Describes the codec type supported by the Codec plugin.
 *
 * ValueType: enum Plugin::CodecType
 */
#define PLUGIN_INFO_EXTRA_CODEC_TYPE        "codec_type" // NOLINT: macro constant

} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_INFO_H
