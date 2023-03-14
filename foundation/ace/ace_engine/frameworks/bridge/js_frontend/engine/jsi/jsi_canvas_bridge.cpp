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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_canvas_bridge.h"

#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"

#include "core/components/custom_paint/offscreen_canvas.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/engine/jsi/ark_js_value.h"
#include "frameworks/bridge/js_frontend/engine/jsi/jsi_offscreen_canvas_bridge.h"

#ifdef PIXEL_MAP_SUPPORTED
#include "pixel_map.h"
#include "pixel_map_napi.h"
#endif

namespace OHOS::Ace::Framework {
namespace {

constexpr char CANVAS_TYPE_WEBGL[] = "webgl";
constexpr char CANVAS_TYPE_WEBGL2[] = "webgl2";
constexpr char CANVAS_WEBGL_SO[] = "webglnapi";

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
RefPtr<PixelMap> CreatePixelMapFromNapiValue(const shared_ptr<JsRuntime>& runtime, shared_ptr<JsValue> jsValue)
{
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE(" engine is null.");
        return nullptr;
    }

    auto nativeEngine = static_cast<ArkNativeEngine*>(engine->GetNativeEngine());
    if (!nativeEngine) {
        LOGE("NativeEngine is null");
        return nullptr;
    }

    shared_ptr<ArkJSValue> arkJsValue = std::static_pointer_cast<ArkJSValue>(jsValue);
    if (!arkJsValue) {
        LOGE("arkJsValue is null.");
        return nullptr;
    }
    shared_ptr<ArkJSRuntime> arkRuntime = std::static_pointer_cast<ArkJSRuntime>(runtime);
    if (!arkRuntime) {
        LOGE("arkRuntime is null");
        return nullptr;
    }

    JSValueWrapper valueWrapper = arkJsValue->GetValue(arkRuntime);
    NativeValue* nativeValue = nativeEngine->ValueToNativeValue(valueWrapper);

    PixelMapNapiEntry pixelMapNapiEntry = JsEngine::GetPixelMapNapiEntry();
    if (!pixelMapNapiEntry) {
        LOGE("pixelMapNapiEntry is null");
    }
    void* pixmapPtrAddr = pixelMapNapiEntry(
        reinterpret_cast<napi_env>(nativeEngine), reinterpret_cast<napi_value>(nativeValue));
    if (pixmapPtrAddr == nullptr) {
        LOGE(" Failed to get pixmap pointer");
        return nullptr;
    }
    return PixelMap::CreatePixelMap(pixmapPtrAddr);
}
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

inline NodeId GetCurrentNodeId(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    if (!runtime || !value) {
        LOGE("runtime or value is null.");
        return 0;
    }
    NodeId id = 0;
    auto nodeId = value->GetProperty(runtime, "__nodeId");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    return id < 0 ? 0 : id;
}

void PushTaskToPageById(const shared_ptr<JsRuntime>& runtime, NodeId id,
    const std::function<void(const RefPtr<CanvasTaskPool>&)>& task)
{
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    // push command
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("engine is null.");
        return;
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("page is null.");
        return;
    }
    page->PushCommand(command);
}
void PushTaskToPage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::function<void(const RefPtr<CanvasTaskPool>&)>& task)
{
    if (!runtime || !value) {
        LOGE("runtime or value is null.");
        return;
    }
    // get node id
    NodeId id = GetCurrentNodeId(runtime, value);
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    // push command
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("engine is null.");
        return;
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("page is null.");
        return;
    }
    page->PushCommand(command);
}

inline PaintState JsParseTextState(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    PaintState state;
    auto fontStyle = value->GetProperty(runtime, "font");
    auto textAlign = value->GetProperty(runtime, "textAlign");
    auto textBaseline = value->GetProperty(runtime, "textBaseline");

    // parse font styles
    auto alignStr = textAlign->ToString(runtime);
    auto baselineStr = textBaseline->ToString(runtime);
    auto fontStr = fontStyle->ToString(runtime);
    state.SetTextAlign(ConvertStrToTextAlign(alignStr));
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
        auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontStyle(FontStyle::NORMAL); };
        PushTaskToPage(runtime, value, task);
    }
    state.SetTextStyle(style);
    return state;
}

} // namespace

