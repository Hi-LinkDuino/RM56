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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_CHART_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_CHART_BRIDGE_H

#include "frameworks/bridge/common/dom/dom_chart.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

class ChartBridge : virtual public AceType {
    DECLARE_ACE_TYPE(ChartBridge, AceType)

public:
    void GetAttrOptionsObject(JSContext* ctx, JSValueConst valObject);

    void ParseAttrSegmentArray(JSContext* ctx, JSValueConst valArray);

    void ParseAttrSingleSegment(JSContext* ctx, JSValueConst valArray);

    void GetAttrDatasets(JSContext* ctx, JSValueConst valArray);

    void ParseAttrDataStyle(JSContext* ctx, JSValueConst valObject, LineInfo& line, double index);

    const ChartOptions& GetChartOptions() const
    {
        return chartOptions_;
    }

    const std::vector<MainChart>& GetDatasets() const
    {
        return datasets_;
    }

    const std::vector<Segment>& GetSegments() const
    {
        return segments_;
    }

private:
    ChartOptions chartOptions_;
    std::vector<Segment> segments_;
    std::vector<MainChart> datasets_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_CHART_BRIDGE_H
