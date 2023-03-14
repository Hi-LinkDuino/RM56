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

#include "frameworks/bridge/js_frontend/engine/quickjs/offscreen_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"

namespace OHOS::Ace::Framework {
namespace {
constexpr char IMAGE_SRC[] = "src";
constexpr char IMAGE_WIDTH[] = "width";
constexpr char IMAGE_HEIGHT[] = "height";
template<typename T>
inline T ConvertStrToEnum(const char* key, const LinearMapNode<T>* map, size_t length, T defaultValue)
{
    int64_t index = BinarySearchFindIndex(map, length, key);
    return index != -1 ? map[index].value : defaultValue;
}
inline std::vector<double> GetJsDashValue(JSContext* ctx, JSValueConst value)
{
    ScopedString scopedString(ctx, value);
    std::vector<std::string> props;
    StringUtils::StringSpliter(scopedString.get(), ',', props);
    std::vector<double> segments;

    for (const auto& prop : props) {
        auto val = StringToDouble(prop);
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
const LinearMapNode<TextBaseline> BASELINE_TABLE[] = {
    { "alphabetic", TextBaseline::ALPHABETIC },
    { "bottom", TextBaseline::BOTTOM },
    { "hanging", TextBaseline::HANGING },
    { "ideographic", TextBaseline::IDEOGRAPHIC },
    { "middle", TextBaseline::MIDDLE },
    { "top", TextBaseline::TOP },
};

const std::set<std::string> FONT_WEIGHTS = {
    "normal", "bold", "lighter", "bolder", "100", "200", "300", "400", "500", "600", "700", "800", "900"
};
const std::set<std::string> FONT_STYLES = { "italic", "oblique", "normal" };
const std::set<std::string> FONT_FAMILIES = { "sans-serif", "serif", "monospace" };
const std::set<std::string> QUALITY_TYPE = { "low", "medium", "high" }; // Default value is low.

inline double GetJsDoubleVal(JSContext* ctx, JSValueConst value)
{
    // use ScopedString to parse double
    ScopedString scopedVal(ctx, value);
    auto jsonVal = JsonUtil::ParseJsonData(scopedVal.get());
    if (jsonVal && jsonVal->IsNumber()) {
        return jsonVal->GetDouble();
    }
    return 0.0;
}

inline Rect GetJsRectParam(JSContext* ctx, int32_t argc, JSValueConst* argv)
{
    // 4 parameters: rect(x, y, width, height)
    if ((!argv) || (argc != 4)) {
        LOGE("argc error, argc = %{private}d", argc);
        return Rect();
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    double width = GetJsDoubleVal(ctx, argv[2]);
    double height = GetJsDoubleVal(ctx, argv[3]);
    Rect rect = Rect(x, y, width, height);
    return rect;
}

inline PaintState JsParseTextDirection(JSContext* ctx, JSValue value)
{
    PaintState state;
    JSValue textDirection = JS_GetPropertyStr(ctx, value, "textDirection");
    ScopedString directionStr(ctx, textDirection);
    state.SetOffTextDirection(ConvertStrToTextDirection(directionStr.get()));

    JS_FreeValue(ctx, textDirection);
    return state;
}

inline PaintState JsParseTextState(JSContext* ctx, JSValue value)
{
    PaintState state;
    JSValue fontStyle = JS_GetPropertyStr(ctx, value, "font");
    JSValue textAlign = JS_GetPropertyStr(ctx, value, "textAlign");
    JSValue textBaseline = JS_GetPropertyStr(ctx, value, "textBaseline");
    JSValue textDirection = JS_GetPropertyStr(ctx, value, "textDirection");

    // parse font styles
    ScopedString alignStr(ctx, textAlign);
    ScopedString directionStr(ctx, textDirection);
    ScopedString baselineStr(ctx, textBaseline);
    state.SetTextAlign(ConvertStrToTextAlign(alignStr.get()));
    state.SetOffTextDirection(ConvertStrToTextDirection(directionStr.get()));

    TextStyle style;
    style.SetTextBaseline(
        ConvertStrToEnum(baselineStr.get(), BASELINE_TABLE, ArraySize(BASELINE_TABLE), TextBaseline::ALPHABETIC));
    ScopedString font(ctx, fontStyle);
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(font.get(), ' ', fontProps);
    for (const auto& fontProp : fontProps) {
        if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            style.SetFontWeight(ConvertStrToFontWeight(fontProp));
        } else if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
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
    state.SetTextStyle(style);
    JS_FreeValue(ctx, fontStyle);
    JS_FreeValue(ctx, textAlign);
    JS_FreeValue(ctx, textBaseline);
    JS_FreeValue(ctx, textDirection);

    return state;
}
}
std::unordered_map<int32_t, RefPtr<CanvasPath2D>> OffscreenCanvasBridge::path2Ds_;
int32_t OffscreenCanvasBridge::gradientCount_ = 0;
int32_t OffscreenCanvasBridge::patternCount_ = 0;
int32_t OffscreenCanvasBridge::path2dCount_ = 0;
std::unordered_map<int32_t, Pattern> OffscreenCanvasBridge::pattern_;
std::unordered_map<int32_t, Gradient> OffscreenCanvasBridge::gradientColors_;
int32_t OffscreenCanvasBridge::globalBridgeId_ = 0;

int32_t GetCurrentBridgeId(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue bridgeId = JS_GetPropertyStr(ctx, value, "__bridgeId");
    if (JS_IsInteger(bridgeId) && (JS_ToInt32(ctx, &id, bridgeId)) < 0) {
        id = 0;
    }
    JS_FreeValue(ctx, bridgeId);
    return id;
}

RefPtr<OffscreenCanvasBridge> GetOffscreenCanvasBridge(JSContext* ctx, JSValueConst value)
{
    int32_t bridgeId =  GetCurrentBridgeId(ctx, value);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance) {
        auto page = instance->GetRunningPage();
        if (page) {
            auto bridge = AceType::DynamicCast<OffscreenCanvasBridge>(page->GetOffscreenCanvasBridgeById(bridgeId));
            return bridge;
        }
    }

    return nullptr;
}

OffscreenCanvasBridge::OffscreenCanvasBridge(JSContext* ctx, int32_t width, int32_t height)
{
    width_ = width;
    height_ = height;
    bridgeId_ = globalBridgeId_++;
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance) {
        auto pipelineContext = instance->GetDelegate()->GetPipelineContext();
        if (pipelineContext) {
            offscreenCanvas_ = pipelineContext->CreateOffscreenCanvas(width_, height_);
        }
    }
}

const JSCFunctionListEntry JS_ANIMATION_FUNCS[] = {
    JS_CGETSET_DEF("fillStyle", OffscreenCanvasBridge::JsFillStyleGetter, OffscreenCanvasBridge::JsFillStyleSetter),
    JS_CGETSET_DEF("strokeStyle", OffscreenCanvasBridge::JsStrokeStyleGetter,
        OffscreenCanvasBridge::JsStrokeStyleSetter),
    JS_CGETSET_DEF("lineCap", OffscreenCanvasBridge::JsLineCapGetter, OffscreenCanvasBridge::JsLineCapSetter),
    JS_CGETSET_DEF("lineJoin", OffscreenCanvasBridge::JsLineJoinGetter, OffscreenCanvasBridge::JsLineJoinSetter),
    JS_CGETSET_DEF("miterLimit", OffscreenCanvasBridge::JsMiterLimitGetter, OffscreenCanvasBridge::JsMiterLimitSetter),
    JS_CGETSET_DEF("lineWidth", OffscreenCanvasBridge::JsLineWidthGetter, OffscreenCanvasBridge::JsLineWidthSetter),
    JS_CGETSET_DEF("textAlign", OffscreenCanvasBridge::JsTextAlignGetter, OffscreenCanvasBridge::JsTextAlignSetter),
    JS_CGETSET_DEF("textBaseline", OffscreenCanvasBridge::JsTextBaselineGetter,
        OffscreenCanvasBridge::JsTextBaselineSetter),
    JS_CGETSET_DEF("font", OffscreenCanvasBridge::JsFontGetter, OffscreenCanvasBridge::JsFontSetter),
    JS_CGETSET_DEF("globalAlpha", OffscreenCanvasBridge::JsAlphaGetter, OffscreenCanvasBridge::JsAlphaSetter),
    JS_CGETSET_DEF("globalCompositeOperation",
        OffscreenCanvasBridge::JsCompositeOperationGetter, OffscreenCanvasBridge::JsCompositeOperationSetter),
    JS_CGETSET_DEF("lineDashOffset", OffscreenCanvasBridge::JsLineDashOffsetGetter,
        OffscreenCanvasBridge::JsLineDashOffsetSetter),
    JS_CGETSET_DEF("shadowBlur", OffscreenCanvasBridge::JsShadowBlurGetter, OffscreenCanvasBridge::JsShadowBlurSetter),
    JS_CGETSET_DEF("shadowColor", OffscreenCanvasBridge::JsShadowColorGetter,
        OffscreenCanvasBridge::JsShadowColorSetter),
    JS_CGETSET_DEF("shadowOffsetX", OffscreenCanvasBridge::JsShadowOffsetXGetter,
        OffscreenCanvasBridge::JsShadowOffsetXSetter),
    JS_CGETSET_DEF("shadowOffsetY", OffscreenCanvasBridge::JsShadowOffsetYGetter,
        OffscreenCanvasBridge::JsShadowOffsetYSetter),
    JS_CGETSET_DEF("imageSmoothingEnabled",
        OffscreenCanvasBridge::JsSmoothingEnabledGetter, OffscreenCanvasBridge::JsSmoothingEnabledSetter),
    JS_CGETSET_DEF("imageSmoothingQuality",
        OffscreenCanvasBridge::JsSmoothingQualityGetter, OffscreenCanvasBridge::JsSmoothingQualitySetter),
    JS_CGETSET_DEF("filter",
        OffscreenCanvasBridge::JsFilterParamGetter, OffscreenCanvasBridge::JsFilterParamSetter),
};

JSValue OffscreenCanvasBridge::GetBridge(JSContext* ctx)
{
    JSValue bridge = JS_NewObject(ctx);

    const std::vector<std::pair<const char*, JSValue>> propertyTable = {
        { "__bridgeId", JS_NewInt32(ctx, bridgeId_) },
        { "getContext", JS_NewCFunction(ctx, JsGetContext, "getContext", 2) },
        { "transferToImageBitmap", JS_NewCFunction(ctx, JsTransferToImageBitmap, "transferToImageBitmap", 0)},
        { "toDataURL", JS_NewCFunction(ctx, JsToDataURL, "toDataURL", 2)},
    };

    for (const auto& iter : propertyTable) {
        JS_SetPropertyStr(ctx, bridge, iter.first, iter.second);
    }
    return bridge;
}

Gradient OffscreenCanvasBridge::GetGradient(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        JS_FreeValue(ctx, nodeId);
        return Gradient();
    }
    JS_FreeValue(ctx, nodeId);
    return gradientColors_[id];
}

Pattern OffscreenCanvasBridge::GetPattern(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        return Pattern();
    }
    JS_FreeValue(ctx, nodeId);
    return pattern_[id];
}

