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
#ifndef SEGMENT_ANALYSIS_FACTORY_H
#define SEGMENT_ANALYSIS_FACTORY_H
#include "segment_analysis_onelayer.h"
#include "segment_analysis_twolayers.h"
#include "singleton.h"
namespace OHOS {
namespace HiviewDFX {
namespace {
static const std::string SEGMENT_TYPE_ONELAYER = "OneLayer";
static const std::string SEGMENT_TYPE_TWOLAYERS = "TwoLayers";
}
class SegmentAnalysisFactory : public Singleton<SegmentAnalysisFactory> {
public:
    SegmentAnalysisFactory() {};
    ~SegmentAnalysisFactory() {};
    SegmentAnalysisFactory(const SegmentAnalysisFactory&) = delete;
    SegmentAnalysisFactory& operator=(const SegmentAnalysisFactory&) = delete;
    SegmentAnalysisFactory(SegmentAnalysisFactory&&) = delete;
    SegmentAnalysisFactory& operator=(SegmentAnalysisFactory&&) = delete;
    std::unique_ptr<SegmentAnalysis> CreateParser(const std::string& type) const;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif /* SEGMENT_ANALYSIS_FACTORY_H */
