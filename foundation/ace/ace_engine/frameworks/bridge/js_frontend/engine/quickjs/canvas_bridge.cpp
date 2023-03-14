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

#include "frameworks/bridge/js_frontend/engine/quickjs/canvas_bridge.h"

#include "base/utils/linear_map.h"
#include "core/components/custom_paint/offscreen_canvas.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/offscreen_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_engine.h"

namespace OHOS::Ace::Framework {

namespace {

constexpr char IMAGE_SRC[] = "src";
constexpr char IMAGE_WIDTH[] = "width";
constexpr char IMAGE_HEIGHT[] = "height";

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
constexpr char CANVAS_TYPE_WEBGL[] = "webgl";
constexpr char CANVAS_TYPE_WEBGL2[] = "webgl2";
constexpr char CANVAS_WEBGL_SO[] = "webglnapi";
#endif

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

const std::set<std::string> FONT_WEIGHTS = { "normal", "bold", "lighter", "bolder",
    "100", "200", "300", "400", "500", "600", "700", "800", "900" };
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

inline NodeId GetCurrentNodeId(JSContext* ctx, JSValueConst value)
{
    NodeId id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__nodeId");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        id = 0;
    }
    JS_FreeValue(ctx, nodeId);
    return id;
}

void PushTaskToPage(JSContext* ctx, JSValueConst value, const std::function<void(const RefPtr<CanvasTaskPool>&)>& task)
{
    // get node id
    NodeId id = GetCurrentNodeId(ctx, value);
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    // push command
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    ACE_DCHECK(instance);
    auto page = instance->GetRunningPage();
    if (!page) {
        return;
    }
    page->PushCommand(command);
}

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
void PushTaskToPageById(JSContext* ctx, NodeId id, const std::function<void(const RefPtr<CanvasTaskPool>&)>& task)
{
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    // push command
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    ACE_DCHECK(instance);
    auto page = instance->GetRunningPage();
    if (!page) {
        return;
    }
    page->PushCommand(command);
}
#endif

inline PaintState JsParseTextState(JSContext* ctx, JSValue value)
{
    PaintState state;
    JSValue fontStyle = JS_GetPropertyStr(ctx, value, "font");
    JSValue textAlign = JS_GetPropertyStr(ctx, value, "textAlign");
    JSValue textBaseline = JS_GetPropertyStr(ctx, value, "textBaseline");

    // parse font styles
    ScopedString alignStr(ctx, textAlign);
    ScopedString baselineStr(ctx, textBaseline);
    state.SetTextAlign(ConvertStrToTextAlign(alignStr.get()));
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

    return state;
}

} // namespace

int32_t CanvasBridge::gradientCount_ = 0;
int32_t CanvasBridge::patternCount_ = 0;
int32_t CanvasBridge::path2dCount_ = 0;
std::unordered_map<int32_t, Pattern> CanvasBridge::pattern_;
std::unordered_map<int32_t, Gradient> CanvasBridge::gradientColors_;
std::unordered_map<int32_t, RefPtr<CanvasPath2D>> CanvasBridge::path2Ds_;

const JSCFunctionListEntry JS_ANIMATION_FUNCS[] = {
    JS_CGETSET_DEF("fillStyle", CanvasBridge::JsFillStyleGetter, CanvasBridge::JsFillStyleSetter),
    JS_CGETSET_DEF("strokeStyle", CanvasBridge::JsStrokeStyleGetter, CanvasBridge::JsStrokeStyleSetter),
    JS_CGETSET_DEF("lineCap", CanvasBridge::JsLineCapGetter, CanvasBridge::JsLineCapSetter),
    JS_CGETSET_DEF("lineJoin", CanvasBridge::JsLineJoinGetter, CanvasBridge::JsLineJoinSetter),
    JS_CGETSET_DEF("miterLimit", CanvasBridge::JsMiterLimitGetter, CanvasBridge::JsMiterLimitSetter),
    JS_CGETSET_DEF("lineWidth", CanvasBridge::JsLineWidthGetter, CanvasBridge::JsLineWidthSetter),
    JS_CGETSET_DEF("textAlign", CanvasBridge::JsTextAlignGetter, CanvasBridge::JsTextAlignSetter),
    JS_CGETSET_DEF("textBaseline", CanvasBridge::JsTextBaselineGetter, CanvasBridge::JsTextBaselineSetter),
    JS_CGETSET_DEF("font", CanvasBridge::JsFontGetter, CanvasBridge::JsFontSetter),
    JS_CGETSET_DEF("globalAlpha", CanvasBridge::JsAlphaGetter, CanvasBridge::JsAlphaSetter),
    JS_CGETSET_DEF("globalCompositeOperation",
        CanvasBridge::JsCompositeOperationGetter, CanvasBridge::JsCompositeOperationSetter),
    JS_CGETSET_DEF("lineDashOffset", CanvasBridge::JsLineDashOffsetGetter, CanvasBridge::JsLineDashOffsetSetter),
    JS_CGETSET_DEF("shadowBlur", CanvasBridge::JsShadowBlurGetter, CanvasBridge::JsShadowBlurSetter),
    JS_CGETSET_DEF("shadowColor", CanvasBridge::JsShadowColorGetter, CanvasBridge::JsShadowColorSetter),
    JS_CGETSET_DEF("shadowOffsetX", CanvasBridge::JsShadowOffsetXGetter, CanvasBridge::JsShadowOffsetXSetter),
    JS_CGETSET_DEF("shadowOffsetY", CanvasBridge::JsShadowOffsetYGetter, CanvasBridge::JsShadowOffsetYSetter),
    JS_CGETSET_DEF("imageSmoothingEnabled",
        CanvasBridge::JsSmoothingEnabledGetter, CanvasBridge::JsSmoothingEnabledSetter),
    JS_CGETSET_DEF("width", CanvasBridge::JsWidthGetter, nullptr),
    JS_CGETSET_DEF("height", CanvasBridge::JsHeightGetter, nullptr),
};

