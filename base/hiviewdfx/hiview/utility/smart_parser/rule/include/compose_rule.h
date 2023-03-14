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
#ifndef COMPOSE_RULE_H
#define COMPOSE_RULE_H
#include <list>
#include <map>
#include <string>
#include <vector>

#include <json/json.h>
namespace OHOS {
namespace HiviewDFX {
class ComposeRule {
public:
    ComposeRule() {};
    ~ComposeRule() {};
    void ParseComposeRule(const std::string& config, const std::string& type,
        std::vector<std::string> featureIds);
    std::list<std::pair<std::string, std::map<std::string, std::string>>> GetComposeRule() const;

private:
    void ParseRule(const Json::Value& jsonRoot, const std::string& type, std::vector<std::string>& featureIds);
    static bool ComparePrio(const std::string& featureIdOne, const std::string& featureIdTwo);
    std::map<std::string, std::string> GetMapFromJson(const Json::Value& json, const std::string& featureId);

private:
    std::list<std::pair<std::string, std::map<std::string, std::string>>> composeRules_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* COMPOSE_RULE_H */
