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
#ifndef BASE_HIVIEWDFX_HIVIEW_CORE_PLUGIN_CONFIG_H
#define BASE_HIVIEWDFX_HIVIEW_CORE_PLUGIN_CONFIG_H
#include <list>
#include <map>
#include <string>
namespace OHOS {
namespace HiviewDFX {
class PluginConfig {
public:
    struct PluginInfo {
        bool isStatic;
        bool isEventSource;
        int32_t loadDelay;
        std::string name;
        std::string workHandlerType;
        std::string workHandlerName;
        std::list<std::string> pipelineNameList;
    };

    struct PipelineInfo {
        std::string name;
        std::list<std::string> pluginNameList;
    };

    struct WorkPoolInfo {
        std::string name;
        int32_t threadCount;
    };

    explicit PluginConfig(const std::string& file);
    ~PluginConfig(){};
    bool StartParse();
    const std::list<PluginConfig::PluginInfo>& GetPluginInfoList() const;
    const std::list<PluginConfig::PipelineInfo>& GetPipelineInfoList() const;

private:
    void ParsePlugin(const std::string& pluginStr);
    void ParsePipeline(const std::string& pipelineStr);
    void ParsePipelineGroup(const std::string& pipelineGroup);
    std::string configPath_;
    std::map<std::string, WorkPoolInfo> workPoolInfoMap_;
    std::list<PluginInfo> pluginInfoList_;
    std::list<PipelineInfo> pipelineInfoList_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // BASE_HIVIEWDFX_HIVIEW_CORE_PLUGIN_CONFIG_H