JSValue OffscreenCanvasBridge::JsGetContext(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ScopedString tmp(ctx, value);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        JSValue renderContext = JS_NewObject(ctx);
        const std::vector<std::pair<const char*, JSValue>> propertyTable = {
            { "__bridgeId", JS_NewInt32(ctx, bridge->bridgeId_) },
            { "createLinearGradient", JS_NewCFunction(ctx, JsCreateLinearGradient, "createLinearGradient", 4) },
            { "createRadialGradient", JS_NewCFunction(ctx, JsCreateRadialGradient, "createRadialGradient", 6) },
            { "createPattern", JS_NewCFunction(ctx, JsCreatePattern, "createPattern", 2) },
            { "fillRect", JS_NewCFunction(ctx, JsFillRect, "fillRect", 4) },
            { "clearRect", JS_NewCFunction(ctx, JsClearRect, "clearRect", 4) },
            { "strokeRect", JS_NewCFunction(ctx, JsStrokeRect, "strokeRect", 4) },
            { "beginPath", JS_NewCFunction(ctx, JsBeginPath, "beginPath", 0) },
            { "closePath", JS_NewCFunction(ctx, JsClosePath, "closePath", 0) },
            { "arc", JS_NewCFunction(ctx, JsArc, "arc", 6) },
            { "moveTo", JS_NewCFunction(ctx, JsMoveTo, "moveTo", 2) },
            { "arcTo", JS_NewCFunction(ctx, JsArcTo, "arcTo", 5) },
            { "stroke", JS_NewCFunction(ctx, JsStroke, "stroke", 0) },
            { "rotate", JS_NewCFunction(ctx, JsRotate, "rotate", 1) },
            { "scale", JS_NewCFunction(ctx, JsScale, "scale", 2) },
            { "fillText", JS_NewCFunction(ctx, JsFillText, "fillText", 3) },
            { "strokeText", JS_NewCFunction(ctx, JsStrokeText, "strokeText", 3) },
            { "measureText", JS_NewCFunction(ctx, JsMeasureText, "measureText", 1) },
            { "rect", JS_NewCFunction(ctx, JsRect, "rect", 4) },
            { "fill", JS_NewCFunction(ctx, JsFill, "fill", 0) },
            { "clip", JS_NewCFunction(ctx, JsClip, "clip", 0) },
            { "createImageData", JS_NewCFunction(ctx, JsCreateImageData, "createImageData", 2) },
            { "putImageData", JS_NewCFunction(ctx, JsPutImageData, "putImageData", 7) },
            { "getImageData", JS_NewCFunction(ctx, JsGetImageData, "getImageData", 4) },
            { "restore", JS_NewCFunction(ctx, JsRestore, "restore", 0) },
            { "save", JS_NewCFunction(ctx, JsSave, "save", 0) },
            { "drawImage", JS_NewCFunction(ctx, JsDrawImage, "drawImage", 9) },
            { "lineTo", JS_NewCFunction(ctx, JsLineTo, "lineTo", 2) },
            { "bezierCurveTo", JS_NewCFunction(ctx, JsBezierCurveTo, "bezierCurveTo", 6) },
            { "quadraticCurveTo", JS_NewCFunction(ctx, JsQuadraticCurveTo, "quadraticCurveTo", 4) },
            { "ellipse", JS_NewCFunction(ctx, JsEllipse, "ellipse", 4) },
            { "setTransform", JS_NewCFunction(ctx, JsSetTransform, "setTransform", 6) },
            { "transform", JS_NewCFunction(ctx, JsTransform, "transform", 6) },
            { "translate", JS_NewCFunction(ctx, JsTranslate, "translate", 2) },
            { "getLineDash", JS_NewCFunction(ctx, JsGetLineDash, "getLineDash", 0) },
            { "setLineDash", JS_NewCFunction(ctx, JsSetLineDash, "setLineDash", 1) },
            { "createPath2D", JS_NewCFunction(ctx, JsCreatePath2D, "createPath2D", 0) },
            { "isPointInStroke", JS_NewCFunction(ctx, JsIsPointInStroke, "isPointInStroke", 2) },
            { "isPointInPath", JS_NewCFunction(ctx, JsIsPointInPath, "isPointInPath", 2) },
            { "resetTransform", JS_NewCFunction(ctx, JsResetTransform, "resetTransform", 0) },
        };

        for (const auto& iter : propertyTable) {
            JS_SetPropertyStr(ctx, renderContext, iter.first, iter.second);
        }
        // getter and setter
        JS_SetPropertyFunctionList(ctx, renderContext, JS_ANIMATION_FUNCS, countof(JS_ANIMATION_FUNCS));
        return renderContext;
    }

    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsSetLineDash(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 1 parameter: setLineDash(segments)
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString dash(ctx, argv[0]);
    JS_SetPropertyStr(ctx, value, "lineDash", JS_NewString(ctx, dash.get()));
    auto segments = GetJsDashValue(ctx, argv[0]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetLineDash(segments);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsGetLineDash(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 0 parameter: getLineDash()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto val = JS_GetPropertyStr(ctx, value, "lineDash");
    return val;
}
JSValue OffscreenCanvasBridge::JsTranslate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: translate(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Translate(x, y);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 6 parameters: transform(a, b, c, d, e, f)
    if ((!argv) || (argc != 6)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    TransformParam param;
    param.scaleX = GetJsDoubleVal(ctx, argv[0]);
    param.skewX = GetJsDoubleVal(ctx, argv[1]);
    param.skewY = GetJsDoubleVal(ctx, argv[2]);
    param.scaleY = GetJsDoubleVal(ctx, argv[3]);
    param.translateX = GetJsDoubleVal(ctx, argv[4]);
    param.translateY = GetJsDoubleVal(ctx, argv[5]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Transform(param);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsSetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 6 parameters: setTransform(a, b, c, d, e, f)
    if ((!argv) || (argc != 6)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    TransformParam param;
    param.scaleX = GetJsDoubleVal(ctx, argv[0]);
    param.skewX = GetJsDoubleVal(ctx, argv[1]);
    param.skewY = GetJsDoubleVal(ctx, argv[2]);
    param.scaleY = GetJsDoubleVal(ctx, argv[3]);
    param.translateX = GetJsDoubleVal(ctx, argv[4]);
    param.translateY = GetJsDoubleVal(ctx, argv[5]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetTransform(param);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsEllipse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsEllipse");
    // 7 or 8 parameters: ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise?)
    if ((!argv) || (argc < 7 || argc > 8)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    EllipseParam param;
    param.x = GetJsDoubleVal(ctx, argv[0]);
    param.y = GetJsDoubleVal(ctx, argv[1]);
    param.radiusX = GetJsDoubleVal(ctx, argv[2]);
    param.radiusY = GetJsDoubleVal(ctx, argv[3]);
    param.rotation = GetJsDoubleVal(ctx, argv[4]);
    param.startAngle = GetJsDoubleVal(ctx, argv[5]);
    param.endAngle = GetJsDoubleVal(ctx, argv[6]);
    if (argc == 8) {
        int32_t anti = static_cast<int32_t>(GetJsDoubleVal(ctx, argv[7]));
        param.anticlockwise = (anti == 1);
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Ellipse(param);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsQuadraticCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsQuadraticCurveTo");
    // 4 parameters: quadraticCurveTo(cpx, cpy, x, y)
    if ((!argv) || (argc != 4)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    QuadraticCurveParam param;
    param.cpx = GetJsDoubleVal(ctx, argv[0]);
    param.cpy = GetJsDoubleVal(ctx, argv[1]);
    param.x = GetJsDoubleVal(ctx, argv[2]);
    param.y = GetJsDoubleVal(ctx, argv[3]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->QuadraticCurveTo(param);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsBezierCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsBezierCurveTo");
    // 6 parameters: bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y)
    if ((!argv) || (argc != 6)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    BezierCurveParam param;
    param.cp1x = GetJsDoubleVal(ctx, argv[0]);
    param.cp1y = GetJsDoubleVal(ctx, argv[1]);
    param.cp2x = GetJsDoubleVal(ctx, argv[2]);
    param.cp2y = GetJsDoubleVal(ctx, argv[3]);
    param.x = GetJsDoubleVal(ctx, argv[4]);
    param.y = GetJsDoubleVal(ctx, argv[5]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->BezierCurveTo(param);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsLineTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: lineTo(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->LineTo(x, y);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsScale(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: scale(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Scale(x, y);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsRotate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 1 parameter: rotate(angle)
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double angle = GetJsDoubleVal(ctx, argv[0]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Rotate(angle);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsClosePath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 0 parameter: closePath()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->ClosePath();
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsMoveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: moveTo(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->MoveTo(x, y);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsArcTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsArcTo");
    // 5 parameters: arcTo(x1, y1, x2, y2, radius)
    if ((!argv) || (argc != 5)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ArcToParam param;
    param.x1 = GetJsDoubleVal(ctx, argv[0]);
    param.y1 = GetJsDoubleVal(ctx, argv[1]);
    param.x2 = GetJsDoubleVal(ctx, argv[2]);
    param.y2 = GetJsDoubleVal(ctx, argv[3]);
    param.radius = GetJsDoubleVal(ctx, argv[4]);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->ArcTo(param);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsStroke(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsStroke");
    // 0 or 1 parameter: ctx.stroke() / ctx.stroke(path)
    if (argv != nullptr && argc == 1) {
        JSValue typeVal = JS_GetPropertyStr(ctx, argv[0], "__type");
        ScopedString type(ctx, typeVal);
        if (std::strcmp(type.get(), "path2d") != 0) {
            LOGE("Stroke Path2D failed, target is not path.");
            return JS_NULL;
        }
        auto path = GetPath2D(ctx, argv[0]);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return JS_NULL;
        }
        auto bridge = GetOffscreenCanvasBridge(ctx, value);
        if (bridge) {
            bridge->offscreenCanvas_->Stroke(path);
        }
        return JS_NULL;
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Stroke();
    }
    return JS_NULL;
}

RefPtr<CanvasPath2D> OffscreenCanvasBridge::GetPath2D(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        return nullptr;
    }
    JS_FreeValue(ctx, nodeId);
    return path2Ds_[id];
}

JSValue OffscreenCanvasBridge::JsArc(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsArc");
    // 5 or 6 parameters: arc(x, y, radius, startAngle, endAngle, anticlockwise?)
    if ((!argv) || (argc < 5 || argc > 6)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ArcParam param;
    param.x = GetJsDoubleVal(ctx, argv[0]);
    param.y = GetJsDoubleVal(ctx, argv[1]);
    param.radius = GetJsDoubleVal(ctx, argv[2]);
    param.startAngle = GetJsDoubleVal(ctx, argv[3]);
    param.endAngle = GetJsDoubleVal(ctx, argv[4]);
    if (argc == 6) {
        ScopedString arg(ctx, argv[5]);
        if (!arg.get()) {
            LOGW("no value");
            return JS_NULL;
        }
        std::unique_ptr<JsonValue> argPtr = JsonUtil::ParseJsonString(arg.get());
        if (argPtr && argPtr->IsBool()) {
            param.anticlockwise = argPtr->GetBool();
        }
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Arc(param);
    }
    return JS_NULL;
}
JSValue OffscreenCanvasBridge::JsBeginPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("Js Begin path");
    // 0 parameter: beginPath()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->BeginPath();
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsCreatePath2D(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    JSValue path2D = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, path2D, "__type", JS_NewString(ctx, "path2d"));
    JS_SetPropertyStr(ctx, path2D, "__id", JS_NewInt32(ctx, path2dCount_));
    JS_SetPropertyStr(ctx, path2D, "addPath", JS_NewCFunction(ctx, JsPath2DAddPath, "addPath", 1));
    JS_SetPropertyStr(ctx, path2D, "setTransform", JS_NewCFunction(ctx, JsPath2DSetTransform, "setTransform", 6));
    JS_SetPropertyStr(ctx, path2D, "moveTo", JS_NewCFunction(ctx, JsPath2DMoveTo, "moveTo", 2));
    JS_SetPropertyStr(ctx, path2D, "lineTo", JS_NewCFunction(ctx, JsPath2DLineTo, "lineTo", 2));
    JS_SetPropertyStr(ctx, path2D, "arc", JS_NewCFunction(ctx, JsPath2DArc, "arc", 6));
    JS_SetPropertyStr(ctx, path2D, "arcTo", JS_NewCFunction(ctx, JsPath2DArcTo, "arcTo", 5));
    JS_SetPropertyStr(ctx, path2D, "quadraticCurveTo",
        JS_NewCFunction(ctx, JsPath2DQuadraticCurveTo, "quadraticCurveTo", 4));
    JS_SetPropertyStr(ctx, path2D, "bezierCurveTo", JS_NewCFunction(ctx, JsPath2DBezierCurveTo, "bezierCurveTo", 6));
    JS_SetPropertyStr(ctx, path2D, "ellipse", JS_NewCFunction(ctx, JsPath2DEllipse, "ellipse", 8));
    JS_SetPropertyStr(ctx, path2D, "rect", JS_NewCFunction(ctx, JsPath2DRect, "rect", 4));
    JS_SetPropertyStr(ctx, path2D, "closePath", JS_NewCFunction(ctx, JsPath2DClosePath, "closePath", 0));
    path2Ds_[path2dCount_] = JsMakePath2D(ctx, value, argc, argv);
    ++path2dCount_;
    return path2D;
}

JSValue OffscreenCanvasBridge::JsPath2DAddPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 1 parameter: addPath(path)
    if ((!argv) || (argc != 1)) {
        LOGE("AddPath to Path2D failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("AddPath to Path2D failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("AddPath to Path2D failed, holderPath is null.");
        return JS_NULL;
    }
    JSValue typeVal = JS_GetPropertyStr(ctx, argv[0], "__type");
    ScopedString type(ctx, typeVal);
    if (std::strcmp(type.get(), "path2d") != 0) {
        LOGE("AddPath to Path2D failed, to be added is not path.");
        return JS_NULL;
    }
    auto toBeAdd = GetPath2D(ctx, argv[0]);
    if (toBeAdd == nullptr) {
        LOGE("AddPath to Path2D failed, to be added path is null.");
        return JS_NULL;
    }
    holderPath->AddPath(toBeAdd);
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DSetTransform(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    // 6 parameters: setTransform(a, b, c, d, e, f)
    if ((!argv) || (argc != 6)) {
        LOGE("Call Path2D SetTransform failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D SetTransform failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D SetTransform failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->SetTransform(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]),
        GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]),
        GetJsDoubleVal(ctx, argv[4]), GetJsDoubleVal(ctx, argv[5]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DMoveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: moveTo(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("Call Path2D Arc MoveTo, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D MoveTo failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D MoveTo failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->MoveTo(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DLineTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: lineTo(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("Call Path2D LineTo failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D LineTo failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D LineTo failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->LineTo(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DArc(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 5 or 6 parameters: arc(x, y, radius, startAngle, endAngle, anticlockwise?)
    if ((!argv) || (argc < 5) || (argc > 6)) {
        LOGE("Call Path2D Arc failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D Arc failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Arc failed, holderPath is null.");
        return JS_NULL;
    }
    bool anticlockwise = false;
    if (argc == 6) {
        int32_t anti = static_cast<int32_t>(GetJsDoubleVal(ctx, argv[7]));
        anticlockwise = (anti == 1);
    }
    holderPath->Arc(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]),
        GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]),
        GetJsDoubleVal(ctx, argv[4]), anticlockwise);
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DArcTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 5 parameters: arcTo(x1, y1, x2, y2, radius)
    if ((!argv) || (argc != 5)) {
        LOGE("Call Path2D ArcTo failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D ArcTo failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D ArcTo failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->ArcTo(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]), GetJsDoubleVal(ctx, argv[2]),
        GetJsDoubleVal(ctx, argv[3]), GetJsDoubleVal(ctx, argv[4]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DQuadraticCurveTo(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    // 4 parameters: quadraticCurveTo(cpx, cpy, x, y)
    if ((!argv) || (argc != 4)) {
        LOGE("Call Path2D QuadraticCurveTo failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D QuadraticCurveTo failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D QuadraticCurveTo failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->QuadraticCurveTo(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]),
        GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DBezierCurveTo(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    // 6 parameters: bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y)
    if ((!argv) || (argc != 6)) {
        LOGE("Call Path2D BezierCurveTo failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D BezierCurveTo failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D BezierCurveTo failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->BezierCurveTo(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]),
        GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]),
        GetJsDoubleVal(ctx, argv[4]), GetJsDoubleVal(ctx, argv[5]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DEllipse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 7 or 8 parameters: ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise?)
    if ((!argv) || (argc < 7) || (argc > 8)) {
        LOGE("Call Path2D Ellipse failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D Ellipse failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Ellipse failed, holderPath is null.");
        return JS_NULL;
    }
    bool anticlockwise = false;
    if (argc == 8) {
        int32_t anti = static_cast<int32_t>(GetJsDoubleVal(ctx, argv[7]));
        anticlockwise = (anti == 1);
    }
    holderPath->Ellipse(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]),
        GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]),
        GetJsDoubleVal(ctx, argv[4]), GetJsDoubleVal(ctx, argv[5]),
        GetJsDoubleVal(ctx, argv[6]), anticlockwise);
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 4 parameters: rect(x, y, width, height)
    if ((!argv) || (argc != 4)) {
        LOGE("Call Path2D Rect failed, invalid args.");
        return JS_NULL;
    }
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D Rect failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Rect failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->Rect(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]),
        GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsPath2DClosePath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) < 0) {
        LOGE("Call Path2D ClosePath failed, unknown holder path.");
        return JS_NULL;
    }
    auto holderPath = path2Ds_[id];
    if (holderPath == nullptr) {
        LOGE("Call Path2D ClosePath failed, holderPath is null.");
        return JS_NULL;
    }
    holderPath->ClosePath();
    return JS_NULL;
}

RefPtr<CanvasPath2D> OffscreenCanvasBridge::JsMakePath2D(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    if (argv != nullptr && argc == 1) {
        if (JS_IsString(argv[0])) {
            // Example: ctx.createPath2D("M250 150 L150 350 L350 350 Z")
            return AceType::MakeRefPtr<CanvasPath2D>(JS_ToCString(ctx, argv[0]));
        } else {
            JSValue typeVal = JS_GetPropertyStr(ctx, argv[0], "__type");
            ScopedString type(ctx, typeVal);
            if (std::strcmp(type.get(), "path2d") == 0) {
                // Example: ctx.createPath2D(path1)
                return AceType::MakeRefPtr<CanvasPath2D>(GetPath2D(ctx, argv[0]));
            }
        }
    }
    // Example: ctx.createPath2D()
    return AceType::MakeRefPtr<CanvasPath2D>();
}

JSValue OffscreenCanvasBridge::JsTransferToImageBitmap(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    ScopedString tmp(ctx, value);
    LOGE("111OffscreenCanvasBridge::JsTransferToImageBitmap value = %{public}s", tmp.get());

    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        JSValue imageBitmap = JS_NewObject(ctx);
        const std::vector<std::pair<const char*, JSValue>> propertyTable = {
            { "__bridgeId", JS_NewInt32(ctx, bridge->bridgeId_) },
        };

        for (const auto& iter : propertyTable) {
            JS_SetPropertyStr(ctx, imageBitmap, iter.first, iter.second);
        }

        return imageBitmap;
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsToDataURL(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString arg(ctx, argv[0]);
    if (!arg.get()) {
        LOGW("no value for text");
        return JS_NULL;
    }
    double quality = GetJsDoubleVal(ctx, argv[1]);
    ScopedString tmp(ctx, value);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    std::string url;
    if (bridge) {
        url = bridge->offscreenCanvas_->ToDataURL(arg.get(), quality);
    }

    return JS_NewString(ctx, url.c_str());
}

JSValue OffscreenCanvasBridge::JsCreateLinearGradient(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    // 4 parameters: createLinearGradient(x0, y0, x1, y1)
    if ((!argv) || (argc != 4)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    JSValue gradient = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, gradient, "__type", JS_NewString(ctx, "gradient"));
    JS_SetPropertyStr(ctx, gradient, "__id", JS_NewInt32(ctx, gradientCount_)); // global linear gradients
    JS_SetPropertyStr(ctx, gradient, "addColorStop", JS_NewCFunction(ctx, JsAddColorStop, "addColorStop", 2));
    Offset beginOffset = Offset(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]));
    Offset endOffset = Offset(GetJsDoubleVal(ctx, argv[2]), GetJsDoubleVal(ctx, argv[3]));
    gradientColors_[gradientCount_].SetType(GradientType::LINEAR);
    gradientColors_[gradientCount_].SetBeginOffset(beginOffset);
    gradientColors_[gradientCount_].SetEndOffset(endOffset);
    ++gradientCount_;
    return gradient;
}

JSValue OffscreenCanvasBridge::JsAddColorStop(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 2 parameters: addColorStop(offset, color)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    GradientColor color;
    auto jsColor = JS_ToCString(ctx, argv[1]);
    color.SetColor(Color::FromString(jsColor));
    color.SetDimension(GetJsDoubleVal(ctx, argv[0]));
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && JS_ToInt32(ctx, &id, nodeId) >= 0) {
        gradientColors_[id].AddColor(color);
    }
    JS_FreeValue(ctx, nodeId);
    JS_FreeCString(ctx, jsColor);
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsCreatePattern(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("OffscreenCanvasBridge::JsCreatePattern");
    // 2 parameters: createPattern(image, repetition)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    JSValue pattern = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, pattern, "__id", JS_NewFloat64(ctx, patternCount_));
    JS_SetPropertyStr(ctx, pattern, "__type", JS_NewString(ctx, "pattern"));

    if (!JS_IsObject(argv[0])) {
        return JS_NULL;
    }
    double width = 0.0;
    double height = 0.0;
    std::string imageSrc;
    auto jsSrc = JS_GetPropertyStr(ctx, argv[0], IMAGE_SRC);
    if (!JS_ToBool(ctx, jsSrc)) {
        ParseDomImage(ctx, argv[0], width, height, imageSrc);
    } else {
        auto jsWidth = JS_GetPropertyStr(ctx, argv[0], IMAGE_WIDTH);
        auto jsHeight = JS_GetPropertyStr(ctx, argv[0], IMAGE_HEIGHT);
        auto cImageSrc = JS_ToCString(ctx, jsSrc);
        imageSrc = cImageSrc;
        JS_ToFloat64(ctx, &width, jsWidth);
        JS_ToFloat64(ctx, &height, jsHeight);
        JS_FreeValue(ctx, jsWidth);
        JS_FreeValue(ctx, jsHeight);
        JS_FreeCString(ctx, cImageSrc);
    }
    auto repeat = JS_ToCString(ctx, argv[1]);
    pattern_[patternCount_].SetImgSrc(imageSrc);
    pattern_[patternCount_].SetImageWidth(width);
    pattern_[patternCount_].SetImageHeight(height);
    pattern_[patternCount_].SetRepetition(repeat);
    ++patternCount_;

    JS_FreeValue(ctx, jsSrc);
    JS_FreeCString(ctx, repeat);
    return pattern;
}

JSValue OffscreenCanvasBridge::JsCreateRadialGradient(JSContext* ctx, JSValueConst value,
    int32_t argc, JSValueConst* argv)
{
    LOGE("OffscreenCanvasBridge::JsCreateRadialGradient");
    // 6 parameters: createRadialGradient(x0, y0, r0, x1, y1, r1)
    if ((!argv) || (argc != 6)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    JSValue gradient = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, gradient, "__type", JS_NewString(ctx, "gradient"));
    JS_SetPropertyStr(ctx, gradient, "__id", JS_NewInt32(ctx, gradientCount_)); // global linear gradients
    JS_SetPropertyStr(ctx, gradient, "addColorStop", JS_NewCFunction(ctx, JsAddColorStop, "addColorStop", 2));
    Offset innerCenter = Offset(GetJsDoubleVal(ctx, argv[0]), GetJsDoubleVal(ctx, argv[1]));
    Offset outerCenter = Offset(GetJsDoubleVal(ctx, argv[3]), GetJsDoubleVal(ctx, argv[4]));
    double innerRadius = GetJsDoubleVal(ctx, argv[2]);
    double outerRadius = GetJsDoubleVal(ctx, argv[5]);
    gradientColors_[gradientCount_].SetType(GradientType::RADIAL);
    gradientColors_[gradientCount_].SetBeginOffset(innerCenter);
    gradientColors_[gradientCount_].SetEndOffset(outerCenter);
    gradientColors_[gradientCount_].SetInnerRadius(innerRadius);
    gradientColors_[gradientCount_].SetOuterRadius(outerRadius);
    ++gradientCount_;
    return gradient;
}

void OffscreenCanvasBridge::ParseDomImage(JSContext* ctx, JSValueConst value,
    double& width, double& height, std::string& src)
{
    auto jsAttr = JS_GetPropertyStr(ctx, value, "attr");
    auto jsSrc = JS_GetPropertyStr(ctx, jsAttr, IMAGE_SRC);
    auto imgSrc = JS_ToCString(ctx, jsSrc);
    src = imgSrc;

    auto jsStyle = JS_GetPropertyStr(ctx, value, "style");
    auto jsWidth = JS_GetPropertyStr(ctx, jsStyle, IMAGE_WIDTH);
    auto jsHeight = JS_GetPropertyStr(ctx, jsStyle, IMAGE_HEIGHT);
    auto cWidth = JS_ToCString(ctx, jsWidth);
    auto cHeight = JS_ToCString(ctx, jsHeight);
    std::string strWidth = cWidth;
    std::string strHeight = cHeight;
    width = StringToDouble(strWidth);
    height = StringToDouble(strHeight);
    if (NearZero(width)) {
        width = StringToDouble(strWidth.substr(0, strWidth.size() - 2)); // remove px units
    }
    if (NearZero(height)) {
        height = StringToDouble(strHeight.substr(0, strHeight.size() - 2));
    }

    JS_FreeValue(ctx, jsAttr);
    JS_FreeValue(ctx, jsSrc);
    JS_FreeValue(ctx, jsStyle);
    JS_FreeValue(ctx, jsWidth);
    JS_FreeValue(ctx, jsHeight);
    JS_FreeCString(ctx, cWidth);
    JS_FreeCString(ctx, cHeight);
    JS_FreeCString(ctx, imgSrc);
}

JSValue OffscreenCanvasBridge::JsFillRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ScopedString tmp(ctx, value);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        double x = GetJsDoubleVal(ctx, argv[0]);
        double y = GetJsDoubleVal(ctx, argv[1]);
        double width = GetJsDoubleVal(ctx, argv[2]);
        double height = GetJsDoubleVal(ctx, argv[3]);
        Rect rect = Rect(x, y, width, height);
        bridge->offscreenCanvas_->FillRect(rect);
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsClearRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ScopedString tmp(ctx, value);

    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        double x = GetJsDoubleVal(ctx, argv[0]);
        double y = GetJsDoubleVal(ctx, argv[1]);
        double width = GetJsDoubleVal(ctx, argv[2]);
        double height = GetJsDoubleVal(ctx, argv[3]);
        Rect rect = Rect(x, y, width, height);
        bridge->offscreenCanvas_->ClearRect(rect);
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsStrokeRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    ScopedString tmp(ctx, value);

    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        double x = GetJsDoubleVal(ctx, argv[0]);
        double y = GetJsDoubleVal(ctx, argv[1]);
        double width = GetJsDoubleVal(ctx, argv[2]);
        double height = GetJsDoubleVal(ctx, argv[3]);
        Rect rect = Rect(x, y, width, height);
        bridge->offscreenCanvas_->StrokeRect(rect);
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsRect");
    Rect rect = GetJsRectParam(ctx, argc, argv);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->AddRect(rect);
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsFill(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsFill");
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Fill();
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsClip(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsClip");
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Clip();
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsCreateImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (!argv) {
        return JS_NULL;
    }

    // 1 or 2 parameters: createImageData(imagedata) / createImageData(width, height)
    if (argc != 1 && argc != 2) {
        return JS_NULL;
    }

    JSValue imageData = JS_NewObject(ctx);
    int32_t width = 0;
    int32_t height = 0;

    if (argc == 2) {
        JS_ToInt32(ctx, &width, argv[0]);
        JS_ToInt32(ctx, &height, argv[1]);
    }

    if (argc == 1 && JS_IsObject(argv[0])) {
        auto jsWidth = JS_GetPropertyStr(ctx, argv[0], "width");
        auto jsHeight = JS_GetPropertyStr(ctx, argv[0], "height");
        JS_ToInt32(ctx, &width, jsWidth);
        JS_ToInt32(ctx, &height, jsHeight);
        JS_FreeValue(ctx, jsWidth);
        JS_FreeValue(ctx, jsHeight);
    }

    JSValue colorArray = JS_NewArray(ctx);
    uint32_t count = 0;
    for (auto i = 0; i < width; i++) {
        for (auto j = 0; j < height; j++) {
            JS_SetPropertyUint32(ctx, colorArray, count, JS_NewInt32(ctx, 255));
            JS_SetPropertyUint32(ctx, colorArray, count + 1, JS_NewInt32(ctx, 255));
            JS_SetPropertyUint32(ctx, colorArray, count + 2, JS_NewInt32(ctx, 255));
            JS_SetPropertyUint32(ctx, colorArray, count + 3, JS_NewInt32(ctx, 255));
            count += 4;
        }
    }

    JS_SetPropertyStr(ctx, imageData, "width", JS_NewInt32(ctx, width));
    JS_SetPropertyStr(ctx, imageData, "height", JS_NewInt32(ctx, height));
    JS_SetPropertyStr(ctx, imageData, "data", colorArray);
    return imageData;
}

JSValue OffscreenCanvasBridge::JsPutImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (!argv) {
        return JS_NULL;
    }

    // 3 or 7 parameters: putImageData(data, dx, dy) / putImageData(data, dx, dy, dirtyX, dirtyY, dirtyW, dirtyH)
    if (argc != 3 && argc != 7 && !JS_IsObject(argv[0])) {
        return JS_NULL;
    }

    int32_t width = 0;
    int32_t height = 0;
    auto jsWidth = JS_GetPropertyStr(ctx, argv[0], "width");
    auto jsHeight = JS_GetPropertyStr(ctx, argv[0], "height");
    JS_ToInt32(ctx, &width, jsWidth);
    JS_ToInt32(ctx, &height, jsHeight);
    ImageData imageData;
    std::vector<std::string> array;
    ParseImageData(ctx, argv, argc, array, imageData);

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
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->PutImageData(imageData);
    }

    JS_FreeValue(ctx, jsWidth);
    JS_FreeValue(ctx, jsHeight);
    return JS_NULL;
}

void OffscreenCanvasBridge::ParseImageData(
    JSContext* ctx, JSValueConst* argv, int32_t argc, std::vector<std::string>& array, ImageData& imageData)
{
    int32_t width = 0;
    int32_t height = 0;
    auto jsWidth = JS_GetPropertyStr(ctx, argv[0], "width");
    auto jsHeight = JS_GetPropertyStr(ctx, argv[0], "height");
    auto jsData = JS_GetPropertyStr(ctx, argv[0], "data");
    JS_ToInt32(ctx, &width, jsWidth);
    JS_ToInt32(ctx, &height, jsHeight);
    ScopedString scopedString(ctx, jsData);
    StringUtils::StringSpliter(scopedString.get(), ',', array);

    JS_ToInt32(ctx, &imageData.x, argv[1]);
    JS_ToInt32(ctx, &imageData.y, argv[2]);
    imageData.dirtyWidth = width;
    imageData.dirtyHeight = height;

    if (argc == 7) {
        JS_ToInt32(ctx, &imageData.dirtyX, argv[3]);
        JS_ToInt32(ctx, &imageData.dirtyY, argv[4]);
        JS_ToInt32(ctx, &imageData.dirtyWidth, argv[5]);
        JS_ToInt32(ctx, &imageData.dirtyHeight, argv[6]);
    }

    imageData.dirtyWidth = imageData.dirtyX < 0 ? std::min(imageData.dirtyX + imageData.dirtyWidth, width)
        : std::min(width - imageData.dirtyX, imageData.dirtyWidth);
    imageData.dirtyHeight = imageData.dirtyY < 0 ? std::min(imageData.dirtyY + imageData.dirtyHeight, height)
        : std::min(height - imageData.dirtyY, imageData.dirtyHeight);

    JS_FreeValue(ctx, jsWidth);
    JS_FreeValue(ctx, jsHeight);
    JS_FreeValue(ctx, jsData);
}

JSValue OffscreenCanvasBridge::JsGetImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 4 parameters: getImageData(sx, sy, sw, sh)
    if (!argv || (argc != 4)) {
        return JS_NULL;
    }
    Rect rect = GetJsRectParam(ctx, argc, argv);
    rect.SetLeft(static_cast<int32_t>(rect.Left()));
    rect.SetTop(static_cast<int32_t>(rect.Top()));
    rect.SetWidth(static_cast<int32_t>(rect.Width()));
    rect.SetHeight(static_cast<int32_t>(rect.Height()));
    std::unique_ptr<ImageData> data;
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        data = bridge->offscreenCanvas_->GetImageData(rect.Left(), rect.Top(), rect.Width(), rect.Height());
    }
    JSValue imageData = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, imageData, "width", JS_NewInt32(ctx, data->dirtyWidth));
    JS_SetPropertyStr(ctx, imageData, "height", JS_NewInt32(ctx, data->dirtyHeight));
    JSValue colorArray = JS_NewArray(ctx);
    uint32_t count = 0;
    // travel data
    for (auto i = 0; i < data->dirtyHeight; i++) {
        for (auto j = 0; j < data->dirtyWidth; j++) {
            // a pixel includes 4 data: red/green/blue/alpha
            int32_t idx = i * data->dirtyWidth + j;
            auto pixel = data->data[idx];
            JS_SetPropertyUint32(ctx, colorArray, count, JS_NewInt32(ctx, pixel.GetRed()));
            JS_SetPropertyUint32(ctx, colorArray, count + 1, JS_NewInt32(ctx, pixel.GetGreen()));
            JS_SetPropertyUint32(ctx, colorArray, count + 2, JS_NewInt32(ctx, pixel.GetBlue()));
            JS_SetPropertyUint32(ctx, colorArray, count + 3, JS_NewInt32(ctx, pixel.GetAlpha()));
            count += 4;
        }
    }
    JS_SetPropertyStr(ctx, imageData, "data", colorArray);
    return imageData;
}

JSValue OffscreenCanvasBridge::JsDrawImage(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (!argv || !JS_IsObject(argv[0])) {
        return JS_NULL;
    }
    CanvasImage image;
    double width = 0.0;
    double height = 0.0;
    auto src = JS_GetPropertyStr(ctx, argv[0], IMAGE_SRC);
    if (!JS_ToBool(ctx, src)) {
        ParseDomImage(ctx, argv[0], width, height, image.src);
    } else {
        auto imgSrc = JS_ToCString(ctx, src);
        image.src = imgSrc;
        auto jsWidth = JS_GetPropertyStr(ctx, argv[0], IMAGE_WIDTH);
        auto jsHeight = JS_GetPropertyStr(ctx, argv[0], IMAGE_HEIGHT);
        JS_ToFloat64(ctx, &width, jsWidth);
        JS_ToFloat64(ctx, &height, jsHeight);
        JS_FreeValue(ctx, jsWidth);
        JS_FreeValue(ctx, jsHeight);
        JS_FreeCString(ctx, imgSrc);
    }
    switch (argc) {
        // 3 parameters: drawImage(image, dx, dy)
        case 3:
            image.flag = 0;
            image.dx = GetJsDoubleVal(ctx, argv[1]);
            image.dy = GetJsDoubleVal(ctx, argv[2]);
            break;
            // 5 parameters: drawImage(image, dx, dy, dWidth, dHeight)
        case 5:
            image.flag = 1;
            image.dx = GetJsDoubleVal(ctx, argv[1]);
            image.dy = GetJsDoubleVal(ctx, argv[2]);
            image.dWidth = GetJsDoubleVal(ctx, argv[3]);
            image.dHeight = GetJsDoubleVal(ctx, argv[4]);
            break;
            // 9 parameters: drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight)
        case 9:
            image.flag = 2;
            image.sx = GetJsDoubleVal(ctx, argv[1]);
            image.sy = GetJsDoubleVal(ctx, argv[2]);
            image.sWidth = GetJsDoubleVal(ctx, argv[3]);
            image.sHeight = GetJsDoubleVal(ctx, argv[4]);
            image.dx = GetJsDoubleVal(ctx, argv[5]);
            image.dy = GetJsDoubleVal(ctx, argv[6]);
            image.dWidth = GetJsDoubleVal(ctx, argv[7]);
            image.dHeight = GetJsDoubleVal(ctx, argv[8]);
            break;
        default:
            break;
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->DrawImage(image, width, height);
    }

    JS_FreeValue(ctx, src);
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsRestore(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("OffscreenCanvasBridge::JsRestore");
    // 0 parameter: restore()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Restore();
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsSave(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 0 parameter: save()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->Save();
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsFillStyleGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__fillStyle");
}

JSValue OffscreenCanvasBridge::JsHandleStyleSetter(
    JSContext* ctx, JSValueConst value, JSValueConst proto, FunctionCode functionCode)
{
    if (JS_IsString(proto)) {
        auto colorStr = JS_ToCString(ctx, proto);
        auto color = Color::FromString(colorStr);
        auto bridge = GetOffscreenCanvasBridge(ctx, value);
        if (bridge) {
            if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
                bridge->offscreenCanvas_->SetStrokeColor(color);
                bridge->offscreenCanvas_->SetStrokePattern(Pattern());
                bridge->offscreenCanvas_->SetStrokeGradient(Gradient());
            } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
                bridge->offscreenCanvas_->SetFillColor(color);
                bridge->offscreenCanvas_->SetFillPattern(Pattern());
                bridge->offscreenCanvas_->SetFillGradient(Gradient());
            }
        }
        JS_FreeCString(ctx, colorStr);
    } else {
        JSValue typeVal = JS_GetPropertyStr(ctx, proto, "__type");
        ScopedString type(ctx, typeVal);
        if (std::strcmp(type.get(), "gradient") == 0) {
            auto gradient = GetGradient(ctx, proto);
            auto bridge = GetOffscreenCanvasBridge(ctx, value);
            if (bridge) {
                if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
                    bridge->offscreenCanvas_->SetStrokeGradient(gradient);
                    bridge->offscreenCanvas_->SetStrokeColor(Color());
                    bridge->offscreenCanvas_->SetStrokePattern(Pattern());
                } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
                    bridge->offscreenCanvas_->SetFillGradient(gradient);
                    bridge->offscreenCanvas_->SetFillColor(Color());
                    bridge->offscreenCanvas_->SetFillPattern(Pattern());
                }
            }
        } else if (std::strcmp(type.get(), "pattern") == 0) {
            auto pattern = GetPattern(ctx, proto);
            auto bridge = GetOffscreenCanvasBridge(ctx, value);
            if (bridge) {
                if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
                    bridge->offscreenCanvas_->SetStrokePattern(pattern);
                    bridge->offscreenCanvas_->SetStrokeGradient(Gradient());
                    bridge->offscreenCanvas_->SetStrokeColor(Color());
                } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
                    bridge->offscreenCanvas_->SetFillPattern(pattern);
                    bridge->offscreenCanvas_->SetFillGradient(Gradient());
                    bridge->offscreenCanvas_->SetFillColor(Color());
                }
            }
        }
    }

    if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
        JS_SetPropertyStr(ctx, value, "__strokeStyle", JS_DupValue(ctx, proto));
    } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
        JS_SetPropertyStr(ctx, value, "__fillStyle", JS_DupValue(ctx, proto));
    } else {
        LOGW("No such type for set property.");
    }
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsFillStyleSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    return JsHandleStyleSetter(ctx, value, proto, FunctionCode::FILL_STYLE_SETTER);
}

JSValue OffscreenCanvasBridge::JsStrokeStyleGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__strokeStyle");
}

JSValue OffscreenCanvasBridge::JsStrokeStyleSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    return JsHandleStyleSetter(ctx, value, proto, FunctionCode::STROKE_STYLE_SETTER);
}

JSValue OffscreenCanvasBridge::JsLineCapGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineCap");
}

JSValue OffscreenCanvasBridge::JsLineCapSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString capStr(ctx, proto);
    static const LinearMapNode<LineCapStyle> lineCapTable[] = {
        { "butt", LineCapStyle::BUTT },
        { "round", LineCapStyle::ROUND },
        { "square", LineCapStyle::SQUARE },
    };
    auto lineCap = ConvertStrToEnum(capStr.get(), lineCapTable, ArraySize(lineCapTable), LineCapStyle::BUTT);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetLineCap(lineCap);
    }

    JS_SetPropertyStr(ctx, value, "__lineCap", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsLineJoinGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineJoin");
}

JSValue OffscreenCanvasBridge::JsLineJoinSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString joinStr(ctx, proto);
    static const LinearMapNode<LineJoinStyle> lineJoinTable[3] = {
        { "bevel", LineJoinStyle::BEVEL },
        { "miter", LineJoinStyle::MITER },
        { "round", LineJoinStyle::ROUND },
    };
    auto lineJoin = ConvertStrToEnum(joinStr.get(), lineJoinTable, ArraySize(lineJoinTable), LineJoinStyle::MITER);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetLineJoin(lineJoin);
    }

    JS_SetPropertyStr(ctx, value, "__lineJoin", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsMiterLimitGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__miterLimit");
}

JSValue OffscreenCanvasBridge::JsMiterLimitSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double limit = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetMiterLimit(limit);
    }

    JS_SetPropertyStr(ctx, value, "__miterLimit", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsLineWidthGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineWidth");
}

JSValue OffscreenCanvasBridge::JsLineWidthSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double lineWidth = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetLineWidth(lineWidth);
    }

    JS_SetPropertyStr(ctx, value, "__lineWidth", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsTextAlignGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__textAlign");
}

JSValue OffscreenCanvasBridge::JsTextAlignSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString alignStr(ctx, proto);
    auto align = ConvertStrToTextAlign(alignStr.get());
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetTextAlign(align);
    }

    JS_SetPropertyStr(ctx, value, "__textAlign", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsTextBaselineGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__textBaseline");
}

