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
#include "rule.h"

#include "compose_rule.h"
#include "extract_rule.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
namespace {
    const std::string COMPOSE_CONFIG = "compose_rule.json";
    const std::string EXTRACT_CONFIG = "extract_rule.json";
}
    std::map<std::string, FeatureSet> extractRule_;
    std::list<std::pair<std::string, std::map<std::string, std::string>>> composeRule_;
    std::map<std::string, std::vector<std::string>> segStatusCfg_;

void Rule::ParseRule()
{
    ExtractRule extractRule;
    ComposeRule composeRule;
    std::string extractConfig = analysisConfig_ + "/" + EXTRACT_CONFIG;
    extractRule.ParseExtractRule(eventType_, extractConfig, eventPath_);
    extractRule_ = extractRule.GetExtractRule();
    segStatusCfg_ = extractRule.GetSegStatusCfg();
    std::string composeConfig = analysisConfig_ + "/" + COMPOSE_CONFIG;
    composeRule.ParseComposeRule(composeConfig, eventType_, extractRule.GetFeatureId());
    composeRule_ = composeRule.GetComposeRule();
}

std::map<std::string, FeatureSet> Rule::GetExtractRule()
{
    return extractRule_;
}

std::list<std::pair<std::string, std::map<std::string, std::string>>> Rule::GetComposeRule()
{
    return composeRule_;
}

std::map<std::string, std::vector<std::string>> Rule::GetSegStatusCfg()
{
    return segStatusCfg_;
}
} // namespace HiviewDFX
} // namespace OHOS