/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_offscreen_canvas_bridge.h"

#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_engine.h"

namespace OHOS::Ace::Framework {
namespace {

template<typename T>
inline T ConvertStrToEnum(const char* key, const LinearMapNode<T>* map, size_t length, T defaultValue)
{
    int64_t index = BinarySearchFindIndex(map, length, key);
    return index != -1 ? map[index].value : defaultValue;
}

const LinearMapNode<TextBaseline> BASELINE_TABLE[] = {
    { "alphabetic", TextBaseline::ALPHABETIC },
    { "bottom", TextBaseline::BOTTOM },
    { "hanging", TextBaseline::HANGING },
    { "ideographic", TextBaseline::IDEOGRAPHIC },
    { "middle", TextBaseline::MIDDLE },
    { "top", TextBaseline::TOP },
};

const std::set<std::string> FONT_WEIGHTS = { "normal", "bold", "lighter", "bolder", "100", "200", "300", "400", "500",
    "600", "700", "800", "900" };
const std::set<std::string> FONT_STYLES = { "italic", "oblique", "normal" };
const std::set<std::string> FONT_FAMILIES = { "sans-serif", "serif", "monospace" };
const std::set<std::string> QUALITY_TYPE = { "low", "medium", "high" }; // Default value is low.

inline double GetJsDoubleVal(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    if (!runtime || !value) {
        LOGE("runtime or value is null.");
        return 0.0;
    }
    if (value->IsNumber(runtime) || value->IsString(runtime)) {
        return value->ToDouble(runtime);
    }
    return 0.0;
}

inline std::vector<double> GetJsDashValue(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    std::vector<double> segments;
    if (!runtime || !value) {
        LOGE("runtime or value is null.");
        return segments;
    }
    auto valueStr = value->ToString(runtime);
    std::vector<std::string> props;
    StringUtils::StringSpliter(valueStr, ',', props);
    for (const auto& prop : props) {
        auto val = StringUtils::StringToDouble(prop);
        // if there only exists 0 in props, it means that there is no dash style
        if (NearZero(val) && props.size() == 1) {
            return segments;
        }
        segments.emplace_back(val);
    }
    // if segment size is odd, copy one more to even
    if (segments.size() % 2 != 0) {
        segments.insert(segments.end(), segments.begin(), segments.end());
    }
    return segments;
}

inline Rect GetJsRectParam(
    const shared_ptr<JsRuntime>& runtime, int32_t argc, const std::vector<shared_ptr<JsValue>>& argv)
{
    if (!runtime) {
        LOGE("runtime is null.");
        return Rect();
    }
    if (argc != 4) {
        LOGE("argc error. argc = %{private}d", argc);
        return Rect();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    double width = GetJsDoubleVal(runtime, argv[2]);
    double height = GetJsDoubleVal(runtime, argv[3]);
    Rect rect = Rect(x, y, width, height);
    return rect;
}

inline int32_t GetCurrentBridgeId(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    if (!runtime || !value) {
        LOGE("runtime or value is null.");
        return 0;
    }
    int32_t id = 0;
    auto bridgeId = value->GetProperty(runtime, "__bridgeId");
    if (bridgeId && bridgeId->IsInt32(runtime)) {
        id = bridgeId->ToInt32(runtime);
    }
    return id < 0 ? 0 : id;
}

RefPtr<JsiOffscreenCanvasBridge> GetOffscreenCanvasBridge(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    int32_t bridgeId = GetCurrentBridgeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (engine) {
        auto page = engine->GetRunningPage();
        if (page) {
            auto bridge = AceType::DynamicCast<JsiOffscreenCanvasBridge>(page->GetOffscreenCanvasBridgeById(bridgeId));
            return bridge;
        }
    }
    return nullptr;
}

RefPtr<OffscreenCanvas> GlobalGetOffscreenCanvas(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    auto bridge = GetOffscreenCanvasBridge(runtime, value);
    if (bridge) {
        return bridge->GetOffscreenCanvas();
    }
    return nullptr;
}

inline PaintState JsParseTextDirection(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    PaintState state;
    auto textDirection = value->GetProperty(runtime, "textDirection");
    auto directionStr = textDirection->ToString(runtime);
    state.SetOffTextDirection(ConvertStrToTextDirection(directionStr));

    return state;
}

inline PaintState JsParseTextState(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    PaintState state;
    auto fontStyle = value->GetProperty(runtime, "font");
    auto textAlign = value->GetProperty(runtime, "textAlign");
    auto textBaseline = value->GetProperty(runtime, "textBaseline");
    auto textDirection = value->GetProperty(runtime, "textDirection");

    // parse font styles
    auto alignStr = textAlign->ToString(runtime);
    auto baselineStr = textBaseline->ToString(runtime);
    auto fontStr = fontStyle->ToString(runtime);
    auto directionStr = textDirection->ToString(runtime);
    state.SetTextAlign(ConvertStrToTextAlign(alignStr));
    state.SetOffTextDirection(ConvertStrToTextDirection(directionStr));
    TextStyle style;
    style.SetTextBaseline(
        ConvertStrToEnum(baselineStr.c_str(), BASELINE_TABLE, ArraySize(BASELINE_TABLE), TextBaseline::ALPHABETIC));
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(fontStr, ' ', fontProps);
    bool updateFontStyle = false;
    for (const auto& fontProp : fontProps) {
        if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            style.SetFontWeight(ConvertStrToFontWeight(fontProp));
        } else if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            updateFontStyle = true;
            style.SetFontStyle(ConvertStrToFontStyle(fontProp));
        } else if (FONT_FAMILIES.find(fontProp) != FONT_FAMILIES.end()) {
            style.SetFontFamilies(ConvertStrToFontFamilies(fontProp));
        } else if (fontProp.find("px") != std::string::npos) {
            std::string fontSize = fontProp.substr(0, fontProp.size() - 2);
            style.SetFontSize(Dimension(StringToDouble(fontProp)));
        } else {
            LOGW("parse text error");
        }
    }
    if (!updateFontStyle) {
        auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
        if (offscreenCanvas) {
            offscreenCanvas->SetFontStyle(FontStyle::NORMAL);
        }
    }
    state.SetTextStyle(style);
    return state;
}

} // namespace

int32_t JsiOffscreenCanvasBridge::gradientCount_ = 0;
int32_t JsiOffscreenCanvasBridge::patternCount_ = 0;
int32_t JsiOffscreenCanvasBridge::path2dCount_ = 0;
int32_t JsiOffscreenCanvasBridge::globalBridgeId_ = 0;
std::unordered_map<int32_t, Pattern> JsiOffscreenCanvasBridge::pattern_;
std::unordered_map<int32_t, Gradient> JsiOffscreenCanvasBridge::gradientColors_;
std::unordered_map<int32_t, RefPtr<CanvasPath2D>> JsiOffscreenCanvasBridge::path2Ds_;

