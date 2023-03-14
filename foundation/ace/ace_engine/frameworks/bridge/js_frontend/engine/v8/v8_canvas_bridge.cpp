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

#include <memory>

#include "frameworks/bridge/js_frontend/engine/v8/v8_canvas_bridge.h"

#include "base/utils/string_utils.h"
#include "core/components/custom_paint/offscreen_canvas.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_offscreen_canvas_bridge.h"
#include "frameworks/bridge/js_frontend/js_command.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr char IMAGE_SRC[] = "src";
constexpr char IMAGE_WIDTH[] = "width";
constexpr char IMAGE_HEIGHT[] = "height";

constexpr char CANVAS_TYPE_WEBGL[] = "webgl";
constexpr char CANVAS_TYPE_WEBGL2[] = "webgl2";
constexpr char CANVAS_WEBGL_SO[] = "webglnapi";

template<typename T>
inline T ConvertStrToEnum(const std::string& key, const std::unordered_map<std::string, T>& map, T defaultValue)
{
    auto iter = map.find(key);
    return iter != map.end() ? iter->second : defaultValue;
}

const std::unordered_map<std::string, TextBaseline> BASELINE_TABLE = {
    { "alphabetic", TextBaseline::ALPHABETIC },
    { "ideographic", TextBaseline::IDEOGRAPHIC },
    { "top", TextBaseline::TOP },
    { "bottom", TextBaseline::BOTTOM },
    { "middle", TextBaseline::MIDDLE },
    { "hanging", TextBaseline::HANGING },
};

const std::set<std::string> FONT_WEIGHTS = { "normal", "bold", "lighter", "bolder",
    "100", "200", "300", "400", "500", "600", "700", "800", "900" };
const std::set<std::string> FONT_STYLES = { "italic", "oblique", "normal" };
const std::set<std::string> FONT_FAMILIES = { "sans-serif", "serif", "monospace" };
const std::set<std::string> QUALITY_TYPE = { "low", "medium", "high" }; // Default value is low.

inline NodeId GetCurrentNodeId(const v8::Local<v8::Context>& context, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    NodeId id = value->Get(context, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked())
                    .ToLocalChecked()->Int32Value(context).ToChecked();
    if (id < 0) {
        return 0;
    }
    return id;
}

void PushTaskToPage(const v8::Local<v8::Context>& context, v8::Local<v8::Object> value,
    const std::function<void(const RefPtr<CanvasTaskPool>&)>& task)
{
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    NodeId id = GetCurrentNodeId(context, value);

    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    // push command
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }
    (*page)->PushCommand(command);
}

void PushTaskToPageById(const v8::Local<v8::Context>& context, NodeId id,
    const std::function<void(const RefPtr<CanvasTaskPool>&)>& task)
{
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    // push command
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }
    (*page)->PushCommand(command);
}

inline std::vector<double> GetDashValue(const v8::FunctionCallbackInfo<v8::Value>& args, uint32_t index)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    std::vector<double> segments;
    v8::String::Utf8Value value(isolate, args[index]->ToString(context).ToLocalChecked());
    std::vector<std::string> props;
    if (!(*value)) {
        return segments;
    }
    StringUtils::StringSpliter(*value, ',', props);

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

inline Rect GetRectParam(const v8::FunctionCallbackInfo<v8::Value>& args, const v8::Local<v8::Context>& context)
{
    // 4 parameters: rect(x, y, width, height)
    if (args.Length() != 4) {
        return Rect();
    }
    double x = args[0]->Int32Value(context).ToChecked();
    double y = args[1]->Int32Value(context).ToChecked();
    double width = args[2]->Int32Value(context).ToChecked();
    double height = args[3]->Int32Value(context).ToChecked();
    Rect rect = Rect(x, y, width, height);
    return rect;
}

inline PaintState ParseTextState(const v8::Local<v8::Context>& context, v8::Local<v8::Object> value)
{
    PaintState state;
    auto fontStyle =
        value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "font").ToLocalChecked()).ToLocalChecked();
    auto textAlign = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "textAlign").ToLocalChecked())
                         .ToLocalChecked();
    auto textBaseline =
        value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "textBaseline").ToLocalChecked())
            .ToLocalChecked();

    // parse font styles
    v8::String::Utf8Value alignStr(context->GetIsolate(), textAlign);
    v8::String::Utf8Value baselineStr(context->GetIsolate(), textBaseline);
    if (*alignStr) {
        state.SetTextAlign(ConvertStrToTextAlign(*alignStr));
    }
    TextStyle style;
    if (*baselineStr) {
        style.SetTextBaseline(ConvertStrToEnum(*baselineStr, BASELINE_TABLE, TextBaseline::ALPHABETIC));
    }
    v8::String::Utf8Value font(context->GetIsolate(), fontStyle);
    std::vector<std::string> fontProps;
    if (*font) {
        StringUtils::StringSpliter(*font, ' ', fontProps);
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
    }
    state.SetTextStyle(style);
    return state;
}

} // namespace

int32_t V8CanvasBridge::gradientCount_ = 0;
int32_t V8CanvasBridge::patternCount_ = 0;
int32_t V8CanvasBridge::path2dCount_ = 0;
std::unordered_map<int32_t, Pattern> V8CanvasBridge::pattern_;
std::unordered_map<int32_t, Gradient> V8CanvasBridge::gradientColors_;
std::unordered_map<int32_t, RefPtr<CanvasPath2D>> V8CanvasBridge::path2Ds_;

V8CanvasBridge::~V8CanvasBridge()
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

