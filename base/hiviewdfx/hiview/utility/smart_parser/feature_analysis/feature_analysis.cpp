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
#include "feature_analysis.h"

#include <algorithm>
#include <list>
#include <regex>
#include <string>
#include <vector>

#include "file_util.h"
#include "log_util.h"
#include "logger.h"
#include "segment_analysis_factory.h"
#include "string_util.h"

using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("FeatureAnalysis");

const string FeatureAnalysis::COMPOSE_PLUS = "+";
const string FeatureAnalysis::COMPOSE_COLON = ":";
namespace {
    const string PARAMETER_REASON = "REASON";
}

FeatureAnalysis::~FeatureAnalysis()
{
    HIVIEW_LOGD("<%{public}d> FeatureAnalysis::~FeatureAnalysis.", taskId_);
}

bool FeatureAnalysis::AnalysisLog()
{
    HIVIEW_LOGI("<%{public}d> begin analysis <%{public}s>, eventType is <%{public}s>.", taskId_,
        featureSet_.fullPath.c_str(), eventType_.c_str());

    Extract();
    if (paramSeekRecord_.empty()) {
        errorCode_ = EXTRACT_ERROR;
        return false;
    }
    Compose();
    ExtraProcess();
    return true;
}

void FeatureAnalysis::Extract()
{
    stringstream buffer("");
    if (!LogUtil::ReadFileBuff(featureSet_.fullPath, buffer) || !buffer.good() || buffer.eof()) {
        errorCode_ = BUFFER_ERROR;
        HIVIEW_LOGE("<%{public}d> file is invalid", taskId_);
        return;
    }

    // extract info
    RawInfoPosition(buffer);
}

void FeatureAnalysis::RawInfoPosition(stringstream& buffer)
{
    int skipStep = (featureSet_.skipStep > 0) ? featureSet_.skipStep : MAX_SKIP_LINE;
    int dismatchCount = 1; // default : countDismatch - 1 >= skipSpace
    string line;
    bool segmentStart = false;
    HIVIEW_LOGI("<%{public}d> skipStep is %{public}d. size:%{public}zu", taskId_, skipStep, featureSet_.rules.size());
    while (getline(buffer, line)) {
        if (line.length() > 2048 || // 2048 : max length of line
            (CheckStartSegment(segmentStart) && (line.empty() || line[0] == ' ' || line[0] == '\t'))) {
            continue;
        }
        GetCursorInfo(buffer, line);
        for (auto iterCmd = featureSet_.rules.begin(); iterCmd != featureSet_.rules.end();) {
            // Check the variable symbol and replace it with the parameter value of the variable
            FeatureRule& featureCmd = *iterCmd;
            if (CheckDepend(featureCmd)) {
                // The depend element exists but the dependent feature has no value
                iterCmd++;
                continue;
            }

            if (CheckVariableParam(featureCmd) && IsSourceMatch(line, featureCmd)) {
                int num = featureCmd.num;
                bool matchFlag = ParseElementForParam(line, featureCmd);
                while (--num > 0 && getline(buffer, line)) {
                    GetCursorInfo(buffer, line);
                    ParseElementForParam(line, featureCmd);
                }

                if (matchFlag && featureCmd.cmdType == L2_RULES) {
                    iterCmd = featureSet_.rules.erase(iterCmd); // erase will iterCmd++, but break avoid out of range
                } else {
                    iterCmd++;
                }
                dismatchCount = 0;
                break;
            } else {
                iterCmd++;
            }
        }
        dismatchCount++;
        if (featureSet_.rules.empty() || dismatchCount - 1 >= skipStep) {
            break;
        }
    }
}

void FeatureAnalysis::GetCursorInfo(stringstream& buff, const string& line)
{
    line_ = line;
    lineCursor_ = static_cast<int>(buff.tellg()) - static_cast<int>(line.length()) - 1;
}

bool FeatureAnalysis::CheckStartSegment(bool& segmentStart) const
{
    if (segmentStart) {
        return segmentStart;
    }
    vector<std::pair<std::string, LineFeature>> info = paramSeekRecord_;
    for (const auto& one : info) {
        if (one.first.find("LayerTwoCmd") != string::npos ||
            one.first.find("LayerOneCmd") != string::npos) {
            segmentStart = true;
            break;
        }
    }
    return segmentStart;
}

