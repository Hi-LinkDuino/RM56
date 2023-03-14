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
#include "smart_parser.h"

#include "feature_analysis.h"
#include "file_util.h"
#include "logger.h"
#include "rule.h"

using namespace std;

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("SmartParser");

std::map<std::string, std::string> SmartParser::Analysis(const std::string& eventPath,
    const std::string& analysisConfig, const std::string& eventType)
{
    std::string config;
    if (!FileUtil::PathToRealPath(analysisConfig, config) || config.empty()) {
        HIVIEW_LOGE("config %{public}s isn't real path.", analysisConfig.c_str());
        return {};
    }

    Rule rule(eventPath, config, eventType);
    rule.ParseRule();
    auto featureSets = rule.GetExtractRule();
    auto composeRules = rule.GetComposeRule();
    auto segStatusCfg = rule.GetSegStatusCfg();

    std::map<std::string, std::string> eventInfoMap;
    for (const auto& composeRule : composeRules) {
        FeatureAnalysis feature(featureSets[composeRule.first], composeRule.second, eventType);
        if (feature.AnalysisLog()) {
            feature.SegmentAnalysis(featureSets[composeRule.first], feature.GetParamSeekRecord(), segStatusCfg);
            auto result = feature.GetReasult();
            for (const auto& one : result) {
                eventInfoMap.emplace(one.first, one.second);
            }
        }
    }
    return eventInfoMap;
}
} // namespace HiviewDFX
} // namespace OHOS
