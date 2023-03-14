/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "plugin_factory.h"

#include "logger.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-PluginFactory");
std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<PluginRegistInfo>>> PluginFactory::GetGlobalPluginRegistryMap()
{
    static std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<PluginRegistInfo>>> pluginMap;
    if (pluginMap == nullptr) {
        pluginMap = std::make_shared<std::unordered_map<std::string, std::shared_ptr<PluginRegistInfo>>>();
    }
    return pluginMap;
}

std::shared_ptr<PluginRegistInfo> PluginFactory::GetGlobalPluginInfo(const std::string& name)
{
    auto map = GetGlobalPluginRegistryMap();
    auto it = map->find(name);
    if (it != map->end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Plugin> PluginFactory::GetPlugin(const std::string& name)
{
    auto info = GetGlobalPluginInfo(name);
    if (info != nullptr) {
        return info->getPluginObject();
    }
    return nullptr;
}

void PluginFactory::RegisterPlugin(const std::string& name, std::shared_ptr<PluginRegistInfo> func)
{
    if (func->getPluginObject == nullptr) {
        HIVIEW_LOGW("Register null plugin constructor from %{public}s.", name.c_str());
        return;
    }
    // force update plugin constructor
    auto pluginMap = GetGlobalPluginRegistryMap();
    if (pluginMap->find(name) == pluginMap->end()) {
        pluginMap->insert(std::pair<std::string, std::shared_ptr<PluginRegistInfo>>(name, func));
    } else {
        HIVIEW_LOGW("plugin %{public}s already exists! register plugin failed", name.c_str());
    }
#ifdef _WIN32
    // When PluginFactory is loading, the logger is not loaded.
#else
    HIVIEW_LOGD("Register plugin constructor from %{public}s.", name.c_str());
#endif // _WIN32
}

void PluginFactory::UnregisterPlugin(const std::string& name)
{
    HIVIEW_LOGD("UnregisterPlugin from %{public}s.", name.c_str());
    auto pluginMap = GetGlobalPluginRegistryMap();
    pluginMap->erase(name);
}
} // namespace HiviewDFX
} // namespace OHOS
