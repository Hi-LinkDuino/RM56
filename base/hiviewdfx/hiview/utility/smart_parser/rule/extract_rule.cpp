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
#include "extract_rule.h"

#include <fstream>
#include <regex>

#include "file_util.h"
#include "log_util.h"
#include "logger.h"
#include "string_util.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("ExtractRule");
void ExtractRule::ParseExtractRule(const string& eventType, const string& config, const string& path)
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
    if (!ret || !errs.empty()) {
        HIVIEW_LOGE("Json parse fail, err is %{public}s in %{public}s.", errs.c_str(), config.c_str());
        return;
    }
#else
    if (!reader.parse(fin, root)) {
        HIVIEW_LOGE("Json parse fail in %{public}s.", config.c_str());
        return;
    }
#endif
    ParseSegStatusCfg(root);
    ParseRule(eventType, root, path);
    return;
}

void ExtractRule::ParseSegStatusCfg(const Json::Value& json)
{
    if (!json.isMember(L1_SEG_STATUS)) {
        HIVIEW_LOGE("faild to get json number %{public}s.", L1_SEG_STATUS.c_str());
        return;
    }

    Json::Value arrayObj = json[L1_SEG_STATUS];
    int arrayObjSize = static_cast<int>(arrayObj.size());
    if (arrayObjSize > JSON_ARRAY_THRESHOLD) {
        arrayObjSize = JSON_ARRAY_THRESHOLD;
        HIVIEW_LOGI("json array has been resized to threshold value.");
    }
    for (int i = 0; i < arrayObjSize; i++) {
        string name = arrayObj[i]["namespace"].asString();
        vector<string> cfg;
        cfg.emplace_back(arrayObj[i]["matchKey"].asString());
        cfg.emplace_back(arrayObj[i]["desc"].asString());
        if (!name.empty() && !cfg[0].empty()) {
            segStatusCfgMap_.emplace(make_pair(name, cfg));
        }
    }
}

/*
 * parse and store into feature set
 */
void ExtractRule::ParseRule(const string& eventType, const Json::Value& json, const string& path)
{
    for (auto iter = json.begin(); iter != json.end(); iter++) {
        auto key = iter.key().asString();
        if (key.find("Rule") == std::string::npos) {
            continue;
        }
        auto value = (*iter);
        string dirOrFile = value[L2_DIR_OR_FILE].asString();
        if (dirOrFile.empty()) {
            continue;
        }
        string subcatalog = value[L2_SUBCATELOG].asString();
        vector<string> featureIds = SplitFeatureId(value);
        FeatureSet fsets{};
        for (const auto& featureId : featureIds) {
            if (!IsMatchId(eventType, featureId) || !IsMatchPath(path, dirOrFile, subcatalog, fsets.fullPath)) {
                continue;
            }
            fsets.skipStep = value[L2_SKIP].asInt();
            fsets.segmentType = value[L2_SEGMENT_TYPE].asString();
            fsets.startSegVec = GetJsonArray(value, L2_SEGMENT_START);
            fsets.segStackVec = GetJsonArray(value, L2_SEGMENT_STACK);
            // 1st: parse feature
            ParseRule(value, fsets.rules);
            featureSets_.emplace(pair<string, FeatureSet>(featureId, fsets));
            featureIds_.emplace_back(featureId);
            HIVIEW_LOGI("ParseFile eventId %{public}s, FeatureId %{public}s.", eventType.c_str(), featureId.c_str());
        }
    }
    HIVIEW_LOGD("ParseFile end.");
    return;
}

vector<string> ExtractRule::GetFeatureId()
{
    return featureIds_;
}

bool ExtractRule::IsMatchId(const string& eventType, const string& featureId) const
{
    string firstMatch = StringUtil::GetRightSubstr(featureId, "_");
    if (StringUtil::GetRleftSubstr(firstMatch, "_") == eventType) {
        return true;
    }
    return false;
}