CanvasBridge::~CanvasBridge()
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (webglRenderContext_) {
        delete webglRenderContext_;
        webglRenderContext_ = nullptr;
    }
    if (webgl2RenderContext_) {
        delete webgl2RenderContext_;
        webgl2RenderContext_ = nullptr;
    }
#endif
}

void CanvasBridge::HandleJsContext(JSContext* ctx, NodeId id, const std::string& args, JsEngineInstance* engine)
{
    LOGD("CanvasBridge::HandleJsContext");
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (argsValue && argsValue->IsArray() && argsValue->GetArraySize() > 0) {
        auto typeArg = argsValue->GetArrayItem(0);
        if (typeArg && typeArg->IsString()) {
            std::string type = typeArg->GetString();
            if (type == std::string(CANVAS_TYPE_WEBGL)) {
                JSHandleWebglContext(ctx, id, args, engine, webglRenderContext_);
                return;
            } else if (type == std::string(CANVAS_TYPE_WEBGL2)) {
                JSHandleWebglContext(ctx, id, args, engine, webgl2RenderContext_);
                return;
            }
        }
    }
#endif

    renderContext_ = JS_NewObject(ctx);
    const std::vector<std::pair<const char*, JSValue>> contextTable = {
        { "__nodeId", JS_NewInt32(ctx, id) },
        { "createLinearGradient", JS_NewCFunction(ctx, JsCreateLinearGradient, "createLinearGradient", 4) },
        { "createRadialGradient", JS_NewCFunction(ctx, JsCreateRadialGradient, "createRadialGradient", 6) },
        { "fillRect", JS_NewCFunction(ctx, JsFillRect, "fillRect", 4) },
        { "strokeRect", JS_NewCFunction(ctx, JsStrokeRect, "strokeRect", 4) },
        { "clearRect", JS_NewCFunction(ctx, JsClearRect, "clearRect", 4) },
        { "fillText", JS_NewCFunction(ctx, JsFillText, "fillText", 3) },
        { "strokeText", JS_NewCFunction(ctx, JsStrokeText, "strokeText", 3) },
        { "measureText", JS_NewCFunction(ctx, JsMeasureText, "measureText", 1) },
        { "moveTo", JS_NewCFunction(ctx, JsMoveTo, "moveTo", 2) },
        { "lineTo", JS_NewCFunction(ctx, JsLineTo, "lineTo", 2) },
        { "bezierCurveTo", JS_NewCFunction(ctx, JsBezierCurveTo, "bezierCurveTo", 6) },
        { "quadraticCurveTo", JS_NewCFunction(ctx, JsQuadraticCurveTo, "quadraticCurveTo", 4) },
        { "arcTo", JS_NewCFunction(ctx, JsArcTo, "arcTo", 5) },
        { "arc", JS_NewCFunction(ctx, JsArc, "arc", 6) },
        { "ellipse", JS_NewCFunction(ctx, JsEllipse, "ellipse", 4) },
        { "fill", JS_NewCFunction(ctx, JsFill, "fill", 0) },
        { "stroke", JS_NewCFunction(ctx, JsStroke, "stroke", 0) },
        { "clip", JS_NewCFunction(ctx, JsClip, "clip", 0) },
        { "rect", JS_NewCFunction(ctx, JsRect, "rect", 4) },
        { "beginPath", JS_NewCFunction(ctx, JsBeginPath, "beginPath", 0) },
        { "closePath", JS_NewCFunction(ctx, JsClosePath, "closePath", 0) },
        { "restore", JS_NewCFunction(ctx, JsRestore, "restore", 0) },
        { "save", JS_NewCFunction(ctx, JsSave, "save", 0) },
        { "rotate", JS_NewCFunction(ctx, JsRotate, "rotate", 1) },
        { "scale", JS_NewCFunction(ctx, JsScale, "scale", 2) },
        { "setTransform", JS_NewCFunction(ctx, JsSetTransform, "setTransform", 6) },
        { "transform", JS_NewCFunction(ctx, JsTransform, "transform", 6) },
        { "translate", JS_NewCFunction(ctx, JsTranslate, "translate", 2) },
        { "getLineDash", JS_NewCFunction(ctx, JsGetLineDash, "getLineDash", 0) },
        { "setLineDash", JS_NewCFunction(ctx, JsSetLineDash, "setLineDash", 1) },
        { "drawImage", JS_NewCFunction(ctx, JsDrawImage, "drawImage", 9) },
        { "createPath2D", JS_NewCFunction(ctx, JsCreatePath2D, "createPath2D", 0) },
        { "createPattern", JS_NewCFunction(ctx, JsCreatePattern, "createPattern", 2) },
        { "createImageData", JS_NewCFunction(ctx, JsCreateImageData, "createImageData", 2) },
        { "putImageData", JS_NewCFunction(ctx, JsPutImageData, "putImageData", 7) },
        { "getImageData", JS_NewCFunction(ctx, JsGetImageData, "getImageData", 4) },
        { "getJsonData", JS_NewCFunction(ctx, JsGetJsonData, "getJsonData", 1)},
        { "transferFromImageBitmap", JS_NewCFunction(ctx, JsTransferFromImageBitmap, "transferFromImageBitmap", 1)},
    };

    for (const auto& iter : contextTable) {
        JS_SetPropertyStr(ctx, renderContext_, iter.first, iter.second);
    }
    // getter and setter
    JS_SetPropertyFunctionList(ctx, renderContext_, JS_ANIMATION_FUNCS, countof(JS_ANIMATION_FUNCS));
    JsSetAntiAlias(ctx, id, args);
}

