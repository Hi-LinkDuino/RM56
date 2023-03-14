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
#ifndef HIVIEW_PLUGIN_BUNDLE_H
#define HIVIEW_PLUGIN_BUNDLE_H
#include <string>

#include "dynamic_module.h"
#include "plugin.h"
#include "plugin_config.h"

namespace OHOS {
namespace HiviewDFX {
class PluginBundle {
public:
    PluginBundle (const std::string& name, const PluginConfig& config, DynamicModule module)
        : name_(name), config_(config), module_(module) {};
    ~PluginBundle ();
    PluginConfig GetBundleConfig() const
    {
        return config_;
    }
    void ReleaseDynamicModule();
private:
    std::string name_;
    PluginConfig config_;
    DynamicModule module_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BUNDLE_LIFECYCLE_MANAGER_H