std::vector<std::string> ExtractRule::GetJsonArray(const Json::Value& json, const string& param)
{
    if (json.isNull() || !json.isMember(param) || !json[param].isArray()) {
        HIVIEW_LOGE("faild to get json array number %{public}s.\n", param.c_str());
        return {};
    }

    int jsonSize = static_cast<int>(json[param].size());
    if (jsonSize > JSON_ARRAY_THRESHOLD) {
        jsonSize = JSON_ARRAY_THRESHOLD;
        HIVIEW_LOGI("json array has been resized to threshold value.");
    }
    std::vector<std::string> result;
    for (int i = 0; i < jsonSize; i++) {
        result.push_back(json[param][i].asString());
    }
    return result;
}

/**
 * sourcefile: the full path
 * name: static path
 * pattern: dynamic path
 */
bool ExtractRule::IsMatchPath(const string& sourceFile, const string& name, const string& pattern,
    string& desPath) const
{
    HIVIEW_LOGI("sourceFile is %{public}s, name is %{public}s, pattern is %{public}s.\n",
        sourceFile.c_str(), name.c_str(), pattern.c_str());
    desPath = sourceFile;

    if (LogUtil::IsTestModel(sourceFile, name, pattern, desPath)) {
        HIVIEW_LOGI("this is test model, desPath is %{public}s.\n", desPath.c_str());
        return true;
    }

    if (pattern.empty()) {
        desPath = name;
        return LogUtil::FileExist(desPath);
    }

    std::vector<std::string> parts;
    StringUtil::SplitStr(pattern, "/", parts, false, false);
    std::string out = (name.back() == '/') ? name : (name + "/");
    for (auto& part : parts) {
        smatch result;
        if (regex_match(sourceFile, result, regex(out + part))) {
            out = ((*(sourceFile.rbegin())) == '/') ? sourceFile : (sourceFile + "/");
        } else {
            out += part + "/";
        }
    }
    desPath = out.substr(0, out.size() - 1);
    return LogUtil::FileExist(desPath);
}

vector<string> ExtractRule::SplitFeatureId(const Json::Value& object) const
{
    if (!object.isMember(L2_FEATUREID)) {
        HIVIEW_LOGE("faild to get json number %{public}s.", L1_SEG_STATUS.c_str());
        return {};
    }

    vector<string> result;
    StringUtil::SplitStr(object[L2_FEATUREID].asString(), ",", result, false, false);
    return result;
}

void ExtractRule::ParseRule(const Json::Value& object, list<FeatureRule>& features) const
{
    if (!object.isMember(L2_RULES)) {
        return;
    }
    ParseRuleParam(object[L2_RULES], features, L2_RULES);

    if (!object.isMember(L2_SEGMENT_RULE)) {
        return;
    }
    ParseRuleParam(object[L2_SEGMENT_RULE], features, L2_SEGMENT_RULE);
}

void ExtractRule::ParseRuleParam(const Json::Value& object, list<FeatureRule>& features, const string& type) const
{
    int objectSize = static_cast<int>(object.size());
    if (objectSize > JSON_ARRAY_THRESHOLD) {
        objectSize = JSON_ARRAY_THRESHOLD;
        HIVIEW_LOGI("json array has been resized to threshold value.");
    }
    for (int i = 0; i < objectSize; i++) {
        FeatureRule command{};
        command.cmdType = type;
        command.name = object[i][L3_NAMESPACE].asString();
        command.source = object[i][L3_MATCH_KEY].asString();
        command.depend = object[i][L3_DEPEND].asString();
        GetExtractParam(object[i], command.param, L3_PARAM);
        command.num = object[i][L3_NUM].asInt();
        if (command.num > 0 && type == L2_RULES) {
            HIVIEW_LOGE("rule command can't have num.\n");
            continue;
        }
        features.emplace_back(command);
    }
}

void ExtractRule::GetExtractParam(const Json::Value& rules,
    std::map<std::string, std::string>& param, const std::string& preKey) const {
    for (auto iter = rules.begin(); iter != rules.end(); iter++) {
        auto pos = iter.key().asString().find(preKey);
        if (pos == 0) {
            param.emplace(iter.key().asString(), (*iter).asString());
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS
