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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_CHART_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_CHART_BRIDGE_H

#include "core/components/data_panel/data_panel_component.h"
#include "frameworks/bridge/common/dom/dom_chart.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"

namespace OHOS::Ace::Framework {

class V8ChartBridge : virtual public AceType {
    DECLARE_ACE_TYPE(V8ChartBridge, AceType)

public:
    void GetAttrOptionsObject(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject);

    void GetAttrDatasets(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valArray);

    void ParseAttrSegmentArray(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valArray);

    void ParseAttrSingleSegment(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject);

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
    std::vector<MainChart> datasets_;
    std::vector<Segment> segments_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_CHART_BRIDGE_H