int32_t JsiCanvasBridge::gradientCount_ = 0;
int32_t JsiCanvasBridge::patternCount_ = 0;
int32_t JsiCanvasBridge::path2dCount_ = 0;
std::unordered_map<int32_t, Pattern> JsiCanvasBridge::pattern_;
std::unordered_map<int32_t, Gradient> JsiCanvasBridge::gradientColors_;
std::unordered_map<int32_t, RefPtr<CanvasPath2D>> JsiCanvasBridge::path2Ds_;

JsiCanvasBridge::~JsiCanvasBridge()
{
    if (webglRenderContext_) {
        delete webglRenderContext_;
        webglRenderContext_ = nullptr;
    }
    if (webgl2RenderContext_) {
        delete webgl2RenderContext_;
        webgl2RenderContext_ = nullptr;
    }
}

void JsiCanvasBridge::HandleJsContext(const shared_ptr<JsRuntime>& runtime, NodeId id, const std::string& args)
{
    LOGD("JsiCanvasBridge::HandleJsContext");
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (argsValue && argsValue->IsArray() && argsValue->GetArraySize() > 0) {
        auto typeArg = argsValue->GetArrayItem(0);
        if (typeArg && typeArg->IsString()) {
            std::string type = typeArg->GetString();
            if (type == std::string(CANVAS_TYPE_WEBGL)) {
                HandleWebglContext(runtime, id, args, webglRenderContext_);
                return;
            } else if (type == std::string(CANVAS_TYPE_WEBGL2)) {
                HandleWebglContext(runtime, id, args, webgl2RenderContext_);
                return;
            }
        }
    }

    renderContext_ = runtime->NewObject();
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
#ifdef PIXEL_MAP_SUPPORTED
        { "getPixelMap", JsGetPixelMap },
#endif
        { "getJsonData", JsGetJsonData },
        { "transferFromImageBitmap", JsTransferFromImageBitmap },
        { "drawBitmapMesh", JsDrawBitmapMesh },
    };
    renderContext_->SetProperty(runtime, "__nodeId", runtime->NewInt32(id));
    for (const auto& iter : contextTable) {
        renderContext_->SetProperty(runtime, iter.first, runtime->NewFunction(iter.second));
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
        { "width", JsWidthGetter, nullptr },
        { "height", JsHeightGetter, nullptr }
    };
    for (const auto& item : animationFuncs) {
        auto getterTempl = runtime->NewFunction(std::get<1>(item));
        auto setterTempl = runtime->NewFunction(std::get<2>(item));
        bool ret = renderContext_->SetAccessorProperty(runtime, std::get<0>(item), getterTempl, setterTempl);
        if (!ret) {
            LOGE("Canvas set accessor property failed., name: %{public}s", std::get<0>(item).c_str());
        }
    }
    JsSetAntiAlias(runtime, id, args);
}

