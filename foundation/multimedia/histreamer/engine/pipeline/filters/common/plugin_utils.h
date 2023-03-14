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

#ifndef HISTREAMER_PIPELINE_FILTER_PLUGIN_UTILS_H
#define HISTREAMER_PIPELINE_FILTER_PLUGIN_UTILS_H

#include <functional>
#include <memory>

#include "pipeline/core/error_code.h"
#include "pipeline/core/compatible_check.h"
#include "plugin/common/plugin_types.h"
#include "plugin/common/plugin_tags.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/core/plugin_info.h"
#include "plugin/core/plugin_manager.h"
#include "plugin/core/plugin_meta.h"
#include "pipeline/core/type_define.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
#define RETURN_AGAIN_IF_NULL(plugin) \
do { \
    if ((plugin) == nullptr) { \
        return ErrorCode::ERROR_AGAIN; \
    } \
} while (0)

template<typename T>
bool AssignParameterIfMatch(Tag tag, T& ret, const Plugin::ValueType& val);

/**
 * translate plugin error into pipeline error code
 * @param pluginError
 * @return
 */
ErrorCode TranslatePluginStatus(Plugin::Status pluginError);

bool TranslateIntoParameter(const int &key, OHOS::Media::Plugin::Tag &tag);

std::vector<std::pair<std::shared_ptr<Plugin::PluginInfo>, Plugin::Capability>>
        FindAvailablePlugins(const Plugin::Capability& upStreamCaps, Plugin::PluginType pluginType);

std::vector<std::shared_ptr<Plugin::PluginInfo>> FindAvailablePluginsByOutputMime(const std::string& outputMime,
                                                                                  Plugin::PluginType pluginType);
uint8_t GetBytesPerSample(Plugin::AudioSampleFormat fmt);

std::string Capability2String(const Capability& capability);

std::string Meta2String(const Plugin::Meta& meta);
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_FILTER_PLUGIN_UTILS_H
