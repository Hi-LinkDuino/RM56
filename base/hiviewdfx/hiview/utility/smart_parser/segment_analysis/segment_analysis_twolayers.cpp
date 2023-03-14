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
#include "segment_analysis_twolayers.h"

#include <unistd.h>

#include "logger.h"
#include "log_util.h"
#include "string_util.h"
using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("SegmentAnalysisTwoLayers");
SegmentAnalysisTwoLayers::SegmentAnalysisTwoLayers()
{
    nextHandleVec_.clear();
    nextHandleVec_.push_back(make_pair(SEGMENT_WAIT_OTHER,
        bind(&SegmentAnalysisTwoLayers::GetNextSegWaitOther, this,
            placeholders::_1, placeholders::_2, placeholders::_3)));
    nextHandleVec_.push_back(make_pair(SEGMENT_IPC_TRANS,
        bind(&SegmentAnalysisTwoLayers::GetNextSegIpcTrans, this,
            placeholders::_1, placeholders::_2, placeholders::_3)));
    nextHandleVec_.push_back(make_pair(SEGMENT_IPC_FULL,
        bind(&SegmentAnalysisTwoLayers::GetNextSegIpcFull, this,
            placeholders::_1, placeholders::_2, placeholders::_3)));
}

bool SegmentAnalysisTwoLayers::CheckParam(const ParamFeature& feature, const vector<string>& startSeg) const
{
    if (startSeg.size() != 2 || feature.size() == 0) { // 2: size of startSeg vector
        HIVIEW_LOGE("startSeg size: %{public}zu, ParamFeature size: %{public}zu", startSeg.size(), feature.size());
        return false;
    }
    return true;
}

void SegmentAnalysisTwoLayers::AddSegment(UniqSegment& seg)
{
    if (curLayerOneSegId_ < 0) {
        HIVIEW_LOGE("segment id invalid: %{public}d", curLayerOneSegId_);
        return;
    }
    segInfoVec_.push_back(make_tuple(curLayerOneSegId_, seg->GetValue(SEGMENT_PARENT_NAME),
                                     seg->GetId(), seg->GetName(), seg->GetSysTid()));
    pairMap_.insert(make_pair(make_pair(curLayerOneSegId_, seg->GetId()), move(seg)));
}

// key: pair(pid, tid)
bool SegmentAnalysisTwoLayers::GetSegment(const pair<int, int>& key, UniqSegment& seg)
{
    auto it = pairMap_.find(key);
    if (it != pairMap_.end()) {
        seg = move(it->second);
        pairMap_.erase(it);
        return true;
    }
    return false;
}

// key: pair(pname, tname)
bool SegmentAnalysisTwoLayers::GetSegment(const pair<string, string>& key, UniqSegment& seg)
{
    pair<int, int> p(-1, -1);
    for (const auto& info : segInfoVec_) {
        if (get<TUPLE_IDX_PNAME>(info) == key.first && get<TUPLE_IDX_TNAME>(info) == key.second) {
            p.first = get<TUPLE_IDX_PID>(info);
            p.second = get<TUPLE_IDX_TID>(info);
            break;
        }
    }
    return (p.first < 0 || p.second < 0) ? false : GetSegment(p, seg);
}

// key: pair(pid, sysTid)
bool SegmentAnalysisTwoLayers::GetSegmentBySysTid(const pair<int, int>& key, UniqSegment& seg)
{
    pair<int, int> p(-1, -1);
    for (const auto& info : segInfoVec_) {
        if (get<TUPLE_IDX_PID>(info) == key.first && get<TUPLE_IDX_SYSTID>(info) == key.second) {
            p.first = get<TUPLE_IDX_PID>(info);
            p.second = get<TUPLE_IDX_TID>(info);
            break;
        }
    }
    return (p.first < 0 || p.second < 0) ? false : GetSegment(p, seg);
}

bool SegmentAnalysisTwoLayers::GetStartSegment(const vector<string>& startSeg)
{
    // search by (pid, sysTid) or (processName, threadName)
    int pid = StringUtil::StrToInt(startSeg[0]);
    int sysTid = StringUtil::StrToInt(startSeg[1]);
    if (pid > 0 && sysTid > 0) {
        if (!GetSegmentBySysTid(make_pair(pid, sysTid), startSeg_)) {
            HIVIEW_LOGE("Failed to get segment: %{public}s %{public}s", startSeg[0].c_str(), startSeg[1].c_str());
            return false;
        }
    } else {
        if (!GetSegment(make_pair(startSeg[0], startSeg[1]), startSeg_)) {
            HIVIEW_LOGE("Failed to get segment: %{public}s %{public}s", startSeg[0].c_str(), startSeg[1].c_str());
            if (!GetSegment(make_pair(startSeg[0], SEGMENT_DEFAULT_THD_NAME), startSeg_)) {
                HIVIEW_LOGE("Failed to get segment: %{public}s main", startSeg[0].c_str());
                return false;
            }
        }
    }
    return true;
}