void CanvasBridge::JSHandleWebglContext(
    JSContext* ctx, NodeId id, const std::string& args, JsEngineInstance* engine,
    CanvasRenderContextBase*& canvasRenderContext)
{
    LOGD("CanvasBridge::HandleWebglContext");
    if (engine == nullptr) {
        LOGE("engine is null");
        return;
    }
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    QuickJSNativeEngine* nativeEngine = static_cast<QjsEngineInstance*>(engine)->GetQuickJSNativeEngine();
    if (nativeEngine == nullptr) {
        LOGE("nativeEngine is null");
        return;
    }
    std::string moduleName(CANVAS_WEBGL_SO);
    std::string pluginId(std::to_string(id));
    renderContext_ = nativeEngine->GetModuleFromName(
        moduleName, false, pluginId, args, WEBGL_RENDER_CONTEXT_NAME, reinterpret_cast<void**>(&canvasRenderContext));
    if (!canvasRenderContext) {
        LOGE("CanvasBridge invalid canvasRenderContext");
        return;
    }

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (!instance) {
        return;
    }
    auto page = instance->GetRunningPage();
    if (!page) {
        LOGE("page is null.");
        return;
    }
    auto task = [canvasRenderContext, page, id]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            LOGE("DOMCanvas is null.");
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto pool = paintChild->GetTaskPool();
        if (!pool) {
            LOGE("TaskPool is null.");
            return;
        }
        pool->WebGLInit(canvasRenderContext);
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);

    canvasRenderContext->Init();

    auto onWebGLUpdateCallback = [ctx, id]() {
        auto task = [](const RefPtr<CanvasTaskPool>& pool) {
            pool->WebGLUpdate();
        };
        PushTaskToPageById(ctx, id, task);
    };
    canvasRenderContext->SetUpdateCallback(onWebGLUpdateCallback);
#endif
}

