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
#ifndef SEGMENT_ANALYSIS_ONELAYER_H
#define SEGMENT_ANALYSIS_ONELAYER_H
#include "segment_analysis.h"
namespace OHOS {
namespace HiviewDFX {
namespace {
static const std::string DEFAULT_LAYER_ONE_STACK_MATCH = "\\w+\\+0x[0-9a-f]+/0x[0-9a-f]+";
}
// analyze one-layer segments, such as hungtask panic
class SegmentAnalysisOneLayer : public SegmentAnalysis {
public:
    SegmentAnalysisOneLayer() {}
    ~SegmentAnalysisOneLayer() override {}
    SegmentAnalysisOneLayer(const SegmentAnalysisOneLayer&) = delete;
    SegmentAnalysisOneLayer& operator=(const SegmentAnalysisOneLayer&) = delete;
    SegmentAnalysisOneLayer(SegmentAnalysisOneLayer&&) = delete;
    SegmentAnalysisOneLayer& operator=(SegmentAnalysisOneLayer&&) = delete;

protected:
    bool CheckParam(const ParamFeature& feature, const std::vector<std::string>& startSeg) const override;
    void RecordOtherSegment(const ParamFeature& feature,
        int layerOneId, ParamFeatureIter begin, ParamFeatureIter end) override {}
    bool GetStartSegment(const std::vector<std::string>& startSeg) override;
    bool NeedAnalyzeBlockedChain(void) const override
    {
        return false;
    }
    void AnalyzeBlockedChain() override {}
    void GetBlockedChain(void) override {}
    void GetEndStack(const std::vector<std::string>& segStack) override;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SEGMENT_ANALYSIS_ONELAYER_H */