void V8CanvasBridge::HandleContext(
    const v8::Local<v8::Context>& ctx, NodeId id, const std::string& args, JsEngineInstance* engine)
{
    LOGD("V8CanvasBridge::HandleJsContext");
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (argsValue && argsValue->IsArray() && argsValue->GetArraySize() > 0) {
        auto typeArg = argsValue->GetArrayItem(0);
        if (typeArg && typeArg->IsString()) {
            std::string type = typeArg->GetString();
            if (type == std::string(CANVAS_TYPE_WEBGL)) {
                HandleWebglContext(ctx, id, args, engine, webglRenderContext_);
                return;
            } else if (type == std::string(CANVAS_TYPE_WEBGL2)) {
                HandleWebglContext(ctx, id, args, engine, webgl2RenderContext_);
                return;
            }
        }
    }

    renderContext_ = v8::Object::New(ctx->GetIsolate());
    renderContext_->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__nodeId").ToLocalChecked(),
        v8::Int32::New(ctx->GetIsolate(), id)).ToChecked();

    const std::unordered_map<const char*, v8::Local<v8::Function>> contextTable = {
        { "createLinearGradient",
            v8::Function::New(ctx, CreateLinearGradient, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "createRadialGradient",
            v8::Function::New(ctx, CreateRadialGradient, v8::Local<v8::Value>(), 6).ToLocalChecked() },
        { "fillRect", v8::Function::New(ctx, FillRect, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "strokeRect", v8::Function::New(ctx, StrokeRect, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "clearRect", v8::Function::New(ctx, ClearRect, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "fillText", v8::Function::New(ctx, FillText, v8::Local<v8::Value>(), 3).ToLocalChecked() },
        { "strokeText", v8::Function::New(ctx, StrokeText, v8::Local<v8::Value>(), 3).ToLocalChecked() },
        { "measureText", v8::Function::New(ctx, MeasureText, v8::Local<v8::Value>(), 1).ToLocalChecked() },
        { "moveTo", v8::Function::New(ctx, MoveTo, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "lineTo", v8::Function::New(ctx, LineTo, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "bezierCurveTo", v8::Function::New(ctx, BezierCurveTo, v8::Local<v8::Value>(), 6).ToLocalChecked() },
        { "quadraticCurveTo", v8::Function::New(ctx, QuadraticCurveTo, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "arcTo", v8::Function::New(ctx, ArcTo, v8::Local<v8::Value>(), 5).ToLocalChecked() },
        { "arc", v8::Function::New(ctx, Arc, v8::Local<v8::Value>(), 6).ToLocalChecked() },
        { "ellipse", v8::Function::New(ctx, Ellipse, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "fill", v8::Function::New(ctx, Fill, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "stroke", v8::Function::New(ctx, Stroke, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "clip", v8::Function::New(ctx, Clip, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "rect", v8::Function::New(ctx, DrawRect, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "beginPath", v8::Function::New(ctx, BeginPath, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "closePath", v8::Function::New(ctx, ClosePath, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "restore", v8::Function::New(ctx, Restore, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "save", v8::Function::New(ctx, Save, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "rotate", v8::Function::New(ctx, Rotate, v8::Local<v8::Value>(), 1).ToLocalChecked() },
        { "scale", v8::Function::New(ctx, Scale, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "setTransform", v8::Function::New(ctx, SetTransform, v8::Local<v8::Value>(), 6).ToLocalChecked() },
        { "transform", v8::Function::New(ctx, Transform, v8::Local<v8::Value>(), 6).ToLocalChecked() },
        { "translate", v8::Function::New(ctx, Translate, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "getLineDash", v8::Function::New(ctx, GetLineDash, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "setLineDash", v8::Function::New(ctx, SetLineDash, v8::Local<v8::Value>(), 1).ToLocalChecked() },
        { "drawImage", v8::Function::New(ctx, DrawImage, v8::Local<v8::Value>(), 9).ToLocalChecked() },
        { "createPath2D", v8::Function::New(ctx, CreatePath2D, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "createPattern", v8::Function::New(ctx, CreatePattern, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "createImageData", v8::Function::New(ctx, CreateImageData, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "putImageData", v8::Function::New(ctx, PutImageData, v8::Local<v8::Value>(), 7).ToLocalChecked() },
        { "getImageData", v8::Function::New(ctx, GetImageData, v8::Local<v8::Value>(), 4).ToLocalChecked() },
        { "getJsonData", v8::Function::New(ctx, GetJsonData, v8::Local<v8::Value>(), 1).ToLocalChecked() },
        { "transferFromImageBitmap",
            v8::Function::New(ctx, TransferFromImageBitmap, v8::Local<v8::Value>(), 1).ToLocalChecked() },
        { "drawBitmapMesh",
            v8::Function::New(ctx, DrawBitmapMesh, v8::Local<v8::Value>(), 4).ToLocalChecked() },
    };
    for (const auto& iter : contextTable) {
        renderContext_->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), iter.first).ToLocalChecked(), iter.second)
            .ToChecked();
    }
    static const std::vector<std::tuple<std::string, v8::FunctionCallback, v8::FunctionCallback>> v8AnimationFuncs = {
        { "fillStyle", V8CanvasBridge::FillStyleGetter, V8CanvasBridge::FillStyleSetter },
        { "strokeStyle", V8CanvasBridge::StrokeStyleGetter, V8CanvasBridge::StrokeStyleSetter },
        { "lineCap", V8CanvasBridge::LineCapGetter, V8CanvasBridge::LineCapSetter },
        { "lineJoin", V8CanvasBridge::LineJoinGetter, V8CanvasBridge::LineJoinSetter },
        { "miterLimit", V8CanvasBridge::MiterLimitGetter, V8CanvasBridge::MiterLimitSetter },
        { "lineWidth", V8CanvasBridge::LineWidthGetter, V8CanvasBridge::LineWidthSetter },
        { "textAlign", V8CanvasBridge::TextAlignGetter, V8CanvasBridge::TextAlignSetter },
        { "textBaseline", V8CanvasBridge::TextBaselineGetter, V8CanvasBridge::TextBaselineSetter },
        { "font", V8CanvasBridge::FontGetter, V8CanvasBridge::FontSetter },
        { "globalAlpha", V8CanvasBridge::AlphaGetter, V8CanvasBridge::AlphaSetter },
        { "globalCompositeOperation", V8CanvasBridge::CompositeOperationGetter,
            V8CanvasBridge::CompositeOperationSetter },
        { "lineDashOffset", V8CanvasBridge::LineDashOffsetGetter, V8CanvasBridge::LineDashOffsetSetter },
        { "shadowBlur", V8CanvasBridge::ShadowBlurGetter, V8CanvasBridge::ShadowBlurSetter },
        { "shadowColor", V8CanvasBridge::ShadowColorGetter, V8CanvasBridge::ShadowColorSetter },
        { "shadowOffsetX", V8CanvasBridge::ShadowOffsetXGetter, V8CanvasBridge::ShadowOffsetXSetter },
        { "shadowOffsetY", V8CanvasBridge::ShadowOffsetYGetter, V8CanvasBridge::ShadowOffsetYSetter },
        { "imageSmoothingEnabled", V8CanvasBridge::SmoothingEnabledGetter, V8CanvasBridge::SmoothingEnabledSetter },
        { "imageSmoothingQuality", V8CanvasBridge::SmoothingQualityGetter, V8CanvasBridge::SmoothingQualitySetter },
        { "width", V8CanvasBridge::WidthGetter, nullptr },
        { "height", V8CanvasBridge::HeightGetter, nullptr }
    };
    for (const auto& item : v8AnimationFuncs) {
        auto getter_templ = v8::FunctionTemplate::New(ctx->GetIsolate(), std::get<1>(item));
        auto setter_templ = v8::FunctionTemplate::New(ctx->GetIsolate(), std::get<2>(item));
        renderContext_->SetAccessorProperty(
            v8::String::NewFromUtf8(ctx->GetIsolate(), std::get<0>(item).c_str()).ToLocalChecked(),
            getter_templ->GetFunction(ctx).ToLocalChecked(), setter_templ->GetFunction(ctx).ToLocalChecked());
    }
    SetAntiAlias(ctx, id, args);
}

void V8CanvasBridge::HandleWebglContext(
    const v8::Local<v8::Context>& ctx, NodeId id, const std::string& args, JsEngineInstance* engine,
    CanvasRenderContextBase*& canvasRenderContext)
{
    LOGD("V8CanvasBridge::HandleWebglContext");
    if (engine == nullptr) {
        LOGE("engine is null");
        return;
    }
    std::shared_ptr<V8NativeEngine> nativeEngine = static_cast<V8EngineInstance*>(engine)->GetV8NativeEngine();
    if (!nativeEngine) {
        LOGE("nativeEngine is null");
        return;
    }
    std::string moduleName(CANVAS_WEBGL_SO);
    std::string pluginId(std::to_string(id));
    renderContext_ = nativeEngine->GetModuleFromName(
        moduleName, false, pluginId, args, WEBGL_RENDER_CONTEXT_NAME, reinterpret_cast<void**>(&canvasRenderContext));
    if (!canvasRenderContext) {
        LOGE("V8CanvasBridge invalid canvasRenderContext");
        return;
    }

    auto page = static_cast<RefPtr<JsAcePage>*>(ctx->GetIsolate()->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        LOGE("page is null.");
        return;
    }
    auto task = [canvasRenderContext, page, id]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(
        ctx->GetIsolate()->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (*delegate == nullptr) {
        LOGE("delegate is null.");
        return;
    }
    (*delegate)->PostSyncTaskToPage(task);

    canvasRenderContext->Init();

    auto onWebGLUpdateCallback = [ctx, id]() {
        auto task = [](const RefPtr<CanvasTaskPool>& pool) {
            pool->WebGLUpdate();
        };
        PushTaskToPageById(ctx, id, task);
    };
    canvasRenderContext->SetUpdateCallback(onWebGLUpdateCallback);
}

void V8CanvasBridge::HandleToDataURL(const v8::Local<v8::Context>& ctx, NodeId id, const std::string& args)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        LOGE("ToDataURL failed, page is null.");
        return;
    }
    std::string dataUrl;
    auto task = [id, page, args, &dataUrl]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (*delegate == nullptr) {
        LOGE("ToDataURL failed, Delegate is null.");
        return;
    }
    (*delegate)->PostSyncTaskToPage(task);
    dataURL_ = v8::String::NewFromUtf8(ctx->GetIsolate(), dataUrl.c_str()).ToLocalChecked();
}

void V8CanvasBridge::SetAntiAlias(const v8::Local<v8::Context>& ctx, NodeId id, const std::string& args)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (!page || !(*page) || !delegate || !(*delegate)) {
        return;
    }
    bool isEnabled = args.find("\"antialias\":true") != std::string::npos;
    auto task = [isEnabled](const RefPtr<CanvasTaskPool>& pool) { pool->SetAntiAlias(isEnabled); };
    auto command = Referenced::MakeRefPtr<JsCommandContextOperation>(id, task);
    (*page)->PushCommand(command);
    if ((*page)->CheckPageCreated()) {
        (*delegate)->TriggerPageUpdate((*page)->GetPageId());
    }
}

void V8CanvasBridge::CreateLinearGradient(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::CreateLinearGradient");
    // 4 parameters: createLinearGradient(x0, y0, x1, y1)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Object> gradient = v8::Object::New(isolate);
    gradient->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "gradient").ToLocalChecked()).ToChecked();
    gradient->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
        v8::Integer::New(isolate, gradientCount_)).ToChecked();
    gradient->Set(context, v8::String::NewFromUtf8(isolate, "addColorStop").ToLocalChecked(),
        v8::Function::New(context, AddColorStop, v8::Local<v8::Value>(), 2).ToLocalChecked()).ToChecked();
    Offset beginOffset = Offset(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked());
    Offset endOffset = Offset(args[2]->NumberValue(context).ToChecked(), args[3]->NumberValue(context).ToChecked());
    gradientColors_[gradientCount_].SetType(GradientType::LINEAR);
    gradientColors_[gradientCount_].SetBeginOffset(beginOffset);
    gradientColors_[gradientCount_].SetEndOffset(endOffset);
    ++gradientCount_;
    args.GetReturnValue().Set(gradient);
}

void V8CanvasBridge::CreateRadialGradient(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::CreateRadialGradient");
    // 6 parameters: createRadialGradient(x0, y0, r0, x1, y1, r1)
    if (args.Length() != 6) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Object> gradient = v8::Object::New(isolate);
    gradient->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "gradient").ToLocalChecked()).ToChecked();
    gradient->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
        v8::Integer::New(isolate, gradientCount_)).ToChecked();
    gradient->Set(context, v8::String::NewFromUtf8(isolate, "addColorStop").ToLocalChecked(),
        v8::Function::New(context, AddColorStop, v8::Local<v8::Value>(), 2).ToLocalChecked()).ToChecked();
    Offset innerCenter = Offset(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked());
    Offset outerCenter = Offset(args[3]->NumberValue(context).ToChecked(), args[4]->NumberValue(context).ToChecked());
    double innerRadius = args[2]->NumberValue(context).ToChecked();
    double outerRadius = args[5]->NumberValue(context).ToChecked();
    gradientColors_[gradientCount_].SetType(GradientType::RADIAL);
    gradientColors_[gradientCount_].SetBeginOffset(innerCenter);
    gradientColors_[gradientCount_].SetEndOffset(outerCenter);
    gradientColors_[gradientCount_].SetInnerRadius(innerRadius);
    gradientColors_[gradientCount_].SetOuterRadius(outerRadius);
    ++gradientCount_;
    args.GetReturnValue().Set(gradient);
}

void V8CanvasBridge::AddColorStop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: addColorStop(offset, color)
    if (args.Length() != 2) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    GradientColor color;
    v8::String::Utf8Value colorStr(isolate, args[1]->ToString(context).ToLocalChecked());
    if (*colorStr) {
        color.SetColor(Color::FromString(*colorStr));
    }
    color.SetDimension(args[0]->NumberValue(context).ToChecked());
    int32_t id = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (id < 0) {
        return;
    }
    gradientColors_[id].AddColor(color);
}

void V8CanvasBridge::FillRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::FillRect");
    // 4 parameters: fillRect(x, y, width, height)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    Rect rect = GetRectParam(args, context);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->FillRect(rect); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::StrokeRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::StrokeRect");
    // 4 parameters: strokeRect(x, y, width, height)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    Rect rect = GetRectParam(args, context);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->StrokeRect(rect); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::ClearRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::ClearRect");
    // 4 parameters: clearRect(x, y, width, height)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    Rect rect = GetRectParam(args, context);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->ClearRect(rect); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::FillText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::FillText");
    // 3 parameters: fillText(text, x, y, [maxWidth])
    if (args.Length() != 3 || !args[0]->IsString()) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::String::Utf8Value arg(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!(*arg)) {
        return;
    }
    std::string text = *arg;
    double x = args[1]->NumberValue(context).ToChecked();
    double y = args[2]->NumberValue(context).ToChecked();
    auto task = [text, x, y](const RefPtr<CanvasTaskPool>& pool) { pool->FillText(text, Offset(x, y)); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::StrokeText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8CanvasBridge::StrokeText");
    // 3 parameters: strokeText(text, x, y, [maxWidth])
    if (args.Length() != 3 || !args[0]->IsString()) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::String::Utf8Value arg(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!(*arg)) {
        return;
    }
    std::string text = *arg;
    double x = args[1]->NumberValue(context).ToChecked();
    double y = args[2]->NumberValue(context).ToChecked();
    auto task = [text, x, y](const RefPtr<CanvasTaskPool>& pool) { pool->StrokeText(text, Offset(x, y)); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::MeasureText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 parameter: measureText(text)
    if (args.Length() != 1) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::String::Utf8Value arg(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!*arg) {
        return;
    }

    std::string text = *arg;
    auto textState = ParseTextState(context, args.Holder());
    NodeId id = GetCurrentNodeId(context, args.Holder());
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }
    double width = 0.0;
    double height = 0.0;
    auto task = [&text, &textState, id, page, &width, &height]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);
    v8::Local<v8::Object> textMetrics = v8::Object::New(isolate);
    textMetrics
        ->Set(context, v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(), v8::Number::New(isolate, width))
        .ToChecked();
    textMetrics
        ->Set(context, v8::String::NewFromUtf8(isolate, "height").ToLocalChecked(), v8::Number::New(isolate, height))
        .ToChecked();
    args.GetReturnValue().Set(textMetrics);
}

void V8CanvasBridge::BeginPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 0 parameter: beginPath()
    if (args.Length() != 0) {
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->BeginPath(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::ClosePath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 0 parameter: closePath()
    if (args.Length() != 0) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->ClosePath(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: moveTo(x, y)
    if (args.Length() != 2) {
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    double x = args[0]->NumberValue(context).ToChecked();
    double y = args[1]->NumberValue(context).ToChecked();
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->MoveTo(x, y); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::LineTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: lineTo(x, y)
    if (args.Length() != 2) {
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    double x = args[0]->NumberValue(context).ToChecked();
    double y = args[1]->NumberValue(context).ToChecked();
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->LineTo(x, y); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 6 parameters: bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y)
    if (args.Length() != 6) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    BezierCurveParam param;
    param.cp1x = args[0]->NumberValue(context).ToChecked();
    param.cp1y = args[1]->NumberValue(context).ToChecked();
    param.cp2x = args[2]->NumberValue(context).ToChecked();
    param.cp2y = args[3]->NumberValue(context).ToChecked();
    param.x = args[4]->NumberValue(context).ToChecked();
    param.y = args[5]->NumberValue(context).ToChecked();
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->BezierCurveTo(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 4 parameters: quadraticCurveTo(cpx, cpy, x, y)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    QuadraticCurveParam param;
    param.cpx = args[0]->NumberValue(context).ToChecked();
    param.cpy = args[1]->NumberValue(context).ToChecked();
    param.x = args[2]->NumberValue(context).ToChecked();
    param.y = args[3]->NumberValue(context).ToChecked();
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->QuadraticCurveTo(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Arc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 5 or 6 parameters: arc(x, y, radius, startAngle, endAngle, anticlockwise?)
    if (args.Length() < 5 || args.Length() > 6) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    ArcParam param;
    param.x = args[0]->NumberValue(context).ToChecked();
    param.y = args[1]->NumberValue(context).ToChecked();
    param.radius = args[2]->NumberValue(context).ToChecked();
    param.startAngle = args[3]->NumberValue(context).ToChecked();
    param.endAngle = args[4]->NumberValue(context).ToChecked();
    if (args.Length() == 6) {
        if (args[5]->IsBoolean()) {
            param.anticlockwise = args[5]->ToBoolean(isolate)->Value();
        }
    }
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Arc(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::ArcTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 5 parameters: arcTo(x1, y1, x2, y2, radius)
    if (args.Length() != 5) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    ArcToParam param;
    param.x1 = args[0]->NumberValue(context).ToChecked();
    param.y1 = args[1]->NumberValue(context).ToChecked();
    param.x2 = args[2]->NumberValue(context).ToChecked();
    param.y2 = args[3]->NumberValue(context).ToChecked();
    param.radius = args[4]->NumberValue(context).ToChecked();
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->ArcTo(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Ellipse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 7 or 8 parameters: ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise?)
    if (args.Length() < 7 || args.Length() > 8) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    EllipseParam param;
    param.x = args[0]->NumberValue(context).ToChecked();
    param.y = args[1]->NumberValue(context).ToChecked();
    param.radiusX = args[2]->NumberValue(context).ToChecked();
    param.radiusY = args[3]->NumberValue(context).ToChecked();
    param.rotation = args[4]->NumberValue(context).ToChecked();
    param.startAngle = args[5]->NumberValue(context).ToChecked();
    param.endAngle = args[6]->NumberValue(context).ToChecked();
    if (args.Length() == 8) {
        int32_t anti = static_cast<int32_t>(args[7]->NumberValue(context).ToChecked());
        param.anticlockwise = (anti == 1);
    }
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Ellipse(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::DrawRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    Rect rect = GetRectParam(args, context);
    auto task = [rect](const RefPtr<CanvasTaskPool>& pool) { pool->AddRect(rect); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Fill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Fill(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Stroke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    // 0 or 1 parameter: ctx.stroke() / ctx.stroke(path)
    if (args.Length() == 1) {
        auto object = args[0]->ToObject(context).ToLocalChecked();
        auto type = object->Get(context,
            v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value value(isolate, type->ToString(context).ToLocalChecked());
        if (*value == nullptr || std::strcmp(*value, "path2d") != 0) {
            LOGE("Stroke Path2D failed, target is not path.");
            return;
        }
        auto path = GetPath2D(context, object);
        if (path == nullptr) {
            LOGE("Stroke Path2D failed, target path is null.");
            return;
        }
        auto task = [path](const RefPtr<CanvasTaskPool>& pool) { pool->Stroke(path); };
        PushTaskToPage(context, args.Holder(), task);
        return;
    }
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Stroke(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Clip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Clip(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Restore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Restore(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Save(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->Save(); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Rotate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 parameter: rotate(angle)
    if (args.Length() != 1) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    double angle = args[0]->NumberValue(context).ToChecked();
    auto task = [angle](const RefPtr<CanvasTaskPool>& pool) { pool->Rotate(angle); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Scale(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: scale(x, y)
    if (args.Length() != 2) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    double x = args[0]->NumberValue(context).ToChecked();
    double y = args[1]->NumberValue(context).ToChecked();
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->Scale(x, y); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::SetTransform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 6 parameters: setTransform(a, b, c, d, e, f)
    if (args.Length() != 6) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    TransformParam param;
    param.scaleX = args[0]->NumberValue(context).ToChecked();
    param.skewX = args[1]->NumberValue(context).ToChecked();
    param.skewY = args[2]->NumberValue(context).ToChecked();
    param.scaleY = args[3]->NumberValue(context).ToChecked();
    param.translateX = args[4]->NumberValue(context).ToChecked();
    param.translateY = args[5]->NumberValue(context).ToChecked();
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->SetTransform(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Transform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 6 parameters: transform(a, b, c, d, e, f)
    if (args.Length() != 6) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    TransformParam param;
    param.scaleX = args[0]->NumberValue(context).ToChecked();
    param.skewX = args[1]->NumberValue(context).ToChecked();
    param.skewY = args[2]->NumberValue(context).ToChecked();
    param.scaleY = args[3]->NumberValue(context).ToChecked();
    param.translateX = args[4]->NumberValue(context).ToChecked();
    param.translateY = args[5]->NumberValue(context).ToChecked();
    auto task = [param](const RefPtr<CanvasTaskPool>& pool) { pool->Transform(param); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::Translate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: translate(x, y)
    if (args.Length() != 2) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    double x = args[0]->NumberValue(context).ToChecked();
    double y = args[1]->NumberValue(context).ToChecked();
    auto task = [x, y](const RefPtr<CanvasTaskPool>& pool) { pool->Translate(x, y); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::SetLineDash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 parameter: setLineDash(segments)
    if (args.Length() != 1) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::String::Utf8Value dash(isolate, args[0]->ToString(context).ToLocalChecked());
    if (*dash) {
        args.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "lineDash").ToLocalChecked(),
                v8::String::NewFromUtf8(isolate, *dash).ToLocalChecked())
            .ToChecked();
        auto segments = GetDashValue(args, 0);
        auto task = [segments](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineDash(segments); };
        PushTaskToPage(context, args.Holder(), task);
    }
}

void V8CanvasBridge::GetLineDash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto val =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "lineDash").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(val);
}

void V8CanvasBridge::ParseDomImage(
    const v8::FunctionCallbackInfo<v8::Value>& args, double& width, double& height, std::string& src)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto imageObject = args[0]->ToObject(context).ToLocalChecked();

    auto attr = imageObject->Get(context, v8::String::NewFromUtf8(isolate, "attr").ToLocalChecked())
                    .ToLocalChecked()
                    ->ToObject(context)
                    .ToLocalChecked();
    auto str = attr->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_SRC).ToLocalChecked())
                   .ToLocalChecked()
                   ->ToString(context)
                   .ToLocalChecked();
    v8::String::Utf8Value utf8Value(isolate, str);
    if (*utf8Value) {
        src = *utf8Value;
    }

    auto style = imageObject->Get(context, v8::String::NewFromUtf8(isolate, "style").ToLocalChecked())
                     .ToLocalChecked()
                     ->ToObject(context)
                     .ToLocalChecked();
    auto v8StrWidth = style->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked())
                          .ToLocalChecked()
                          ->ToString(context)
                          .ToLocalChecked();
    auto v8StrHeight = style->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked())
                           .ToLocalChecked()
                           ->ToString(context)
                           .ToLocalChecked();

    v8::String::Utf8Value strWidth(isolate, v8StrWidth);
    v8::String::Utf8Value strHeight(isolate, v8StrHeight);
    if (*strWidth) {
        std::string stringWidth(*strWidth);
        width = StringToDouble(stringWidth);
        if (NearZero(width)) {
            width = StringToDouble(stringWidth.substr(0, stringWidth.size() - 2)); // remove px units
        }
    }
    if (*strHeight) {
        std::string stringHeight(*strHeight);
        height = StringToDouble(stringHeight);
        if (NearZero(height)) {
            height = StringToDouble(stringHeight.substr(0, stringHeight.size() - 2));
        }
    }
}

void V8CanvasBridge::DrawImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (!args[0]->IsObject() || args.Length() < 3) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    CanvasImage image;
    double width = 0.0;
    double height = 0.0;
    auto imageObject = args[0]->ToObject(context).ToLocalChecked();
    auto src = imageObject->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_SRC).ToLocalChecked()).ToLocalChecked();
    if (!src->IsString()) {
        ParseDomImage(args, width, height, image.src);
    } else {
        v8::Local<v8::Value> str;
        bool checked = src->ToString(context).ToLocal(&str);
        if (checked) {
            v8::String::Utf8Value utf8Value(isolate, str);
            if (*utf8Value) {
                image.src = *utf8Value;
            }
            width = imageObject->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked())
                        .ToLocalChecked()
                        ->NumberValue(context)
                        .ToChecked();
            height = imageObject->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked())
                         .ToLocalChecked()
                         ->NumberValue(context)
                         .ToChecked();
        }
    }
    switch (args.Length()) {
        // 3 parameters: drawImage(image, dx, dy)
        case 3:
            image.flag = 0;
            image.dx = args[1]->NumberValue(context).ToChecked();
            image.dy = args[2]->NumberValue(context).ToChecked();
            break;
        // 5 parameters: drawImage(image, dx, dy, dWidth, dHeight)
        case 5:
            image.flag = 1;
            image.dx = args[1]->NumberValue(context).ToChecked();
            image.dy = args[2]->NumberValue(context).ToChecked();
            image.dWidth = args[3]->NumberValue(context).ToChecked();
            image.dHeight = args[4]->NumberValue(context).ToChecked();
            break;
        // 9 parameters: drawImage(image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight)
        case 9:
            image.flag = 2;
            image.sx = args[1]->NumberValue(context).ToChecked();
            image.sy = args[2]->NumberValue(context).ToChecked();
            image.sWidth = args[3]->NumberValue(context).ToChecked();
            image.sHeight = args[4]->NumberValue(context).ToChecked();
            image.dx = args[5]->NumberValue(context).ToChecked();
            image.dy = args[6]->NumberValue(context).ToChecked();
            image.dWidth = args[7]->NumberValue(context).ToChecked();
            image.dHeight = args[8]->NumberValue(context).ToChecked();
            break;
        default:
            break;
    }
    auto task = [image, width, height](const RefPtr<CanvasTaskPool>& pool) { pool->DrawImage(image, width, height); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::CreatePath2D(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto path2D = v8::Object::New(isolate);
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
        v8::Integer::New(isolate, path2dCount_)).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "path2d").ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "addPath").ToLocalChecked(),
        v8::Function::New(context, Path2DAddPath, v8::Local<v8::Value>(), 1).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "setTransform").ToLocalChecked(),
        v8::Function::New(context, Path2DSetTransform, v8::Local<v8::Value>(), 6).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "moveTo").ToLocalChecked(),
        v8::Function::New(context, Path2DMoveTo, v8::Local<v8::Value>(), 2).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "lineTo").ToLocalChecked(),
        v8::Function::New(context, Path2DLineTo, v8::Local<v8::Value>(), 2).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "arc").ToLocalChecked(),
        v8::Function::New(context, Path2DArc, v8::Local<v8::Value>(), 6).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "arcTo").ToLocalChecked(),
        v8::Function::New(context, Path2DArcTo, v8::Local<v8::Value>(), 5).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "quadraticCurveTo").ToLocalChecked(),
        v8::Function::New(context, Path2DQuadraticCurveTo, v8::Local<v8::Value>(), 4).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "bezierCurveTo").ToLocalChecked(),
        v8::Function::New(context, Path2DBezierCurveTo, v8::Local<v8::Value>(), 6).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "ellipse").ToLocalChecked(),
        v8::Function::New(context, Path2DEllipse, v8::Local<v8::Value>(), 8).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "rect").ToLocalChecked(),
        v8::Function::New(context, Path2DRect, v8::Local<v8::Value>(), 4).ToLocalChecked()).ToChecked();
    path2D->Set(context, v8::String::NewFromUtf8(isolate, "closePath").ToLocalChecked(),
        v8::Function::New(context, Path2DClosePath, v8::Local<v8::Value>(), 0).ToLocalChecked()).ToChecked();
    path2Ds_[path2dCount_] = MakePath2D(args);
    ++path2dCount_;
    args.GetReturnValue().Set(path2D);
}

void V8CanvasBridge::Path2DAddPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 parameter: addPath(path)
    if (args.Length() != 1) {
        LOGE("AddPath to Path2D failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("AddPath to Path2D failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("AddPath to Path2D failed, holderPath is null.");
        return;
    }

    object = args[0]->ToObject(context).ToLocalChecked();
    auto type = object->Get(context,
        v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
    v8::String::Utf8Value value(isolate, type->ToString(context).ToLocalChecked());
    if (*value == nullptr || std::strcmp(*value, "path2d") != 0) {
        LOGE("AddPath to Path2D failed, to be added is not path.");
        return;
    }
    auto toBeAdd = GetPath2D(context, object);
    if (toBeAdd == nullptr) {
        LOGE("AddPath to Path2D failed, to be added path is null.");
        return;
    }
    holderPath->AddPath(toBeAdd);
}

void V8CanvasBridge::Path2DSetTransform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 6 parameters: setTransform(a, b, c, d, e, f)
    if (args.Length() != 6) {
        LOGE("Call Path2D SetTransform failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D SetTransform failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D SetTransform failed, holderPath is null.");
        return;
    }
    holderPath->SetTransform(args[0]->NumberValue(context).ToChecked(),
                             args[1]->NumberValue(context).ToChecked(),
                             args[2]->NumberValue(context).ToChecked(),
                             args[3]->NumberValue(context).ToChecked(),
                             args[4]->NumberValue(context).ToChecked(),
                             args[5]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DMoveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: moveTo(x, y)
    if (args.Length() != 2) {
        LOGE("Call Path2D Arc MoveTo, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D MoveTo failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D MoveTo failed, holderPath is null.");
        return;
    }
    holderPath->MoveTo(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DLineTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: lineTo(x, y)
    if (args.Length() != 2) {
        LOGE("Call Path2D LineTo failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D LineTo failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D LineTo failed, holderPath is null.");
        return;
    }
    holderPath->LineTo(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DArc(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 5 or 6 parameters: arc(x, y, radius, startAngle, endAngle, anticlockwise?)
    if (args.Length() < 5 || args.Length() > 6) {
        LOGE("Call Path2D Arc failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D Arc failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Arc failed, holderPath is null.");
        return;
    }
    bool anticlockwise = false;
    if (args.Length() == 6) {
        anticlockwise = args[5]->ToBoolean(isolate)->Value();
    }
    holderPath->Arc(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked(),
                    args[2]->NumberValue(context).ToChecked(), args[3]->NumberValue(context).ToChecked(),
                    args[4]->NumberValue(context).ToChecked(), anticlockwise);
}

void V8CanvasBridge::Path2DArcTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 5 parameters: arcTo(x1, y1, x2, y2, radius)
    if (args.Length() != 5) {
        LOGE("Call Path2D ArcTo failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D ArcTo failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D ArcTo failed, holderPath is null.");
        return;
    }
    holderPath->ArcTo(args[0]->NumberValue(context).ToChecked(),
                      args[1]->NumberValue(context).ToChecked(),
                      args[2]->NumberValue(context).ToChecked(),
                      args[3]->NumberValue(context).ToChecked(),
                      args[4]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DQuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 4 parameters: quadraticCurveTo(cpx, cpy, x, y)
    if (args.Length() != 4) {
        LOGE("Call Path2D QuadraticCurveTo failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D QuadraticCurveTo failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D QuadraticCurveTo failed, holderPath is null.");
        return;
    }
    holderPath->QuadraticCurveTo(args[0]->NumberValue(context).ToChecked(),
                                 args[1]->NumberValue(context).ToChecked(),
                                 args[2]->NumberValue(context).ToChecked(),
                                 args[3]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DBezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 6 parameters: bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y)
    if (args.Length() != 6) {
        LOGE("Call Path2D BezierCurveTo failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D BezierCurveTo failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D BezierCurveTo failed, holderPath is null.");
        return;
    }
    holderPath->BezierCurveTo(args[0]->NumberValue(context).ToChecked(),
                              args[1]->NumberValue(context).ToChecked(),
                              args[2]->NumberValue(context).ToChecked(),
                              args[3]->NumberValue(context).ToChecked(),
                              args[4]->NumberValue(context).ToChecked(),
                              args[5]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DEllipse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 7 or 8 parameters: ellipse(x, y, radiusX, radiusY, rotation, startAngle, endAngle, anticlockwise?)
    if (args.Length() < 7 || args.Length() > 8) {
        LOGE("Call Path2D Ellipse failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D Ellipse failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Ellipse failed, holderPath is null.");
        return;
    }
    bool anticlockwise = false;
    if (args.Length() == 8) {
        anticlockwise = static_cast<int32_t>(args[7]->NumberValue(context).ToChecked()) == 1;
    }
    holderPath->Ellipse(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked(),
                        args[2]->NumberValue(context).ToChecked(), args[3]->NumberValue(context).ToChecked(),
                        args[4]->NumberValue(context).ToChecked(), args[5]->NumberValue(context).ToChecked(),
                        args[6]->NumberValue(context).ToChecked(), anticlockwise);
}

void V8CanvasBridge::Path2DRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 4 parameters: rect(x, y, width, height)
    if (args.Length() != 4) {
        LOGE("Call Path2D Rect failed, invalid args.");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D Rect failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D Rect failed, holderPath is null.");
        return;
    }
    holderPath->Rect(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked(),
                     args[2]->NumberValue(context).ToChecked(), args[3]->NumberValue(context).ToChecked());
}

void V8CanvasBridge::Path2DClosePath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    if (holderId < 0) {
        LOGE("Call Path2D ClosePath failed, unknown holder path.");
        return;
    }
    auto holderPath = path2Ds_[holderId];
    if (holderPath == nullptr) {
        LOGE("Call Path2D ClosePath failed, holderPath is null.");
        return;
    }
    holderPath->ClosePath();
}

RefPtr<CanvasPath2D> V8CanvasBridge::MakePath2D(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() == 1) {
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);
        auto context = isolate->GetCurrentContext();
        if (args[0]->IsString()) {
            v8::String::Utf8Value cmds(isolate, args[0]->ToString(context).ToLocalChecked());
            if (*cmds) {
                // Example: ctx.createPath2D("M250 150 L150 350 L350 350 Z")
                return AceType::MakeRefPtr<CanvasPath2D>(std::string(*cmds));
            }
        } else {
            auto object = args[0]->ToObject(context).ToLocalChecked();
            auto type = object->Get(context,
                v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
            v8::String::Utf8Value value(isolate, type->ToString(context).ToLocalChecked());
            if (*value && std::strcmp(*value, "path2d") == 0) {
                // Example: ctx.createPath2D(path1)
                return AceType::MakeRefPtr<CanvasPath2D>(GetPath2D(context, object));
            }
        }
    }
    // Example: ctx.createPath2D()
    return AceType::MakeRefPtr<CanvasPath2D>();
}

RefPtr<CanvasPath2D> V8CanvasBridge::GetPath2D(const v8::Local<v8::Context>& context,
    const v8::Local<v8::Object>& value)
{
    int32_t id = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "__id").ToLocalChecked())
                     .ToLocalChecked()->Int32Value(context).ToChecked();
    if (id < 0) {
        return nullptr;
    }
    return path2Ds_[id];
}

void V8CanvasBridge::CreatePattern(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: createPattern(image, repetition)
    if (args.Length() != 2) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto pattern = v8::Object::New(isolate);
    pattern->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
        v8::Integer::New(isolate, patternCount_)).ToChecked();
    pattern->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "pattern").ToLocalChecked()).ToChecked();
    if (!args[0]->IsObject()) {
        return;
    }
    double width = 0.0;
    double height = 0.0;
    std::string imageSrc;
    auto imageObject = args[0]->ToObject(context).ToLocalChecked();
    auto src = imageObject->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_SRC).ToLocalChecked()).ToLocalChecked();
    if (!src->IsString()) {
        ParseDomImage(args, width, height, imageSrc);
    } else {
        v8::Local<v8::Value> str;
        bool checked = src->ToString(context).ToLocal(&str);
        if (checked) {
            v8::String::Utf8Value utf8Value(isolate, str);
            if (*utf8Value) {
                imageSrc = *utf8Value;
            }
        }
        width = imageObject->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked())
                    .ToLocalChecked()
                    ->NumberValue(context)
                    .ToChecked();
        height = imageObject->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked())
                     .ToLocalChecked()
                     ->NumberValue(context)
                     .ToChecked();
    }
    auto strValue = args[1]->ToString(context).ToLocalChecked();
    v8::String::Utf8Value repeat(isolate, strValue);
    pattern_[patternCount_].SetImgSrc(imageSrc);
    pattern_[patternCount_].SetImageWidth(width);
    pattern_[patternCount_].SetImageHeight(height);
    if (*repeat) {
        pattern_[patternCount_].SetRepetition(*repeat);
    }
    ++patternCount_;
    args.GetReturnValue().Set(pattern);
}