void SegmentAnalysisTwoLayers::RecordLayerTwoSegment(const ParamFeature& feature,
    ParamFeatureIter begin, ParamFeatureIter end)
{
    auto isFirst = [](const pair<string, LineFeature> &p) {return p.first.find(SEGMENT_CMD_LAYER_TWO) == 0;};
    auto iter = find_if(begin, end, isFirst);
    while (iter != end) {
        auto nextBegin = iter->first;
        string value = iter->second.value;
        auto seg = make_unique<Segment>();
        seg->SetValue(StringUtil::GetRightSubstr(iter->first, SEGMENT_DELIM), iter->second.value);
        seg->SetValue(SEGMENT_CURSOR, to_string(iter->second.lineCursor));
        seg->SetValue(SEGMENT_PARENT_ID, to_string(curLayerOneSegId_));
        seg->SetValue(SEGMENT_PARENT_NAME, GetNameById(curLayerOneSegId_));
        for (int i = 1; i < LAYER_TWO_CMD_PARAM_NUM; ++i) {
            ++iter; // to prevent dead loop
            if (iter != end &&
                iter->first != nextBegin &&
                iter->first.find(SEGMENT_CMD_LAYER_TWO) == 0) {
                seg->SetValue(StringUtil::GetRightSubstr(iter->first, SEGMENT_DELIM), iter->second.value);
            } else {
                HIVIEW_LOGE("wrong format, nextBegin: %{public}s %{public}s", nextBegin.c_str(), value.c_str());
                break;
            }
        }
        AddSegment(seg);
        auto isNext = [&](const pair<string, LineFeature> &p) {return p.first == nextBegin;};
        iter = find_if(iter, end, isNext);
    }
}

void SegmentAnalysisTwoLayers::RecordIpcSegment(const ParamFeature& feature)
{
    if (isIpcBuilded_) {
        return;
    }
    auto isEqual = [](const pair<string, LineFeature> &p) {return p.first == SEGMENT_CMD_IPC + "." + SEGMENT_NAME;};
    auto iter = find_if(begin(feature), end(feature), isEqual);
    if (iter != end(feature)) {
        ipcSeg_ = make_unique<Segment>();
        ipcSeg_->SetValue(SEGMENT_NAME, iter->second.value);
        ipcSeg_->SetValue(SEGMENT_CURSOR, to_string(iter->second.lineCursor));
    }
    isIpcBuilded_ = true;
}

void SegmentAnalysisTwoLayers::RecordOtherSegment(const ParamFeature& feature,
    int layerOneId, ParamFeatureIter begin, ParamFeatureIter end)
{
    /*
     * examples of feature:
     * ("LayerOneCmd.s_id", LineFeature)  -- two-layers segment starts
     * ("LayerOneCmd.s_name", LineFeature)
     * ("LayerTwoCmd.s_id", LineFeature)
     * ("LayerTwoCmd.s_name", LineFeature)
     * ("LayerTwoCmd.s_sysTid", LineFeature)
     * ...
     * ("LayerTwoCmd.s_id", LineFeature)
     * ("LayerTwoCmd.s_name", LineFeature)
     * ("LayerTwoCmd.s_sysTid", LineFeature)
     * ("LayerOneCmd.s_id", LineFeature)  -- another two-layers segment starts
     * ("LayerOneCmd.s_name", LineFeature)
     * ("LayerTwoCmd.s_id", LineFeature)
     * ("LayerTwoCmd.s_name", LineFeature)
     * ("LayerTwoCmd.s_sysTid", LineFeature)
     * ...
     * ("LayerTwoCmd.s_id", LineFeature)
     * ("LayerTwoCmd.s_name", LineFeature)
     * ("LayerTwoCmd.s_sysTid", LineFeature)
     */
    if (layerOneId < 0) {
        HIVIEW_LOGE("segment id invalid: %{public}d", layerOneId);
        return;
    }
    SetCurLayerOneSegmentId(layerOneId); // when a new two-layers segment starts, save its id
    RecordLayerTwoSegment(feature, begin, end);
    RecordIpcSegment(feature);
}

bool SegmentAnalysisTwoLayers::HasNextSegment(const UniqSegment& seg) const
{
    if (!seg->GetValue(SEGMENT_WAIT_OTHER).empty() ||
        !seg->GetValue(SEGMENT_IPC_TRANS).empty() ||
        !seg->GetValue(SEGMENT_IPC_FULL).empty()) {
        return true;
    }
    return false;
}

bool SegmentAnalysisTwoLayers::GetNextSegWaitOther(int pid, const UniqSegment& seg, UniqSegment& nextSeg)
{
    return GetSegment(make_pair(pid, StringUtil::StrToInt(seg->GetValue(SEGMENT_WAIT_OTHER))), nextSeg);
}