// line match source or not
bool FeatureAnalysis::IsSourceMatch(const string& line, const FeatureRule& rule) const
{
    string cmdSrc = rule.source;
    // if startwith "@R@"
    if (L3_REGULAR_DESCRIPTOR == cmdSrc.substr(0, L3_REGULAR_DESCRIPTOR.length())) {
        cmdSrc = cmdSrc.substr(L3_REGULAR_DESCRIPTOR.length());
        return regex_search(line, regex(cmdSrc));
    }

    // handle OR or AND expression
    bool isOrExp = (cmdSrc.find(L3_OR_DESCRIPTOR) == string::npos) ? false : true;
    bool isAndExp = (cmdSrc.find(L3_AND_DESCRIPTOR) == string::npos) ? false : true;
    if (!isOrExp && !isAndExp) {
        return line.find(cmdSrc) != string::npos;
    } else if (isOrExp) {
        return IsMatchOrExpression(line, cmdSrc);
    } else if (isAndExp) {
        return IsMatchAndExpression(line, cmdSrc);
    }
    return false;
}

bool FeatureAnalysis::IsMatchOrExpression(const string& line, const string& src) const
{
    vector<string> srcSplit;
    StringUtil::SplitStr(src, L3_OR_DESCRIPTOR, srcSplit, false, false);
    for (auto str : srcSplit) {
        if (line.find(str) != string::npos) {
            return true;
        }
    }
    return false;
}

bool FeatureAnalysis::IsMatchAndExpression(const string& line, const string& src) const
{
    string lineCpy = line;
    size_t pos;
    vector<string> srcSplit;
    StringUtil::SplitStr(src, L3_AND_DESCRIPTOR, srcSplit, false, false);
    for (auto str : srcSplit) {
        pos = lineCpy.find(str);
        if (pos == string::npos) {
            return false;
        }
        lineCpy = lineCpy.substr(pos + str.length());
    }
    return true;
}

bool FeatureAnalysis::ParseElementForParam(const string& src, FeatureRule& rule)
{
    if (rule.param.empty()) {
        return true; // if param is empty, erase the rule
    }

    bool hasContinue = false;
    for (auto iter = rule.param.begin(); iter != rule.param.end();) {
        // subParam.first: parameter name; subParam.second: the expression to match
        string reg = "";
        smatch result;
        int seekType = GetSeekInfo(iter->second, reg);
        hasContinue = (seekType == LAST_MATCH) ? true : hasContinue;
        if (reg.find(L3_VARIABLE_TRACE_BLOCK) != string::npos || regex_search(src, result, regex(reg))) {
            string value = result.str(1).empty() ? "" : string(result.str(1));
            SetParamRecord(rule.name + "." + iter->first, FormatLineFeature(value, reg), seekType);
            SetStackRegex(rule.name + "." + iter->first, reg);
            if (seekType == FIRST_MATCH && rule.cmdType == L2_RULES) {
                iter = rule.param.erase(iter);
            } else {
                iter++;
            }
        } else {
            iter++;
        }
    }

    return hasContinue ? false : rule.param.empty();
}

int FeatureAnalysis::GetSeekInfo(const string& param, string& value) const
{
    if (param.find(L3_SEEK_LAST) != string::npos) {
        value = StringUtil::GetRightSubstr(param, L3_SEEK_LAST);
        return LAST_MATCH;
    }
    value = param;
    return FIRST_MATCH;
}

bool FeatureAnalysis::CheckVariableParam(FeatureRule& rule) const
{
    // Check whether there is a variable operator &@& in the command
    string symbol = "";
    string value = "";
    bool hasValSymbol = CheckVariable(rule, L3_DESCRIPTOR_LEFT, L3_DESCRIPTOR_RIGHT);
    if (!hasValSymbol) {
        return true;
    }
    // To replace it if there is one or more variable symbol
    for (const auto& param : paramSeekRecord_) {
        symbol = L3_DESCRIPTOR_LEFT + param.first + L3_DESCRIPTOR_RIGHT;
        value = param.second.value;
        ReplaceVariable(rule, symbol, value);
    }
    return !CheckVariable(rule, L3_DESCRIPTOR_LEFT, L3_DESCRIPTOR_RIGHT); // check var in config
}

