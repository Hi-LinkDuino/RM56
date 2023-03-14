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
#include "segment_analysis_onelayer.h"

#include <unistd.h>

#include "log_util.h"
#include "logger.h"
#include "string_util.h"
using namespace std;
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("SegmentAnalysisOneLayer");
bool SegmentAnalysisOneLayer::CheckParam(const ParamFeature& feature, const vector<string>& startSeg) const
{
    if (startSeg.size() != 1 || feature.size() == 0) {
        HIVIEW_LOGE("startSeg size: %{public}zu, ParamFeature size: %{public}zu", startSeg.size(), feature.size());
        return false;
    }
    return true;
}

bool SegmentAnalysisOneLayer::GetStartSegment(const vector<string>& startSeg)
{
    if (GetSegment(StringUtil::StrToInt(startSeg[0]), startSeg_)) {
        return true;
    }
    HIVIEW_LOGI("Failed to get startSeg: %{public}s, just analyze the first", startSeg[0].c_str());
    if (segMap_.empty()) {
        HIVIEW_LOGE("Error, segment record empty!");
        return false;
    }
    auto it = segMap_.begin();
    startSeg_ = move(it->second);
    segMap_.erase(it); // no need to keep it
    return true;
}

void SegmentAnalysisOneLayer::GetEndStack(const vector<string>& segStack)
{
    int cursor = StringUtil::StrToInt(startSeg_->GetValue(SEGMENT_CURSOR));
    if (cursor < 0) {
        HIVIEW_LOGE("Failed to get cursor, segment: %{public}s", startSeg_->GetName().c_str());
        return;
    }
    // only extract endStack, reason and blockedChain keep empty
    string stackMatch = (segStack.size() == 2) ? segStack[0] : DEFAULT_LAYER_ONE_STACK_MATCH; // 2: size of segStack
    string stackStart = (segStack.size() == 2) ? segStack[1] : stackMatch; // 2: size of segStack
    LogUtil::GetTrace(errLogBuf_, cursor, stackMatch, endStack_, stackStart);
}
} // namespace HiviewDFX
} // namespace OHOS