void JsiCanvasBridge::HandleWebglContext(const shared_ptr<JsRuntime>& runtime,
    NodeId id, const std::string& args, CanvasRenderContextBase*& canvasRenderContext)
{
    LOGD("JsiCanvasBridge::HandleWebglContext");
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("engine is null.");
        return;
    }

    auto nativeEngine = static_cast<ArkNativeEngine*>(engine->GetNativeEngine());
    if (!nativeEngine) {
        LOGE("NativeEngine is null");
        return;
    }

    std::string moduleName(CANVAS_WEBGL_SO);
    std::string pluginId(std::to_string(id));
    auto arkObjectRef = nativeEngine->GetModuleFromName(
        moduleName, false, pluginId, args, WEBGL_RENDER_CONTEXT_NAME, reinterpret_cast<void**>(&canvasRenderContext));
    if (!canvasRenderContext) {
        LOGE("CanvasBridge invalid canvasRenderContext");
        return;
    }

    renderContext_ = runtime->NewObject();
    auto renderContext = std::static_pointer_cast<ArkJSValue>(renderContext_);
    shared_ptr<ArkJSRuntime> pandaRuntime = std::static_pointer_cast<ArkJSRuntime>(runtime);
    LocalScope scope(pandaRuntime->GetEcmaVm());
    Local<ObjectRef> obj = arkObjectRef->ToObject(pandaRuntime->GetEcmaVm());
    if (obj.CheckException()) {
        LOGE("Get local object failed.");
        return;
    }
    renderContext->SetValue(pandaRuntime, obj);

    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("page is null.");
        return;
    }

    auto task = [canvasRenderContext, page, id]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto pool = paintChild->GetTaskPool();
        if (!pool) {
            return;
        }
        pool->WebGLInit(canvasRenderContext);
    };

    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("ToDataURL failed. delegate is null.");
        return;
    }

    delegate->PostSyncTaskToPage(task);

    canvasRenderContext->Init();

    auto onWebGLUpdateCallback = [runtime, id]() {
        auto task = [](const RefPtr<CanvasTaskPool>& pool) {
            pool->WebGLUpdate();
        };
        PushTaskToPageById(runtime, id, task);
    };
    canvasRenderContext->SetUpdateCallback(onWebGLUpdateCallback);
}

void JsiCanvasBridge::HandleToDataURL(const shared_ptr<JsRuntime>& runtime, NodeId id, const std::string& args)
{
    if (!runtime) {
        LOGE("HandleToDataURL failed. runtime is null.");
        return;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("HandleToDataURL failed. engine is null.");
        return;
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("HandleToDataURL failed. page is null.");
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("ToDataURL failed. delegate is null.");
        return;
    }
    delegate->PostSyncTaskToPage(task);
    dataURL_ = runtime->NewString(dataUrl);
}

void JsiCanvasBridge::JsSetAntiAlias(const shared_ptr<JsRuntime>& runtime, NodeId id, const std::string& args)
{
    if (!runtime) {
        LOGE("JsSetAntiAlias failed. runtime is null.");
        return;
    }
    bool isEnabled = args.find("\"antialias\":true") != std::string::npos;
    auto task = [isEnabled](const RefPtr<CanvasTaskPool>& pool) { pool->SetAntiAlias(isEnabled); };
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsSetAntiAlias failed. engine is null.");
        return;
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsSetAntiAlias failed. page is null.");
        return;
    }
    page->PushCommand(command);
    auto delegate = engine->GetFrontendDelegate();
    if (page->CheckPageCreated() && delegate) {
        delegate->TriggerPageUpdate(page->GetPageId());
    }
}

