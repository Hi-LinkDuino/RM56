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
#ifndef HIVIEW_BASE_PLUGIN_PLATFORM_CONFIG_H
#define HIVIEW_BASE_PLUGIN_PLATFORM_CONFIG_H
#include <string>

#include <chrono>
#include <list>
#include <regex>
namespace OHOS {
namespace HiviewDFX {
class HiviewPlatformConfig {
public:
    struct PlatformConfigInfo {
        std::string defaultPluginConfigName;
        std::string pluginConfigFileDir;
        std::string dynamicLibSearchDir;
        std::string dynamicLib64SearchDir;
        std::string cloudUpdateConfigDir;
        std::string workDir;
        std::string commercialWorkDir;
        std::string persistDir;
    };
    explicit HiviewPlatformConfig(const std::string& configPath);
    ~HiviewPlatformConfig(){};
    bool ParsesConfig(PlatformConfigInfo& ret);

private:
    void InitData();

    std::string configPath_;
    std::list<std::regex> RegexList_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_PLUGIN_PLATFORM_CONFIG_H