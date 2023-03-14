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
#ifndef EXTRACT_RULE_H
#define EXTRACT_RULE_H
#include <list>
#include <map>
#include <string>
#include <vector>

#include <json/json.h>

#include "syntax_rules.h"
namespace OHOS {
namespace HiviewDFX {
class ExtractRule {
public:
    ExtractRule() {};
    ~ExtractRule() {};
    void ParseExtractRule(const std::string& eventType, const std::string& config, const std::string& fullPath);
    std::vector<std::string> GetFeatureId();
    std::map<std::string, FeatureSet> GetExtractRule() { return featureSets_; };
    std::map<std::string, std::vector<std::string>> GetSegStatusCfg() { return segStatusCfgMap_; };

private:
    static constexpr int JSON_ARRAY_THRESHOLD = 50;
    
    void ParseSegStatusCfg(const Json::Value& json);
    void ParseRule(const std::string& eventType, const Json::Value& json, const std::string& fullPath);
    bool IsMatchId(const std::string& eventType, const std::string& featureId) const;
    bool IsMatchPath(const std::string& sourceFile, const std::string& name, const std::string& pattern,
        std::string& desPath) const;
    std::vector<std::string> SplitFeatureId(const Json::Value& oject) const;
    void ParseRule(const Json::Value& oject, std::list<FeatureRule>& features) const;
    void ParseRuleParam(const Json::Value& oject, std::list<FeatureRule>& features, const std::string& type) const;
    void GetExtractParam(const Json::Value& rules,
        std::map<std::string, std::string>& param, const std::string& preKey) const;
    std::vector<std::string> GetJsonArray(const Json::Value& json, const std::string& param);

private:
    std::map<std::string, FeatureSet> featureSets_;
    std::map<std::string, std::string> lockInfoMap_;
    std::vector<std::string> featureIds_;
    std::map<std::string, std::vector<std::string>> segStatusCfgMap_; // key: name  value: [matchKey, desc]
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* EXTRACT_RULE_H */