JSValue OffscreenCanvasBridge::JsTextBaselineSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString baselineStr(ctx, proto);
    auto baseline =
        ConvertStrToEnum(baselineStr.get(), BASELINE_TABLE, ArraySize(BASELINE_TABLE), TextBaseline::ALPHABETIC);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetTextBaseline(baseline);
    }

    JS_SetPropertyStr(ctx, value, "__textBaseline", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsFontGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__font");
}

JSValue OffscreenCanvasBridge::JsFontSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString fontStr(ctx, proto);
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(fontStr.get(), ' ', fontProps);
    bool updateFontStyle = false;
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (!bridge) {
        JS_SetPropertyStr(ctx, value, "__font", JS_DupValue(ctx, proto));
        return JS_UNDEFINED;
    }

    for (const auto& fontProp : fontProps) {
        if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            auto weight = ConvertStrToFontWeight(fontProp);
            bridge->offscreenCanvas_->SetFontWeight(weight);
        } else if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            updateFontStyle = true;
            auto fontStyle = ConvertStrToFontStyle(fontProp);
            bridge->offscreenCanvas_->SetFontStyle(fontStyle);
        } else if (FONT_FAMILIES.find(fontProp) != FONT_FAMILIES.end()) {
            auto families = ConvertStrToFontFamilies(fontProp);
            bridge->offscreenCanvas_->SetFontFamilies(families);
        } else if (fontProp.find("px") != std::string::npos) {
            std::string fontSize = fontProp.substr(0, fontProp.size() - 2);
            auto size = Dimension(StringToDouble(fontProp));
            bridge->offscreenCanvas_->SetFontSize(size);
        }
    }
    if (!updateFontStyle) {
        bridge->offscreenCanvas_->SetFontStyle(FontStyle::NORMAL);
    }
    JS_SetPropertyStr(ctx, value, "__font", JS_DupValue(ctx, proto));
    return JS_UNDEFINED;
}