bool FeatureAnalysis::CheckVariable(const FeatureRule& rule, const string& leftTag, const string& rightTag) const
{
    if ((rule.source.find(leftTag) != string::npos && rule.source.find(rightTag) != string::npos) ||
        (rule.depend.find(leftTag) != string::npos && rule.depend.find(rightTag) != string::npos)) {
        return true;
    }
    for (auto subParam : rule.param) {
        if (subParam.second.find(leftTag) != string::npos && subParam.second.find(rightTag) != string::npos) {
            return true;
        }
    }

    return false;
}

void FeatureAnalysis::ReplaceVariable(FeatureRule& rule, const string& symbol, const string& value) const
{
    ReplaceVariable(rule.source, symbol, value, rule.source);
    ReplaceVariable(rule.depend, symbol, value, rule.depend);
    for (auto subParam : rule.param) {
        if (ReplaceVariable(subParam.second, symbol, value, subParam.second)) {
            rule.param[subParam.first] = subParam.second;
        }
    }
}

bool FeatureAnalysis::ReplaceVariable(const string& src, const string& param,
    const string& value, string& des) const
{
    des = src;
    size_t pos = src.find(param);
    if (pos != string::npos) {
        des.replace(pos, param.length(), value, 0, value.length());
        return true;
    }

    return false;
}

/*
 * return false if the depend element does not exist
 * return false if the depend element exists and the feature that the depend element relies on has a value
 * return true if the depend element exists and the feature that the depend element relies on has no value
 */
bool FeatureAnalysis::CheckDepend(const FeatureRule& rule) const
{
    bool result = false;
    if (rule.depend.empty()) {
        return result;
    }

    if (paramSeekRecord_.empty()) {  // depend exist but no value
        return true;
    }

    result = true;
    for (const auto& one : paramSeekRecord_) {
        if (one.first.find(rule.depend) != string::npos) {
            result = false; // depend exist but value exist
            break;
        }
    }
    return result;
}

LineFeature FeatureAnalysis::FormatLineFeature(const string& value, const string& regex) const
{
    LineFeature paramRecord{};
    paramRecord.value = value;
    paramRecord.lineCursor = lineCursor_;
    return paramRecord;
}

/*
 * info : eventinfo_ or relatedInfo_ result
 * params : rule info, for example : key : value
 *  "SUBJECT": "BasicParam.s_subject",
 *  "END_STACK": "MainCallTrace.s_trust_stack",
 * features : Log feature
 */
void FeatureAnalysis::Compose()
{
    string result;
    for (const auto& param : composeRule_) {
        // keep ordered rule_prio, REASON is specially composed
        if (eventInfo_.find(param.first) != eventInfo_.end() && param.first != PARAMETER_REASON) {
            continue;
        }
        result = ComposeParam(param.second);
        if (!result.empty()) {
            eventInfo_[param.first] = (param.first == PARAMETER_REASON) ?
                (eventInfo_[param.first] + COMPOSE_COLON + result) : result;
        }
    }
    ProcessReason(eventInfo_);
}

string FeatureAnalysis::ComposeTrace(const string& filePath, const string& param,
    const vector<pair<string, LineFeature>>& lineFeatures, const string& regex) const
{
    string result;
    auto iter = find_if(lineFeatures.begin(), lineFeatures.end(),
        [&param](const pair<string, LineFeature>& one) {return one.first == param;});
    if (iter != lineFeatures.end()) {
        stringstream buffer("");
        if (LogUtil::ReadFileBuff(filePath, buffer)) {
            LogUtil::GetTrace(buffer, iter->second.lineCursor, regex, result);
        }
    }
    return result;
}

string FeatureAnalysis::ComposeParam(const string& param) const
{
    std::vector<std::pair<std::string, LineFeature>> lineFeatures = paramSeekRecord_;
    vector<string> params = SplitParam(param);
    vector<string> results;
    for (const auto& key : params) {
        auto iter = find_if(lineFeatures.begin(), lineFeatures.end(),
            [&key](const pair<string, LineFeature>& one) {return one.first == key;});
        if (iter != lineFeatures.end()) {
            auto regexIter = stackRegex_.find(key);
            if (regexIter != stackRegex_.end()) {
                auto value = ComposeTrace(featureSet_.fullPath, key, lineFeatures, regexIter->second);
                results.emplace_back(value);
            } else {
                results.emplace_back(iter->second.value);
            }
        }
    }

    string tag;
    if (!results.empty()) {
        if (param.find(COMPOSE_PLUS) != string::npos) {
            tag = " ";
        } else if (param.find(COMPOSE_COLON) != string::npos) {
            tag = COMPOSE_COLON;
        } else {
            tag = "";
        }
    }
    string result = StringUtil::VectorToString(results, false, tag);
    auto end = result.size() - tag.size();
    return (end > 0) ? result.substr(0, end) : result;
}