void CanvasBridge::HandleToDataURL(JSContext* ctx, NodeId id, const std::string& args)
{
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        LOGE("ToDataURL failed, page is null.");
        return;
    }
    std::string dataUrl;
    auto task = [id, page, args, &dataUrl]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            LOGE("ToDataURL failed, DOMCanvas is null.");
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto pool = paintChild->GetTaskPool();
        if (!pool) {
            LOGE("ToDataURL failed, TaskPool is null.");
            return;
        }
        dataUrl = pool->ToDataURL(args);
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);
    dataURL_ = JS_NewString(ctx, dataUrl.c_str());
}

void CanvasBridge::JsSetAntiAlias(JSContext* ctx, NodeId id, const std::string& args)
{
    bool isEnabled = args.find("\"antialias\":true") != std::string::npos;
    auto task = [isEnabled](const RefPtr<CanvasTaskPool>& pool) { pool->SetAntiAlias(isEnabled); };
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        return;
    }
    auto page = instance->GetRunningPage();
    if (!page) {
        return;
    }
    page->PushCommand(command);
    if (page->CheckPageCreated()) {
        instance->GetDelegate()->TriggerPageUpdate(page->GetPageId());
    }
}

JSValue CanvasBridge::JsCreateLinearGradient(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsCreateLinearGradient");
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

JSValue CanvasBridge::JsCreateRadialGradient(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsCreateRadialGradient");
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

JSValue CanvasBridge::JsAddColorStop(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

Gradient CanvasBridge::GetGradient(JSContext* ctx, JSValueConst value)
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

JSValue CanvasBridge::JsTransferFromImageBitmap(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }

    int32_t bridgeId = GetCurrentBridgeId(ctx, argv[0]);

    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    if (!instance) {
        return JS_NULL;
    }
    auto page = instance->GetRunningPage();
    if (page) {
        RefPtr<OffscreenCanvasBridge> bridge = AceType::DynamicCast<OffscreenCanvasBridge>(
            page->GetOffscreenCanvasBridgeById(bridgeId));
        if (bridge) {
            RefPtr<OffscreenCanvas> offscreenCanvas = bridge->offscreenCanvas_;
            auto task = [offscreenCanvas](const RefPtr<CanvasTaskPool>& pool) {
                pool->TransferFromImageBitmap(offscreenCanvas);
            };
            PushTaskToPage(ctx, value, std::move(task));
        }
    }
    return JS_NULL;
}

JSValue CanvasBridge::JsHandleRect(
    JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv, FunctionCode functionCode)
{
    LOGD("CanvasBridge::JsHandleRect function code: %{public}d", functionCode);
    // 4 parameters: rect(x, y, width, height)
    if ((!argv) || (argc != 4)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    Rect rect = GetJsRectParam(ctx, argc, argv);
    auto task = [rect, functionCode](const RefPtr<CanvasTaskPool>& pool) {
        if (functionCode == FunctionCode::FILL_RECT) {
            pool->FillRect(rect);
        } else if (functionCode == FunctionCode::STROKE_RECT) {
            pool->StrokeRect(rect);
        } else if (functionCode == FunctionCode::CLEAR_RECT) {
            pool->ClearRect(rect);
        } else {
            LOGE("not supported");
        }
    };
    PushTaskToPage(ctx, value, std::move(task));
    return JS_NULL;
}

JSValue CanvasBridge::JsFillRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsHandleRect(ctx, value, argc, argv, FunctionCode::FILL_RECT);
}

JSValue CanvasBridge::JsStrokeRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsHandleRect(ctx, value, argc, argv, FunctionCode::STROKE_RECT);
}

JSValue CanvasBridge::JsClearRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsHandleRect(ctx, value, argc, argv, FunctionCode::CLEAR_RECT);
}

JSValue CanvasBridge::JsHandleText(
    JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv, FunctionCode functionCode)
{
    LOGD("CanvasBridge::JsHandleText function code: %{public}d", functionCode);
    // 3 parameters: fillText(text, x, y, [maxWidth])
    if ((!argv) || (argc != 3)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString arg(ctx, argv[0]);
    if (!arg.get()) {
        return JS_NULL;
    }
    std::string text = arg.get();
    double x = GetJsDoubleVal(ctx, argv[1]);
    double y = GetJsDoubleVal(ctx, argv[2]);
    auto task = [text, x, y, functionCode](const RefPtr<CanvasTaskPool>& pool) {
        if (functionCode == FunctionCode::FILL_TEXT) {
            pool->FillText(text, Offset(x, y));
        } else if (functionCode == FunctionCode::STROKE_TEXT) {
            pool->StrokeText(text, Offset(x, y));
        } else {
            LOGE("not supported");
        }
    };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsFillText(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsHandleText(ctx, value, argc, argv, FunctionCode::FILL_TEXT);
}

JSValue CanvasBridge::JsStrokeText(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsHandleText(ctx, value, argc, argv, FunctionCode::STROKE_TEXT);
}

JSValue CanvasBridge::JsMeasureText(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    NodeId id = GetCurrentNodeId(ctx, value);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
    }
    double width = 0.0;
    double height = 0.0;
    auto task = [&text, &textState, id, page, &width, &height]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto canvasTask = paintChild->GetTaskPool();
        if (!canvasTask) {
            return;
        }
        width = canvasTask->MeasureText(text, textState);
        height = canvasTask->MeasureTextHeight(text, textState);
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);
    JSValue textMetrics = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, textMetrics, "width", JS_NewFloat64(ctx, width));
    JS_SetPropertyStr(ctx, textMetrics, "height", JS_NewFloat64(ctx, height));
    return textMetrics;
}