JSValue OffscreenCanvasBridge::JsAlphaGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__globalAlpha");
}

JSValue OffscreenCanvasBridge::JsAlphaSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double alpha = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetAlpha(alpha);
    }
    JS_SetPropertyStr(ctx, value, "__globalAlpha", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsCompositeOperationGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__globalCompositeOperation");
}

JSValue OffscreenCanvasBridge::JsCompositeOperationSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString typeStr(ctx, proto);
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
        typeStr.get(), compositeOperationTable, ArraySize(compositeOperationTable), CompositeOperation::SOURCE_OVER);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetCompositeType(type);
    }

    JS_SetPropertyStr(ctx, value, "__globalCompositeOperation", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsLineDashOffsetGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineDash");
}

JSValue OffscreenCanvasBridge::JsLineDashOffsetSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double dashoffset = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetLineDashOffset(dashoffset);
    }

    JS_SetPropertyStr(ctx, value, "__lineDash", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsShadowBlurGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowBlur");
}

JSValue OffscreenCanvasBridge::JsShadowBlurSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double blur = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetShadowBlur(blur);
    }
    JS_SetPropertyStr(ctx, value, "__shadowBlur", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsShadowColorGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowColor");
}

JSValue OffscreenCanvasBridge::JsShadowColorSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString colorStr(ctx, proto);
    auto color = Color::FromString(colorStr.get());
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetShadowColor(color);
    }
    JS_SetPropertyStr(ctx, value, "__shadowColor", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsShadowOffsetXGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowOffsetX");
}