vector<string> FeatureAnalysis::SplitParam(const string& param) const
{
    vector<string> params;
    if (param.find(COMPOSE_PLUS) != string::npos) {
        StringUtil::SplitStr(param, COMPOSE_PLUS, params, false, false);
    } else if (param.find(COMPOSE_COLON) != string::npos) {
        StringUtil::SplitStr(param, COMPOSE_COLON, params, false, false);
    } else {
        params.emplace_back(param);
    }
    return params;
}

void FeatureAnalysis::ProcessReason(map<string, string>& info)
{
    if (info.find(PARAMETER_REASON) != info.end() &&
        info[PARAMETER_REASON].substr(0, 1) == COMPOSE_COLON) { // 1: first char
        info[PARAMETER_REASON] = info[PARAMETER_REASON].substr(1);
    }
}

void FeatureAnalysis::SetParamRecord(const std::string& key, const LineFeature& value, const int type)
{
    if (type == LAST_MATCH) {
        for (auto iter = paramSeekRecord_.begin(); iter != paramSeekRecord_.end(); iter++) {
            if (iter->first == key) {
                paramSeekRecord_.erase(iter);
                break;
            }
        }
    }
    paramSeekRecord_.emplace_back(pair<string, LineFeature>(key, value));
}

void FeatureAnalysis::SetStackRegex(const std::string& key, const std::string& regex)
{
    if (regex.find(L3_VARIABLE_TRACE_BLOCK) != string::npos) {
        stackRegex_.emplace(pair(key, StringUtil::EraseString(regex, L3_VARIABLE_TRACE_BLOCK)));
    }
}

void FeatureAnalysis::ExtraProcess()
{
    if (eventType_ == "JAVA_CRASH" && eventInfo_.find("END_STACK") != eventInfo_.end()) {
        string lastLine;
        GetCrashFaultLine(featureSet_.fullPath, lastLine);
        eventInfo_["END_STACK"] = lastLine + LogUtil::SPLIT_PATTERN + eventInfo_["END_STACK"];
    }
}

void FeatureAnalysis::GetCrashFaultLine(const std::string& file, std::string& line) const
{
    stringstream buffer;
    LogUtil::ReadFileBuff(file, buffer);
    if (!buffer.good() || buffer.eof()) {
        return;
    }

    std::string lineTrace;
    while (getline(buffer, lineTrace)) {
        if (lineTrace.empty()) {
            getline(buffer, line);
            return;
        }
    }
    line = "NoExceptionInfo";
}

void FeatureAnalysis::SegmentAnalysis(FeatureSet& featureSet,
    const std::vector<std::pair<std::string, LineFeature>>& paramSeekRecord,
    std::map<std::string, std::vector<std::string>>& segStatusCfg)
{
    if (featureSet.segmentType.empty()) {
        return;
    }

    auto segAnalysis = SegmentAnalysisFactory::GetInstance().CreateParser(featureSet.segmentType);
    if (segAnalysis != nullptr) {
        auto startSeg = featureSet.startSegVec; // like: ["BasicParam.s_process", "main"]
        UpdateStartSegment(paramSeekRecord, startSeg);
        segAnalysis->SetSegStatusCfg(segStatusCfg);
        if (segAnalysis->Analyze(featureSet.fullPath, paramSeekRecord, startSeg, featureSet.segStackVec)) {
            std::map<std::string, std::string> segEventInfo;
            segAnalysis->GetResult(segEventInfo);
            for (const auto& seg : segEventInfo) {
                eventInfo_[seg.first] = seg.second;
            }
        }
    }
}

void FeatureAnalysis::UpdateStartSegment(const vector<pair<string, LineFeature>>& rec,
    vector<string>& startSeg) const
{
    for (auto&& name : startSeg) {
        auto isEqual = [&](const pair<string, LineFeature> &p) {return p.first == name;};
        auto iter = find_if(begin(rec), end(rec), isEqual);
        if (iter != end(rec)) {
            name = iter->second.value;  // replace with the actual feature value
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS
