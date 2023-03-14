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
#include "plugin_config.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include "logger.h"
#include "string_util.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-PluginConfig");
PluginConfig::PluginConfig(const std::string& file) : configPath_(file) {}
bool PluginConfig::StartParse()
{
    std::ifstream in;
    in.open(configPath_);
    if (!in.is_open()) {
        HIVIEW_LOGW("fail to open config file.");
        return false;
    }
    int fieldCount = 0;
    std::string field = "";
    std::string buf = "";
    const int fieldNameField = 1;
    const int fieldCountField = 2;
    while (getline(in, buf)) {
        std::string strTmp = StringUtil::TrimStr(buf);
        if (strTmp == "") {
            continue;
        }
        std::smatch result;
        if (fieldCount == 0) {
            if (!regex_search(strTmp, result, std::regex("(plugins|pipelines|pipelinegroups)\\s*:\\s*(\\d+)"))) {
                HIVIEW_LOGW("match field failed.");
                in.close();
                return false;
            }

            field = StringUtil::TrimStr(result[fieldNameField]);
            fieldCount = atoi(std::string(result[fieldCountField]).c_str());
            HIVIEW_LOGD("find field:%{public}s.", field.c_str());
            continue;
        }

        if (field == "plugins") {
            ParsePlugin(strTmp);
        } else if (field == "pipelines") {
            ParsePipeline(strTmp);
        } else if (field == "pipelinegroups") {
            ParsePipelineGroup(strTmp);
        }
        fieldCount--;
    }
    in.close();
    return true;
}

void PluginConfig::ParsePlugin(const std::string& pluginStr)
{
    std::smatch result;
    if (!regex_search(pluginStr, result,
        std::regex("([^\\[]+)\\[([^\\[:]*):?([^:]*):?([^\\]]*)\\]\\s*:(\\d+)\\s+(dynamic|static)"))) {
        HIVIEW_LOGW("no match plugin expression found! \n");
        return;
    }

    // EventProcessorExample1[thread:t-example-2]:30 static
    const int pluginNameField = 1;
    const int workHandlerTypeField = 2;
    const int workHandlerNameField = 3;
    const int countOfThreadPoolField = 4;
    const int loadDelayField = 5;
    const int loadTypeField = 6;
    PluginInfo info;

    info.isStatic = (result[loadTypeField] == "static");
    info.isEventSource = false;
    info.loadDelay = atoi(std::string(result[loadDelayField]).c_str());
    info.name = result[pluginNameField];
    info.workHandlerType = result[workHandlerTypeField];
    info.workHandlerName = result[workHandlerNameField];

    // EventProcessorExample4[pool:pool-example-4:4]:0 static
    if (result[workHandlerTypeField] == "pool") {
        WorkPoolInfo poolInfo;
        poolInfo.name = result[workHandlerNameField],
        poolInfo.threadCount = atoi(std::string(result[countOfThreadPoolField]).c_str()),
        workPoolInfoMap_[poolInfo.name] = std::move(poolInfo);
    }
    pluginInfoList_.push_back(std::move(info));
}

void PluginConfig::ParsePipeline(const std::string& pipelineStr)
{
    std::smatch result;
    // FromTwo2Three:EventProcessorExample2 EventProcessorExample3
    if (!regex_search(pipelineStr, result, std::regex("(\\S+)\\s*:(.+)"))) {
        HIVIEW_LOGW("Fail to match pipeline expression.");
        return;
    }

    const int pipelineNameField = 1;
    const int pluginNameListField = 2;
    PipelineInfo pipelineInfo;
    pipelineInfo.name = result.str(pipelineNameField);
    pipelineInfo.pluginNameList = StringUtil::SplitStr(result.str(pluginNameListField));
    pipelineInfoList_.push_back(std::move(pipelineInfo));
}

void PluginConfig::ParsePipelineGroup(const std::string& pipelineGroup)
{
    std::smatch result;
    // EventSourceExample:FromTwo2Three FromThree2Two
    if (!regex_search(pipelineGroup, result, std::regex("(\\S+)\\s*:(.+)"))) {
        HIVIEW_LOGW("Fail to match pipeline group expression.");
        return;
    }

    const int pipelineGroupNameField = 1;
    const int pipelineNameListField = 2;
    std::string eventSourceName = StringUtil::TrimStr(result.str(pipelineGroupNameField));
    auto ret = std::find_if(pluginInfoList_.begin(), pluginInfoList_.end(), [&](PluginInfo& info) {
        if (info.name == eventSourceName) {
            info.isEventSource = true;
            info.pipelineNameList = StringUtil::SplitStr(result.str(pipelineNameListField));
            return true;
        }
        return false;
    });
    if (ret != std::end(pluginInfoList_)) {
        HIVIEW_LOGD("%s is an event source.", eventSourceName.c_str());
    }
}

const std::list<PluginConfig::PluginInfo>& PluginConfig::GetPluginInfoList() const
{
    return pluginInfoList_;
}

const std::list<PluginConfig::PipelineInfo>& PluginConfig::GetPipelineInfoList() const
{
    return pipelineInfoList_;
}
} // namespace HiviewDFX
} // namespace OHOS