JSValue OffscreenCanvasBridge::JsShadowOffsetXSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double offsetX = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetShadowOffsetX(offsetX);
    }
    JS_SetPropertyStr(ctx, value, "__shadowOffsetX", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsShadowOffsetYGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowOffsetY");
}

JSValue OffscreenCanvasBridge::JsShadowOffsetYSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double offsetY = GetJsDoubleVal(ctx, proto);
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetShadowOffsetY(offsetY);
    }
    JS_SetPropertyStr(ctx, value, "__shadowOffsetY", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsSmoothingEnabledGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__imageSmoothingEnabled");
}

JSValue OffscreenCanvasBridge::JsSmoothingEnabledSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString enabledStr(ctx, proto);
    if (!enabledStr.get()) {
        return JS_NULL;
    }
    bool enabled = std::string(enabledStr.get()) == "true";
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetSmoothingEnabled(enabled);
    }

    JS_SetPropertyStr(ctx, value, "__imageSmoothingEnabled", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsSmoothingQualityGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__imageSmoothingQuality");
}

JSValue OffscreenCanvasBridge::JsSmoothingQualitySetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString qualityStr(ctx, proto);
    if (!qualityStr.get()) {
        return JS_NULL;
    }
    std::string quality = qualityStr.get();
    if (QUALITY_TYPE.find(quality) == QUALITY_TYPE.end()) {
        return JS_NULL;
    }

    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetSmoothingQuality(quality);
    }

    JS_SetPropertyStr(ctx, value, "__imageSmoothingQuality", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsFilterParamGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__filter");
}