void V8CanvasBridge::CreateImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 or 2 parameters: createImageData(imagedata) / createImageData(width, height)
    if (args.Length() != 1 && args.Length() != 2) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto imageData = v8::Object::New(isolate);
    int32_t width = 0;
    int32_t height = 0;

    if (args.Length() == 2) {
        width = args[0]->Int32Value(context).ToChecked();
        height = args[1]->Int32Value(context).ToChecked();
    }

    if (args.Length() == 1 && args[0]->IsObject()) {
        v8::Local<v8::Object> object = args[0]->ToObject(context).ToLocalChecked();
        width = object->Get(context, v8::String::NewFromUtf8(isolate, "width").ToLocalChecked())
                    .ToLocalChecked()
                    ->Int32Value(context)
                    .ToChecked();
        height = object->Get(context, v8::String::NewFromUtf8(isolate, "height").ToLocalChecked())
                     .ToLocalChecked()
                     ->Int32Value(context)
                     .ToChecked();
    }
    auto colorArray = v8::Array::New(isolate);
    uint32_t count = 0;
    for (auto i = 0; i < width; i++) {
        for (auto j = 0; j < height; j++) {
            colorArray->Set(context, count, v8::Integer::New(isolate, 255)).ToChecked();
            colorArray->Set(context, count + 1, v8::Integer::New(isolate, 255)).ToChecked();
            colorArray->Set(context, count + 2, v8::Integer::New(isolate, 255)).ToChecked();
            colorArray->Set(context, count + 3, v8::Integer::New(isolate, 255)).ToChecked();
            count += 4;
        }
    }

    imageData
        ->Set(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked(), v8::Int32::New(isolate, width))
        .ToChecked();
    imageData
        ->Set(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked(), v8::Int32::New(isolate, height))
        .ToChecked();
    imageData->Set(context, v8::String::NewFromUtf8(isolate, "data").ToLocalChecked(), colorArray).ToChecked();
    args.GetReturnValue().Set(imageData);
}

