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
#include "compose_rule.h"

#include <algorithm>
#include <fstream>
#include <memory>

#include "file_util.h"
#include "logger.h"
#include "string_util.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("ComposeRule");

void ComposeRule::ParseComposeRule(const string& config, const string& type, vector<string> featureIds)
{
    std::ifstream fin(config, std::ifstream::binary);
#ifdef JSONCPP_VERSION_STRING
    Json::CharReaderBuilder builder;
    Json::CharReaderBuilder::strictMode(&builder.settings_);
    JSONCPP_STRING errs;
#else
    Json::Reader reader(Json::Features::strictMode());
#endif

    Json::Value root;
#ifdef JSONCPP_VERSION_STRING
    bool ret = parseFromStream(builder, fin, &root, &errs);
    if (!ret || !errs.empty() || !root.isMember(type)) {
        HIVIEW_LOGE("Json parse fail, err is %{public}s or %{public}s don't exist in %{public}s.",
            errs.c_str(), type.c_str(), config.c_str());
        return;
    }
#else
    if (!reader.parse(fin, root) || !root.isMember(type)) {
        HIVIEW_LOGE("Json parse fail in %{public}s.", config.c_str());
        return;
    }
#endif

    ParseRule(root[type], type, featureIds);
    HIVIEW_LOGI("ComposeRule ParseFile end.");
    return;
}

std::list<std::pair<std::string, std::map<std::string, std::string>>> ComposeRule::GetComposeRule() const
{
    return composeRules_;
}

void ComposeRule::ParseRule(const Json::Value& json, const string& type, vector<string>& featureIds)
{
    sort(featureIds.begin(), featureIds.end(), ComparePrio);
    for (const auto& featureId : featureIds) {
        map<string, string> composeParams = GetMapFromJson(json, featureId);
        composeRules_.emplace_back(pair<string, map<string, string>>(featureId, composeParams));
    }
}

bool ComposeRule::ComparePrio(const string& featureIdOne, const string& featureIdTwo)
{
    return StringUtil::GetRightSubstr(featureIdOne, "_") < StringUtil::GetRightSubstr(featureIdTwo, "_");
}

std::map<std::string, std::string> ComposeRule::GetMapFromJson(const Json::Value& json, const string& featureId)
{
    if (!json.isMember(featureId)) {
        HIVIEW_LOGE("ComposeRule don't have %{public}s featureId.", featureId.c_str());
        return {};
    }
    std::map<std::string, std::string> result;
    auto value = json[featureId];
    for (auto iter = value.begin(); iter != value.end(); iter++) {
        result.emplace(pair<std::string, std::string>(iter.key().asString(), (*iter).asString()));
    }
    return result;
}
} // namespace HiviewDFX
} // namespace OHOS