JSValue OffscreenCanvasBridge::JsFilterParamSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString qualityStr(ctx, proto);
    if (!qualityStr.get()) {
        return JS_NULL;
    }
    std::string quality = qualityStr.get();
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->SetFilterParam(quality);
    }
    JS_SetPropertyStr(ctx, value, "__filter", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsIsPointInStroke(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (!argv || ((argc != 2) && (argc != 3))) {
        return JS_NULL;
    }
    bool ret = false;
    if (argv != nullptr && argc == 2) {
        double x = GetJsDoubleVal(ctx, argv[0]);
        double y = GetJsDoubleVal(ctx, argv[1]);
        auto bridge = GetOffscreenCanvasBridge(ctx, value);
        if (bridge) {
            ret = bridge->offscreenCanvas_->IsPointInStroke(x, y);
        }
        return JS_NewInt32(ctx, ret ? 1 : 0);
    }

    if (argv != nullptr && argc == 3) {
        double x = GetJsDoubleVal(ctx, argv[1]);
        double y = GetJsDoubleVal(ctx, argv[2]);
        JSValue typeVal = JS_GetPropertyStr(ctx, argv[0], "__type");
        ScopedString type(ctx, typeVal);
        if (std::strcmp(type.get(), "path2d") != 0) {
            LOGE("Stroke Path2D failed, target is not path.");
            return JS_NULL;
        }
        auto path = GetPath2D(ctx, argv[0]);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return JS_NULL;
        }
        auto bridge = GetOffscreenCanvasBridge(ctx, value);
        if (bridge) {
            ret = bridge->offscreenCanvas_->IsPointInStroke(path, x, y);
        }
        return JS_NewInt32(ctx, ret ? 1 : 0);
    }
    return JS_NewInt32(ctx, ret ? 1 : 0);
}