bool SegmentAnalysisTwoLayers::GetNextSegIpcTrans(int pid, UniqSegment& seg, UniqSegment& nextSeg)
{
    if (reason_.empty()) {
        reason_ = REASON_IPC_TIMEOUT;
    }
    if (ipcSeg_ == nullptr) {
        HIVIEW_LOGE("Failed to get ipc segment, cur %{public}d:%{public}d", pid, seg->GetSysTid());
        return false;
    }
    int cursor = StringUtil::StrToInt(ipcSeg_->GetValue(SEGMENT_CURSOR));
    if (cursor < 0) {
        HIVIEW_LOGE("Failed to get cursor, segment: %{public}s", ipcSeg_->GetName().c_str());
        return false;
    }
    HIVIEW_LOGI("Begin parse ipc info, cursor:%{public}d", cursor);
    LogUtil logParse;
    pair<int, int> pidTid(-1, -1);
    vector<string> lines;
    tie(pidTid, lines) = logParse.ParseIpcInfo(errLogBuf_, cursor, make_pair(pid, seg->GetSysTid()));
    if (!errLogBuf_.good()) {
        HIVIEW_LOGI("After parse ipc info, the state may change to EOF, need to clear");
        errLogBuf_.clear();
    }
    if (pidTid.first > 0) {
        string ipcInfo;
        for (const auto& line : lines) {
            ipcInfo += line + LogUtil::SPLIT_PATTERN;
        }
        seg->SetValue(SEGMENT_IPC_INFO, ipcInfo);
        HIVIEW_LOGI("ipc target, %{public}d:%{public}d", pidTid.first, pidTid.second);
        return GetSegmentBySysTid(pidTid, nextSeg);
    }
    HIVIEW_LOGE("Parse ipc info failed, cursor:%{public}d, %{public}d:%{public}d", cursor, pid, seg->GetSysTid());
    return false;
}

void SegmentAnalysisTwoLayers::ExtractSegment(UniqSegment& seg)
{
    if (!seg->GetValue(SEGMENT_EXTRACTED).empty()) {
        HIVIEW_LOGI("segment: %{public}s already extracted", seg->GetName().c_str());
        return;
    }
    int cursor = StringUtil::StrToInt(seg->GetValue(SEGMENT_CURSOR));
    if (cursor < 0) {
        HIVIEW_LOGE("Failed to get cursor, segment: %{public}s", seg->GetName().c_str());
        return;
    }
    errLogBuf_.seekg(cursor, ios::beg);
    int num = 0;
    string line;
    while (getline(errLogBuf_, line) && !line.empty() && num++ < LogUtil::TOTAL_LINE_NUM) {
        for (auto iter = segStatusCfg_.begin(); iter != segStatusCfg_.end(); iter++) {
            smatch result;
            if (!iter->second[0].empty() && regex_search(line, result, regex(iter->second[0]))) {
                seg->SetValue(iter->first, result.str(1));
            }
        }
        if (segStatusCfg_.empty()) {
            break;
        }
    }
    seg->SetValue(SEGMENT_EXTRACTED, "true");
}

bool SegmentAnalysisTwoLayers::GetFirstBlockedIpcSeg(int pid, UniqSegment& seg)
{
    for (const auto& i : segInfoVec_) {
        if (get<TUPLE_IDX_PID>(i) == pid && get<TUPLE_IDX_TNAME>(i).find(SEGMENT_IPC_THREAD + to_string(pid)) == 0) {
            auto it = pairMap_.find(make_pair(get<TUPLE_IDX_PID>(i), get<TUPLE_IDX_TID>(i)));
            if (it == pairMap_.end()) {
                continue;
            }
            if (it->second->GetValue(SEGMENT_STATUS).empty()) { // hasn't been extracted yet
                ExtractSegment(it->second);
            }
            if (it->second->GetValue(SEGMENT_STATUS) != "Runnable") {
                seg = move(it->second);
                pairMap_.erase(it);
                return true;
            }
        }
    }
    return false;
}

bool SegmentAnalysisTwoLayers::GetNextSegIpcFull(int pid, const UniqSegment& seg, UniqSegment& nextSeg)
{
    if (reason_.empty()) {
        reason_ = REASON_IPC_FULL;
    }
    return GetFirstBlockedIpcSeg(pid, nextSeg);
}

bool SegmentAnalysisTwoLayers::GetNextSegment(UniqSegment& seg, UniqSegment& nextSeg)
{
    int pid = StringUtil::StrToInt(seg->GetValue(SEGMENT_PARENT_ID));
    if (pid < 0) {
        HIVIEW_LOGE("Invalid pid: %{public}d, segment: %{public}s", pid, seg->GetName().c_str());
        return false;
    }
    for (const auto& handle : nextHandleVec_) {
        if (!seg->GetValue(handle.first).empty()) {
            return handle.second(pid, seg, nextSeg);
        }
    }
    HIVIEW_LOGI("No next handle for segment: %{public}s", seg->GetName().c_str());
    return false;
}

