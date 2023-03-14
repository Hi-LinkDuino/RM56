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

#include "frameworks/bridge/js_frontend/engine/v8/v8_chart_bridge.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

namespace {

constexpr uint32_t VALID_ARRAY_LENGTH = 2;
constexpr uint32_t ARRAY_X_VALUE = 0;
constexpr uint32_t ARRAY_Y_VALUE = 1;

void GetAttrOptionsAxis(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, AxisOption& axisOption)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsNumber() || val->IsBoolean() || val->IsString()) {
            static const std::unordered_map<std::string,
                void (*)(v8::Local<v8::Context>, v8::Local<v8::Value>, AxisOption&)>
                chartOptionAxisMap = {
                    { "min",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, AxisOption& axis) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                axis.min = StringToDouble(valStr);
                            }
                        } },
                    { "max",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, AxisOption& axis) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                axis.max = StringToDouble(valStr);
                            }
                        } },
                    { "axisTick",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, AxisOption& axis) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                axis.tickNumber = StringToInt(valStr);
                            }
                        } },
                    { "display",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, AxisOption& axis) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                axis.display = StringToBool(valStr);
                            }
                        } },
                    { "color",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, AxisOption& axis) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                axis.color = Color::FromString(valStr);
                            }
                        } },
                };
            auto iter = chartOptionAxisMap.find(keyStr);
            if (iter != chartOptionAxisMap.end()) {
                iter->second(ctx, val, axisOption);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("key : %{public}s, value of unsupported type. Ignoring!", keyStr);
        }
    }
}

void GetAttrOptionsSeriesPoint(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, PointInfo& pointInfo)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();
    pointInfo.SetDisplay(true);
    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsString() || val->IsNumber() || val->IsBoolean()) {
            static const std::unordered_map<std::string,
                void (*)(v8::Local<v8::Context>, v8::Local<v8::Value>, PointInfo&)>
                chartOptionsPointMap = {
                    { "shape",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, PointInfo& pointInfo) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                PointShape shape =
                                    (strcmp(valStr, "circle") == 0)
                                        ? PointShape::CIRCLE
                                        : (strcmp(valStr, "square") == 0) ? PointShape::SQUARE : PointShape::TRIANGLE;
                                pointInfo.SetPointShape(shape);
                            }
                        } },
                    { "display",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, PointInfo& pointInfo) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                pointInfo.SetDisplay(StringToBool(valStr));
                            }
                        } },
                    { "size",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, PointInfo& pointInfo) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                pointInfo.SetPointSize(StringToDimension(valStr));
                            }
                        } },
                    { "strokeWidth",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, PointInfo& pointInfo) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                pointInfo.SetPointStrokeWidth(StringToDimension(valStr));
                            }
                        } },
                    { "strokeColor",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, PointInfo& pointInfo) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                pointInfo.SetStrokeColor(Color::FromString(valStr));
                            }
                        } },
                    { "fillColor",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, PointInfo& pointInfo) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                pointInfo.SetFillColor(Color::FromString(valStr));
                            }
                        } },
                };
            auto iter = chartOptionsPointMap.find(keyStr);
            if (iter != chartOptionsPointMap.end()) {
                iter->second(ctx, val, pointInfo);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("key : %{public}s, value of unsupported type. Ignoring!", keyStr);
        }
    }
}