JSValue OffscreenCanvasBridge::JsIsPointInPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (!argv || ((argc != 2) && (argc != 3))) {
        return JS_NULL;
    }
    bool ret = false;
    if (argv != nullptr && argc == 2) {
        double x = GetJsDoubleVal(ctx, argv[0]);
        double y = GetJsDoubleVal(ctx, argv[1]);
        auto bridge = GetOffscreenCanvasBridge(ctx, value);
        if (bridge) {
            ret = bridge->offscreenCanvas_->IsPointInPath(x, y);
        }
        return JS_NewInt32(ctx, ret ? 1 : 0);
    }

    if (argv != nullptr && argc == 3) {
        double x = GetJsDoubleVal(ctx, argv[1]);
        double y = GetJsDoubleVal(ctx, argv[2]);
        JSValue typeVal = JS_GetPropertyStr(ctx, argv[0], "__type");
        ScopedString type(ctx, typeVal);
        if (std::strcmp(type.get(), "path2d") != 0) {
            LOGE("Stroke Path2D failed, target is not path.");
            return JS_NULL;
        }
        auto path = GetPath2D(ctx, argv[0]);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return JS_NULL;
        }
        auto bridge = GetOffscreenCanvasBridge(ctx, value);
        if (bridge) {
            ret = bridge->offscreenCanvas_->IsPointInPath(path, x, y);
        }
        return JS_NewInt32(ctx, ret ? 1 : 0);
    }
    return JS_NewInt32(ctx, ret ? 1 : 0);
}

