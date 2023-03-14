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
#include "hiview_platform_config.h"

#include <chrono>
#include <fstream>
#include <regex>

#include "file_util.h"
#include "logger.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-HiviewPlatformConfig");
namespace {
    constexpr char DEFAULT_PLATFORM_CONFIG_PATH[] = "/system/etc/hiview/hiview_platform_config";
    constexpr int DATA_FIELD = 1;
    constexpr int CONFIG_NAME_NO = 0;
    constexpr int CONFIG_FILE_DIR_NO = 1;
    constexpr int LIB_DIR_NO = 2;
    constexpr int LIB64_DIR_NO = 3;
    constexpr int UPDATE_CONFIG_DIR_NO = 4;
    constexpr int WORK_DIR_NO = 5;
    constexpr int COMMERCIAL_WORK_DIR_NO = 6;
    constexpr int PRESIST_DIR_NO = 7;
}

HiviewPlatformConfig::HiviewPlatformConfig(const std::string& configPath)
{
    if (configPath != "") {
        configPath_ = configPath;
    } else {
        configPath_ = DEFAULT_PLATFORM_CONFIG_PATH;
    }
}

void HiviewPlatformConfig::InitData()
{
    RegexList_.push_back(std::regex("DEFAULT_PLUGIN_CONFIG_NAME\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("PLUGIN_CONFIG_FILE_DIR\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("DYNAMIC_LIB_SEARCH_DIR\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("DYNAMIC_LIB64_SEARCH_DIR\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("CLOUD_UPDATE_CONFIG_DIR\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("WORK_DIR\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("COMMERCIAL_WORK_DIR\\s*=\\s*\"(.*)\".*"));
    RegexList_.push_back(std::regex("PERSIST_DIR\\s*=\\s*\"(.*)\".*"));
}

bool HiviewPlatformConfig::ParsesConfig(HiviewPlatformConfig::PlatformConfigInfo& ret)
{
    if (!FileUtil::FileExists(configPath_) || FileUtil::IsDirectory(configPath_)) {
        return false;
    }

    std::ifstream in;
    in.open(configPath_);
    if (!in.is_open()) {
        HIVIEW_LOGW("fail to open config file.\n");
        return false;
    }

    InitData();

    std::string buf = "";
    int index = 0;
    std::vector<std::string> retString;
    while (getline(in, buf)) {
        std::smatch result;
        if (!regex_search(buf, result, RegexList_.front())) {
            HIVIEW_LOGW("match version failed.\n");
            return false;
        }
        retString.push_back(result[DATA_FIELD]);
        HIVIEW_LOGD("_%{public}s_", retString[index].c_str());
        RegexList_.pop_front();
        ++index;
        if (RegexList_.empty()) {
            break;
        }
    }
    ret.defaultPluginConfigName = retString[CONFIG_NAME_NO];
    ret.pluginConfigFileDir = retString[CONFIG_FILE_DIR_NO];
    ret.dynamicLibSearchDir = retString[LIB_DIR_NO];
    ret.dynamicLib64SearchDir = retString[LIB64_DIR_NO];
    ret.cloudUpdateConfigDir = retString[UPDATE_CONFIG_DIR_NO];
    ret.workDir = retString[WORK_DIR_NO];
    ret.commercialWorkDir = retString[COMMERCIAL_WORK_DIR_NO];
    ret.persistDir = retString[PRESIST_DIR_NO];
    return true;
}
} // namespace HiviewDFX
} // namespace OHOS