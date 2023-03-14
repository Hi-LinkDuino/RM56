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

#include "frameworks/bridge/js_frontend/engine/quickjs/chart_bridge.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

namespace {

constexpr int32_t VALID_ARRAY_LENGTH = 2;
constexpr uint32_t ARRAY_X_VALUE = 0;
constexpr uint32_t ARRAY_Y_VALUE = 1;

void GetAttrOptionsAxis(JSContext* ctx, JSValueConst valObject, AxisOption& axisOption)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    JS_GetOwnPropertyNames(ctx, &pTab, &len, valObject, JS_GPN_STRING_MASK);
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsNumber(val) || JS_IsBool(val) || JS_IsString(val)) {
            // must sorted by key.
            static const LinearMapNode<void (*)(const char*, AxisOption&)> chartOptionsAxisMap[] = {
                { "axisTick",
                    [](const char* valStr, AxisOption& axis) {
                        axis.tickNumber = StringUtils::StringToInt(valStr);
                    } },
                { "color",
                    [](const char* valStr, AxisOption& axis) {
                        axis.color = Color::FromString(valStr);
                    } },
                { "display",
                    [](const char* valStr, AxisOption& axis) {
                        axis.display = StringToBool(valStr);
                    } },
                { "max",
                    [](const char* valStr, AxisOption& axis) {
                        axis.max = StringToDouble(valStr);
                    } },
                { "min",
                    [](const char* valStr, AxisOption& axis) {
                        axis.min = StringToDouble(valStr);
                    } },
            };
            auto iter = BinarySearchFindIndex(chartOptionsAxisMap, ArraySize(chartOptionsAxisMap), key);
            if (iter != -1) {
                ScopedString styleVal(ctx, val);
                chartOptionsAxisMap[iter].value(styleVal.get(), axisOption);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("key : %{public}s, value of unsupported type. Ignoring!", key);
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void GetAttrOptionsSeriesPoint(JSContext* ctx, JSValueConst valObject, PointInfo& pointInfo)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    pointInfo.SetDisplay(true);
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(val) || JS_IsNumber(val) || JS_IsBool(val)) {
            static const LinearMapNode<void (*)(const char*, PointInfo&)> chartOptionsPointMap[] = {
                { "display",
                    [](const char* valStr, PointInfo& pointInfo) {
                        pointInfo.SetDisplay(StringToBool(valStr));
                    } },
                { "fillColor",
                    [](const char* valStr, PointInfo& pointInfo) {
                        pointInfo.SetFillColor(Color::FromString(valStr));
                    } },
                { "shape",
                    [](const char* valStr, PointInfo& pointInfo) {
                        PointShape shape =
                            (strcmp(valStr, "circle") == 0)
                                ? PointShape::CIRCLE
                                : (strcmp(valStr, "square") == 0) ? PointShape::SQUARE : PointShape::TRIANGLE;
                        pointInfo.SetPointShape(shape);
                    } },
                { "size",
                    [](const char* valStr, PointInfo& pointInfo) {
                        pointInfo.SetPointSize(StringToDimension(valStr));
                    } },
                { "strokeColor",
                    [](const char* valStr, PointInfo& pointInfo) {
                        pointInfo.SetStrokeColor(Color::FromString(valStr));
                    } },
                { "strokeWidth",
                    [](const char* valStr, PointInfo& pointInfo) {
                        pointInfo.SetPointStrokeWidth(StringToDimension(valStr));
                    } },
            };
            auto iter = BinarySearchFindIndex(chartOptionsPointMap, ArraySize(chartOptionsPointMap), key);
            if (iter != -1) {
                ScopedString styleVal(ctx, val);
                chartOptionsPointMap[iter].value(styleVal.get(), pointInfo);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("key : %{public}s, value of unsupported type. Ignoring!", key);
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void GetChartAttrOptionsSeriesLineStyle(JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(val) || JS_IsBool(val) || JS_IsNumber(val)) {
            static const LinearMapNode<void (*)(const char*, ChartOptions&)>
                chartOptionsSeriesLinestyleMap[] = {
                    { "lineGradient",
                        [](const char* valStr, ChartOptions& chartOptions) {
                            chartOptions.SetWholeLineGradient(StringToBool(valStr));
                        } },
                    { "smooth",
                        [](const char* valStr, ChartOptions& chartOptions) {
                            chartOptions.SetSmoothFlag(StringToBool(valStr));
                        } },
                    { "targetColor",
                        [](const char* valStr, ChartOptions& chartOptions) {
                            chartOptions.SetTargetColor(Color::FromString(valStr));
                        } },
                    { "width",
                        [](const char* valStr, ChartOptions& chartOptions) {
                            chartOptions.SetLineWidth(StringToDouble(valStr));
                        } },
                };
            auto iter =
                BinarySearchFindIndex(chartOptionsSeriesLinestyleMap, ArraySize(chartOptionsSeriesLinestyleMap), key);
            if (iter != -1) {
                ScopedString styleVal(ctx, val);
                chartOptionsSeriesLinestyleMap[iter].value(styleVal.get(), chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void GetChartAttrOptionsSeriesLoop(JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    chartOptions.SetLoop(true);
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(val) || JS_IsBool(val) || JS_IsNumber(val)) {
            static const LinearMapNode<void (*)(const char*, ChartOptions&)>
                chartOptionsSeriesLoopMap[] = {
                    { "gradient",
                        [](const char* valStr, ChartOptions& chartOptions) {
                            chartOptions.SetLineGradient(StringToBool(valStr));
                        } },
                    { "margin",
                        [](const char* valStr, ChartOptions& chartOptions) {
                            chartOptions.SetErasePointNumber(StringUtils::StringToInt(valStr));
                        } },
                };
            auto iter = BinarySearchFindIndex(chartOptionsSeriesLoopMap, ArraySize(chartOptionsSeriesLoopMap), key);
            if (iter != -1) {
                ScopedString styleVal(ctx, val);
                chartOptionsSeriesLoopMap[iter].value(styleVal.get(), chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void GetChartAttrOptionsSeries(JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsObject(val)) {
            static const LinearMapNode<void (*)(JSContext*, JSValueConst, ChartOptions&)> chartOptionsSeriesMap[] = {
                { "bottomPoint",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        PointInfo pointInfo;
                        GetAttrOptionsSeriesPoint(ctx, valObject, pointInfo);
                        chartOptions.SetBottomPoint(pointInfo);
                    } },
                { "headPoint",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        PointInfo pointInfo;
                        GetAttrOptionsSeriesPoint(ctx, valObject, pointInfo);
                        chartOptions.SetHeadPoint(pointInfo);
                    } },
                { "lineStyle",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        GetChartAttrOptionsSeriesLineStyle(ctx, valObject, chartOptions);
                    } },
                { "loop",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        GetChartAttrOptionsSeriesLoop(ctx, valObject, chartOptions);
                    } },

                { "topPoint",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        PointInfo pointInfo;
                        GetAttrOptionsSeriesPoint(ctx, valObject, pointInfo);
                        chartOptions.SetTopPoint(pointInfo);
                    } },
            };
            auto iter = BinarySearchFindIndex(chartOptionsSeriesMap, ArraySize(chartOptionsSeriesMap), key);
            if (iter != -1) {
                chartOptionsSeriesMap[iter].value(ctx, val, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void ParseTextInfoAndSegmentInfo(
    JSContext* ctx, JSValue val, const char* key, TextInfo& textInfo, SegmentInfo& segmentInfo)
{
    ScopedString stringVal(ctx, val);
    const char* convertedVal = stringVal.get();
    if (strcmp(key, "description") == 0) {
        textInfo.SetTextValue(convertedVal);
    } else if (strcmp(key, "textLocation") == 0) {
        if (strcmp(convertedVal, "top") == 0) {
            textInfo.SetPlacement(Placement::TOP);
        } else if (strcmp(convertedVal, "bottom") == 0) {
            textInfo.SetPlacement(Placement::BOTTOM);
        } else if (strcmp(convertedVal, "none") == 0) {
            textInfo.SetPlacement(Placement::NONE);
        }
    } else if (strcmp(key, "lineDash") == 0) {
        std::vector<std::string> dash;
        StringUtils::StringSpliter(convertedVal, ',', dash);
        if (dash.size() > 0) {
            segmentInfo.SetLineType(dash[0] == "dashed" ? LineType::DASHED : LineType::SOLID);
        }
        if (dash.size() > 1) {
            segmentInfo.SetSolidWidth(StringToDouble(dash[1]));
        }
        if (dash.size() > 2) {
            segmentInfo.SetSpaceWidth(StringToDouble(dash[2]));
        }
    } else if (strcmp(key, "lineColor") == 0) {
        segmentInfo.SetSegmentColor(Color::FromString(convertedVal));
    } else if (strcmp(key, "textColor") == 0) {
        textInfo.SetColor(Color::FromString(convertedVal));
    } else {
        LOGW("key is %{public}s. Ignoring!", key);
    }
}

void ParseAttrDataStyle(JSContext* ctx, JSValueConst valObject, LineInfo& line, double index)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    PointInfo pointInfo;
    TextInfo textInfo;
    SegmentInfo segmentInfo;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(val)) {
            ParseTextInfoAndSegmentInfo(ctx, val, key, textInfo, segmentInfo);
        } else if (JS_IsObject(val)) {
            if (strcmp(key, "pointStyle") == 0) {
                GetAttrOptionsSeriesPoint(ctx, val, pointInfo);
            }
        } else if (JS_IsNumber(val)) {
            if (strcmp(key, "value") == 0) {
                ScopedString onlyYStringVal(ctx, val);
                const char* onlyY = onlyYStringVal.get();
                pointInfo.SetX(static_cast<double>(index));
                pointInfo.SetY(StringToDouble(onlyY));
            }
        } else {
            LOGW("key is %{public}s. Ignoring!", key);
        }
        line.SetPointInfo(pointInfo);
        line.SetTextInfo(textInfo);
        line.SetSegmentInfo(segmentInfo);
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void GetAttrDataSetData(JSContext* ctx, JSValueConst dataArrayVal, MainChart& dataSet)
{
    int32_t length = QJSUtils::JsGetArrayLength(ctx, dataArrayVal);
    std::vector<LineInfo> line;
    for (int32_t j = 0; j < length; ++j) {
        JSValue dataArrayItemVal = JS_GetPropertyUint32(ctx, dataArrayVal, j);
        LineInfo lineInfo;
        PointInfo pointInfo;
        if (JS_IsArray(ctx, dataArrayItemVal)) { // Coordinates are two-dimensional arrays
            int32_t dataArrayIterLen = QJSUtils::JsGetArrayLength(ctx, dataArrayItemVal);
            if (dataArrayIterLen != VALID_ARRAY_LENGTH) { // Check coordinates are not two-dimensional arrays
                LOGW("Attr Datasets data type unsupported!");
                JS_FreeValue(ctx, dataArrayItemVal);
                continue;
            }
            JSValue xVal = JS_GetPropertyUint32(ctx, dataArrayItemVal, ARRAY_X_VALUE);
            ScopedString xStringVal(ctx, xVal);
            const char* x = xStringVal.get();
            pointInfo.SetX(StringToDouble(x));

            JSValue yVal = JS_GetPropertyUint32(ctx, dataArrayItemVal, ARRAY_Y_VALUE);
            ScopedString yStringVal(ctx, yVal);
            const char* y = yStringVal.get();
            pointInfo.SetY(StringToDouble(y));
            lineInfo.SetPointInfo(pointInfo);
            JS_FreeValue(ctx, xVal);
            JS_FreeValue(ctx, yVal);
        } else if (JS_IsNumber(dataArrayItemVal)) { // Coordinates as a one-dimensional array
            ScopedString onlyYStringVal(ctx, dataArrayItemVal);
            const char* onlyY = onlyYStringVal.get();
            // When only the Y value is passed in, the X value is sequentially +1
            pointInfo.SetX(static_cast<double>(j));
            pointInfo.SetY(StringToDouble(onlyY));
            lineInfo.SetPointInfo(pointInfo);
        } else if (JS_IsObject(dataArrayItemVal)) {
            ParseAttrDataStyle(ctx, dataArrayItemVal, lineInfo, j);
        } else {
            LOGW("value of unsupported type. Ignoring!");
            JS_FreeValue(ctx, dataArrayItemVal);
            continue;
        }
        line.push_back(lineInfo);
        JS_FreeValue(ctx, dataArrayItemVal);
    }
    dataSet.SetData(line);
}

void GetAttrDataset(JSContext* ctx, JSValueConst valObject, MainChart& dataSet)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsNumber(val) || JS_IsBool(val) || JS_IsString(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            if (strcmp(key, "gradient") == 0) {
                dataSet.SetGradient(StringToBool(valStr));
            } else if (strcmp(key, "strokeColor") == 0) {
                dataSet.SetStrokeColor(Color::FromString(valStr));
            } else if (strcmp(key, "fillColor") == 0) {
                dataSet.SetFillColor(Color::FromString(valStr));
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else if (JS_IsArray(ctx, val) && strcmp(key, "data") == 0) {
            GetAttrDataSetData(ctx, val, dataSet);
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void ParseAttrSegment(JSContext* ctx, JSValue valObject, Segment& segment)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsNumber(val) || JS_IsString(val)) {
            ScopedString styleVal(ctx, val);
            const char* valStr = styleVal.get();
            if (strcmp(key, "value") == 0) {
                segment.SetValue(StringToDouble(valStr));
            } else if (strcmp(key, "startColor") == 0) {
                segment.SetStartColor(Color::FromString(valStr));
                segment.SetColorType(SegmentStyleType::USE_COLOR);
            } else if (strcmp(key, "endColor") == 0) {
                segment.SetEndColor(Color::FromString(valStr));
                segment.SetColorType(SegmentStyleType::USE_COLOR);
            } else if (strcmp(key, "name") == 0) {
                segment.SetSegmentName(valStr);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

} // namespace

void ChartBridge::GetAttrOptionsObject(JSContext* ctx, JSValueConst valObject)
{
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue val = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsObject(val) || JS_IsString(val) || JS_IsBool(val) || JS_IsNumber(val)) {
            static const LinearMapNode<void (*)(JSContext*, JSValueConst, ChartOptions&)> chartOptionsMap[] = {
                { "series",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        GetChartAttrOptionsSeries(ctx, valObject, chartOptions);
                    } },
                { "xAxis",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        AxisOption xAxis;
                        GetAttrOptionsAxis(ctx, valObject, xAxis);
                        chartOptions.SetXAxis(xAxis);
                    } },
                { "yAxis",
                    [](JSContext* ctx, JSValueConst valObject, ChartOptions& chartOptions) {
                        AxisOption yAxis;
                        GetAttrOptionsAxis(ctx, valObject, yAxis);
                        chartOptions.SetYAxis(yAxis);
                    } },
            };
            auto iter = BinarySearchFindIndex(chartOptionsMap, ArraySize(chartOptionsMap), key);
            if (iter != -1) {
                chartOptionsMap[iter].value(ctx, val, chartOptions_);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, val);
    }
    js_free(ctx, pTab);
}

void ChartBridge::GetAttrDatasets(JSContext* ctx, JSValueConst valArray)
{
    int32_t len = QJSUtils::JsGetArrayLength(ctx, valArray);
    for (int32_t i = 0; i < len; ++i) {
        JSValue itemVal = JS_GetPropertyUint32(ctx, valArray, i);
        MainChart chart;
        if (JS_IsObject(itemVal)) {
            GetAttrDataset(ctx, itemVal, chart);
            datasets_.push_back(chart);
        }
        JS_FreeValue(ctx, itemVal);
    }
}

void ChartBridge::ParseAttrSegmentArray(JSContext* ctx, JSValueConst valArray)
{
    int32_t len = QJSUtils::JsGetArrayLength(ctx, valArray);
    for (int32_t i = 0; i < len; ++i) {
        JSValue itemVal = JS_GetPropertyUint32(ctx, valArray, i);
        if (JS_IsObject(itemVal)) {
            Segment segment;
            ParseAttrSegment(ctx, itemVal, segment);
            segments_.emplace_back(std::move(segment));
        }
        JS_FreeValue(ctx, itemVal);
    }
}

void ChartBridge::ParseAttrSingleSegment(JSContext* ctx, JSValueConst valObject)
{
    if (!JS_IsObject(valObject)) {
        LOGW("fail to parse segment, because it is not an object");
        return;
    }
    segments_.clear();
    Segment segment;
    ParseAttrSegment(ctx, valObject, segment);
    segments_.emplace_back(std::move(segment));
}

} // namespace OHOS::Ace::Framework