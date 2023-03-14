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
#ifndef SEGMENT_ANALYSIS_TWOLAYERS_H
#define SEGMENT_ANALYSIS_TWOLAYERS_H
#include <functional>
#include <unordered_map>

#include "segment_analysis.h"
namespace OHOS {
namespace HiviewDFX {
namespace {
static const std::string SEGMENT_STATUS = "segStatus";
static const std::string SEGMENT_PARENT_ID = "parentId";
static const std::string SEGMENT_EXTRACTED = "extracted";
static const std::string SEGMENT_IPC_INFO = "ipcInfo";
static const std::string SEGMENT_IPC_THREAD = "Binder:";
static const std::string SEGMENT_DEFAULT_THD_NAME = "main";
static const std::string SEGMENT_CMD_LAYER_TWO = "LayerTwoCmd";
static const std::string SEGMENT_CMD_IPC = "IpcCmd";
static const std::string SEGMENT_WAIT_OTHER = "segWaitOther";
static const std::string SEGMENT_IPC_TRANS = "segIpcTrans";
static const std::string SEGMENT_IPC_FULL = "segIpcFull";
static const std::string REASON_THREAD_TIMEOUT = "THREAD_TIMEOUT";
static const std::string REASON_IPC_TIMEOUT = "IPC_TIMEOUT";
static const std::string REASON_IPC_FULL = "IPC_FULL";
static const std::string REASON_DEAD_LOCK = "DEAD_LOCK";
static const std::string DEFAULT_LAYER_TWO_STACK_MATCH = "^\\s+at\\s+|^\\s+-\\s\\w";
}
// analyze two-layers segments, such as appfreeze
using NextHandle = std::function<bool(int, UniqSegment&, UniqSegment&)>;
struct PairHash {
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>()(p.first);
        auto h2 = std::hash<T2>()(p.second);
        return h1 ^ h2;
    }
};
class SegmentAnalysisTwoLayers : public SegmentAnalysis {
public:
    SegmentAnalysisTwoLayers();
    ~SegmentAnalysisTwoLayers() override {}
    SegmentAnalysisTwoLayers(const SegmentAnalysisTwoLayers&) = delete;
    SegmentAnalysisTwoLayers& operator=(const SegmentAnalysisTwoLayers&) = delete;
    SegmentAnalysisTwoLayers(SegmentAnalysisTwoLayers&&) = delete;
    SegmentAnalysisTwoLayers& operator=(SegmentAnalysisTwoLayers&&) = delete;

protected:
    bool CheckParam(const ParamFeature& feature, const std::vector<std::string>& startSeg) const override;
    void RecordOtherSegment(const ParamFeature& feature,
        int layerOneId, ParamFeatureIter begin, ParamFeatureIter end) override;
    bool GetStartSegment(const std::vector<std::string>& startSeg) override;
    bool NeedAnalyzeBlockedChain(void) const override
    {
        return true;
    }
    void AnalyzeBlockedChain() override;
    void GetBlockedChain(void) override;
    void GetEndStack(const std::vector<std::string>& segStack) override;

private:
    void SetCurLayerOneSegmentId(int id)
    {
        curLayerOneSegId_ = id;
    }
    void AddSegment(UniqSegment& seg);
    bool GetSegment(const std::pair<std::string, std::string>& key, UniqSegment& seg); // key: pair(pname, tname)
    bool GetSegment(const std::pair<int, int>& key, UniqSegment& seg); // key: pair(pid, tid)
    bool GetSegmentBySysTid(const std::pair<int, int>& key, UniqSegment& seg); // key: pair(pid, sysTid)
    void RecordLayerTwoSegment(const ParamFeature& feature, ParamFeatureIter begin, ParamFeatureIter end);
    void RecordIpcSegment(const ParamFeature& feature);
    bool HasNextSegment(const UniqSegment& seg) const;
    bool GetNextSegWaitOther(int pid, const UniqSegment& seg, UniqSegment& nextSeg);
    bool GetNextSegIpcTrans(int pid, UniqSegment& seg, UniqSegment& nextSeg);
    void ExtractSegment(UniqSegment& seg);
    bool GetFirstBlockedIpcSeg(int pid, UniqSegment& seg);
    bool GetNextSegIpcFull(int pid, const UniqSegment& seg, UniqSegment& nextSeg);
    bool GetNextSegment(UniqSegment& seg, UniqSegment& nextSeg);
    bool NextSegHasAnalyzed(const UniqSegment& seg, Segment*& retSeg) const;
    std::string GetOriginalInfo(int cursor);

    int curLayerOneSegId_ {-1};
    bool isIpcBuilded_ {false};
    UniqSegment ipcSeg_;
    std::unordered_map<std::pair<int, int>, UniqSegment, PairHash> pairMap_; // key: pair(pid, tid)
    // tuple: pid pname tid tname systid, store segment info by their order in log file
    std::vector<std::tuple<int, std::string, int, std::string, int>> segInfoVec_;
    std::vector<std::pair<std::string, NextHandle>> nextHandleVec_;
    std::vector<UniqSegment> blockedSegVec_;
    static constexpr int LAYER_TWO_CMD_PARAM_NUM = 3;
    static constexpr int TUPLE_IDX_PID = 0;
    static constexpr int TUPLE_IDX_PNAME = 1;
    static constexpr int TUPLE_IDX_TID = 2;
    static constexpr int TUPLE_IDX_TNAME = 3;
    static constexpr int TUPLE_IDX_SYSTID = 4;
    static constexpr size_t SEGMENT_MAX_BLOCKED_NUM = 16;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SEGMENT_ANALYSIS_TWOLAYERS_H */
