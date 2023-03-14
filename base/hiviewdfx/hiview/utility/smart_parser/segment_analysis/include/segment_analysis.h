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
#ifndef SEGMENT_ANALYSIS_H
#define SEGMENT_ANALYSIS_H
#include <memory>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "segment.h"
#include "syntax_rules.h"
namespace OHOS {
namespace HiviewDFX {
namespace {
static const std::string SEGMENT_CURSOR = "LineCursor";
static const std::string SEGMENT_CMD_LAYER_ONE = "LayerOneCmd";
static const std::string SEGMENT_DELIM = ".";
static const std::string RESULT_REASON = "REASON";
static const std::string RESULT_BLOCKED_CHAIN = "BLOCKED_CHAIN";
static const std::string RESULT_ALL_STACKS = "ALL_STACKS";
static const std::string RESULT_END_STACK = "END_STACK";
}

// base class
using ParamFeature = std::vector<std::pair<std::string, LineFeature>>;
using ParamFeatureIter = std::vector<std::pair<std::string, LineFeature>>::const_iterator;
using UniqSegment = std::unique_ptr<Segment>;
class SegmentAnalysis {
public:
    SegmentAnalysis() {}
    virtual ~SegmentAnalysis();
    SegmentAnalysis(const SegmentAnalysis&) = delete;
    SegmentAnalysis& operator=(const SegmentAnalysis&) = delete;
    SegmentAnalysis(SegmentAnalysis&&) = delete;
    SegmentAnalysis& operator=(SegmentAnalysis&&) = delete;

    bool Analyze(const std::string& logFile, const ParamFeature& feature,
        const std::vector<std::string>& startSeg, const std::vector<std::string>& segStackMatch);
    void GetResult(std::map<std::string, std::string>& result) const;
    void SetSegStatusCfg(std::map<std::string, std::vector<std::string>>& segStatusCfg)
    {
        segStatusCfg_ = segStatusCfg;
    };

protected:
    void AddSegment(int id, UniqSegment& seg);
    bool GetSegment(int id, UniqSegment& seg);
    std::string GetNameById(int id) const;
    void RecordLayerOneSegment(const ParamFeature& feature);
    virtual bool CheckParam(const ParamFeature& feature, const std::vector<std::string>& startSeg) const = 0;
    virtual void RecordOtherSegment(const ParamFeature& feature,
        int layerOneId, ParamFeatureIter begin, ParamFeatureIter end) = 0;
    virtual bool GetStartSegment(const std::vector<std::string>& startSeg) = 0;
    virtual bool NeedAnalyzeBlockedChain(void) const = 0;
    virtual void AnalyzeBlockedChain() = 0;
    virtual void GetBlockedChain(void) = 0;
    virtual void GetEndStack(const std::vector<std::string>& segStack) = 0;

protected:
    std::map<int, UniqSegment> segMap_; // key: id
    std::vector<std::tuple<int, std::string>> idNameVec_; // tuple: (id, name)
    UniqSegment startSeg_;
    std::string reason_;
    std::string blockedChain_;
    std::string allStacks_;
    std::string endStack_;
    std::stringstream errLogBuf_;
    std::map<std::string, std::vector<std::string>> segStatusCfg_;
    static constexpr int LAYER_ONE_CMD_PARAM_NUM = 2;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SEGMENT_ANALYSIS_H */