void V8CanvasBridge::PutImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 3 or 7 parameters: putImageData(data, dx, dy) / putImageData(data, dx, dy, dirtyX, dirtyY, dirtyW, dirtyH)
    if (args.Length() != 3 && args.Length() != 7 && !args[0]->IsObject()) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto object = args[0]->ToObject(context).ToLocalChecked();
    int32_t width = object->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked())
                        .ToLocalChecked()
                        ->Int32Value(context)
                        .ToChecked();
    int32_t height = object->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked())
                         .ToLocalChecked()
                         ->Int32Value(context)
                         .ToChecked();
    ImageData imageData;
    std::vector<std::string> array;
    ParseImageData(args, array, imageData);
    int32_t num = 0;
    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            if ((i >= imageData.dirtyY) && (i - imageData.dirtyY < imageData.dirtyHeight) && (j >= imageData.dirtyX) &&
                (j - imageData.dirtyX < imageData.dirtyWidth)) {
                uint32_t flag = j + width * i;
                if (array.size() > (4 * flag + 3)) {
                    auto red = StringToInt(array[4 * flag]);
                    auto green = StringToInt(array[4 * flag + 1]);
                    auto blue = StringToInt(array[4 * flag + 2]);
                    auto alpha = StringToInt(array[4 * flag + 3]);
                    if (num < imageData.dirtyWidth * imageData.dirtyHeight) {
                        imageData.data.emplace_back(Color::FromARGB(alpha, red, green, blue));
                    }
                    ++num;
                }
            }
        }
    }

    auto task = [imageData](const RefPtr<CanvasTaskPool>& pool) { pool->PutImageData(imageData); };
    PushTaskToPage(context, args.Holder(), task);
}