JsiOffscreenCanvasBridge::JsiOffscreenCanvasBridge(
    const RefPtr<PipelineContext>& pipelineContext, int32_t width, int32_t height)
{
    width_ = width;
    height_ = height;
    bridgeId_ = globalBridgeId_++;
    if (pipelineContext) {
        offscreenCanvas_ = pipelineContext->CreateOffscreenCanvas(width_, height_);
    }
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::GetBridge(const shared_ptr<JsRuntime>& runtime)
{
    auto res = runtime->NewObject();
    res->SetProperty(runtime, "__bridgeId", runtime->NewInt32(bridgeId_));
    const std::vector<std::pair<const std::string, RegisterFunctionType>> propertyTable = {
        { "getContext", JsGetContext },
        { "transferToImageBitmap", JsTransferToImageBitmap },
        { "toDataURL", JsToDataURL },
    };
    for (const auto& iter : propertyTable) {
        res->SetProperty(runtime, iter.first, runtime->NewFunction(iter.second));
    }
    return res;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsGetContext(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto bridge = GetOffscreenCanvasBridge(runtime, value);
    if (bridge) {
        shared_ptr<JsValue> renderContext = runtime->NewObject();
        const std::vector<std::pair<const std::string, RegisterFunctionType>> contextTable = {
            { "createLinearGradient", JsCreateLinearGradient },
            { "createRadialGradient", JsCreateRadialGradient },
            { "fillRect", JsFillRect },
            { "strokeRect", JsStrokeRect },
            { "clearRect", JsClearRect },
            { "fillText", JsFillText },
            { "strokeText", JsStrokeText },
            { "measureText", JsMeasureText },
            { "moveTo", JsMoveTo },
            { "lineTo", JsLineTo },
            { "bezierCurveTo", JsBezierCurveTo },
            { "quadraticCurveTo", JsQuadraticCurveTo },
            { "arcTo", JsArcTo },
            { "arc", JsArc },
            { "ellipse", JsEllipse },
            { "fill", JsFill },
            { "stroke", JsStroke },
            { "clip", JsClip },
            { "rect", JsRect },
            { "beginPath", JsBeginPath },
            { "closePath", JsClosePath },
            { "restore", JsRestore },
            { "save", JsSave },
            { "rotate", JsRotate },
            { "scale", JsScale },
            { "setTransform", JsSetTransform },
            { "transform", JsTransform },
            { "translate", JsTranslate },
            { "getLineDash", JsGetLineDash },
            { "setLineDash", JsSetLineDash },
            { "drawImage", JsDrawImage },
            { "createPath2D", JsCreatePath2D },
            { "createPattern", JsCreatePattern },
            { "createImageData", JsCreateImageData },
            { "putImageData", JsPutImageData },
            { "getImageData", JsGetImageData },
            { "isPointInStroke", JsIsPointInStroke },
            { "isPointInPath", JsIsPointInPath },
            { "resetTransform", JsResetTransform },
        };
        renderContext->SetProperty(runtime, "__bridgeId", runtime->NewInt32(bridge->GetBridgeId()));
        for (const auto& iter : contextTable) {
            renderContext->SetProperty(runtime, iter.first, runtime->NewFunction(iter.second));
        }
        static const std::vector<std::tuple<std::string, RegisterFunctionType, RegisterFunctionType>> animationFuncs = {
            { "fillStyle", JsFillStyleGetter, JsFillStyleSetter },
            { "strokeStyle", JsStrokeStyleGetter, JsStrokeStyleSetter },
            { "lineCap", JsLineCapGetter, JsLineCapSetter },
            { "lineJoin", JsLineJoinGetter, JsLineJoinSetter },
            { "miterLimit", JsMiterLimitGetter, JsMiterLimitSetter },
            { "lineWidth", JsLineWidthGetter, JsLineWidthSetter },
            { "textAlign", JsTextAlignGetter, JsTextAlignSetter },
            { "textBaseline", JsTextBaselineGetter, JsTextBaselineSetter },
            { "font", JsFontGetter, JsFontSetter },
            { "globalAlpha", JsAlphaGetter, JsAlphaSetter },
            { "globalCompositeOperation", JsCompositeOperationGetter, JsCompositeOperationSetter },
            { "lineDashOffset", JsLineDashOffsetGetter, JsLineDashOffsetSetter },
            { "shadowBlur", JsShadowBlurGetter, JsShadowBlurSetter },
            { "shadowColor", JsShadowColorGetter, JsShadowColorSetter },
            { "shadowOffsetX", JsShadowOffsetXGetter, JsShadowOffsetXSetter },
            { "shadowOffsetY", JsShadowOffsetYGetter, JsShadowOffsetYSetter },
            { "imageSmoothingEnabled", JsSmoothingEnabledGetter, JsSmoothingEnabledSetter },
            { "imageSmoothingQuality", JsSmoothingQualityGetter, JsSmoothingQualitySetter },
            { "filter", JsFilterParamGetter, JsFilterParamSetter }
        };
        for (const auto& item : animationFuncs) {
            auto getterTempl = runtime->NewFunction(std::get<1>(item));
            auto setterTempl = runtime->NewFunction(std::get<2>(item));
            bool ret = renderContext->SetAccessorProperty(runtime, std::get<0>(item), getterTempl, setterTempl);
            if (!ret) {
                LOGE("Canvas set accessor property failed., name: %{public}s", std::get<0>(item).c_str());
            }
        }
        return renderContext;
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCreateLinearGradient(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsCreateLinearGradient");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> gradient = runtime->NewObject();

    gradient->SetProperty(runtime, "__type", runtime->NewString("gradient"));
    gradient->SetProperty(runtime, "__id", runtime->NewInt32(gradientCount_));
    gradient->SetProperty(runtime, "addColorStop", runtime->NewFunction(JsAddColorStop));

    Offset beginOffset = Offset(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]));
    Offset endOffset = Offset(GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]));
    gradientColors_[gradientCount_].SetBeginOffset(beginOffset);
    gradientColors_[gradientCount_].SetEndOffset(endOffset);
    ++gradientCount_;
    return gradient;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCreateRadialGradient(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::CreateRadialGradient");
    // 6 parameters: createRadialGradient(x0, y0, r0, x1, y1, r1)
    if (argc != 6) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    shared_ptr<JsValue> gradient = runtime->NewObject();
    gradient->SetProperty(runtime, "__type", runtime->NewString("gradient"));
    gradient->SetProperty(runtime, "__id", runtime->NewInt32(gradientCount_));
    gradient->SetProperty(runtime, "addColorStop", runtime->NewFunction(JsAddColorStop));
    Offset innerCenter = Offset(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]));
    Offset outerCenter = Offset(GetJsDoubleVal(runtime, argv[3]), GetJsDoubleVal(runtime, argv[4]));
    double innerRadius = GetJsDoubleVal(runtime, argv[2]);
    double outerRadius = GetJsDoubleVal(runtime, argv[5]);
    gradientColors_[gradientCount_].SetType(GradientType::RADIAL);
    gradientColors_[gradientCount_].SetBeginOffset(innerCenter);
    gradientColors_[gradientCount_].SetEndOffset(outerCenter);
    gradientColors_[gradientCount_].SetInnerRadius(innerRadius);
    gradientColors_[gradientCount_].SetOuterRadius(outerRadius);
    ++gradientCount_;
    return gradient;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsAddColorStop(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }

    GradientColor color;
    auto jsColor = argv[1]->ToString(runtime);
    color.SetColor(Color::FromString(jsColor));
    color.SetDimension(GetJsDoubleVal(runtime, argv[0]));
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    gradientColors_[id].AddColor(color);
    return runtime->NewUndefined();
}

