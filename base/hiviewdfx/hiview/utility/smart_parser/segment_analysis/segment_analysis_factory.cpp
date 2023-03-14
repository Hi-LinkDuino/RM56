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
#include "segment_analysis_factory.h"

#include "logger.h"
using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("SegmentAnalysisFactory");
unique_ptr<SegmentAnalysis> SegmentAnalysisFactory::CreateParser(const string& type) const
{
    unique_ptr<SegmentAnalysis> parser = nullptr;
    if (type == SEGMENT_TYPE_ONELAYER) {
        parser = make_unique<SegmentAnalysisOneLayer>();
    } else if (type == SEGMENT_TYPE_TWOLAYERS) {
        parser = make_unique<SegmentAnalysisTwoLayers>();
    } else {
        HIVIEW_LOGE("CreateParser() failed, invalid type: %{public}s", type.c_str());
    }
    return parser;
}
} // namespace HiviewDFX
} // namespace OHOS