shared_ptr<JsValue> JsiCanvasBridge::JsCreateLinearGradient(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsCreateLinearGradient");
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

shared_ptr<JsValue> JsiCanvasBridge::JsCreateRadialGradient(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::CreateRadialGradient");
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

shared_ptr<JsValue> JsiCanvasBridge::JsAddColorStop(const shared_ptr<JsRuntime>& runtime,
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

Gradient JsiCanvasBridge::GetGradient(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
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

shared_ptr<JsValue> JsiCanvasBridge::JsFillRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsFillRect");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->FillRect(rect); };
    PushTaskToPage(runtime, value, std::move(task));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsStrokeRect(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsStrokeRect");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->StrokeRect(rect); };
    PushTaskToPage(runtime, value, std::move(task));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsClearRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsClearRect");
    if (argc != 4) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->ClearRect(rect); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsFillText(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsFillText");
    if (argc != 3) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    if (!argv[0]) {
        return runtime->NewUndefined();
    }
    auto text = argv[0]->ToString(runtime);
    double x = GetJsDoubleVal(runtime, argv[1]);
    double y = GetJsDoubleVal(runtime, argv[2]);
    auto task = [text, x, y](const RefPtr<CanvasTaskPool>& pool) { pool->FillText(text, Offset(x, y)); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsStrokeText(const shared_ptr<JsRuntime>& runtime,
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
    auto text = argv[0]->ToString(runtime);
    double x = GetJsDoubleVal(runtime, argv[1]);
    double y = GetJsDoubleVal(runtime, argv[2]);
    auto task = [text, x, y](const RefPtr<CanvasTaskPool>& pool) { pool->StrokeText(text, Offset(x, y)); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsMeasureText(const shared_ptr<JsRuntime>& runtime,
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
    NodeId id = GetCurrentNodeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsMeasureText failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsMeasureText failed. page is null.");
        return runtime->NewUndefined();
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsMeasureText failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);
    auto textMetrics = runtime->NewObject();
    textMetrics->SetProperty(runtime, "width", runtime->NewNumber(width));
    textMetrics->SetProperty(runtime, "height", runtime->NewNumber(height));
    return textMetrics;
}

shared_ptr<JsValue> JsiCanvasBridge::JsBeginPath(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("Js Begin path");
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->BeginPath(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsClosePath(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("js close path");
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->ClosePath(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsMoveTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsMoveTo");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->MoveTo(x, y); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsLineTo");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->LineTo(x, y); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsBezierCurveTo(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->BezierCurveTo(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsQuadraticCurveTo(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->QuadraticCurveTo(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsArc(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Arc(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsArcTo(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->ArcTo(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsEllipse(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Ellipse(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsRect(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsRect");
    Rect rect = GetJsRectParam(runtime, argc, argv);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->AddRect(rect); };
    PushTaskToPage(runtime, value, std::move(task));
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsFill(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsFill");
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Fill(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsStroke(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
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
        auto task = [path](const RefPtr<CanvasTaskPool>& pool) { pool->Stroke(path); };
        PushTaskToPage(runtime, value, task);
        return runtime->NewUndefined();
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Stroke(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsClip(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsClip");
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Clip(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsRestore(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsRestore");
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Restore(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsSave(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Save(); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsRotate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsRotate");
    double angle = GetJsDoubleVal(runtime, argv[0]);
    auto task = [angle](const RefPtr<CanvasTaskPool>& pool) { pool->Rotate(angle); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsScale(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsScale");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->Scale(x, y); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsSetTransform(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsSetTransform");
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->SetTransform(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsTransform(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsTransform");
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
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Transform(param); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsTranslate(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsTranslate");
    if (argc != 2) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    double x = GetJsDoubleVal(runtime, argv[0]);
    double y = GetJsDoubleVal(runtime, argv[1]);
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->Translate(x, y); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsSetLineDash(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsSetLineDash");
    if (argc != 1) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto dash = argv[0]->ToString(runtime);
    value->SetProperty(runtime, "lineDash", runtime->NewString(dash));
    auto segments = GetJsDashValue(runtime, argv[0]);
    auto task = [segments](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineDash(segments); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsGetLineDash(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 0) {
        LOGE("argc error, argc = %{private}d", argc);
        return runtime->NewUndefined();
    }
    auto val = value->GetProperty(runtime, "lineDash");
    return val;
}

void JsiCanvasBridge::ParseDomImage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
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

shared_ptr<JsValue> JsiCanvasBridge::JsDrawImage(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (!argv[0] || !argv[0]->IsObject(runtime)) {
        return runtime->NewUndefined();
    }

    RefPtr<PixelMap> pixelMap = nullptr;
    bool isPixelMap = false;

    CanvasImage image;
    double width = 0.0;
    double height = 0.0;
    auto src = argv[0]->GetProperty(runtime, DOM_SRC);
    if (src->IsUndefined(runtime)) {
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        pixelMap = CreatePixelMapFromNapiValue(runtime, argv[0]);
        if (!pixelMap) {
            LOGE("pixelMap is null");
        }
        isPixelMap = true;
#else
        return runtime->NewUndefined();
#endif
    } else if (!src->IsString(runtime)) {
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
    auto task = [image, width, height, isPixelMap, pixelMap](const RefPtr<CanvasTaskPool>& pool) {
        if (isPixelMap) {
            pool->DrawPixelMap(pixelMap, image);
        } else {
            pool->DrawImage(image, width, height);
        }
    };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsCreatePath2D(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DAddPath(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DSetTransform(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DMoveTo(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DLineTo(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DArc(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value,
    const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DArcTo(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DQuadraticCurveTo(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DBezierCurveTo(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DEllipse(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DRect(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPath2DClosePath(const shared_ptr<JsRuntime>& runtime,
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

RefPtr<CanvasPath2D> JsiCanvasBridge::JsMakePath2D(const shared_ptr<JsRuntime>& runtime,
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

RefPtr<CanvasPath2D> JsiCanvasBridge::GetPath2D(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
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

shared_ptr<JsValue> JsiCanvasBridge::JsCreatePattern(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    LOGD("JsiCanvasBridge::JsCreatePattern");
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

Pattern JsiCanvasBridge::GetPattern(const shared_ptr<JsRuntime>& runtime, const shared_ptr<JsValue>& value)
{
    int32_t id = -1;
    auto nodeId = value->GetProperty(runtime, "__id");
    if (nodeId && nodeId->IsInt32(runtime)) {
        id = nodeId->ToInt32(runtime);
    }
    return id < 0 ? Pattern() : pattern_[id];
}

shared_ptr<JsValue> JsiCanvasBridge::JsCreateImageData(const shared_ptr<JsRuntime>& runtime,
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

shared_ptr<JsValue> JsiCanvasBridge::JsPutImageData(const shared_ptr<JsRuntime>& runtime,
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

    auto task = [imageData](const RefPtr<CanvasTaskPool>& pool) { pool->PutImageData(imageData); };
    PushTaskToPage(runtime, value, task);
    return runtime->NewUndefined();
}

void JsiCanvasBridge::ParseImageData(const shared_ptr<JsRuntime>& runtime, const std::vector<shared_ptr<JsValue>>& argv,
    int32_t argc, std::vector<std::string>& array, ImageData& imageData)
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

shared_ptr<JsValue> JsiCanvasBridge::JsGetImageData(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 4) {
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, std::move(argv));
    NodeId id = GetCurrentNodeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetImageData failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetImageData failed. page is null.");
        return runtime->NewUndefined();
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetImageData failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);

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

shared_ptr<JsValue>  JsiCanvasBridge::JsGetPixelMap(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
#ifdef PIXEL_MAP_SUPPORTED
    // 0 Get input param
    if (argc != 4) {
        return runtime->NewUndefined();
    }
    Rect rect = GetJsRectParam(runtime, argc, std::move(argv));
    NodeId id = GetCurrentNodeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetImageData failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetImageData failed. page is null.");
        return runtime->NewUndefined();
    }
    std::unique_ptr<ImageData> imageData;
    auto task = [id, page, &rect, &imageData]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>(page->GetDomDocument()->GetDOMNodeById(id));
        if (!canvas) {
            return;
        }
        auto paintChild = AceType::DynamicCast<CustomPaintComponent>(canvas->GetSpecializedComponent());
        auto canvasTask = paintChild->GetTaskPool();
        if (!canvasTask) {
            return;
        }
        imageData = canvasTask->GetImageData(rect.Left(), rect.Top(), rect.Width(), rect.Height());
    };
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetImageData failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);

    // 1 Get data from canvas
    uint32_t final_height = static_cast<uint32_t>(imageData->dirtyHeight);
    uint32_t final_width = static_cast<uint32_t>(imageData->dirtyWidth);
    uint32_t length = final_height * final_width;
    uint32_t* data = new uint32_t[length];
    for (uint32_t i = 0; i < final_height; i++) {
        for (uint32_t j = 0; j < final_width; j++) {
            uint32_t idx = i * final_width + j;
            Color pixel = imageData->data[idx];
            data[idx] = pixel.GetValue();
        }
    }

    // 2 Create pixelmap
    OHOS::Media::InitializationOptions options;
    options.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    options.pixelFormat = OHOS::Media::PixelFormat::RGBA_8888;
    options.scaleMode = OHOS::Media::ScaleMode::CENTER_CROP;
    options.size.width = static_cast<int32_t>(final_width);
    options.size.height = static_cast<int32_t>(final_height);
    options.editable = true;
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap = OHOS::Media::PixelMap::Create(data, length, options);
    delete[] data;
    if (pixelmap == nullptr) {
        LOGE(" pixelmap is null.");
        return runtime->NewUndefined();
    }

    // 3 pixelmap to NapiValue
    auto nativeEngine = static_cast<ArkNativeEngine*>(engine->GetNativeEngine());
    if (!nativeEngine) {
        LOGE("NativeEngine is null");
        return runtime->NewUndefined();
    }
    napi_env env = reinterpret_cast<napi_env>(nativeEngine);
    std::shared_ptr<OHOS::Media::PixelMap> sharedPixelmap(pixelmap.release());
    napi_value napiValue = OHOS::Media::PixelMapNapi::CreatePixelMap(env, sharedPixelmap);

    if (!napiValue) {
        LOGE("napiValue is null");
        return runtime->NewUndefined();
    }

    // 4 NapiValue to JsValue
    NativeValue* nativeValue = reinterpret_cast<NativeValue*>(napiValue);
    if (!nativeValue) {
        LOGE("nativeValue is null");
        return runtime->NewUndefined();
    }

    Global<JSValueRef> globalRef = *nativeValue;
    auto arkRuntime = std::static_pointer_cast<ArkJSRuntime>(runtime);
    if (!arkRuntime) {
        LOGE("arkRuntime is null");
        return runtime->NewUndefined();
    }
    auto jsValue = std::make_shared<ArkJSValue>(arkRuntime, globalRef.ToLocal(arkRuntime->GetEcmaVm()));
    if (!jsValue) {
        LOGE("jsValue is null");
        return runtime->NewUndefined();
    }

    return jsValue;
#else
    return runtime->NewUndefined();
#endif
}

shared_ptr<JsValue> JsiCanvasBridge::JsGetJsonData(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        return runtime->NewUndefined();
    }
    NodeId id = GetCurrentNodeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetJsonData failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetJsonData failed. page is null.");
        return runtime->NewUndefined();
    }
    std::string path = argv[0]->ToString(runtime);
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetJsonData failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);

    return runtime->NewString(jsonData.c_str());
}

shared_ptr<JsValue> JsiCanvasBridge::JsTransferFromImageBitmap(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 1) {
        return runtime->NewUndefined();
    }

    int32_t bridgeId = 0;
    auto id = argv[0]->GetProperty(runtime, "__bridgeId");
    if (id && id->IsInt32(runtime)) {
        bridgeId = id->ToInt32(runtime);
    }
    bridgeId = bridgeId < 0 ? 0 : bridgeId;

    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (page) {
        RefPtr<JsiOffscreenCanvasBridge> bridge = AceType::DynamicCast<JsiOffscreenCanvasBridge>(
            page->GetOffscreenCanvasBridgeById(bridgeId));
        if (bridge) {
            auto offscreenCanvas = bridge->GetOffscreenCanvas();
            auto task = [offscreenCanvas](const RefPtr<CanvasTaskPool>& pool) {
                pool->TransferFromImageBitmap(offscreenCanvas);
            };
            PushTaskToPage(runtime, value, task);
        }
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsDrawBitmapMesh(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argc != 4) {
        return runtime->NewUndefined();
    }
    int32_t bridgeId = 0;
    auto id = argv[0]->GetProperty(runtime, "__bridgeId");
    if (id && id->IsInt32(runtime)) {
        bridgeId = id->ToInt32(runtime);
    }
    bridgeId = bridgeId < 0 ? 0 : bridgeId;
    auto mesh = GetJsDashValue(runtime, argv[1]);
    int32_t column = argv[2]->ToInt32(runtime);
    int32_t row = argv[3]->ToInt32(runtime);

    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (page) {
        RefPtr<JsiOffscreenCanvasBridge> bridge = AceType::DynamicCast<JsiOffscreenCanvasBridge>(
            page->GetOffscreenCanvasBridgeById(bridgeId));
        if (bridge) {
            auto offscreenCanvas = bridge->GetOffscreenCanvas();
            auto task = [offscreenCanvas, mesh, column, row](const RefPtr<CanvasTaskPool>& pool) {
                pool->DrawBitmapMesh(offscreenCanvas, mesh, column, row);
            };
            PushTaskToPage(runtime, value, task);
        }
    }
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsFillStyleGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__fillStyle");
}

shared_ptr<JsValue> JsiCanvasBridge::JsFillStyleSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    if (proto->IsString(runtime)) {
        auto colorStr = proto->ToString(runtime);
        auto color = Color::FromString(colorStr);
        auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFillColor(color); };
        PushTaskToPage(runtime, value, task);
    } else {
        auto typeVal = proto->GetProperty(runtime, "__type");
        auto type = typeVal->ToString(runtime);
        if (type == "gradient") {
            auto gradient = GetGradient(runtime, proto);
            auto task = [gradient](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFillGradient(gradient); };
            PushTaskToPage(runtime, value, task);
        } else if (type == "pattern") {
            auto pattern = GetPattern(runtime, proto);
            auto task = [pattern](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFillPattern(pattern); };
            PushTaskToPage(runtime, value, task);
        } else {
            LOGW("No such type for fill style.");
        }
    }
    value->SetProperty(runtime, "__fillStyle", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsStrokeStyleGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__strokeStyle");
}

shared_ptr<JsValue> JsiCanvasBridge::JsStrokeStyleSetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    if (argv.empty() || argc == 0) {
        return runtime->NewUndefined();
    }
    auto proto = argv[0];
    if (!proto) {
        return runtime->NewUndefined();
    }
    if (proto->IsString(runtime)) {
        auto colorStr = proto->ToString(runtime);
        auto color = Color::FromString(colorStr);
        auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateStrokeColor(color); };
        PushTaskToPage(runtime, value, task);
    } else {
        auto typeVal = proto->GetProperty(runtime, "__type");
        auto type = typeVal->ToString(runtime);
        if (type == "gradient") {
            auto gradient = GetGradient(runtime, proto);
            auto task = [gradient](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateStrokeGradient(gradient); };
            PushTaskToPage(runtime, value, task);
        } else if (type == "pattern") {
            auto pattern = GetPattern(runtime, proto);
            auto task = [pattern](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateStrokePattern(pattern); };
            PushTaskToPage(runtime, value, task);
        } else {
            LOGW("No such type for stroke style.");
        }
    }
    value->SetProperty(runtime, "__strokeStyle", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineCapGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineCap");
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineCapSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [lineCap](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineCap(lineCap); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__lineCap", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineJoinGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineJoin");
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineJoinSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [lineJoin](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineJoin(lineJoin); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__lineJoin", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsMiterLimitGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__miterLimit");
}

shared_ptr<JsValue> JsiCanvasBridge::JsMiterLimitSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [limit](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateMiterLimit(limit); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__miterLimit", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineWidthGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineWidth");
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineWidthSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [lineWidth](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineWidth(lineWidth); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__lineWidth", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsTextAlignGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__textAlign");
}

shared_ptr<JsValue> JsiCanvasBridge::JsTextAlignSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [align](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateTextAlign(align); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__textAlign", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsTextBaselineGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__textBaseline");
}

shared_ptr<JsValue> JsiCanvasBridge::JsTextBaselineSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [baseline](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateTextBaseline(baseline); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__textBaseline", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsFontGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__font");
}

shared_ptr<JsValue> JsiCanvasBridge::JsFontSetter(const shared_ptr<JsRuntime>& runtime,
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
    for (const auto& fontProp : fontProps) {
        if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            auto fontStyle = ConvertStrToFontStyle(fontProp);
            auto task = [fontStyle](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontStyle(fontStyle); };
            PushTaskToPage(runtime, value, task);
        } else if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            auto weight = ConvertStrToFontWeight(fontProp);
            auto task = [weight](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontWeight(weight); };
            PushTaskToPage(runtime, value, task);
        } else if (FONT_FAMILIES.find(fontProp) != FONT_FAMILIES.end()) {
            auto families = ConvertStrToFontFamilies(fontProp);
            auto task = [families](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontFamilies(families); };
            PushTaskToPage(runtime, value, task);
        } else if (fontProp.find("px") != std::string::npos) {
            std::string fontSize = fontProp.substr(0, fontProp.size() - 2);
            auto size = Dimension(StringToDouble(fontProp));
            auto task = [size](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontSize(size); };
            PushTaskToPage(runtime, value, task);
        } else {
            LOGW("parse text error");
        }
    }
    value->SetProperty(runtime, "__font", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsAlphaGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__globalAlpha");
}

shared_ptr<JsValue> JsiCanvasBridge::JsAlphaSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [alpha](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateGlobalAlpha(alpha); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__globalAlpha", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsCompositeOperationGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__globalCompositeOperation");
}

shared_ptr<JsValue> JsiCanvasBridge::JsCompositeOperationSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [type](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateCompositeOperation(type); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__globalCompositeOperation", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineDashOffsetGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__lineDash");
}

shared_ptr<JsValue> JsiCanvasBridge::JsLineDashOffsetSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [dashoffset](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineDashOffset(dashoffset); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__lineDash", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowBlurGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowBlur");
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowBlurSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [blur](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowBlur(blur); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__shadowBlur", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowColorGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowColor");
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowColorSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowColor(color); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__shadowColor", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowOffsetXGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowOffsetX");
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowOffsetXSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [offsetX](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowOffsetX(offsetX); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__shadowOffsetX", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowOffsetYGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__shadowOffsetY");
}

shared_ptr<JsValue> JsiCanvasBridge::JsShadowOffsetYSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [offsetY](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowOffsetY(offsetY); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__shadowOffsetY", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsSmoothingEnabledGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__imageSmoothingEnabled");
}

shared_ptr<JsValue> JsiCanvasBridge::JsSmoothingEnabledSetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [enabled](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateSmoothingEnabled(enabled); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__imageSmoothingEnabled", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsSmoothingQualityGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    return value->GetProperty(runtime, "__imageSmoothingQuality");
}

shared_ptr<JsValue> JsiCanvasBridge::JsSmoothingQualitySetter(const shared_ptr<JsRuntime>& runtime,
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
    auto task = [quality](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateSmoothingQuality(quality); };
    PushTaskToPage(runtime, value, task);
    value->SetProperty(runtime, "__imageSmoothingQuality", proto);
    return runtime->NewUndefined();
}

shared_ptr<JsValue> JsiCanvasBridge::JsWidthGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    NodeId id = GetCurrentNodeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        return runtime->NewUndefined();
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);

    return runtime->NewNumber(width);
}

shared_ptr<JsValue> JsiCanvasBridge::JsHeightGetter(const shared_ptr<JsRuntime>& runtime,
    const shared_ptr<JsValue>& value, const std::vector<shared_ptr<JsValue>>& argv, int32_t argc)
{
    NodeId id = GetCurrentNodeId(runtime, value);
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        return runtime->NewUndefined();
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
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);

    return runtime->NewNumber(height);
}

} // namespace OHOS::Ace::Framework
