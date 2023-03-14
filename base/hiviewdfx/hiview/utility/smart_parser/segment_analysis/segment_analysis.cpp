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
#include "segment_analysis.h"

#include <fcntl.h>
#include <unistd.h>

#include "file_util.h"
#include "log_util.h"
#include "logger.h"
#include "string_util.h"
using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("SegmentAnalysis");
SegmentAnalysis::~SegmentAnalysis()
{
    errLogBuf_.str("");
}

bool SegmentAnalysis::Analyze(const string& logFile, const ParamFeature& feature,
    const vector<string>& startSeg, const vector<string>& segStackMatch)
{
    if (!CheckParam(feature, startSeg)) {
        HIVIEW_LOGE("logFile: %{public}s, CheckParam failed", logFile.c_str());
        return false;
    }
    HIVIEW_LOGI("logFile: %{public}s, feature size: %{public}zu, startSeg: %{public}s",
        logFile.c_str(), feature.size(), startSeg[0].c_str());
    if (segMap_.size() == 0) {
        RecordLayerOneSegment(feature);
    }
    if (!GetStartSegment(startSeg) || startSeg_ == nullptr) {
        HIVIEW_LOGE("Failed to get any segment, segMap_ size: %{public}zu\n", segMap_.size());
        return false;
    }
    if (!LogUtil::ReadFileBuff(logFile, errLogBuf_)) {
        HIVIEW_LOGE("read logFile: %{public}s failed\n", logFile.c_str());
        return false;
    }
    if (NeedAnalyzeBlockedChain()) {
        AnalyzeBlockedChain();
        GetBlockedChain();
    }
    GetEndStack(segStackMatch);
    return true;
}

void SegmentAnalysis::GetResult(map<string, string>& result) const
{
    if (!reason_.empty()) {
        result[RESULT_REASON] = reason_;
    }
    if (!blockedChain_.empty()) {
        result[RESULT_BLOCKED_CHAIN] = blockedChain_;
    }
    if (!allStacks_.empty()) {
        result[RESULT_ALL_STACKS] = allStacks_;
    }
    if (!endStack_.empty()) {
        result[RESULT_END_STACK] = endStack_;
    }
}

void SegmentAnalysis::AddSegment(int id, UniqSegment& seg)
{
    if (id < 0) {
        HIVIEW_LOGE("segment id invalid: %{public}d", id);
        return;
    }
    idNameVec_.push_back(make_tuple(id, seg->GetName()));
    segMap_.insert(make_pair(id, move(seg))); // if id already exists, no more update
}

bool SegmentAnalysis::GetSegment(int id, UniqSegment& seg)
{
    auto it = segMap_.find(id);
    if (it != segMap_.end()) {
        seg = move(it->second);
        segMap_.erase(it); // no need to keep it in the map
        return true;
    }
    return false;
}

string SegmentAnalysis::GetNameById(int id) const
{
    for (const auto& idName : idNameVec_) {
        if (get<0>(idName) == id) {
            return get<1>(idName);
        }
    }
    return string("");
}

void SegmentAnalysis::RecordLayerOneSegment(const ParamFeature& feature)
{
    /*
     * examples of feature:
     * ("LayerOneCmd.v_id", LineFeature)
     * ("LayerOneCmd.v_name", LineFeature)
     * ...
     * ("LayerOneCmd.v_id", LineFeature)
     * ("LayerOneCmd.v_name", LineFeature)
     */
    auto isFirst = [](const pair<string, LineFeature> &p) {return p.first.find(SEGMENT_CMD_LAYER_ONE) == 0;};
    auto iter = find_if(begin(feature), end(feature), isFirst);
    while (iter != end(feature)) {
        auto nextBegin = iter->first;
        string value = iter->second.value;
        auto seg = make_unique<Segment>();
        seg->SetValue(StringUtil::GetRightSubstr(iter->first, SEGMENT_DELIM), value);
        seg->SetValue(SEGMENT_CURSOR, to_string(iter->second.lineCursor));
        for (int i = 1; i < LAYER_ONE_CMD_PARAM_NUM; ++i) {
            ++iter; // to prevent dead loop
            if (iter != end(feature) &&
                iter->first != nextBegin &&
                iter->first.find(SEGMENT_CMD_LAYER_ONE) == 0) {
                seg->SetValue(StringUtil::GetRightSubstr(iter->first, SEGMENT_DELIM), iter->second.value);
            } else {
                HIVIEW_LOGE("wrong format, nextBegin: %{public}s %{public}s", nextBegin.c_str(), value.c_str());
                break;
            }
        }
        int id = seg->GetId();
        AddSegment(id, seg);
        auto isNext = [&](const pair<string, LineFeature> &p) {return p.first == nextBegin;};
        auto iterNext = find_if(iter, end(feature), isNext);
        RecordOtherSegment(feature, id, iter, iterNext);
        iter = iterNext;
    }
}
} // namespace HiviewDFX
} // namespace OHOS