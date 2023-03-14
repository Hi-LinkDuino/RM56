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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_chart_bridge.h"

#include "base/utils/linear_map.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t VALID_ARRAY_LENGTH = 2;
constexpr uint32_t ARRAY_X_VALUE = 0;
constexpr uint32_t ARRAY_Y_VALUE = 1;

void GetAttrOptionsAxis(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, AxisOption& axisOption)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> item = valObject->GetProperty(runtime, key);
        if (!item) {
            LOGW("item value is null. Ignoring!");
            continue;
        }
        if (item->IsNumber(runtime) || item->IsBoolean(runtime) || item->IsString(runtime)) {
            std::string valStr = item->ToString(runtime);
            // must sorted by key.
            static const LinearMapNode<void (*)(const std::string&, AxisOption&)> chartOptionsAxisMap[] = {
                { "axisTick",
                    [](const std::string& valStr, AxisOption& axis) { axis.tickNumber = StringToInt(valStr); } },
                { "color",
                    [](const std::string& valStr, AxisOption& axis) { axis.color = Color::FromString(valStr); } },
                { "display", [](const std::string& valStr, AxisOption& axis) { axis.display = StringToBool(valStr); } },
                { "max", [](const std::string& valStr, AxisOption& axis) { axis.max = StringToDouble(valStr); } },
                { "min", [](const std::string& valStr, AxisOption& axis) { axis.min = StringToDouble(valStr); } },
            };
            auto iter = BinarySearchFindIndex(chartOptionsAxisMap, ArraySize(chartOptionsAxisMap), keyStr.c_str());
            if (iter != -1) {
                chartOptionsAxisMap[iter].value(valStr, axisOption);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetAttrOptionsSeriesPoint(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, PointInfo& pointInfo)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    pointInfo.SetDisplay(true);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> item = valObject->GetProperty(runtime, key);
        if (!item) {
            LOGW("item value is null. Ignoring!");
            continue;
        }
        if (item->IsNumber(runtime) || item->IsBoolean(runtime) || item->IsString(runtime)) {
            std::string valStr = item->ToString(runtime);
            static const LinearMapNode<void (*)(const std::string&, PointInfo&)> chartOptionsPointMap[] = {
                { "display", [](const std::string& valStr,
                                 PointInfo& pointInfo) { pointInfo.SetDisplay(StringToBool(valStr)); } },
                { "fillColor", [](const std::string& valStr,
                                   PointInfo& pointInfo) { pointInfo.SetFillColor(Color::FromString(valStr)); } },
                { "shape",
                    [](const std::string& valStr, PointInfo& pointInfo) {
                        PointShape shape = (valStr == "circle")
                                               ? PointShape::CIRCLE
                                               : (valStr == "square") ? PointShape::SQUARE : PointShape::TRIANGLE;
                        pointInfo.SetPointShape(shape);
                    } },
                { "size", [](const std::string& valStr,
                              PointInfo& pointInfo) { pointInfo.SetPointSize(StringToDimension(valStr)); } },
                { "strokeColor", [](const std::string& valStr,
                                     PointInfo& pointInfo) { pointInfo.SetStrokeColor(Color::FromString(valStr)); } },
                { "strokeWidth",
                    [](const std::string& valStr, PointInfo& pointInfo) {
                        pointInfo.SetPointStrokeWidth(StringToDimension(valStr));
                    } },
            };
            auto iter = BinarySearchFindIndex(chartOptionsPointMap, ArraySize(chartOptionsPointMap), keyStr.c_str());
            if (iter != -1) {
                chartOptionsPointMap[iter].value(valStr, pointInfo);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetChartAttrOptionsSeriesLineStyle(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, ChartOptions& chartOptions)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> item = valObject->GetProperty(runtime, key);
        if (!item) {
            LOGW("item value is null. Ignoring!");
            continue;
        }
        if (item->IsNumber(runtime) || item->IsBoolean(runtime) || item->IsString(runtime)) {
            std::string valStr = item->ToString(runtime);
            static const LinearMapNode<void (*)(const std::string&, ChartOptions&)> chartOptionsSeriesLineStyleMap[] = {
                { "lineGradient", [](const std::string& valStr, ChartOptions& chartOptions) {
                    chartOptions.SetWholeLineGradient(StringToBool(valStr)); } },
                { "smooth", [](const std::string& valStr,
                                ChartOptions& chartOptions) { chartOptions.SetSmoothFlag(StringToBool(valStr)); } },
                { "targetColor", [](const std::string& valStr, ChartOptions& chartOptions) {
                    chartOptions.SetTargetColor(Color::FromString(valStr)); } },
                { "width", [](const std::string& valStr,
                               ChartOptions& chartOptions) { chartOptions.SetLineWidth(StringToDouble(valStr)); } },
            };
            auto iter = BinarySearchFindIndex(
                chartOptionsSeriesLineStyleMap, ArraySize(chartOptionsSeriesLineStyleMap), keyStr.c_str());
            if (iter != -1) {
                chartOptionsSeriesLineStyleMap[iter].value(valStr, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetChartAttrOptionsSeriesLoop(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, ChartOptions& chartOptions)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    chartOptions.SetLoop(true);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> val = valObject->GetProperty(runtime, key);
        if (!val) {
            LOGW("val is null. Ignoring!");
            continue;
        }
        if (val->IsNumber(runtime) || val->IsBoolean(runtime) || val->IsString(runtime)) {
            std::string valStr = val->ToString(runtime);
            static const LinearMapNode<void (*)(const std::string&, ChartOptions&)> chartOptionsSeriesLoopMap[] = {
                { "gradient", [](const std::string& valStr,
                                  ChartOptions& chartOptions) { chartOptions.SetLineGradient(StringToBool(valStr)); } },
                { "margin",
                    [](const std::string& valStr, ChartOptions& chartOptions) {
                        chartOptions.SetErasePointNumber(StringToInt(valStr));
                    } },
            };
            auto iter =
                BinarySearchFindIndex(chartOptionsSeriesLoopMap, ArraySize(chartOptionsSeriesLoopMap), keyStr.c_str());
            if (iter != -1) {
                chartOptionsSeriesLoopMap[iter].value(valStr, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetChartAttrOptionsSeries(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, ChartOptions& chartOptions)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> val = valObject->GetProperty(runtime, key);
        if (!val) {
            LOGW("val is null. Ignoring!");
            continue;
        }
        if (val->IsObject(runtime)) {
            static const LinearMapNode<void (*)(
                const shared_ptr<JsRuntime>&, const shared_ptr<JsValue>&, ChartOptions&)>
                chartOptionsSeriesMap[] = {
                    { "bottomPoint",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            PointInfo pointInfo;
                            GetAttrOptionsSeriesPoint(runtime, valObject, pointInfo);
                            chartOptions.SetBottomPoint(pointInfo);
                        } },
                    { "headPoint",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            PointInfo pointInfo;
                            GetAttrOptionsSeriesPoint(runtime, valObject, pointInfo);
                            chartOptions.SetHeadPoint(pointInfo);
                        } },
                    { "lineStyle",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            GetChartAttrOptionsSeriesLineStyle(runtime, valObject, chartOptions);
                        } },
                    { "loop",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            GetChartAttrOptionsSeriesLoop(runtime, valObject, chartOptions);
                        } },

                    { "topPoint",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            PointInfo pointInfo;
                            GetAttrOptionsSeriesPoint(runtime, valObject, pointInfo);
                            chartOptions.SetTopPoint(pointInfo);
                        } },
                };
            auto iter = BinarySearchFindIndex(chartOptionsSeriesMap, ArraySize(chartOptionsSeriesMap), keyStr.c_str());
            if (iter != -1) {
                chartOptionsSeriesMap[iter].value(runtime, val, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void ParseTextInfoAndSegmentInfo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& val,
    const std::string& key, TextInfo& textInfo, SegmentInfo& segmentInfo)
{
    if (!runtime || !val) {
        LOGE("runtime or val is null.");
        return;
    }
    std::string valStr = val->ToString(runtime);
    if (key == "description") {
        textInfo.SetTextValue(valStr);
    } else if (key == "textLocation") {
        if (valStr == "top") {
            textInfo.SetPlacement(Placement::TOP);
        } else if (valStr == "bottom") {
            textInfo.SetPlacement(Placement::BOTTOM);
        } else if (valStr == "none") {
            textInfo.SetPlacement(Placement::NONE);
        }
    } else if (key == "lineDash") {
        std::vector<std::string> dash;
        StringUtils::StringSpliter(valStr, ',', dash);
        if (!dash.empty()) {
            segmentInfo.SetLineType(dash[0] == "dashed" ? LineType::DASHED : LineType::SOLID);
        }
        if (dash.size() > 1) {
            segmentInfo.SetSolidWidth(StringToDouble(dash[1]));
        }
        if (dash.size() > 2) {
            segmentInfo.SetSpaceWidth(StringToDouble(dash[2]));
        }
    } else if (key == "lineColor") {
        segmentInfo.SetSegmentColor(Color::FromString(valStr));
    } else if (key == "textColor") {
        textInfo.SetColor(Color::FromString(valStr));
    } else {
        LOGW("key is %{public}s. Ignoring!", key.c_str());
    }
}

void ParseAttrDataStyle(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, double index, LineInfo& line)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    PointInfo pointInfo;
    TextInfo textInfo;
    SegmentInfo segmentInfo;
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> val = valObject->GetProperty(runtime, key);
        if (!val) {
            LOGW("val value is null. Ignoring!");
            continue;
        }
        if (val->IsString(runtime)) {
            ParseTextInfoAndSegmentInfo(runtime, val, keyStr, textInfo, segmentInfo);
        } else if (val->IsObject(runtime)) {
            if (keyStr == "pointStyle") {
                GetAttrOptionsSeriesPoint(runtime, val, pointInfo);
            }
        } else if (val->IsNumber(runtime)) {
            if (keyStr == "value") {
                pointInfo.SetX(index);
                pointInfo.SetY(StringToDouble(val->ToString(runtime)));
            }
        } else {
            LOGW("key is %{public}s. Ignoring!", keyStr.c_str());
        }
        line.SetPointInfo(pointInfo);
        line.SetTextInfo(textInfo);
        line.SetSegmentInfo(segmentInfo);
    }
}

void GetAttrDataSetData(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& dataArrayVal, MainChart& dataSet)
{
    if (!runtime || !dataArrayVal) {
        LOGE("runtime or dataArrayVal is null.");
        return;
    }
    int32_t length = dataArrayVal->GetArrayLength(runtime);
    std::vector<LineInfo> line;
    for (int32_t j = 0; j < length; ++j) {
        auto dataArrayItemVal = dataArrayVal->GetProperty(runtime, j);
        LineInfo lineInfo;
        PointInfo pointInfo;
        if (dataArrayItemVal->IsArray(runtime)) { // Coordinates are two-dimensional arrays
            int32_t dataArrayIterLen = dataArrayItemVal->GetArrayLength(runtime);
            if (dataArrayIterLen != VALID_ARRAY_LENGTH) { // Check coordinates are not two-dimensional arrays
                LOGW("Attr Datasets data type unsupported!");
                continue;
            }
            auto xVal = dataArrayItemVal->GetProperty(runtime, ARRAY_X_VALUE);
            pointInfo.SetX(StringToDouble(xVal->ToString(runtime)));

            auto yVal = dataArrayItemVal->GetProperty(runtime, ARRAY_Y_VALUE);
            pointInfo.SetX(StringToDouble(yVal->ToString(runtime)));
            lineInfo.SetPointInfo(pointInfo);
        } else if (dataArrayItemVal->IsNumber(runtime)) { // Coordinates as a one-dimensional array
            // When only the Y value is passed in, the X value is sequentially +1
            pointInfo.SetX(static_cast<double>(j));
            pointInfo.SetY(StringToDouble(dataArrayItemVal->ToString(runtime)));
            lineInfo.SetPointInfo(pointInfo);
        } else if (dataArrayItemVal->IsObject(runtime)) {
            ParseAttrDataStyle(runtime, dataArrayItemVal, static_cast<double>(j), lineInfo);
        } else {
            LOGW("value of unsupported type. Ignoring!");
            continue;
        }
        line.push_back(lineInfo);
    }
    dataSet.SetData(line);
}

void GetAttrDataset(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, MainChart& dataSet)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> val = valObject->GetProperty(runtime, key);
        if (!val) {
            LOGW("val is null. Ignoring!");
            continue;
        }
        if (val->IsNumber(runtime) || val->IsBoolean(runtime) || val->IsString(runtime)) {
            std::string valStr = val->ToString(runtime);
            if (keyStr == "gradient") {
                dataSet.SetGradient(StringToBool(valStr));
            } else if (keyStr == "strokeColor") {
                dataSet.SetStrokeColor(Color::FromString(valStr));
            } else if (keyStr == "fillColor") {
                dataSet.SetFillColor(Color::FromString(valStr));
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else if (val->IsArray(runtime) && keyStr == "data") {
            GetAttrDataSetData(runtime, val, dataSet);
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void ParseAttrSegment(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject, Segment& segment)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> val = valObject->GetProperty(runtime, key);
        if (!val) {
            LOGW("val is null. Ignoring!");
            continue;
        }
        if (val->IsNumber(runtime) || val->IsString(runtime)) {
            std::string valStr = val->ToString(runtime);
            if (keyStr == "value") {
                segment.SetValue(StringToDouble(valStr));
            } else if (keyStr == "startColor") {
                segment.SetStartColor(Color::FromString(valStr));
                segment.SetColorType(SegmentStyleType::USE_COLOR);
            } else if (keyStr == "endColor") {
                segment.SetEndColor(Color::FromString(valStr));
                segment.SetColorType(SegmentStyleType::USE_COLOR);
            } else if (keyStr == "name") {
                segment.SetSegmentName(valStr);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace

void JsiChartBridge::GetAttrOptionsObject(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject)
{
    if (!runtime || !valObject || !valObject->IsObject(runtime)) {
        LOGE("none found attrs");
        return;
    }
    shared_ptr<JsValue> propertyNames;
    int32_t len = 0;
    valObject->GetPropertyNames(runtime, propertyNames, len);
    for (auto i = 0; i < len; i++) {
        shared_ptr<JsValue> key = propertyNames->GetElement(runtime, i);
        if (!key) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        std::string keyStr = key->ToString(runtime);
        shared_ptr<JsValue> val = valObject->GetProperty(runtime, key);
        if (val->IsString(runtime) || val->IsNumber(runtime) || val->IsBoolean(runtime) || val->IsObject(runtime)) {
            static const LinearMapNode<void (*)(
                const shared_ptr<JsRuntime>&, const shared_ptr<JsValue>&, ChartOptions&)>
                chartOptionsMap[] = {
                    { "series",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            GetChartAttrOptionsSeries(runtime, valObject, chartOptions);
                        } },
                    { "xAxis",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            AxisOption xAxis;
                            GetAttrOptionsAxis(runtime, valObject, xAxis);
                            chartOptions.SetXAxis(xAxis);
                        } },
                    { "yAxis",
                        [](const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject,
                            ChartOptions& chartOptions) {
                            AxisOption yAxis;
                            GetAttrOptionsAxis(runtime, valObject, yAxis);
                            chartOptions.SetYAxis(yAxis);
                        } },
                };
            auto iter = BinarySearchFindIndex(chartOptionsMap, ArraySize(chartOptionsMap), keyStr.c_str());
            if (iter != -1) {
                chartOptionsMap[iter].value(runtime, val, chartOptions_);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr.c_str());
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void JsiChartBridge::GetAttrDatasets(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valArray)
{
    int32_t len = valArray->GetArrayLength(runtime);
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> itemVal = valArray->GetProperty(runtime, i);
        if (itemVal->IsObject(runtime)) {
            MainChart chart;
            GetAttrDataset(runtime, itemVal, chart);
            datasets_.push_back(chart);
        }
    }
}

void JsiChartBridge::ParseAttrSegmentArray(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valArray)
{
    int32_t len = valArray->GetArrayLength(runtime);
    for (int32_t i = 0; i < len; ++i) {
        shared_ptr<JsValue> itemVal = valArray->GetProperty(runtime, i);
        if (itemVal->IsObject(runtime)) {
            Segment segment;
            ParseAttrSegment(runtime, itemVal, segment);
            segments_.emplace_back(std::move(segment));
        }
    }
}

void JsiChartBridge::ParseAttrSingleSegment(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& valObject)
{
    if (!valObject->IsObject(runtime)) {
        LOGW("fail to parse segment, because it is not an object");
        return;
    }
    segments_.clear();
    Segment segment;
    ParseAttrSegment(runtime, valObject, segment);
    segments_.emplace_back(std::move(segment));
}

} // namespace OHOS::Ace::Framework