void GetChartAttrOptionsSeriesLineStyle(
    v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsString() || val->IsBoolean() || val->IsNumber()) {
            static const std::unordered_map<std::string,
                void (*)(v8::Local<v8::Context>, v8::Local<v8::Value>, ChartOptions&)>
                chartOptionsSeriesLineStyleMap = {
                    { "width",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, ChartOptions& chartOptions) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                chartOptions.SetLineWidth(StringToDouble(valStr));
                            }
                        } },
                    { "smooth",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, ChartOptions& chartOptions) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                chartOptions.SetSmoothFlag(StringToBool(valStr));
                            }
                        } },
                    { "lineGradient",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, ChartOptions& chartOptions) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                chartOptions.SetWholeLineGradient(StringToBool(valStr));
                            }
                        } },
                    { "targetColor",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, ChartOptions& chartOptions) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                chartOptions.SetTargetColor(Color::FromString(valStr));
                            }
                        } },
                };
            auto iter = chartOptionsSeriesLineStyleMap.find(keyStr);
            if (iter != chartOptionsSeriesLineStyleMap.end()) {
                iter->second(ctx, val, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetChartAttrOptionsSeriesLoop(
    v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    chartOptions.SetLoop(true);
    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsString() || val->IsBoolean() || val->IsNumber()) {
            static const std::unordered_map<std::string,
                void (*)(v8::Local<v8::Context>, v8::Local<v8::Value>, ChartOptions&)>
                chartOptionsSeriesLoopMap = {
                    { "margin",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, ChartOptions& chartOptions) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                chartOptions.SetErasePointNumber(StringToInt(valStr));
                            }
                        } },
                    { "gradient",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, ChartOptions& chartOptions) {
                            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
                            const char* valStr = *valV8Str;
                            if (valStr != nullptr) {
                                chartOptions.SetLineGradient(StringToBool(valStr));
                            }
                        } },
                };
            auto iter = chartOptionsSeriesLoopMap.find(keyStr);
            if (iter != chartOptionsSeriesLoopMap.end()) {
                iter->second(ctx, val, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void GetChartAttrOptionsSeries(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();
    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsObject()) {
            static const std::unordered_map<std::string,
                void (*)(v8::Local<v8::Context>, v8::Local<v8::Value>, ChartOptions&)>
                chartOptionsSeriesMap = {
                    { "lineStyle",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            GetChartAttrOptionsSeriesLineStyle(ctx, valObject, chartOptions);
                        } },
                    { "loop",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            GetChartAttrOptionsSeriesLoop(ctx, valObject, chartOptions);
                        } },
                    { "headPoint",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            PointInfo pointInfo;
                            GetAttrOptionsSeriesPoint(ctx, valObject, pointInfo);
                            chartOptions.SetHeadPoint(pointInfo);
                        } },
                    { "topPoint",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            PointInfo pointInfo;
                            GetAttrOptionsSeriesPoint(ctx, valObject, pointInfo);
                            chartOptions.SetTopPoint(pointInfo);
                        } },
                    { "bottomPoint",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            PointInfo pointInfo;
                            GetAttrOptionsSeriesPoint(ctx, valObject, pointInfo);
                            chartOptions.SetBottomPoint(pointInfo);
                        } },
                };
            auto iter = chartOptionsSeriesMap.find(keyStr);
            if (iter != chartOptionsSeriesMap.end()) {
                iter->second(ctx, val, chartOptions);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void ParseTextInfoAndSegmentInfo(v8::Local<v8::Context> ctx, v8::Local<v8::Value> val, const char* keyStr,
    TextInfo& textInfo, SegmentInfo& segmentInfo)
{
    v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
    const char* valStr = *valV8Str;
    if (valStr == nullptr) {
        LOGI("fail to parse style in chart. Ignoring!");
        return;
    }
    if (strcmp(keyStr, "description") == 0) {
        textInfo.SetTextValue(valStr);
    } else if (strcmp(keyStr, "textLocation") == 0) {
        if (strcmp(valStr, "top") == 0) {
            textInfo.SetPlacement(Placement::TOP);
        } else if (strcmp(valStr, "bottom") == 0) {
            textInfo.SetPlacement(Placement::BOTTOM);
        } else if (strcmp(valStr, "none") == 0) {
            textInfo.SetPlacement(Placement::NONE);
        }
    } else if (strcmp(keyStr, "lineDash") == 0) {
        std::vector<std::string> dash;
        StringUtils::StringSpliter(valStr, ',', dash);
        if (dash.size() == 1 || dash.size() == 3) {
            if (dash[0] == "dashed") {
                segmentInfo.SetLineType(LineType::DASHED);
            } else {
                segmentInfo.SetLineType(LineType::SOLID);
            }
            if (dash.size() > 1) {
                segmentInfo.SetSolidWidth(StringToDouble(dash[1]));
            }
            if (dash.size() > 2) {
                segmentInfo.SetSpaceWidth(StringToDouble(dash[2]));
            }
        }
    } else if (strcmp(keyStr, "lineColor") == 0) {
        segmentInfo.SetSegmentColor(Color::FromString(valStr));
    } else if (strcmp(keyStr, "textColor") == 0) {
        textInfo.SetColor(Color::FromString(valStr));
    } else {
        LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
    }
}

void ParseAttrDataStyle(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, LineInfo& line, double index)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }

    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();
    PointInfo pointInfo;
    TextInfo textInfo;
    SegmentInfo segmentInfo;
    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsNumber()) {
            if (strcmp(keyStr, "value") == 0) {
                auto numberValue = val->ToNumber(ctx).ToLocalChecked();
                pointInfo.SetX(index);
                if (numberValue.IsEmpty()) {
                    continue;
                }
                pointInfo.SetY(numberValue->Value());
            }
        } else if (val->IsString()) {
            ParseTextInfoAndSegmentInfo(ctx, val, keyStr, textInfo, segmentInfo);
        } else if (val->IsObject()) {
            if (strcmp(keyStr, "pointStyle") == 0) {
                GetAttrOptionsSeriesPoint(ctx, val, pointInfo);
            }
        }
    }
    line.SetPointInfo(pointInfo);
    line.SetSegmentInfo(segmentInfo);
    line.SetTextInfo(textInfo);
}