JSValue CanvasBridge::JsBeginPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("Js Begin path");
    // 0 parameter: beginPath()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->BeginPath(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsClosePath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("js close path");
    // 0 parameter: closePath()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->ClosePath(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsMoveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsMoveTo");
    // 2 parameters: moveTo(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->MoveTo(x, y); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsLineTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsLineTo");
    // 2 parameters: lineTo(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->LineTo(x, y); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsBezierCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->BezierCurveTo(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsQuadraticCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->QuadraticCurveTo(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsArc(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Arc(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsArcTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->ArcTo(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsEllipse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Ellipse(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsRect");
    Rect rect = GetJsRectParam(ctx, argc, argv);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->AddRect(rect); };
    PushTaskToPage(ctx, value, std::move(task));
    return JS_NULL;
}

JSValue CanvasBridge::JsFill(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsFill");
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Fill(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsStroke(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
        auto task = [path](const RefPtr<CanvasTaskPool>& pool) { pool->Stroke(path); };
        PushTaskToPage(ctx, value, task);
        return JS_NULL;
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Stroke(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsClip(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("JsClip");
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Clip(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsRestore(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsRestore");
    // 0 parameter: restore()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Restore(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsSave(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 0 parameter: save()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Save(); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsRotate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsRotate");
    // 1 parameter: rotate(angle)
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double angle = GetJsDoubleVal(ctx, argv[0]);
    auto task = [angle](const RefPtr<CanvasTaskPool>& pool) { pool->Rotate(angle); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsScale(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsScale");
    // 2 parameters: scale(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->Scale(x, y); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsSetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsSetTransform");
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->SetTransform(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsTransform");
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Transform(param); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsTranslate(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsTranslate");
    // 2 parameters: translate(x, y)
    if ((!argv) || (argc != 2)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    double x = GetJsDoubleVal(ctx, argv[0]);
    double y = GetJsDoubleVal(ctx, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->Translate(x, y); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsSetLineDash(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsSetLineDash");
    // 1 parameter: setLineDash(segments)
    if ((!argv) || (argc != 1)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    ScopedString dash(ctx, argv[0]);
    JS_SetPropertyStr(ctx, value, "lineDash", JS_NewString(ctx, dash.get()));
    auto segments = GetJsDashValue(ctx, argv[0]);
    auto task = [segments](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineDash(segments); };
    PushTaskToPage(ctx, value, task);
    return JS_NULL;
}

JSValue CanvasBridge::JsGetLineDash(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 0 parameter: getLineDash()
    if ((!argv) || (argc != 0)) {
        LOGE("argc error, argc = %{private}d", argc);
        return JS_NULL;
    }
    auto val = JS_GetPropertyStr(ctx, value, "lineDash");
    return val;
}

void CanvasBridge::ParseDomImage(JSContext* ctx, JSValueConst value, double& width, double& height, std::string& src)
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

JSValue CanvasBridge::JsDrawImage(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    auto task = [image, width, height](const RefPtr<CanvasTaskPool>& pool) { pool->DrawImage(image, width, height); };
    PushTaskToPage(ctx, value, task);

    JS_FreeValue(ctx, src);
    return JS_NULL;
}

JSValue CanvasBridge::JsCreatePath2D(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    JSValue path2D = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, path2D, "__type", JS_NewString(ctx, "path2d"));
    JS_SetPropertyStr(ctx, path2D, "__id", JS_NewInt32(ctx, path2dCount_));
    JS_SetPropertyStr(ctx, path2D, "addPath", JS_NewCFunction(ctx, JsPath2DAddPath, "addPath", 1));
    JS_SetPropertyStr(ctx, path2D, "setTransform", JS_NewCFunction(ctx, JsPath2DSetTransform, "setTransform", 6));
    JS_SetPropertyStr(ctx, path2D, "moveTo", JS_NewCFunction(ctx, JsPath2DMoveTo, "moveTo", 2));
    JS_SetPropertyStr(ctx, path2D, "lineTo", JS_NewCFunction(ctx, JsPath2DLineTo, "addColorStop", 2));
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

JSValue CanvasBridge::JsPath2DAddPath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DSetTransform(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DMoveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DLineTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DArc(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DArcTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DQuadraticCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DBezierCurveTo(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DEllipse(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DRect(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPath2DClosePath(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

RefPtr<CanvasPath2D> CanvasBridge::JsMakePath2D(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

RefPtr<CanvasPath2D> CanvasBridge::GetPath2D(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        return nullptr;
    }
    JS_FreeValue(ctx, nodeId);
    return path2Ds_[id];
}

JSValue CanvasBridge::JsCreatePattern(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    LOGD("CanvasBridge::JsCreatePattern");
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

Pattern CanvasBridge::GetPattern(JSContext* ctx, JSValueConst value)
{
    int32_t id = 0;
    JSValue nodeId = JS_GetPropertyStr(ctx, value, "__id");
    if (JS_IsInteger(nodeId) && (JS_ToInt32(ctx, &id, nodeId)) < 0) {
        return Pattern();
    }
    JS_FreeValue(ctx, nodeId);
    return pattern_[id];
}

JSValue CanvasBridge::JsCreateImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

JSValue CanvasBridge::JsPutImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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

    auto task = [imageData](const RefPtr<CanvasTaskPool>& pool) { pool->PutImageData(imageData); };
    PushTaskToPage(ctx, value, task);

    JS_FreeValue(ctx, jsWidth);
    JS_FreeValue(ctx, jsHeight);
    return JS_NULL;
}

void CanvasBridge::ParseImageData(
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

JSValue CanvasBridge::JsGetImageData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
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
    NodeId id = GetCurrentNodeId(ctx, value);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
    }
    std::unique_ptr<ImageData> data;
    auto task = [id, page, &rect, &data]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto canvasTask = paintChild->GetTaskPool();
        if (!canvasTask) {
            return;
        }
        data = canvasTask->GetImageData(rect.Left(), rect.Top(), rect.Width(), rect.Height());
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);

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

JSValue CanvasBridge::JsGetJsonData(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // 1 parameters: JsGetJsonData(path)
    if (!argv || (argc != 1)) {
        return JS_NULL;
    }
    NodeId id = GetCurrentNodeId(ctx, value);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
    }

    std::string path = JS_ToCString(ctx, argv[0]);
    std::string jsonData;
    auto task = [id, page, path, &jsonData]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto canvasTask = paintChild->GetTaskPool();
        if (!canvasTask) {
            return;
        }
        jsonData = canvasTask->GetJsonData(path);
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);

    return JS_NewString(ctx, jsonData.c_str());
}

JSValue CanvasBridge::JsFillStyleGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__fillStyle");
}

JSValue CanvasBridge::JsHandleStyleSetter(
    JSContext* ctx, JSValueConst value, JSValueConst proto, FunctionCode functionCode)
{
    if (JS_IsString(proto)) {
        auto colorStr = JS_ToCString(ctx, proto);
        auto color = Color::FromString(colorStr);
        auto task = [color, functionCode](const RefPtr<CanvasTaskPool>& pool) {
            if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
                pool->UpdateStrokeColor(color);
            } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
                pool->UpdateFillColor(color);
            } else {
                LOGW("unsupported function for update color.");
            }
        };
        PushTaskToPage(ctx, value, task);
        JS_FreeCString(ctx, colorStr);
    } else {
        JSValue typeVal = JS_GetPropertyStr(ctx, proto, "__type");
        ScopedString type(ctx, typeVal);
        if (std::strcmp(type.get(), "gradient") == 0) {
            auto gradient = GetGradient(ctx, proto);
            auto task = [gradient, functionCode](const RefPtr<CanvasTaskPool>& pool) {
                if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
                    pool->UpdateStrokeGradient(gradient);
                } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
                    pool->UpdateFillGradient(gradient);
                } else {
                    LOGW("unsupported function for update gradient.");
                }
            };
            PushTaskToPage(ctx, value, task);
        } else if (std::strcmp(type.get(), "pattern") == 0) {
            auto pattern = GetPattern(ctx, proto);
            auto task = [pattern, functionCode](const RefPtr<CanvasTaskPool>& pool) {
                if (functionCode == FunctionCode::STROKE_STYLE_SETTER) {
                    pool->UpdateStrokePattern(pattern);
                } else if (functionCode == FunctionCode::FILL_STYLE_SETTER) {
                    pool->UpdateFillPattern(pattern);
                } else {
                    LOGW("No such type for update pattern.");
                }
            };
            PushTaskToPage(ctx, value, task);
        } else {
            LOGW("unsupported function for stroke style.");
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

JSValue CanvasBridge::JsFillStyleSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    return JsHandleStyleSetter(ctx, value, proto, FunctionCode::FILL_STYLE_SETTER);
}

JSValue CanvasBridge::JsStrokeStyleGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__strokeStyle");
}

JSValue CanvasBridge::JsStrokeStyleSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    return JsHandleStyleSetter(ctx, value, proto, FunctionCode::STROKE_STYLE_SETTER);
}

JSValue CanvasBridge::JsLineCapGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineCap");
}

JSValue CanvasBridge::JsLineCapSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString capStr(ctx, proto);
    static const LinearMapNode<LineCapStyle> lineCapTable[] = {
        { "butt", LineCapStyle::BUTT },
        { "round", LineCapStyle::ROUND },
        { "square", LineCapStyle::SQUARE },
    };
    auto lineCap = ConvertStrToEnum(capStr.get(), lineCapTable, ArraySize(lineCapTable), LineCapStyle::BUTT);
    auto task = [lineCap](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineCap(lineCap); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__lineCap", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsLineJoinGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineJoin");
}

JSValue CanvasBridge::JsLineJoinSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString joinStr(ctx, proto);
    static const LinearMapNode<LineJoinStyle> lineJoinTable[3] = {
        { "bevel", LineJoinStyle::BEVEL },
        { "miter", LineJoinStyle::MITER },
        { "round", LineJoinStyle::ROUND },
    };
    auto lineJoin = ConvertStrToEnum(joinStr.get(), lineJoinTable, ArraySize(lineJoinTable), LineJoinStyle::MITER);
    auto task = [lineJoin](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineJoin(lineJoin); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__lineJoin", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsMiterLimitGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__miterLimit");
}

JSValue CanvasBridge::JsMiterLimitSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double limit = GetJsDoubleVal(ctx, proto);
    auto task = [limit](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateMiterLimit(limit); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__miterLimit", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsLineWidthGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineWidth");
}

JSValue CanvasBridge::JsLineWidthSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double lineWidth = GetJsDoubleVal(ctx, proto);
    auto task = [lineWidth](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineWidth(lineWidth); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__lineWidth", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsTextAlignGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__textAlign");
}

JSValue CanvasBridge::JsTextAlignSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString alignStr(ctx, proto);
    auto align = ConvertStrToTextAlign(alignStr.get());
    auto task = [align](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateTextAlign(align); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__textAlign", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsTextBaselineGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__textBaseline");
}

JSValue CanvasBridge::JsTextBaselineSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString baselineStr(ctx, proto);
    auto baseline =
        ConvertStrToEnum(baselineStr.get(), BASELINE_TABLE, ArraySize(BASELINE_TABLE), TextBaseline::ALPHABETIC);
    auto task = [baseline](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateTextBaseline(baseline); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__textBaseline", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsFontGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__font");
}

JSValue CanvasBridge::JsFontSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString fontStr(ctx, proto);
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(fontStr.get(), ' ', fontProps);
    bool updateFontStyle = false;
    for (const auto& fontProp : fontProps) {
        if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            auto weight = ConvertStrToFontWeight(fontProp);
            auto task = [weight](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontWeight(weight); };
            PushTaskToPage(ctx, value, task);
        } else if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            updateFontStyle = true;
            auto fontStyle = ConvertStrToFontStyle(fontProp);
            auto task = [fontStyle](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontStyle(fontStyle); };
            PushTaskToPage(ctx, value, task);
        } else if (FONT_FAMILIES.find(fontProp) != FONT_FAMILIES.end()) {
            auto families = ConvertStrToFontFamilies(fontProp);
            auto task = [families](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontFamilies(families); };
            PushTaskToPage(ctx, value, task);
        } else if (fontProp.find("px") != std::string::npos) {
            std::string fontSize = fontProp.substr(0, fontProp.size() - 2);
            auto size = Dimension(StringToDouble(fontProp));
            auto task = [size](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontSize(size); };
            PushTaskToPage(ctx, value, task);
        } else {
            LOGW("parse text error");
        }
    }
    if (!updateFontStyle) {
        auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontStyle(FontStyle::NORMAL); };
        PushTaskToPage(ctx, value, task);
    }
    JS_SetPropertyStr(ctx, value, "__font", JS_DupValue(ctx, proto));
    return JS_UNDEFINED;
}

JSValue CanvasBridge::JsAlphaGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__globalAlpha");
}