bool SegmentAnalysisTwoLayers::NextSegHasAnalyzed(const UniqSegment& seg, Segment*& retSeg) const
{
    int lockTookTid = StringUtil::StrToInt(seg->GetValue(SEGMENT_WAIT_OTHER));
    if (lockTookTid < 0) {
        return false;
    }
    int pid = StringUtil::StrToInt(seg->GetValue(SEGMENT_PARENT_ID));
    auto isEqual = [&](const UniqSegment& s) {return s->GetId() == lockTookTid &&
        StringUtil::StrToInt(s->GetValue(SEGMENT_PARENT_ID)) == pid;};
    auto iter = find_if(begin(blockedSegVec_), end(blockedSegVec_), isEqual);
    if (iter != end(blockedSegVec_)) {
        retSeg = (*iter).get();
        return true;
    }
    return false;
}

// from startSeg, add all dependent segments to blockedSegVec_, update reason_ at the same time
void SegmentAnalysisTwoLayers::AnalyzeBlockedChain()
{
    ExtractSegment(startSeg_);
    blockedSegVec_.push_back(move(startSeg_));
    startSeg_ = nullptr;

    UniqSegment nextSeg = nullptr;
    while (HasNextSegment(blockedSegVec_.back()) && blockedSegVec_.size() <= SEGMENT_MAX_BLOCKED_NUM) {
        if (GetNextSegment(blockedSegVec_.back(), nextSeg) && nextSeg != nullptr) {
            ExtractSegment(nextSeg);
            blockedSegVec_.push_back(move(nextSeg));
        } else {
            Segment* retSeg = nullptr;
            if (NextSegHasAnalyzed(blockedSegVec_.back(), retSeg)) {
                reason_ = REASON_DEAD_LOCK; // overwrite old value
            }
            break;
        }
    }
    if (reason_.empty()) {
        reason_ = REASON_THREAD_TIMEOUT; // set default value
    }
}

string SegmentAnalysisTwoLayers::GetOriginalInfo(int cursor)
{
    errLogBuf_.seekg(cursor, ios::beg);
    string ret;
    string line;
    int num = 0;
    while (getline(errLogBuf_, line) && num++ < LogUtil::TOTAL_LINE_NUM) {
        if (line.empty()) {
            break;
        }
        ret += line + LogUtil::SPLIT_PATTERN;
    }
    return ret + LogUtil::SPLIT_PATTERN;
}

void SegmentAnalysisTwoLayers::GetBlockedChain(void)
{
    string desc;
    size_t vecSize = blockedSegVec_.size();
    for (size_t i = 0; i < vecSize; ++i) {
        allStacks_ += GetOriginalInfo(StringUtil::StrToInt(blockedSegVec_[i]->GetValue(SEGMENT_CURSOR)));
        desc += blockedSegVec_[i]->GetDesc(segStatusCfg_);
        if (i == vecSize - 1) {
            Segment* retSeg = nullptr;
            if (NextSegHasAnalyzed(blockedSegVec_[i], retSeg)) {
                desc += retSeg->GetFullName();
            }
            break;
        }
        if (!blockedSegVec_[i]->GetValue(SEGMENT_WAIT_OTHER).empty()) {
            desc += blockedSegVec_[i + 1]->GetFullName() + "," + LogUtil::SPLIT_PATTERN;
        } else if (!blockedSegVec_[i]->GetValue(SEGMENT_IPC_TRANS).empty()) {
            desc += LogUtil::SPLIT_PATTERN + blockedSegVec_[i]->GetValue(SEGMENT_IPC_INFO);
        } else {
            desc += LogUtil::SPLIT_PATTERN;
        }
    }
    blockedChain_ = desc;
}

void SegmentAnalysisTwoLayers::GetEndStack(const vector<string>& segStack)
{
    int cursor = StringUtil::StrToInt(blockedSegVec_.back()->GetValue(SEGMENT_CURSOR));
    if (cursor < 0) {
        HIVIEW_LOGE("Failed to get cursor, segment: %{public}s", blockedSegVec_.back()->GetName().c_str());
        return;
    }
    string stackMatch = (segStack.size() == 2) ? segStack[0] : DEFAULT_LAYER_TWO_STACK_MATCH; // 2: size of segStack
    string stackStart = (segStack.size() == 2) ? segStack[1] : stackMatch; // 2: size of segStack
    LogUtil::GetTrace(errLogBuf_, cursor, stackMatch, endStack_, stackStart);
}
} // namespace HiviewDFX
} // namespace OHOS