void GetAttrDataSetData(v8::Local<v8::Context> ctx, v8::Local<v8::Value> dataArrayVal, MainChart& dataSet)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> v8DataArrayVal = dataArrayVal->ToObject(ctx).ToLocalChecked();
    if (!dataArrayVal->ToObject(ctx).ToLocal(&v8DataArrayVal)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8DataArrayVal->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t subLen = properties->Length();

    std::vector<LineInfo> points;
    for (int32_t j = 0; j < subLen; ++j) {
        v8::Local<v8::Value> itemKey = properties->Get(ctx, j).ToLocalChecked();
        v8::Local<v8::Value> dataArrayItemVal = v8DataArrayVal->Get(ctx, itemKey).ToLocalChecked();
        LineInfo lineInfo;
        PointInfo pointInfo;
        if (dataArrayItemVal->IsArray()) { // Coordinates are two-dimensional arrays
            v8::Local<v8::Array> itemArray = v8::Local<v8::Array>::Cast(dataArrayItemVal);
            uint32_t dataArryIterLen = itemArray->Length();

            if (dataArryIterLen != VALID_ARRAY_LENGTH) { // Check coordinates are not two-dimensional arrays
                LOGW("Attr Datasets data type unsupported!");
                continue;
            }
            v8::Local<v8::Value> xVal = itemArray->Get(ctx, ARRAY_X_VALUE).ToLocalChecked();
            v8::String::Utf8Value xValV8Str(isolate, xVal);
            const char* x = *xValV8Str;
            if (x != nullptr) {
                pointInfo.SetX(StringToDouble(x));
            }

            v8::Local<v8::Value> yVal = itemArray->Get(ctx, ARRAY_Y_VALUE).ToLocalChecked();
            v8::String::Utf8Value yValV8Str(isolate, yVal);
            const char* y = *yValV8Str;
            if (y != nullptr) {
                pointInfo.SetY(StringToDouble(y));
            }
            lineInfo.SetPointInfo(pointInfo);
        } else if (dataArrayItemVal->IsNumber()) { // Coordinates as a one-dimensional array
            v8::String::Utf8Value onlyYStringVal(isolate, dataArrayItemVal);
            const char* onlyY = *onlyYStringVal;
            if (onlyY != nullptr) {
                // When only the Y value is passed in, the X value is sequentially +1
                pointInfo.SetX(static_cast<double>(j));
                pointInfo.SetY(StringToDouble(onlyY));
            }
            lineInfo.SetPointInfo(pointInfo);
        } else if (dataArrayItemVal->IsObject()) {
            ParseAttrDataStyle(ctx, dataArrayItemVal, lineInfo, static_cast<double>(j));
        } else {
            LOGW("value of unsupported type. Ignoring!");
            continue;
        }
        points.push_back(lineInfo);
    }
    dataSet.SetData(points);
}

void ParseAttrSegment(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, Segment& segment)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }

    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (!val->IsNumber() && !val->IsString()) {
            LOGI("value of unsupported type in chart. Ignoring!");
            continue;
        }
        v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
        const char* valStr = *valV8Str;
        if (valStr == nullptr) {
            LOGI("fail to parse style in chart. Ignoring!");
            continue;
        }
        if (strcmp(keyStr, "gradient") == 0) {
            segment.SetColorDescriptor(valStr);
            segment.SetColorType(SegmentStyleType::USE_GRADIENT);
        } else if (strcmp(keyStr, "startColor") == 0) {
            segment.SetStartColor(Color::FromString(valStr));
            if (segment.GetColorType() != SegmentStyleType::USE_GRADIENT) {
                segment.SetColorType(SegmentStyleType::USE_COLOR);
            }
        } else if (strcmp(keyStr, "endColor") == 0) {
            segment.SetEndColor(Color::FromString(valStr));
            if (segment.GetColorType() != SegmentStyleType::USE_GRADIENT) {
                segment.SetColorType(SegmentStyleType::USE_COLOR);
            }
        } else if (strcmp(keyStr, "value") == 0) {
            segment.SetValue(StringToDouble(valStr));
        } else if (strcmp(keyStr, "name") == 0) {
            segment.SetSegmentName(valStr);
        } else {
            LOGI("key : %{public}s unsupported. Ignoring!", keyStr);
        }
    }
}