JSValue CanvasBridge::JsAlphaSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double alpha = GetJsDoubleVal(ctx, proto);
    auto task = [alpha](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateGlobalAlpha(alpha); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__globalAlpha", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsCompositeOperationGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__globalCompositeOperation");
}

JSValue CanvasBridge::JsCompositeOperationSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
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
    auto task = [type](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateCompositeOperation(type); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__globalCompositeOperation", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsLineDashOffsetGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__lineDash");
}

JSValue CanvasBridge::JsLineDashOffsetSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double dashoffset = GetJsDoubleVal(ctx, proto);
    auto task = [dashoffset](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineDashOffset(dashoffset); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__lineDash", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsShadowBlurGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowBlur");
}

JSValue CanvasBridge::JsShadowBlurSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double blur = GetJsDoubleVal(ctx, proto);
    auto task = [blur](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowBlur(blur); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__shadowBlur", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsShadowColorGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowColor");
}

JSValue CanvasBridge::JsShadowColorSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString colorStr(ctx, proto);
    auto color = Color::FromString(colorStr.get());
    auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowColor(color); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__shadowColor", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsShadowOffsetXGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowOffsetX");
}

JSValue CanvasBridge::JsShadowOffsetXSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double offsetX = GetJsDoubleVal(ctx, proto);
    auto task = [offsetX](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowOffsetX(offsetX); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__shadowOffsetX", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsShadowOffsetYGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__shadowOffsetY");
}