JSValue OffscreenCanvasBridge::JsResetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->ResetTransform();
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsFillText(
    JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 3 parameters: fillText(text, x, y, [maxWidth])
    if ((!argv) || (argc != 3)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString arg(ctx, argv[0]);
    if (!arg.get()) {
        return JS_NULL;
    }
    auto textState = JsParseTextDirection(ctx, value);

    std::string text = arg.get();
    double x = GetJsDoubleVal(ctx, argv[1]);
    double y = GetJsDoubleVal(ctx, argv[2]);

    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->FillText(text, x, y, textState);
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsStrokeText(
    JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 3 parameters: fillText(text, x, y, [maxWidth])
    if ((!argv) || (argc != 3)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString arg(ctx, argv[0]);
    if (!arg.get()) {
        return JS_NULL;
    }

    auto textState = JsParseTextDirection(ctx, value);
    std::string text = arg.get();
    double x = GetJsDoubleVal(ctx, argv[1]);
    double y = GetJsDoubleVal(ctx, argv[2]);

    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        bridge->offscreenCanvas_->StrokeText(text, x, y, textState);
    }

    return JS_NULL;
}

JSValue OffscreenCanvasBridge::JsMeasureText(
    JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // this func should return TextMetrics, including the width of the text
    LOGD("Js Measure Text");
    // 1 parameter: measureText(text)
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString arg(ctx, argv[0]);
    if (!arg.get()) {
        LOGW("no value for text");
        return JS_NULL;
    }
    std::string text = arg.get();
    auto textState = JsParseTextState(ctx, value);
    double width = 0.0;
    double height = 0.0;
    auto bridge = GetOffscreenCanvasBridge(ctx, value);
    if (bridge) {
        width = bridge->offscreenCanvas_->MeasureText(text, textState);
        height = bridge->offscreenCanvas_->MeasureTextHeight(text, textState);
    }

    JSValue textMetrics = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, textMetrics, "width", JS_NewFloat64(ctx, width));
    JS_SetPropertyStr(ctx, textMetrics, "height", JS_NewFloat64(ctx, height));
    return textMetrics;
}
} // namespace OHOS::Ace::Framework
