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
#ifndef HIVIEW_BASE_PLUGIN_PROXY_H
#define HIVIEW_BASE_PLUGIN_PROXY_H

#include <ctime>
#include <memory>
#include <mutex>

#include "event.h"
#include "plugin.h"
#include "plugin_config.h"

namespace OHOS {
namespace HiviewDFX {
class PluginProxy : public Plugin {
public:
    PluginProxy() {};
    virtual ~PluginProxy() {};
    bool OnEvent(std::shared_ptr<Event>& event) override;
    bool CanProcessEvent(std::shared_ptr<Event> event) override;
    bool CanProcessMoreEvents() override;
    std::string GetHandlerInfo() override;
    void Dump(int fd __UNUSED, const std::vector<std::string>& cmds) override;
    void OnEventListeningCallback(const Event &msg) override;
    bool LoadPluginIfNeed();
    void DestroyInstanceIfNeed(time_t maxIdleTime);
    bool HoldInstance()
    {
        return (plugin_ != nullptr);
    }
    void SetPluginConfig(const PluginConfig::PluginInfo& config)
    {
        config_ = config;
    }
    const PluginConfig::PluginInfo& GetPluginConfig()
    {
        return config_;
    }

private:
    std::shared_ptr<Plugin> plugin_;
    std::mutex lock_;
    PluginConfig::PluginInfo config_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_PLUGIN_PROXY_H