JSValue CanvasBridge::JsShadowOffsetYSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    double offsetY = GetJsDoubleVal(ctx, proto);
    auto task = [offsetY](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowOffsetY(offsetY); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__shadowOffsetY", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsSmoothingEnabledGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__imageSmoothingEnabled");
}

JSValue CanvasBridge::JsSmoothingEnabledSetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString enabledStr(ctx, proto);
    if (!enabledStr.get()) {
        return JS_NULL;
    }
    bool enabled = std::string(enabledStr.get()) == "true";
    auto task = [enabled](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateSmoothingEnabled(enabled); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__imageSmoothingEnabled", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsSmoothingQualityGetter(JSContext* ctx, JSValueConst value)
{
    return JS_GetPropertyStr(ctx, value, "__imageSmoothingQuality");
}

JSValue CanvasBridge::JsSmoothingQualitySetter(JSContext* ctx, JSValueConst value, JSValueConst proto)
{
    ScopedString qualityStr(ctx, proto);
    if (!qualityStr.get()) {
        return JS_NULL;
    }
    std::string quality = qualityStr.get();
    if (QUALITY_TYPE.find(quality) == QUALITY_TYPE.end()) {
        return JS_NULL;
    }
    auto task = [quality](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateSmoothingQuality(quality); };
    PushTaskToPage(ctx, value, task);
    JS_SetPropertyStr(ctx, value, "__imageSmoothingQuality", JS_DupValue(ctx, proto));
    return JS_NULL;
}

JSValue CanvasBridge::JsWidthGetter(JSContext* ctx, JSValueConst value)
{
    NodeId id = GetCurrentNodeId(ctx, value);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
    }

    double width = 0.0;
    auto task = [id, page, &width]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        if (!paintChild) {
            return;
        }
        auto canvasTask = paintChild->GetTaskPool();
        if (!canvasTask) {
            return;
        }
        width = canvasTask->GetWidth();
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);

    return JS_NewFloat64(ctx, width);
}

JSValue CanvasBridge::JsHeightGetter(JSContext* ctx, JSValueConst value)
{
    NodeId id = GetCurrentNodeId(ctx, value);
    auto instance = static_cast<QjsEngineInstance*>(JS_GetContextOpaque(ctx));
    auto page = instance->GetRunningPage();
    if (!page) {
        return JS_NULL;
    }

    double height = 0.0;
    auto task = [id, page, &height]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        if (!paintChild) {
            return;
        }
        auto canvasTask = paintChild->GetTaskPool();
        if (!canvasTask) {
            return;
        }
        height = canvasTask->GetHeight();
    };
    instance->GetDelegate()->PostSyncTaskToPage(task);

    return JS_NewFloat64(ctx, height);
}

} // namespace OHOS::Ace::Framework