void GetAttrDataset(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, MainChart& dataSet)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsNumber() || val->IsBoolean() || val->IsString()) {
            v8::String::Utf8Value valV8Str(ctx->GetIsolate(), val);
            const char* valStr = *valV8Str;

            if (valStr != nullptr) {
                if (strcmp(keyStr, "gradient") == 0) {
                    dataSet.SetGradient(StringToBool(valStr));
                } else if (strcmp(keyStr, "strokeColor") == 0) {
                    dataSet.SetStrokeColor(Color::FromString(valStr));
                } else if (strcmp(keyStr, "fillColor") == 0) {
                    dataSet.SetFillColor(Color::FromString(valStr));
                } else {
                    LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
                }
            }
        } else if (val->IsArray() || strcmp(keyStr, "data") == 0) {
            GetAttrDataSetData(ctx, val, dataSet);
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

} // namespace

void V8ChartBridge::GetAttrOptionsObject(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    if (!valObject->IsObject()) {
        LOGE("none found attrs");
        return;
    }
    v8::Local<v8::Object> v8ValObj;
    if (!valObject->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();
    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> key;
        if (!properties->Get(ctx, i).ToLocal(&key)) {
            LOGW("key is null. Ignoring!");
            continue;
        }
        v8::String::Utf8Value keyV8Str(isolate, key);
        const char* keyStr = *keyV8Str;
        if (keyStr == nullptr) {
            continue;
        }
        v8::Local<v8::Value> val = v8ValObj->Get(ctx, key).ToLocalChecked();
        if (val->IsObject() || val->IsString() || val->IsBoolean() || val->IsNumber()) {
            static const std::unordered_map<std::string,
                void (*)(v8::Local<v8::Context>, v8::Local<v8::Value>, ChartOptions&)>
                chartOptionsMap = {
                    { "xAxis",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            AxisOption xAxis;
                            GetAttrOptionsAxis(ctx, valObject, xAxis);
                            chartOptions.SetXAxis(xAxis);
                        } },
                    { "yAxis",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            AxisOption yAxis;
                            GetAttrOptionsAxis(ctx, valObject, yAxis);
                            chartOptions.SetYAxis(yAxis);
                        } },
                    { "series",
                        [](v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject, ChartOptions& chartOptions) {
                            GetChartAttrOptionsSeries(ctx, valObject, chartOptions);
                        } },
                };

            auto iter = chartOptionsMap.find(keyStr);

            if (iter != chartOptionsMap.end()) {
                iter->second(ctx, val, chartOptions_);
            } else {
                LOGD("key : %{public}s unsupported. Ignoring!", keyStr);
            }
        } else {
            LOGD("value of unsupported type. Ignoring!");
        }
    }
}

void V8ChartBridge::GetAttrDatasets(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valArray)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> v8ValObj;
    if (!valArray->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    for (int32_t i = 0; i < len; ++i) {
        v8::Local<v8::Value> itemKey = properties->Get(ctx, i).ToLocalChecked();
        v8::Local<v8::Value> itemVal = v8ValObj->Get(ctx, itemKey).ToLocalChecked();
        MainChart chart;
        if (itemVal->IsObject()) {
            GetAttrDataset(ctx, itemVal, chart);
            datasets_.push_back(chart);
        }
    }
}

void V8ChartBridge::ParseAttrSegmentArray(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valArray)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);

    v8::Local<v8::Object> v8ValObj;
    if (valArray->ToObject(ctx).IsEmpty() || !valArray->ToObject(ctx).ToLocal(&v8ValObj)) {
        LOGE("Value to Object fail");
        return;
    }
    v8::Local<v8::Array> properties = v8ValObj->GetOwnPropertyNames(ctx).ToLocalChecked();
    int32_t len = properties->Length();

    for (int32_t i = 0; i < len; ++i) {
        v8::Local<v8::Value> itemKey = properties->Get(ctx, i).ToLocalChecked();
        v8::Local<v8::Value> itemVal = v8ValObj->Get(ctx, itemKey).ToLocalChecked();

        if (itemVal->IsObject()) {
            Segment segment;
            ParseAttrSegment(ctx, itemVal, segment);
            segments_.push_back(segment);
        }
    }
}

void V8ChartBridge::ParseAttrSingleSegment(v8::Local<v8::Context> ctx, v8::Local<v8::Value> valObject)
{
    segments_.clear();
    Segment segment;
    ParseAttrSegment(ctx, valObject, segment);
    segments_.push_back(segment);
}

} // namespace OHOS::Ace::Framework