void V8CanvasBridge::ParseImageData(
    const v8::FunctionCallbackInfo<v8::Value>& args, std::vector<std::string>& array, ImageData& imageData)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto object = args[0]->ToObject(context).ToLocalChecked();
    int32_t width = object->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked())
                        .ToLocalChecked()
                        ->Int32Value(context)
                        .ToChecked();
    int32_t height = object->Get(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked())
                         .ToLocalChecked()
                         ->Int32Value(context)
                         .ToChecked();
    auto data = object->Get(context, v8::String::NewFromUtf8(isolate, "data").ToLocalChecked())
                    .ToLocalChecked()
                    ->ToString(context)
                    .ToLocalChecked();
    v8::String::Utf8Value value(isolate, data);
    if (*value) {
        StringUtils::StringSpliter(*value, ',', array);
    }
    imageData.x = args[1]->Int32Value(context).ToChecked();
    imageData.y = args[2]->Int32Value(context).ToChecked();
    imageData.dirtyWidth = width;
    imageData.dirtyHeight = height;

    if (args.Length() == 7) {
        imageData.dirtyX = args[3]->Int32Value(context).ToChecked();
        imageData.dirtyY = args[4]->Int32Value(context).ToChecked();
        imageData.dirtyWidth = args[5]->Int32Value(context).ToChecked();
        imageData.dirtyHeight = args[6]->Int32Value(context).ToChecked();
    }

    imageData.dirtyWidth = imageData.dirtyX < 0 ? std::min(imageData.dirtyX + imageData.dirtyWidth, width)
                                                : std::min(width - imageData.dirtyX, imageData.dirtyWidth);
    imageData.dirtyHeight = imageData.dirtyY < 0 ? std::min(imageData.dirtyY + imageData.dirtyHeight, height)
                                                 : std::min(height - imageData.dirtyY, imageData.dirtyHeight);
}