Gradient JsiOffscreenCanvasBridge::GetGradient(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return Gradient();
    }
    return gradientColors_[id];
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFillRect(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsFillRect");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->FillRect(rect);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsStrokeRect(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsStrokeRect");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->StrokeRect(rect);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsClearRect(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsClearRect");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->ClearRect(rect);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFillText(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsFillText");
    if (argc != 3) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    if (!argv[0]) {
        return runtime->NewUndefined();
    }
    auto textState = JsParseTextDirection(runtime, value);
    auto text = argv[0]->ToString(runtime);
    double x = GetJsDoubleVal(runtime, argv[1]);
    double y = GetJsDoubleVal(runtime, argv[2]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->FillText(text, x, y, textState);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsStrokeText(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsStrokeText");
    if (argc != 3) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    if (!argv[0]) {
        return runtime->NewUndefined();
    }
    auto textState = JsParseTextDirection(runtime, value);
    auto text = argv[0]->ToString(runtime);
    double x = GetJsDoubleVal(runtime, argv[1]);
    double y = GetJsDoubleVal(runtime, argv[2]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->StrokeText(text, x, y, textState);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsMeasureText(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // this func should return TextMetrics, including the width of the text
    LOGD("Js Measure Text");
    if (argc != 1) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    if (!argv[0]) {
        return runtime->NewUndefined();
    }
    auto text = argv[0]->ToString(runtime);
    auto textState = JsParseTextState(runtime, value);
    double width = 0.0;
    double height = 0.0;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        width = offscreenCanvas->MeasureText(text, textState);
        height = offscreenCanvas->MeasureTextHeight(text, textState);
    }
    auto textMetrics = runtime->NewObject();
    textMetrics->SetProperty(runtime, "width", runtime->NewNumber(width));
    textMetrics->SetProperty(runtime, "height", runtime->NewNumber(height));
    return textMetrics;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsBeginPath(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("Js Begin path");
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->BeginPath();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsClosePath(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("js close path");
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->ClosePath();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsMoveTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsMoveTo");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->MoveTo(x, y);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsLineTo");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->LineTo(x, y);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsBezierCurveTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsBezierCurveTo");
    if (argc != 6) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    BezierCurveParam param;
    param.cp1x = GetJsDoubleVal(runtime, argv[0]);
    param.cp1y = GetJsDoubleVal(runtime, argv[1]);
    param.cp2x = GetJsDoubleVal(runtime, argv[2]);
    param.cp2y = GetJsDoubleVal(runtime, argv[3]);
    param.x = GetJsDoubleVal(runtime, argv[4]);
    param.y = GetJsDoubleVal(runtime, argv[5]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->BezierCurveTo(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsQuadraticCurveTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsQuadraticCurveTo");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    QuadraticCurveParam param;
    param.cpx = GetJsDoubleVal(runtime, argv[0]);
    param.cpy = GetJsDoubleVal(runtime, argv[1]);
    param.x = GetJsDoubleVal(runtime, argv[2]);
    param.y = GetJsDoubleVal(runtime, argv[3]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->QuadraticCurveTo(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsArc(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsArc");
    if (argc < 5 || argc > 6) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    ArcParam param;
    param.x = GetJsDoubleVal(runtime, argv[0]);
    param.y = GetJsDoubleVal(runtime, argv[1]);
    param.radius = GetJsDoubleVal(runtime, argv[2]);
    param.startAngle = GetJsDoubleVal(runtime, argv[3]);
    param.endAngle = GetJsDoubleVal(runtime, argv[4]);
    if (argc == 6) {
        if (!argv[5]) {
            LOGW("no value");
            return runtime->NewUndefined();
        }
        std::unique_ptr<JsonValue> argPtr = JsonUtil::ParseJsonString(argv[5]->ToString(runtime));
        if (argPtr && argPtr->IsBool()) {
            param.anticlockwise = argPtr->GetBool();
        }
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Arc(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsArcTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsArcTo");
    if (argc != 5) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    ArcToParam param;
    param.x1 = GetJsDoubleVal(runtime, argv[0]);
    param.y1 = GetJsDoubleVal(runtime, argv[1]);
    param.x2 = GetJsDoubleVal(runtime, argv[2]);
    param.y2 = GetJsDoubleVal(runtime, argv[3]);
    param.radius = GetJsDoubleVal(runtime, argv[4]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->ArcTo(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsEllipse(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 7 or 8 parameters: ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise?)
    if (argc < 7 || argc > 8) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    EllipseParam param;
    param.x = GetJsDoubleVal(runtime, argv[0]);
    param.y = GetJsDoubleVal(runtime, argv[1]);
    param.radiusX = GetJsDoubleVal(runtime, argv[2]);
    param.radiusY = GetJsDoubleVal(runtime, argv[3]);
    param.rotation = GetJsDoubleVal(runtime, argv[4]);
    param.startAngle = GetJsDoubleVal(runtime, argv[5]);
    param.endAngle = GetJsDoubleVal(runtime, argv[6]);
    if (argc == 8) {
        int32_t anti = static_cast<int32_t>(GetJsDoubleVal(runtime, argv[7]));
        param.anticlockwise = (anti == 1);
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Ellipse(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsRect(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsRect");
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->AddRect(rect);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFill(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsFill");
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Fill();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsStroke(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsStroke");
    // 0 or 1 parameter: ctx.stroke() / ctx.stroke(path)
    if (argc == 1) {
        auto typeVal = argv[0]->GetProperty(runtime, "__type");
        auto type = typeVal->ToString(runtime);
        if (type != "path2d") {
            LOGE("Stroke Path2D failed, target is not path.");
            return runtime->NewUndefined();
        }
        auto path = GetPath2D(runtime, argv[0]);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return runtime->NewUndefined();
        }
        auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
        if (offscreenCanvas) {
            offscreenCanvas->Stroke(path);
        }
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Stroke();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsClip(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsClip");
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Clip();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsRestore(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsRestore");
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Restore();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSave(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Save();
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsRotate(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsRotate");
    double angle = GetJsDoubleVal(runtime, argv[0]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Rotate(angle);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsScale(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsScale");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Scale(x, y);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSetTransform(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsSetTransform");
    if (argc != 6) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    TransformParam param;
    param.scaleX = GetJsDoubleVal(runtime, argv[0]);
    param.skewX = GetJsDoubleVal(runtime, argv[1]);
    param.skewY = GetJsDoubleVal(runtime, argv[2]);
    param.scaleY = GetJsDoubleVal(runtime, argv[3]);
    param.translateX = GetJsDoubleVal(runtime, argv[4]);
    param.translateY = GetJsDoubleVal(runtime, argv[5]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetTransform(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTransform(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsTransform");
    if (argc != 6) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    TransformParam param;
    param.scaleX = GetJsDoubleVal(runtime, argv[0]);
    param.skewX = GetJsDoubleVal(runtime, argv[1]);
    param.skewY = GetJsDoubleVal(runtime, argv[2]);
    param.scaleY = GetJsDoubleVal(runtime, argv[3]);
    param.translateX = GetJsDoubleVal(runtime, argv[4]);
    param.translateY = GetJsDoubleVal(runtime, argv[5]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Transform(param);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTranslate(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsTranslate");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->Translate(x, y);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSetLineDash(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsSetLineDash");
    if (argc != 1) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto dash = argv[0]->ToString(runtime);
    value->SetProperty(runtime, "lineDash", runtime->NewString(dash));
    auto segments = GetJsDashValue(runtime, argv[0]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineDash(segments);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsGetLineDash(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto val = value->GetProperty(runtime, "lineDash");
    return val;
}

void JsiOffscreenCanvasBridge::ParseDomImage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    double& width, double& height, std::string& src)
{
    auto jsAttr = value->GetProperty(runtime, "attr");
    auto jsSrc = jsAttr->GetProperty(runtime, DOM_SRC);
    auto imgSrc = jsSrc->ToString(runtime);
    src = imgSrc;

    auto jsStyle = value->GetProperty(runtime, "style");
    auto jsWidth = jsStyle->GetProperty(runtime, DOM_WIDTH);
    auto jsHeight = jsStyle->GetProperty(runtime, DOM_HEIGHT);
    auto cWidth = jsWidth->ToString(runtime);
    auto cHeight = jsHeight->ToString(runtime);
    width = StringToDouble(cWidth);
    height = StringToDouble(cHeight);

    if (NearZero(width)) {
        width = StringToDouble(cWidth.substr(0, cWidth.size() - 2)); // remove px units
    }
    if (NearZero(height)) {
        height = StringToDouble(cHeight.substr(0, cHeight.size() - 2));
    }
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsDrawImage(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (!argv[0] || !argv[0]->IsObject(runtime)) {
        return runtime->NewUndefined();
    }

    CanvasImage image;
    double width = 0.0;
    double height = 0.0;
    auto src = argv[0]->GetProperty(runtime, DOM_SRC);
    if (!src->IsString(runtime)) {
        ParseDomImage(runtime, argv[0], width, height, image.src);
    } else {
        auto imgSrc = src->ToString(runtime);
        image.src = imgSrc;
        auto jsWidth = argv[0]->GetProperty(runtime, DOM_WIDTH);
        auto jsHeight = argv[0]->GetProperty(runtime, DOM_HEIGHT);
        width = jsWidth->ToDouble(runtime);
        height = jsHeight->ToDouble(runtime);
    }
    switch (argc) {
        case 3:
            image.flag = 0;
            image.dx = GetJsDoubleVal(runtime, argv[1]);
            image.dy = GetJsDoubleVal(runtime, argv[2]);
            break;
        case 5:
            image.flag = 1;
            image.dx = GetJsDoubleVal(runtime, argv[1]);
            image.dy = GetJsDoubleVal(runtime, argv[2]);
            image.dWidth = GetJsDoubleVal(runtime, argv[3]);
            image.dHeight = GetJsDoubleVal(runtime, argv[4]);
            break;
        case 9:
            image.flag = 2;
            image.sx = GetJsDoubleVal(runtime, argv[1]);
            image.sy = GetJsDoubleVal(runtime, argv[2]);
            image.sWidth = GetJsDoubleVal(runtime, argv[3]);
            image.sHeight = GetJsDoubleVal(runtime, argv[4]);
            image.dx = GetJsDoubleVal(runtime, argv[5]);
            image.dy = GetJsDoubleVal(runtime, argv[6]);
            image.dWidth = GetJsDoubleVal(runtime, argv[7]);
            image.dHeight = GetJsDoubleVal(runtime, argv[8]);
            break;
        default:
            break;
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->DrawImage(image, width, height);
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCreatePath2D(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    shared_ptr<JsValue> path2D = runtime->NewObject();
    path2D->SetProperty(runtime, "__type", runtime->NewString("path2d"));
    path2D->SetProperty(runtime, "__id", runtime->NewInt32(path2dCount_));
    path2D->SetProperty(runtime, "addPath", runtime->NewFunction(JsPath2DAddPath));
    path2D->SetProperty(runtime, "setTransform", runtime->NewFunction(JsPath2DSetTransform));
    path2D->SetProperty(runtime, "moveTo", runtime->NewFunction(JsPath2DMoveTo));
    path2D->SetProperty(runtime, "lineTo", runtime->NewFunction(JsPath2DLineTo));
    path2D->SetProperty(runtime, "arc", runtime->NewFunction(JsPath2DArc));
    path2D->SetProperty(runtime, "arcTo", runtime->NewFunction(JsPath2DArcTo));
    path2D->SetProperty(runtime, "quadraticCurveTo", runtime->NewFunction(JsPath2DQuadraticCurveTo));
    path2D->SetProperty(runtime, "bezierCurveTo", runtime->NewFunction(JsPath2DBezierCurveTo));
    path2D->SetProperty(runtime, "ellipse", runtime->NewFunction(JsPath2DEllipse));
    path2D->SetProperty(runtime, "rect", runtime->NewFunction(JsPath2DRect));
    path2D->SetProperty(runtime, "closePath", runtime->NewFunction(JsPath2DClosePath));
    path2Ds_[path2dCount_] = JsMakePath2D(runtime, value, argv, argc);
    ++path2dCount_;
    return path2D;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DAddPath(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 1 parameter: addPath(path)
    if (argc != 1) {
        LOGE("AddPath to Path2D failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("AddPath to Path2D failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    auto typeVal = argv[0]->GetProperty(runtime, "__type");
    auto type = typeVal->ToString(runtime);
    if (type != "path2d") {
        LOGE("Stroke Path2D failed, target is not path.");
        return runtime->NewUndefined();
    }
    auto toBeAdd = GetPath2D(runtime, argv[0]);
    if (toBeAdd == nullptr) {
        LOGE("AddPath to Path2D failed, to be added path is null.");
        return runtime->NewUndefined();
    }
    holderPath->AddPath(toBeAdd);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DSetTransform(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 6 parameters: setTransform(a, b, c, d, e, f)
    if (argc != 6) {
        LOGE("Call Path2D SetTransform failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D SetTransform failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->SetTransform(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
                             GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]),
                             GetJsDoubleVal(runtime, argv[4]), GetJsDoubleVal(runtime, argv[5]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DMoveTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 2 parameters: moveTo(x, y)
    if (argc != 2) {
        LOGE("Call Path2D Arc MoveTo, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D MoveTo failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->MoveTo(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DLineTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 2 parameters: lineTo(x, y)
    if (argc != 2) {
        LOGE("Call Path2D LineTo failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D LineTo failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->LineTo(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DArc(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 5 or 6 parameters: arc(x, y, radius, startAngle, endAngle, anticlockwise?)
    if (argc < 5 || argc > 6) {
        LOGE("Call Path2D Arc failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Arc failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    bool anticlockwise = false;
    if (argc == 6) {
        int32_t anti = static_cast<int32_t>(GetJsDoubleVal(runtime, argv[7]));
        anticlockwise = (anti == 1);
    }
    holderPath->Arc(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
                    GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]),
                    GetJsDoubleVal(runtime, argv[4]), anticlockwise);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DArcTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 5 parameters: arcTo(x1, y1, x2, y2, radius)
    if (argc != 5) {
        LOGE("Call Path2D ArcTo failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D ArcTo failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->ArcTo(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
        GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]), GetJsDoubleVal(runtime, argv[4]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DQuadraticCurveTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 4 parameters: quadraticCurveTo(cpx, cpy, x, y)
    if (argc != 4) {
        LOGE("Call Path2D QuadraticCurveTo failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D QuadraticCurveTo failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->QuadraticCurveTo(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
        GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DBezierCurveTo(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 6 parameters: bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y)
    if (argc != 6) {
        LOGE("Call Path2D BezierCurveTo failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D BezierCurveTo failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->BezierCurveTo(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
                              GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]),
                              GetJsDoubleVal(runtime, argv[4]), GetJsDoubleVal(runtime, argv[5]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DEllipse(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 7 or 8 parameters: ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise?)
    if (argc < 7 || argc > 8) {
        LOGE("Call Path2D Ellipse failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Ellipse failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    bool anticlockwise = false;
    if (argc == 8) {
        int32_t anti = static_cast<int32_t>(GetJsDoubleVal(runtime, argv[7]));
        anticlockwise = (anti == 1);
    }
    holderPath->Ellipse(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
                        GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]),
                        GetJsDoubleVal(runtime, argv[4]), GetJsDoubleVal(runtime, argv[5]),
                        GetJsDoubleVal(runtime, argv[6]), anticlockwise);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DRect(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    // 4 parameters: rect(x, y, width, height)
    if (argc != 4) {
        LOGE("Call Path2D Rect failed, invalid args.");
        return runtime->NewUndefined();
    }
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Rect failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->Rect(GetJsDoubleVal(runtime, argv[0]), GetJsDoubleVal(runtime, argv[1]),
        GetJsDoubleVal(runtime, argv[2]), GetJsDoubleVal(runtime, argv[3]));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPath2DClosePath(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    if (id < 0) {
        return runtime->NewUndefined();
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D ClosePath failed, holderPath is null.");
        return runtime->NewUndefined();
    }
    holderPath->ClosePath();
    return runtime->NewUndefined();
}

RefPtr<CanvasPath2D> JsiOffscreenCanvasBridge::JsMakePath2D(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc == 1) {
        if (argv[0]->IsString(runtime)) {
            // Example: ctx.createPath2D("M250 150 L150 350 L350 350 Z")
            return AceType::MakeRefPtr<CanvasPath2D>(argv[0]->ToString(runtime));
        } else {
            auto typeVal = argv[0]->GetProperty(runtime, "__type");
            auto type = typeVal->ToString(runtime);
            if (type == "path2d") {
                // Example: ctx.createPath2D(path1)
                return AceType::MakeRefPtr<CanvasPath2D>(GetPath2D(runtime, argv[0]));
            }
        }
    }
    // Example: ctx.createPath2D()
    return AceType::MakeRefPtr<CanvasPath2D>();
}

RefPtr<CanvasPath2D> JsiOffscreenCanvasBridge::GetPath2D(
    const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    if (!runtime || !value) {
        LOGE("runtime or value is null.");
        return 0;
    }
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        auto id = nodeId->ToInt32(runtime);
        if (id >= 0) {
            return path2Ds_[id];
        }
    }
    return nullptr;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTransferToImageBitmap(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto bridge = GetOffscreenCanvasBridge(runtime, value);
    if (bridge) {
        shared_ptr<JsValue> imageBitmap = runtime->NewObject();
        imageBitmap->SetProperty(runtime, "__bridgeId", runtime->NewInt32(bridge->GetBridgeId()));

        return imageBitmap;
    }

    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsToDataURL(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }

    double quality = GetJsDoubleVal(runtime, argv[1]);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    std::string url;
    if (offscreenCanvas) {
        url = offscreenCanvas->ToDataURL(argv[0]->ToString(runtime), quality);
    }

    return runtime->NewString(url.c_str());
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCreatePattern(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiOffscreenCanvasBridge::JsCreatePattern");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }

    auto pattern = runtime->NewObject();
    pattern->SetProperty(runtime, "__id", runtime->NewInt32(patternCount_));
    pattern->SetProperty(runtime, "__type", runtime->NewString("pattern"));
    if (!argv[0] || !argv[0]->IsObject(runtime)) {
        return runtime->NewUndefined();
    }

    double width = 0.0;
    double height = 0.0;
    std::string imageSrc;
    auto jsSrc = argv[0]->GetProperty(runtime, DOM_SRC);
    if (!jsSrc || !jsSrc->IsString(runtime)) {
        ParseDomImage(runtime, argv[0], width, height, imageSrc);
    } else {
        auto jsWidth = argv[0]->GetProperty(runtime, DOM_WIDTH);
        auto jsHeight = argv[0]->GetProperty(runtime, DOM_HEIGHT);
        imageSrc = jsSrc->ToString(runtime);
        width = jsWidth->ToDouble(runtime);
        height = jsHeight->ToDouble(runtime);
    }
    auto repeat = argv[1]->ToString(runtime);
    pattern_[patternCount_].SetImgSrc(imageSrc);
    pattern_[patternCount_].SetImageWidth(width);
    pattern_[patternCount_].SetImageHeight(height);
    pattern_[patternCount_].SetRepetition(repeat);
    ++patternCount_;
    return pattern;
}

Pattern JsiOffscreenCanvasBridge::GetPattern(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    return id < 0 ? Pattern() : pattern_[id];
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCreateImageData(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1 && argc != 2) {
        return runtime->NewUndefined();
    }

    auto imageData = runtime->NewObject();
    int32_t width = 0;
    int32_t height = 0;

    if (argc == 2) {
        width = argv[0]->ToInt32(runtime);
        height = argv[1]->ToInt32(runtime);
    }

    if (argc == 1 && argv[0]->IsObject(runtime)) {
        auto jsWidth = argv[0]->GetProperty(runtime, DOM_WIDTH);
        auto jsHeight = argv[0]->GetProperty(runtime, DOM_HEIGHT);
        width = jsWidth->ToInt32(runtime);
        height = jsHeight->ToInt32(runtime);
    }

    auto colorArray = runtime->NewArray();
    auto color = runtime->NewInt32(255);
    uint32_t count = 0;
    for (auto i = 0; i < width; i++) {
        for (auto j = 0; j < height; j++) {
            colorArray->SetProperty(runtime, runtime->NewInt32(count), color);
            colorArray->SetProperty(runtime, runtime->NewInt32(count + 1), color);
            colorArray->SetProperty(runtime, runtime->NewInt32(count + 2), color);
            colorArray->SetProperty(runtime, runtime->NewInt32(count + 3), color);
            count += 4;
        }
    }
    imageData->SetProperty(runtime, DOM_WIDTH, runtime->NewInt32(width));
    imageData->SetProperty(runtime, DOM_HEIGHT, runtime->NewInt32(height));
    imageData->SetProperty(runtime, "data", colorArray);
    return imageData;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsPutImageData(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 3 && argc != 7 && !argv[0]->IsObject(runtime)) {
        return runtime->NewUndefined();
    }

    int32_t width = 0;
    int32_t height = 0;
    auto jsWidth = argv[0]->GetProperty(runtime, DOM_WIDTH);
    auto jsHeight = argv[0]->GetProperty(runtime, DOM_HEIGHT);
    width = jsWidth->ToInt32(runtime);
    height = jsHeight->ToInt32(runtime);
    ImageData imageData;
    std::vector<std::string> array;
    ParseImageData(runtime, argv, argc, array, imageData);

    int64_t num = 0;
    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            if ((i >= imageData.dirtyY) && (i - imageData.dirtyY < imageData.dirtyHeight) && (j >= imageData.dirtyX) &&
                (j - imageData.dirtyX < imageData.dirtyWidth)) {
                int32_t flag = j + width * i;
                if (array.size() > static_cast<uint32_t>(4 * flag + 3)) {
                    auto red = StringUtils::StringToInt(array[4 * flag]);
                    auto green = StringUtils::StringToInt(array[4 * flag + 1]);
                    auto blue = StringUtils::StringToInt(array[4 * flag + 2]);
                    auto alpha = StringUtils::StringToInt(array[4 * flag + 3]);
                    if (num < imageData.dirtyWidth * imageData.dirtyHeight) {
                        imageData.data.emplace_back(Color::FromARGB(alpha, red, green, blue));
                    }
                    num++;
                }
            }
        }
    }

    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->PutImageData(imageData);
    }
    return runtime->NewUndefined();
}

void JsiOffscreenCanvasBridge::ParseImageData(const shared_ptr<JsRuntime>& runtime,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc, std::vector<std::string>& array, ImageData& imageData)
{
    int32_t width = 0;
    int32_t height = 0;
    auto jsWidth = argv[0]->GetProperty(runtime, DOM_WIDTH);
    auto jsHeight = argv[0]->GetProperty(runtime, DOM_HEIGHT);
    width = jsWidth->ToInt32(runtime);
    height = jsHeight->ToInt32(runtime);

    auto jsData = argv[0]->GetProperty(runtime, "data");
    auto jsDataStr = jsData->ToString(runtime);
    StringUtils::StringSpliter(jsDataStr, ',', array);

    imageData.x = argv[1]->ToInt32(runtime);
    imageData.y = argv[2]->ToInt32(runtime);
    imageData.dirtyWidth = width;
    imageData.dirtyHeight = height;

    if (argc == 7) {
        imageData.dirtyX = argv[3]->ToInt32(runtime);
        imageData.dirtyY = argv[4]->ToInt32(runtime);
        imageData.dirtyWidth = argv[5]->ToInt32(runtime);
        imageData.dirtyHeight = argv[6]->ToInt32(runtime);
    }

    imageData.dirtyWidth = imageData.dirtyX < 0 ? std::min(imageData.dirtyX + imageData.dirtyWidth, width)
                                                : std::min(width - imageData.dirtyX, imageData.dirtyWidth);
    imageData.dirtyHeight = imageData.dirtyY < 0 ? std::min(imageData.dirtyY + imageData.dirtyHeight, height)
                                                 : std::min(height - imageData.dirtyY, imageData.dirtyHeight);
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsGetImageData(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 4) {
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, std::move(argv));
    std::unique_ptr<ImageData> data;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        data = offscreenCanvas->GetImageData(rect.Left(), rect.Top(), rect.Width(), rect.Height());
    }

    auto imageData = runtime->NewObject();
    auto colorArray = runtime->NewArray();
    imageData->SetProperty(runtime, DOM_WIDTH, runtime->NewInt32(data->dirtyWidth));
    imageData->SetProperty(runtime, DOM_HEIGHT, runtime->NewInt32(data->dirtyHeight));
    uint32_t count = 0;
    // travel data
    for (auto i = 0; i < data->dirtyHeight; i++) {
        for (auto j = 0; j < data->dirtyWidth; j++) {
            // a pixel includes 4 data: red/green/blue/alpha
            int32_t idx = i * data->dirtyWidth + j;
            auto pixel = data->data[idx];
            colorArray->SetProperty(runtime, runtime->NewInt32(count), runtime->NewInt32(pixel.GetRed()));
            colorArray->SetProperty(runtime, runtime->NewInt32(count + 1), runtime->NewInt32(pixel.GetGreen()));
            colorArray->SetProperty(runtime, runtime->NewInt32(count + 2), runtime->NewInt32(pixel.GetBlue()));
            colorArray->SetProperty(runtime, runtime->NewInt32(count + 3), runtime->NewInt32(pixel.GetAlpha()));
            count += 4;
        }
    }
    imageData->SetProperty(runtime, "data", colorArray);
    return imageData;
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFillStyleGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__fillStyle");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFillStyleSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (proto->IsString(runtime)) {
        auto colorStr = proto->ToString(runtime);
        auto color = Color::FromString(colorStr);
        if (offscreenCanvas) {
            offscreenCanvas->SetFillColor(color);
            offscreenCanvas->SetFillPattern(Pattern());
            offscreenCanvas->SetFillGradient(Gradient());
        }
    } else {
        auto typeVal = proto->GetProperty(runtime, "__type");
        auto type = typeVal->ToString(runtime);
        if (type == "gradient") {
            auto gradient = GetGradient(runtime, proto);
            if (offscreenCanvas) {
                offscreenCanvas->SetFillGradient(gradient);
                offscreenCanvas->SetFillColor(Color());
                offscreenCanvas->SetFillPattern(Pattern());
            }
        } else if (type == "pattern") {
            auto pattern = GetPattern(runtime, proto);
            if (offscreenCanvas) {
                offscreenCanvas->SetFillPattern(pattern);
                offscreenCanvas->SetFillGradient(Gradient());
                offscreenCanvas->SetFillColor(Color());
            }
        } else {
            LOGW("No such type for fill style.");
        }
    }
    value->SetProperty(runtime, "__fillStyle", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsStrokeStyleGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__strokeStyle");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsStrokeStyleSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (proto->IsString(runtime)) {
        auto colorStr = proto->ToString(runtime);
        auto color = Color::FromString(colorStr);
        if (offscreenCanvas) {
            offscreenCanvas->SetStrokeColor(color);
            offscreenCanvas->SetStrokePattern(Pattern());
            offscreenCanvas->SetStrokeGradient(Gradient());
        }
    } else {
        auto typeVal = proto->GetProperty(runtime, "__type");
        auto type = typeVal->ToString(runtime);
        if (type == "gradient") {
            auto gradient = GetGradient(runtime, proto);
            if (offscreenCanvas) {
                offscreenCanvas->SetStrokeGradient(gradient);
                offscreenCanvas->SetStrokeColor(Color());
                offscreenCanvas->SetStrokePattern(Pattern());
            }
        } else if (type == "pattern") {
            auto pattern = GetPattern(runtime, proto);
            if (offscreenCanvas) {
                offscreenCanvas->SetStrokePattern(pattern);
                offscreenCanvas->SetStrokeGradient(Gradient());
                offscreenCanvas->SetStrokeColor(Color());
            }
        } else {
            LOGW("No such type for stroke style.");
        }
    }
    value->SetProperty(runtime, "__strokeStyle", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineCapGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineCap");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineCapSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto capStr = proto->ToString(runtime);
    static const LinearMapNode<LineCapStyle> lineCapTable[] = {
        { "butt", LineCapStyle::BUTT },
        { "round", LineCapStyle::ROUND },
        { "square", LineCapStyle::SQUARE },
    };
    auto lineCap = ConvertStrToEnum(capStr.c_str(), lineCapTable, ArraySize(lineCapTable), LineCapStyle::BUTT);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineCap(lineCap);
    }
    value->SetProperty(runtime, "__lineCap", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineJoinGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineJoin");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineJoinSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto joinStr = proto->ToString(runtime);
    static const LinearMapNode<LineJoinStyle> lineJoinTable[3] = {
        { "bevel", LineJoinStyle::BEVEL },
        { "miter", LineJoinStyle::MITER },
        { "round", LineJoinStyle::ROUND },
    };
    auto lineJoin = ConvertStrToEnum(joinStr.c_str(), lineJoinTable, ArraySize(lineJoinTable), LineJoinStyle::MITER);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineJoin(lineJoin);
    }
    value->SetProperty(runtime, "__lineJoin", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsMiterLimitGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__miterLimit");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsMiterLimitSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double limit = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetMiterLimit(limit);
    }
    value->SetProperty(runtime, "__miterLimit", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineWidthGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineWidth");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineWidthSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double lineWidth = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineWidth(lineWidth);
    }
    value->SetProperty(runtime, "__lineWidth", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTextAlignGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__textAlign");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTextAlignSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto alignStr = proto->ToString(runtime);
    auto align = ConvertStrToTextAlign(alignStr);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetTextAlign(align);
    }
    value->SetProperty(runtime, "__textAlign", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTextBaselineGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__textBaseline");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsTextBaselineSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto baselineStr = proto->ToString(runtime);
    auto baseline =
        ConvertStrToEnum(baselineStr.c_str(), BASELINE_TABLE, ArraySize(BASELINE_TABLE), TextBaseline::ALPHABETIC);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetTextBaseline(baseline);
    }
    value->SetProperty(runtime, "__textBaseline", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFontGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__font");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFontSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto fontStr = proto->ToString(runtime);
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(fontStr, ' ', fontProps);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (!offscreenCanvas) {
        value->SetProperty(runtime, "__font", proto);
        return runtime->NewUndefined();
    }
    for (const auto& fontProp : fontProps) {
        if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            auto fontStyle = ConvertStrToFontStyle(fontProp);
            offscreenCanvas->SetFontStyle(fontStyle);
        } else if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            auto weight = ConvertStrToFontWeight(fontProp);
            offscreenCanvas->SetFontWeight(weight);
        } else if (FONT_FAMILIES.find(fontProp) != FONT_FAMILIES.end()) {
            auto families = ConvertStrToFontFamilies(fontProp);
            offscreenCanvas->SetFontFamilies(families);
        } else if (fontProp.find("px") != std::string::npos) {
            std::string fontSize = fontProp.substr(0, fontProp.size() - 2);
            auto size = Dimension(StringToDouble(fontProp));
            offscreenCanvas->SetFontSize(size);
        } else {
            LOGW("parse text error");
        }
    }
    value->SetProperty(runtime, "__font", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsAlphaGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__globalAlpha");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsAlphaSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double alpha = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetAlpha(alpha);
    }
    value->SetProperty(runtime, "__globalAlpha", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCompositeOperationGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__globalCompositeOperation");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsCompositeOperationSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto typeStr = proto->ToString(runtime);
    // this map must be sorted by key.
    static const LinearMapNode<CompositeOperation> compositeOperationTable[] = {
        { "copy", CompositeOperation::COPY },
        { "destination-atop", CompositeOperation::DESTINATION_ATOP },
        { "destination-in", CompositeOperation::DESTINATION_IN },
        { "destination-out", CompositeOperation::DESTINATION_OUT },
        { "destination-over", CompositeOperation::DESTINATION_OVER },
        { "lighter", CompositeOperation::LIGHTER },
        { "source-atop", CompositeOperation::SOURCE_ATOP },
        { "source-in", CompositeOperation::SOURCE_IN },
        { "source-out", CompositeOperation::SOURCE_OUT },
        { "source-over", CompositeOperation::SOURCE_OVER },
        { "xor", CompositeOperation::XOR },
    };
    auto type = ConvertStrToEnum(
        typeStr.c_str(), compositeOperationTable, ArraySize(compositeOperationTable), CompositeOperation::SOURCE_OVER);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetCompositeType(type);
    }
    value->SetProperty(runtime, "__globalCompositeOperation", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineDashOffsetGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineDash");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsLineDashOffsetSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double dashoffset = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineDashOffset(dashoffset);
    }
    value->SetProperty(runtime, "__lineDash", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowBlurGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowBlur");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowBlurSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double blur = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowBlur(blur);
    }
    value->SetProperty(runtime, "__shadowBlur", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowColorGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowColor");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowColorSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    auto colorStr = proto->ToString(runtime);
    auto color = Color::FromString(colorStr);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowColor(color);
    }
    value->SetProperty(runtime, "__shadowColor", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowOffsetXGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowOffsetX");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowOffsetXSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double offsetX = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowOffsetX(offsetX);
    }
    value->SetProperty(runtime, "__shadowOffsetX", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowOffsetYGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowOffsetY");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsShadowOffsetYSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    double offsetY = GetJsDoubleVal(runtime, proto);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowOffsetY(offsetY);
    }
    value->SetProperty(runtime, "__shadowOffsetY", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSmoothingEnabledGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__imageSmoothingEnabled");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSmoothingEnabledSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto || !proto->IsBoolean(runtime)) {
        return runtime->NewUndefined();
    }
    auto enabled = proto->ToBoolean(runtime);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetSmoothingEnabled(enabled);
    }
    value->SetProperty(runtime, "__imageSmoothingEnabled", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSmoothingQualityGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__imageSmoothingQuality");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsSmoothingQualitySetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto || !proto->IsString(runtime)) {
        return runtime->NewUndefined();
    }
    const std::string& quality = proto->ToString(runtime);
    if (quality.empty() || QUALITY_TYPE.find(quality) == QUALITY_TYPE.end()) {
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetSmoothingQuality(quality);
    }
    value->SetProperty(runtime, "__imageSmoothingQuality", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFilterParamGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__filter");
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsFilterParamSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto || !proto->IsString(runtime)) {
        return runtime->NewUndefined();
    }
    const std::string& quality = proto->ToString(runtime);
    if (quality.empty()) {
        return runtime->NewUndefined();
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->SetFilterParam(quality);
    }
    value->SetProperty(runtime, "__filter", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsIsPointInStroke(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || ((argc != 2) && (argc != 3))) {
        return runtime->NewUndefined();
    }
    bool ret = false;
    if (!argv.empty() && argc == 2) {
        double x = GetJsDoubleVal(runtime, argv[0]);
        double y = GetJsDoubleVal(runtime, argv[1]);
        auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInStroke(x, y);
        }
    } else if (!argv.empty() && argc == 3) {
        double x = GetJsDoubleVal(runtime, argv[1]);
        double y = GetJsDoubleVal(runtime, argv[2]);
        auto proto = argv[0];
        if (!proto) {
            return runtime->NewUndefined();
        }
        auto typeVal = proto->GetProperty(runtime, "__type");
        if (!typeVal) {
            return runtime->NewUndefined();
        }
        auto type = typeVal->ToString(runtime);
        if (type != "path2d") {
            LOGE("Stroke Path2D failed, target is not path.");
            return runtime->NewUndefined();
        }
        auto path = GetPath2D(runtime, argv[0]);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return runtime->NewUndefined();
        }
        auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInStroke(path, x, y);
        }
    }
    return runtime->NewInt32(ret ? 1 : 0);
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsIsPointInPath(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || ((argc != 2) && (argc != 3))) {
        return runtime->NewUndefined();
    }
    bool ret = false;
    if (!argv.empty() && argc == 2) {
        double x = GetJsDoubleVal(runtime, argv[0]);
        double y = GetJsDoubleVal(runtime, argv[1]);
        auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInPath(x, y);
        }
    } else if (!argv.empty() && argc == 3) {
        double x = GetJsDoubleVal(runtime, argv[1]);
        double y = GetJsDoubleVal(runtime, argv[2]);
        auto proto = argv[0];
        if (!proto) {
            return runtime->NewUndefined();
        }
        auto typeVal = proto->GetProperty(runtime, "__type");
        if (!typeVal) {
            return runtime->NewUndefined();
        }
        auto type = typeVal->ToString(runtime);
        if (type != "path2d") {
            LOGE("Stroke Path2D failed, target is not path.");
            return runtime->NewUndefined();
        }
        auto path = GetPath2D(runtime, argv[0]);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return runtime->NewUndefined();
        }
        auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInPath(path, x, y);
        }
    }
    return runtime->NewInt32(ret ? 1 : 0);
}

shared_ptr<JsValue> JsiOffscreenCanvasBridge::JsResetTransform(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    auto offscreenCanvas = GlobalGetOffscreenCanvas(runtime, value);
    if (offscreenCanvas) {
        offscreenCanvas->ResetTransform();
    }

    return runtime->NewUndefined();
}

} // namespace OHOS::Ace::Framework