void V8CanvasBridge::GetImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 4 parameters: getImageData(sx, sy, sw, sh)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    Rect rect = GetRectParam(args, context);
    NodeId id = GetCurrentNodeId(context, args.Holder());
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }
    std::unique_ptr<ImageData> data;
    auto task = [id, page, &rect, &data]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);

    auto imageData = v8::Object::New(isolate);
    imageData
        ->Set(context, v8::String::NewFromUtf8(isolate, IMAGE_WIDTH).ToLocalChecked(),
            v8::Integer::New(isolate, data->dirtyWidth))
        .ToChecked();
    imageData
        ->Set(context, v8::String::NewFromUtf8(isolate, IMAGE_HEIGHT).ToLocalChecked(),
            v8::Integer::New(isolate, data->dirtyHeight))
        .ToChecked();
    uint32_t count = 0;
    auto colorArray = v8::Array::New(isolate);
    for (auto i = 0; i < data->dirtyHeight; ++i) {
        for (auto j = 0; j < data->dirtyWidth; ++j) {
            int32_t idx = i * data->dirtyWidth + j;
            auto pixel = data->data[idx];
            colorArray->Set(context, count, v8::Integer::New(isolate, pixel.GetRed())).ToChecked();
            colorArray->Set(context, count + 1, v8::Integer::New(isolate, pixel.GetGreen())).ToChecked();
            colorArray->Set(context, count + 2, v8::Integer::New(isolate, pixel.GetBlue())).ToChecked();
            colorArray->Set(context, count + 3, v8::Integer::New(isolate, pixel.GetAlpha())).ToChecked();
            count += 4;
        }
    }
    imageData->Set(context, v8::String::NewFromUtf8(isolate, "data").ToLocalChecked(), colorArray).ToChecked();
    args.GetReturnValue().Set(imageData);
}

void V8CanvasBridge::GetJsonData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 parameters: GetJsonData(path)
    if (args.Length() != 1) {
        return;
    }

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::String::Utf8Value arg(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!(*arg)) {
        return;
    }
    std::string path = *arg;

    NodeId id = GetCurrentNodeId(context, args.Holder());
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }

    std::string jsonData;
    auto task = [id, page, path, &jsonData]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);

    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, jsonData.c_str()).ToLocalChecked());
}

void V8CanvasBridge::TransferFromImageBitmap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();

    auto object = args[0]->ToObject(ctx).ToLocalChecked();
    int32_t id = object->Get(ctx, v8::String::NewFromUtf8(isolate, "__bridgeId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(ctx).ToChecked();
    int32_t bridgeId = id < 0 ? 0 : id;
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }
    RefPtr<V8OffscreenCanvasBridge> bridge = AceType::DynamicCast<V8OffscreenCanvasBridge>(
        (*page)->GetOffscreenCanvasBridgeById(bridgeId));
    if (!bridge) {
        return;
    }
    auto offscreenCanvas = bridge->GetOffscreenCanvas();
    auto task = [offscreenCanvas](const RefPtr<CanvasTaskPool>& pool) {
        pool->TransferFromImageBitmap(offscreenCanvas);
    };
    PushTaskToPage(ctx, args.Holder(), task);
}

void V8CanvasBridge::DrawBitmapMesh(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();

    auto object = args[0]->ToObject(ctx).ToLocalChecked();
    int32_t id = object->Get(ctx, v8::String::NewFromUtf8(isolate, "__bridgeId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(ctx).ToChecked();
    int32_t bridgeId = id < 0 ? 0 : id;
    auto mesh = GetDashValue(args, 1);
    double column = args[2]->Int32Value(ctx).ToChecked();
    double row = args[3]->Int32Value(ctx).ToChecked();
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }
    RefPtr<V8OffscreenCanvasBridge> bridge = AceType::DynamicCast<V8OffscreenCanvasBridge>(
        (*page)->GetOffscreenCanvasBridgeById(bridgeId));
    if (!bridge) {
        return;
    }
    auto offscreenCanvas = bridge->GetOffscreenCanvas();
    auto task = [offscreenCanvas, mesh, column, row](const RefPtr<CanvasTaskPool>& pool) {
        pool->DrawBitmapMesh(offscreenCanvas, mesh, (int)column, (int)row);
    };
    PushTaskToPage(ctx, args.Holder(), task);
}

Gradient V8CanvasBridge::GetGradient(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value)
{
    int32_t id = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "__id").ToLocalChecked())
                     .ToLocalChecked()->Int32Value(context).ToChecked();
    if (id < 0) {
        return Gradient();
    }
    return gradientColors_[id];
}

Pattern V8CanvasBridge::GetPattern(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value)
{
    int32_t id = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "__id").ToLocalChecked())
                     .ToLocalChecked()->Int32Value(context).ToChecked();
    if (id < 0) {
        return Pattern();
    }
    return pattern_[id];
}

void V8CanvasBridge::FillStyleGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__fillStyle").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::FillStyleSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    if (value->IsString()) {
        v8::String::Utf8Value colorStr(isolate, value->ToString(context).ToLocalChecked());
        if (*colorStr) {
            auto color = Color::FromString(*colorStr);
            auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFillColor(color); };
            PushTaskToPage(context, info.Holder(), task);
        }
    } else {
        auto object = value->ToObject(context).ToLocalChecked();
        auto typeVal =
            object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value type(isolate, typeVal->ToString(context).ToLocalChecked());
        if (*type) {
            if (std::strcmp(*type, "gradient") == 0) {
                auto gradient = GetGradient(context, object);
                auto task = [gradient](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFillGradient(gradient); };
                PushTaskToPage(context, info.Holder(), task);
            } else if (std::strcmp(*type, "pattern") == 0) {
                auto pattern = GetPattern(context, object);
                auto task = [pattern](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFillPattern(pattern); };
                PushTaskToPage(context, info.Holder(), task);
            } else {
                LOGW("No such type for fill style.");
            }
        }
    }
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__fillStyle").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::StrokeStyleGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = info.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__strokeStyle").ToLocalChecked())
                                     .ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::StrokeStyleSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    if (value->IsString()) {
        v8::String::Utf8Value colorStr(isolate, value->ToString(context).ToLocalChecked());
        if (*colorStr) {
            auto color = Color::FromString(*colorStr);
            auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateStrokeColor(color); };
            PushTaskToPage(context, info.Holder(), task);
        }
    } else {
        auto object = value->ToObject(context).ToLocalChecked();
        auto typeVal =
            object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value type(isolate, typeVal->ToString(context).ToLocalChecked());
        if (*type) {
            if (std::strcmp(*type, "gradient") == 0) {
                auto gradient = GetGradient(context, object);
                auto task = [gradient](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateStrokeGradient(gradient); };
                PushTaskToPage(context, info.Holder(), task);
            } else if (std::strcmp(*type, "pattern") == 0) {
                auto pattern = GetPattern(context, object);
                auto task = [pattern](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateStrokePattern(pattern); };
                PushTaskToPage(context, info.Holder(), task);
            } else {
                LOGW("No such type for stroke style.");
            }
        }
    }
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__strokeStyle").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::LineCapGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__lineCap").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::LineCapSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    if (value->IsString()) {
        v8::String::Utf8Value utf8Value(
            info.GetIsolate(), value->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
        static const std::unordered_map<std::string, LineCapStyle> lineCapTable = {
            { "round", LineCapStyle::ROUND },
            { "butt", LineCapStyle::BUTT },
            { "square", LineCapStyle::SQUARE },
        };
        if (*utf8Value) {
            auto lineCap = ConvertStrToEnum(*utf8Value, lineCapTable, LineCapStyle::ROUND);
            auto task = [lineCap](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineCap(lineCap); };
            PushTaskToPage(info.GetIsolate()->GetCurrentContext(), info.Holder(), task);
        }
    }
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineCap").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::LineJoinGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__lineJoin").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::LineJoinSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    v8::String::Utf8Value utf8Value(isolate, value->ToString(context).ToLocalChecked());
    static const std::unordered_map<std::string, LineJoinStyle> lineJoinTable = {
        { "bevel", LineJoinStyle::BEVEL },
        { "round", LineJoinStyle::ROUND },
        { "miter", LineJoinStyle::MITER },
    };
    if (*utf8Value) {
        auto lineJoin = ConvertStrToEnum(*utf8Value, lineJoinTable, LineJoinStyle::MITER);
        auto task = [lineJoin](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineJoin(lineJoin); };
        PushTaskToPage(context, info.Holder(), task);
        info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineJoin").ToLocalChecked(), value).ToChecked();
    }
}

void V8CanvasBridge::MiterLimitGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__miterLimit").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::MiterLimitSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto limit = value->NumberValue(context).ToChecked();
    auto task = [limit](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateMiterLimit(limit); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__miterLimit").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::LineWidthGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.This()->Get(context, v8::String::NewFromUtf8(isolate, "__lineWidth").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}
void V8CanvasBridge::LineWidthSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto lineWidth = value->NumberValue(context).ToChecked();
    auto task = [lineWidth](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineWidth(lineWidth); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineWidth").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::TextAlignGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__textAlign").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::TextAlignSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    v8::String::Utf8Value alignStr(isolate, value->ToString(context).ToLocalChecked());
    if (*alignStr) {
        auto align = ConvertStrToTextAlign(*alignStr);
        auto task = [align](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateTextAlign(align); };
        PushTaskToPage(context, info.Holder(), task);
        info.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__textAlign").ToLocalChecked(), value)
            .ToChecked();
    }
}

void V8CanvasBridge::TextBaselineGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = info.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__textBaseline").ToLocalChecked())
                                     .ToLocalChecked();
    info.GetReturnValue().Set(value);
}
void V8CanvasBridge::TextBaselineSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    v8::String::Utf8Value baselineStr(isolate, value->ToString(context).ToLocalChecked());
    if (*baselineStr) {
        auto baseline = ConvertStrToEnum(*baselineStr, BASELINE_TABLE, TextBaseline::ALPHABETIC);
        auto task = [baseline](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateTextBaseline(baseline); };
        PushTaskToPage(context, info.Holder(), task);
        info.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__textBaseline").ToLocalChecked(), value)
            .ToChecked();
    }
}

void V8CanvasBridge::FontGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__font").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::FontSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    v8::String::Utf8Value fontStr(isolate, value->ToString(context).ToLocalChecked());
    if (!(*fontStr)) {
        return;
    }
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(*fontStr, ' ', fontProps);
    bool updateFontStyle = false;
    for (const auto& fontProp : fontProps) {
        if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            auto weight = ConvertStrToFontWeight(fontProp);
            auto task = [weight](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontWeight(weight); };
            PushTaskToPage(context, info.Holder(), task);
        } else if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            updateFontStyle = true;
            auto fontStyle = ConvertStrToFontStyle(fontProp);
            auto task = [fontStyle](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontStyle(fontStyle); };
            PushTaskToPage(context, info.Holder(), task);
        } else if (FONT_FAMILIES.find(fontProp) != FONT_FAMILIES.end()) {
            auto families = ConvertStrToFontFamilies(fontProp);
            auto task = [families](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontFamilies(families); };
            PushTaskToPage(context, info.Holder(), task);
        } else if (fontProp.find("px") != std::string::npos) {
            std::string fontSize = fontProp.substr(0, fontProp.size() - 2);
            auto size = Dimension(StringToDouble(fontProp));
            auto task = [size](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontSize(size); };
            PushTaskToPage(context, info.Holder(), task);
        } else {
            LOGW("parse text error");
        }
    }
    if (!updateFontStyle) {
        auto task = [](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateFontStyle(FontStyle::NORMAL); };
        PushTaskToPage(context, info.Holder(), task);
    }
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__font").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::AlphaGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = info.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__globalAlpha").ToLocalChecked())
                                     .ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::AlphaSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto alpha = value->NumberValue(context).ToChecked();
    auto task = [alpha](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateGlobalAlpha(alpha); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__globalAlpha").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::CompositeOperationGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__globalCompositeOperation").ToLocalChecked())
            .ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::CompositeOperationSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    v8::String::Utf8Value typeStr(isolate, value->ToString(context).ToLocalChecked());
    if (!(*typeStr)) {
        return;
    }

    static const std::unordered_map<std::string, CompositeOperation> compositeOperationTable = {
        { "source-over", CompositeOperation::SOURCE_OVER },
        { "source-atop", CompositeOperation::SOURCE_ATOP },
        { "source-in", CompositeOperation::SOURCE_IN },
        { "source-out", CompositeOperation::SOURCE_OUT },
        { "destination-over", CompositeOperation::DESTINATION_OVER },
        { "destination-atop", CompositeOperation::DESTINATION_ATOP },
        { "destination-in", CompositeOperation::DESTINATION_IN },
        { "destination-out", CompositeOperation::DESTINATION_OUT },
        { "lighter", CompositeOperation::LIGHTER },
        { "copy", CompositeOperation::COPY },
        { "xor", CompositeOperation::XOR },
    };
    auto type = ConvertStrToEnum(*typeStr, compositeOperationTable, CompositeOperation::SOURCE_OVER);
    auto task = [type](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateCompositeOperation(type); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__globalCompositeOperation").ToLocalChecked(), value)
        .ToChecked();
}

void V8CanvasBridge::LineDashOffsetGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__lineDash").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}
void V8CanvasBridge::LineDashOffsetSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto dashoffset = value->NumberValue(context).ToChecked();
    auto task = [dashoffset](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateLineDashOffset(dashoffset); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineDash").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::ShadowBlurGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__shadowBlur").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::ShadowBlurSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto blur = value->NumberValue(context).ToChecked();
    auto task = [blur](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowBlur(blur); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__shadowBlur").ToLocalChecked(), value).ToChecked();
}

void V8CanvasBridge::ShadowColorGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = info.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__shadowColor").ToLocalChecked())
                                     .ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::ShadowColorSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    v8::String::Utf8Value colorStr(isolate, value->ToString(context).ToLocalChecked());
    if (*colorStr) {
        auto color = Color::FromString(*colorStr);
        auto task = [color](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowColor(color); };
        PushTaskToPage(context, info.Holder(), task);
        info.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__shadowColor").ToLocalChecked(), value)
            .ToChecked();
    }
}

void V8CanvasBridge::ShadowOffsetXGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetX").ToLocalChecked())
            .ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::ShadowOffsetXSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto offsetX = value->NumberValue(context).ToChecked();
    auto task = [offsetX](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowOffsetX(offsetX); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetX").ToLocalChecked(), value)
        .ToChecked();
}

void V8CanvasBridge::ShadowOffsetYGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        info.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetY").ToLocalChecked())
            .ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::ShadowOffsetYSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = info[0];

    auto offsetY = value->NumberValue(context).ToChecked();
    auto task = [offsetY](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateShadowOffsetY(offsetY); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetY").ToLocalChecked(), value)
        .ToChecked();
}

void V8CanvasBridge::SmoothingEnabledGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> value = info.Holder()->Get(context,
        v8::String::NewFromUtf8(isolate, "__imageSmoothingEnabled").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::SmoothingEnabledSetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() != 1 || !info[0]->IsBoolean()) {
        return;
    }
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto enabled = info[0]->ToBoolean(isolate)->Value();
    auto task = [enabled](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateSmoothingEnabled(enabled); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context,
        v8::String::NewFromUtf8(isolate, "__imageSmoothingEnabled").ToLocalChecked(), info[0]).ToChecked();
}

void V8CanvasBridge::SmoothingQualityGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> value = info.Holder()->Get(context,
        v8::String::NewFromUtf8(isolate, "__imageSmoothingQuality").ToLocalChecked()).ToLocalChecked();
    info.GetReturnValue().Set(value);
}

void V8CanvasBridge::SmoothingQualitySetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() != 1 || !info[0]->IsString()) {
        return;
    }
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::String::Utf8Value type(isolate, info[0]->ToString(context).ToLocalChecked());
    if (!(*type) || QUALITY_TYPE.find(*type) == QUALITY_TYPE.end()) {
        return;
    }
    std::string quality = *type;
    auto task = [quality](const RefPtr<CanvasTaskPool>& pool) { pool->UpdateSmoothingQuality(quality); };
    PushTaskToPage(context, info.Holder(), task);
    info.Holder()->Set(context,
        v8::String::NewFromUtf8(isolate, "__imageSmoothingQuality").ToLocalChecked(), info[0]).ToChecked();
}

void V8CanvasBridge::WidthGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    NodeId id = GetCurrentNodeId(context, info.Holder());
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }

    double width = 0.0;
    auto task = [id, page, &width]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);

    info.GetReturnValue().Set(v8::Number::New(isolate, width));
}

void V8CanvasBridge::HeightGetter(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    NodeId id = GetCurrentNodeId(context, info.Holder());
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (!page) {
        return;
    }

    double height = 0.0;
    auto task = [id, page, &height]() {
        auto canvas = AceType::DynamicCast<DOMCanvas>((*page)->GetDomDocument()->GetDOMNodeById(id));
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
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostSyncTaskToPage(task);

    info.GetReturnValue().Set(v8::Number::New(isolate, height));
}

} // namespace OHOS::Ace::Framework
