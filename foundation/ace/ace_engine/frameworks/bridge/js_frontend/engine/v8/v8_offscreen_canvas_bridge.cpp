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

#include "frameworks/bridge/js_frontend/engine/v8/v8_offscreen_canvas_bridge.h"

#include "frameworks/bridge/js_frontend/engine/v8/v8_engine.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr char IMAGE_SRC[] = "src";
constexpr char IMAGE_WIDTH[] = "width";
constexpr char IMAGE_HEIGHT[] = "height";

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

const std::set<std::string> FONT_WEIGHTS = { "normal", "bold", "lighter", "bolder", "100", "200", "300", "400", "500",
    "600", "700", "800", "900" };
const std::set<std::string> FONT_STYLES = { "italic", "oblique", "normal" };
const std::set<std::string> FONT_FAMILIES = { "sans-serif", "serif", "monospace" };
const std::set<std::string> QUALITY_TYPE = { "low", "medium", "high" }; // Default value is low.

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

inline Rect GetRectParam(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 4 parameters: rect(x, y, width, height)
    if (args.Length() != 4) {
        return Rect();
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

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
    auto textDirection =
        value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "textDirection").ToLocalChecked())
            .ToLocalChecked();

    // parse font styles
    v8::String::Utf8Value alignStr(context->GetIsolate(), textAlign);
    v8::String::Utf8Value baselineStr(context->GetIsolate(), textBaseline);
    if (*alignStr) {
        state.SetTextAlign(ConvertStrToTextAlign(*alignStr));
    }
    v8::String::Utf8Value textDirectionStr(context->GetIsolate(), textDirection);
    if (*textDirectionStr) {
        state.SetOffTextDirection(ConvertStrToTextDirection(*textDirectionStr));
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

inline PaintState ParseTextDirection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();

    PaintState state;
    auto textDirection =
        args.Holder()->Get(ctx, v8::String::NewFromUtf8(isolate, "textDirection").ToLocalChecked()).ToLocalChecked();
    v8::String::Utf8Value textDirectionStr(isolate, textDirection);
    if (*textDirectionStr) {
        state.SetOffTextDirection(ConvertStrToTextDirection(*textDirectionStr));
    }
    return state;
}

inline int32_t GetCurrentBridgeId(const v8::Local<v8::Context>& ctx, v8::Local<v8::Object> value)
{
    v8::Isolate* isolate = ctx->GetIsolate();
    v8::HandleScope handleScope(isolate);
    int32_t id = value->Get(ctx, v8::String::NewFromUtf8(isolate, "__bridgeId").ToLocalChecked())
                     .ToLocalChecked()
                     ->Int32Value(ctx)
                     .ToChecked();
    return id < 0 ? 0 : id;
}

RefPtr<V8OffscreenCanvasBridge> GetOffscreenCanvasBridge(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();

    RefPtr<V8OffscreenCanvasBridge> bridge = nullptr;
    int32_t bridgeId = GetCurrentBridgeId(ctx, args.Holder());
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (page) {
        bridge = AceType::DynamicCast<V8OffscreenCanvasBridge>((*page)->GetOffscreenCanvasBridgeById(bridgeId));
    }
    return bridge;
}

RefPtr<OffscreenCanvas> GlobalGetOffscreenCanvas(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto bridge = GetOffscreenCanvasBridge(args);
    if (bridge) {
        return bridge->GetOffscreenCanvas();
    }
    return nullptr;
}

} // namespace

int32_t V8OffscreenCanvasBridge::gradientCount_ = 0;
int32_t V8OffscreenCanvasBridge::patternCount_ = 0;
int32_t V8OffscreenCanvasBridge::path2dCount_ = 0;
int32_t V8OffscreenCanvasBridge::globalBridgeId_ = 0;
std::unordered_map<int32_t, Pattern> V8OffscreenCanvasBridge::pattern_;
std::unordered_map<int32_t, Gradient> V8OffscreenCanvasBridge::gradientColors_;
std::unordered_map<int32_t, RefPtr<CanvasPath2D>> V8OffscreenCanvasBridge::path2Ds_;

V8OffscreenCanvasBridge::V8OffscreenCanvasBridge(
    const RefPtr<PipelineContext>& pipelineContext, const int32_t width, const int32_t height)
{
    width_ = width;
    height_ = height;
    bridgeId_ = globalBridgeId_++;
    if (pipelineContext) {
        offscreenCanvas_ = pipelineContext->CreateOffscreenCanvas(width_, height_);
    }
}

void V8OffscreenCanvasBridge::GetBridge(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> res = v8::Object::New(isolate);
    res->Set(ctx, v8::String::NewFromUtf8(isolate, "__bridgeId").ToLocalChecked(), v8::Int32::New(isolate, bridgeId_))
        .ToChecked();
    const std::unordered_map<const char*, v8::Local<v8::Function>> propertyTable = {
        { "getContext", v8::Function::New(ctx, GetContext, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "transferToImageBitmap",
            v8::Function::New(ctx, TransferToImageBitmap, v8::Local<v8::Value>(), 0).ToLocalChecked() },
        { "toDataURL", v8::Function::New(ctx, ToDataURL, v8::Local<v8::Value>(), 2).ToLocalChecked() },
    };
    for (const auto& iter : propertyTable) {
        res->Set(ctx, v8::String::NewFromUtf8(isolate, iter.first).ToLocalChecked(), iter.second).ToChecked();
    }
    args.GetReturnValue().Set(res);
}

void V8OffscreenCanvasBridge::GetContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();

    auto bridge = GetOffscreenCanvasBridge(args);
    if (!bridge) {
        return;
    }

    auto renderContext = v8::Object::New(ctx->GetIsolate());
    renderContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__bridgeId").ToLocalChecked(),
            v8::Int32::New(ctx->GetIsolate(), bridge->GetBridgeId()))
        .ToChecked();

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
        { "isPointInStroke", v8::Function::New(ctx, IsPointInStroke, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "isPointInPath", v8::Function::New(ctx, IsPointInPath, v8::Local<v8::Value>(), 2).ToLocalChecked() },
        { "resetTransform", v8::Function::New(ctx, ResetTransform, v8::Local<v8::Value>(), 0).ToLocalChecked() },
    };
    for (const auto& iter : contextTable) {
        renderContext->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), iter.first).ToLocalChecked(), iter.second)
            .ToChecked();
    }
    static const std::vector<std::tuple<std::string, v8::FunctionCallback, v8::FunctionCallback>> v8AnimationFuncs = {
        { "fillStyle", V8OffscreenCanvasBridge::FillStyleGetter, V8OffscreenCanvasBridge::FillStyleSetter },
        { "strokeStyle", V8OffscreenCanvasBridge::StrokeStyleGetter, V8OffscreenCanvasBridge::StrokeStyleSetter },
        { "lineCap", V8OffscreenCanvasBridge::LineCapGetter, V8OffscreenCanvasBridge::LineCapSetter },
        { "lineJoin", V8OffscreenCanvasBridge::LineJoinGetter, V8OffscreenCanvasBridge::LineJoinSetter },
        { "miterLimit", V8OffscreenCanvasBridge::MiterLimitGetter, V8OffscreenCanvasBridge::MiterLimitSetter },
        { "lineWidth", V8OffscreenCanvasBridge::LineWidthGetter, V8OffscreenCanvasBridge::LineWidthSetter },
        { "textAlign", V8OffscreenCanvasBridge::TextAlignGetter, V8OffscreenCanvasBridge::TextAlignSetter },
        { "textBaseline", V8OffscreenCanvasBridge::TextBaselineGetter, V8OffscreenCanvasBridge::TextBaselineSetter },
        { "font", V8OffscreenCanvasBridge::FontGetter, V8OffscreenCanvasBridge::FontSetter },
        { "globalAlpha", V8OffscreenCanvasBridge::AlphaGetter, V8OffscreenCanvasBridge::AlphaSetter },
        { "globalCompositeOperation", V8OffscreenCanvasBridge::CompositeOperationGetter,
            V8OffscreenCanvasBridge::CompositeOperationSetter },
        { "lineDashOffset", V8OffscreenCanvasBridge::LineDashOffsetGetter,
            V8OffscreenCanvasBridge::LineDashOffsetSetter },
        { "shadowBlur", V8OffscreenCanvasBridge::ShadowBlurGetter, V8OffscreenCanvasBridge::ShadowBlurSetter },
        { "shadowColor", V8OffscreenCanvasBridge::ShadowColorGetter, V8OffscreenCanvasBridge::ShadowColorSetter },
        { "shadowOffsetX", V8OffscreenCanvasBridge::ShadowOffsetXGetter, V8OffscreenCanvasBridge::ShadowOffsetXSetter },
        { "shadowOffsetY", V8OffscreenCanvasBridge::ShadowOffsetYGetter, V8OffscreenCanvasBridge::ShadowOffsetYSetter },
        { "imageSmoothingEnabled", V8OffscreenCanvasBridge::SmoothingEnabledGetter,
            V8OffscreenCanvasBridge::SmoothingEnabledSetter },
        { "imageSmoothingQuality", V8OffscreenCanvasBridge::SmoothingQualityGetter,
            V8OffscreenCanvasBridge::SmoothingQualitySetter },
        { "filter", V8OffscreenCanvasBridge::FilterParamGetter, V8OffscreenCanvasBridge::FilterParamSetter }
    };
    for (const auto& item : v8AnimationFuncs) {
        auto getter_templ = v8::FunctionTemplate::New(ctx->GetIsolate(), std::get<1>(item));
        auto setter_templ = v8::FunctionTemplate::New(ctx->GetIsolate(), std::get<2>(item));
        renderContext->SetAccessorProperty(
            v8::String::NewFromUtf8(ctx->GetIsolate(), std::get<0>(item).c_str()).ToLocalChecked(),
            getter_templ->GetFunction(ctx).ToLocalChecked(), setter_templ->GetFunction(ctx).ToLocalChecked());
    }
    args.GetReturnValue().Set(renderContext);
}

void V8OffscreenCanvasBridge::CreateLinearGradient(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::CreateLinearGradient");
    // 4 parameters: createLinearGradient(x0, y0, x1, y1)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Object> gradient = v8::Object::New(isolate);
    gradient
        ->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, "gradient").ToLocalChecked())
        .ToChecked();
    gradient
        ->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
            v8::Integer::New(isolate, gradientCount_))
        .ToChecked();
    gradient
        ->Set(context, v8::String::NewFromUtf8(isolate, "addColorStop").ToLocalChecked(),
            v8::Function::New(context, AddColorStop, v8::Local<v8::Value>(), 2).ToLocalChecked())
        .ToChecked();
    Offset beginOffset = Offset(args[0]->NumberValue(context).ToChecked(), args[1]->NumberValue(context).ToChecked());
    Offset endOffset = Offset(args[2]->NumberValue(context).ToChecked(), args[3]->NumberValue(context).ToChecked());
    gradientColors_[gradientCount_].SetType(GradientType::LINEAR);
    gradientColors_[gradientCount_].SetBeginOffset(beginOffset);
    gradientColors_[gradientCount_].SetEndOffset(endOffset);
    ++gradientCount_;
    args.GetReturnValue().Set(gradient);
}

void V8OffscreenCanvasBridge::CreateRadialGradient(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::CreateRadialGradient");
    // 6 parameters: createRadialGradient(x0, y0, r0, x1, y1, r1)
    if (args.Length() != 6) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Object> gradient = v8::Object::New(isolate);
    gradient
        ->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, "gradient").ToLocalChecked())
        .ToChecked();
    gradient
        ->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
            v8::Integer::New(isolate, gradientCount_))
        .ToChecked();
    gradient
        ->Set(context, v8::String::NewFromUtf8(isolate, "addColorStop").ToLocalChecked(),
            v8::Function::New(context, AddColorStop, v8::Local<v8::Value>(), 2).ToLocalChecked())
        .ToChecked();
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

void V8OffscreenCanvasBridge::AddColorStop(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                     .ToLocalChecked()
                     ->Int32Value(context)
                     .ToChecked();
    if (id < 0) {
        return;
    }
    gradientColors_[id].AddColor(color);
}

void V8OffscreenCanvasBridge::FillRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::FillRect");
    // 4 parameters: fillRect(x, y, width, height)
    if (args.Length() != 4) {
        return;
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        Rect rect = GetRectParam(args);
        offscreenCanvas->FillRect(rect);
    }
}

void V8OffscreenCanvasBridge::StrokeRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::StrokeRect");
    // 4 parameters: strokeRect(x, y, width, height)
    if (args.Length() != 4) {
        return;
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        Rect rect = GetRectParam(args);
        offscreenCanvas->StrokeRect(rect);
    }
}

void V8OffscreenCanvasBridge::ClearRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::ClearRect");
    // 4 parameters: clearRect(x, y, width, height)
    if (args.Length() != 4) {
        return;
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        Rect rect = GetRectParam(args);
        offscreenCanvas->ClearRect(rect);
    }
}

void V8OffscreenCanvasBridge::FillText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::FillText");
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
    auto textState = ParseTextDirection(args);
    std::string text = *arg;
    double x = args[1]->NumberValue(context).ToChecked();
    double y = args[2]->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->FillText(text, x, y, textState);
    }
}

void V8OffscreenCanvasBridge::StrokeText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("V8OffscreenCanvasBridge::StrokeText");
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
    auto textState = ParseTextDirection(args);
    std::string text = *arg;
    double x = args[1]->NumberValue(context).ToChecked();
    double y = args[2]->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->StrokeText(text, x, y, textState);
    }
}

void V8OffscreenCanvasBridge::MeasureText(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    double width = 0.0;
    double height = 0.0;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        width = offscreenCanvas->MeasureText(text, textState);
        height = offscreenCanvas->MeasureTextHeight(text, textState);
    }
    v8::Local<v8::Object> textMetrics = v8::Object::New(isolate);
    textMetrics
        ->Set(context, v8::String::NewFromUtf8(isolate, "width").ToLocalChecked(), v8::Number::New(isolate, width))
        .ToChecked();
    textMetrics
        ->Set(context, v8::String::NewFromUtf8(isolate, "height").ToLocalChecked(), v8::Number::New(isolate, height))
        .ToChecked();
    args.GetReturnValue().Set(textMetrics);
}

void V8OffscreenCanvasBridge::BeginPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 0 parameter: beginPath()
    if (args.Length() != 0) {
        return;
    }

    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->BeginPath();
    }
}

void V8OffscreenCanvasBridge::ClosePath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 0 parameter: closePath()
    if (args.Length() != 0) {
        return;
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->ClosePath();
    }
}

void V8OffscreenCanvasBridge::MoveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->MoveTo(x, y);
    }
}

void V8OffscreenCanvasBridge::LineTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->LineTo(x, y);
    }
}

void V8OffscreenCanvasBridge::BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->BezierCurveTo(param);
    }
}

void V8OffscreenCanvasBridge::QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->QuadraticCurveTo(param);
    }
}

void V8OffscreenCanvasBridge::Arc(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Arc(param);
    }
}

void V8OffscreenCanvasBridge::ArcTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->ArcTo(param);
    }
}

void V8OffscreenCanvasBridge::Ellipse(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Ellipse(param);
    }
}

void V8OffscreenCanvasBridge::DrawRect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Rect rect = GetRectParam(args);
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->AddRect(rect);
    }
}

void V8OffscreenCanvasBridge::Fill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Fill();
    }
}

void V8OffscreenCanvasBridge::Stroke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    // 0 or 1 parameter: ctx.stroke() / ctx.stroke(path)
    if (args.Length() == 1) {
        auto object = args[0]->ToObject(context).ToLocalChecked();
        auto type = object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
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
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            offscreenCanvas->Stroke(path);
        }
        return;
    }
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Stroke();
    }
}

void V8OffscreenCanvasBridge::Clip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Clip();
    }
}

void V8OffscreenCanvasBridge::Restore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Restore();
    }
}

void V8OffscreenCanvasBridge::Save(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Save();
    }
}

void V8OffscreenCanvasBridge::Rotate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 1 parameter: rotate(angle)
    if (args.Length() != 1) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    double angle = args[0]->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Rotate(angle);
    }
}

void V8OffscreenCanvasBridge::Scale(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Scale(x, y);
    }
}

void V8OffscreenCanvasBridge::SetTransform(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetTransform(param);
    }
}

void V8OffscreenCanvasBridge::Transform(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Transform(param);
    }
}

void V8OffscreenCanvasBridge::Translate(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->Translate(x, y);
    }
}

void V8OffscreenCanvasBridge::SetLineDash(const v8::FunctionCallbackInfo<v8::Value>& args)
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
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            offscreenCanvas->SetLineDash(segments);
        }
    }
}

void V8OffscreenCanvasBridge::GetLineDash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto val =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "lineDash").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(val);
}

void V8OffscreenCanvasBridge::ParseDomImage(
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

void V8OffscreenCanvasBridge::DrawImage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() < 3 || !args[0]->IsObject()) {
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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->DrawImage(image, width, height);
    }
}

void V8OffscreenCanvasBridge::CreatePath2D(const v8::FunctionCallbackInfo<v8::Value>& args)
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

void V8OffscreenCanvasBridge::Path2DAddPath(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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
    auto type = object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
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

void V8OffscreenCanvasBridge::Path2DSetTransform(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DMoveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DLineTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DArc(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DArcTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DQuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DBezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DEllipse(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DRect(const v8::FunctionCallbackInfo<v8::Value>& args)
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
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

void V8OffscreenCanvasBridge::Path2DClosePath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto object = args.Holder();
    int32_t holderId = object->Get(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked())
                           .ToLocalChecked()
                           ->Int32Value(context)
                           .ToChecked();
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

RefPtr<CanvasPath2D> V8OffscreenCanvasBridge::MakePath2D(const v8::FunctionCallbackInfo<v8::Value>& args)
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
            auto type =
                object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
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

RefPtr<CanvasPath2D> V8OffscreenCanvasBridge::GetPath2D(
    const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value)
{
    int32_t id = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "__id").ToLocalChecked())
                     .ToLocalChecked()
                     ->Int32Value(context)
                     .ToChecked();
    if (id < 0) {
        return nullptr;
    }
    return path2Ds_[id];
}

void V8OffscreenCanvasBridge::TransferToImageBitmap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto bridge = GetOffscreenCanvasBridge(args);
    if (bridge) {
        auto imageBitmap = v8::Object::New(isolate);
        imageBitmap
            ->Set(context, v8::String::NewFromUtf8(isolate, "__bridgeId").ToLocalChecked(),
                v8::Integer::New(isolate, bridge->GetBridgeId()))
            .ToChecked();
        args.GetReturnValue().Set(imageBitmap);
    }
}

void V8OffscreenCanvasBridge::ToDataURL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 2) {
        LOGE("invalid args");
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::String::Utf8Value arg(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!(*arg)) {
        return;
    }
    double quality = args[1]->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    std::string url;
    if (offscreenCanvas) {
        url = offscreenCanvas->ToDataURL(*arg, quality);
    }
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked());
}

void V8OffscreenCanvasBridge::CreatePattern(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 2 parameters: createPattern(image, repetition)
    if (args.Length() != 2) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    auto pattern = v8::Object::New(isolate);
    pattern
        ->Set(context, v8::String::NewFromUtf8(isolate, "__id").ToLocalChecked(),
            v8::Integer::New(isolate, patternCount_))
        .ToChecked();
    pattern
        ->Set(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked(),
            v8::String::NewFromUtf8(isolate, "pattern").ToLocalChecked())
        .ToChecked();
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

void V8OffscreenCanvasBridge::CreateImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
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

void V8OffscreenCanvasBridge::PutImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
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

    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->PutImageData(imageData);
    }
}

void V8OffscreenCanvasBridge::ParseImageData(
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

void V8OffscreenCanvasBridge::GetImageData(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // 4 parameters: getImageData(sx, sy, sw, sh)
    if (args.Length() != 4) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    Rect rect = GetRectParam(args);
    std::unique_ptr<ImageData> data;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        data = offscreenCanvas->GetImageData(rect.Left(), rect.Top(), rect.Width(), rect.Height());
    }

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

Gradient V8OffscreenCanvasBridge::GetGradient(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value)
{
    int32_t id = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "__id").ToLocalChecked())
                     .ToLocalChecked()
                     ->Int32Value(context)
                     .ToChecked();
    if (id < 0) {
        return Gradient();
    }
    return gradientColors_[id];
}

Pattern V8OffscreenCanvasBridge::GetPattern(const v8::Local<v8::Context>& context, const v8::Local<v8::Object>& value)
{
    int32_t id = value->Get(context, v8::String::NewFromUtf8(context->GetIsolate(), "__id").ToLocalChecked())
                     .ToLocalChecked()
                     ->Int32Value(context)
                     .ToChecked();
    if (id < 0) {
        return Pattern();
    }
    return pattern_[id];
}

void V8OffscreenCanvasBridge::FillStyleGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__fillStyle").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::FillStyleSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);

    if (value->IsString()) {
        v8::String::Utf8Value colorStr(isolate, value->ToString(context).ToLocalChecked());
        if (*colorStr) {
            auto color = Color::FromString(*colorStr);
            if (offscreenCanvas) {
                offscreenCanvas->SetFillColor(color);
                offscreenCanvas->SetFillPattern(Pattern());
                offscreenCanvas->SetFillGradient(Gradient());
            }
        }
    } else {
        auto object = value->ToObject(context).ToLocalChecked();
        auto typeVal =
            object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value type(isolate, typeVal->ToString(context).ToLocalChecked());
        if (*type) {
            if (std::strcmp(*type, "gradient") == 0) {
                auto gradient = GetGradient(context, object);
                if (offscreenCanvas) {
                    offscreenCanvas->SetFillGradient(gradient);
                    offscreenCanvas->SetFillColor(Color());
                    offscreenCanvas->SetFillPattern(Pattern());
                }
            } else if (std::strcmp(*type, "pattern") == 0) {
                auto pattern = GetPattern(context, object);
                if (offscreenCanvas) {
                    offscreenCanvas->SetFillPattern(pattern);
                    offscreenCanvas->SetFillGradient(Gradient());
                    offscreenCanvas->SetFillColor(Color());
                }
            } else {
                LOGW("No such type for fill style.");
            }
        }
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__fillStyle").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::StrokeStyleGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = args.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__strokeStyle").ToLocalChecked())
                                     .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::StrokeStyleSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);

    if (value->IsString()) {
        v8::String::Utf8Value colorStr(isolate, value->ToString(context).ToLocalChecked());
        if (*colorStr) {
            auto color = Color::FromString(*colorStr);
            if (offscreenCanvas) {
                offscreenCanvas->SetStrokeColor(color);
                offscreenCanvas->SetStrokePattern(Pattern());
                offscreenCanvas->SetStrokeGradient(Gradient());
            }
        }
    } else {
        auto object = value->ToObject(context).ToLocalChecked();
        auto typeVal =
            object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value type(isolate, typeVal->ToString(context).ToLocalChecked());
        if (*type) {
            if (std::strcmp(*type, "gradient") == 0) {
                auto gradient = GetGradient(context, object);
                if (offscreenCanvas) {
                    offscreenCanvas->SetStrokeGradient(gradient);
                    offscreenCanvas->SetStrokeColor(Color());
                    offscreenCanvas->SetStrokePattern(Pattern());
                }
            } else if (std::strcmp(*type, "pattern") == 0) {
                auto pattern = GetPattern(context, object);
                if (offscreenCanvas) {
                    offscreenCanvas->SetStrokePattern(pattern);
                    offscreenCanvas->SetStrokeGradient(Gradient());
                    offscreenCanvas->SetStrokeColor(Color());
                }
            } else {
                LOGW("No such type for stroke style.");
            }
        }
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__strokeStyle").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::LineCapGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__lineCap").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::LineCapSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    if (value->IsString()) {
        v8::String::Utf8Value utf8Value(
            args.GetIsolate(), value->ToString(args.GetIsolate()->GetCurrentContext()).ToLocalChecked());
        static const std::unordered_map<std::string, LineCapStyle> lineCapTable = {
            { "round", LineCapStyle::ROUND },
            { "butt", LineCapStyle::BUTT },
            { "square", LineCapStyle::SQUARE },
        };
        if (*utf8Value) {
            auto lineCap = ConvertStrToEnum(*utf8Value, lineCapTable, LineCapStyle::ROUND);
            auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
            if (offscreenCanvas) {
                offscreenCanvas->SetLineCap(lineCap);
            }
        }
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineCap").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::LineJoinGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__lineJoin").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::LineJoinSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    v8::String::Utf8Value utf8Value(isolate, value->ToString(context).ToLocalChecked());
    static const std::unordered_map<std::string, LineJoinStyle> lineJoinTable = {
        { "bevel", LineJoinStyle::BEVEL },
        { "round", LineJoinStyle::ROUND },
        { "miter", LineJoinStyle::MITER },
    };
    if (*utf8Value) {
        auto lineJoin = ConvertStrToEnum(*utf8Value, lineJoinTable, LineJoinStyle::MITER);
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            offscreenCanvas->SetLineJoin(lineJoin);
        }
        args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineJoin").ToLocalChecked(), value).ToChecked();
    }
}

void V8OffscreenCanvasBridge::MiterLimitGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__miterLimit").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::MiterLimitSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto limit = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetMiterLimit(limit);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__miterLimit").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::LineWidthGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.This()->Get(context, v8::String::NewFromUtf8(isolate, "__lineWidth").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}
void V8OffscreenCanvasBridge::LineWidthSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto lineWidth = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineWidth(lineWidth);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineWidth").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::TextAlignGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__textAlign").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::TextAlignSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    v8::String::Utf8Value alignStr(isolate, value->ToString(context).ToLocalChecked());
    if (*alignStr) {
        auto align = ConvertStrToTextAlign(*alignStr);
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            offscreenCanvas->SetTextAlign(align);
        }
        args.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__textAlign").ToLocalChecked(), value)
            .ToChecked();
    }
}

void V8OffscreenCanvasBridge::TextBaselineGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = args.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__textBaseline").ToLocalChecked())
                                     .ToLocalChecked();
    args.GetReturnValue().Set(value);
}
void V8OffscreenCanvasBridge::TextBaselineSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    v8::String::Utf8Value baselineStr(isolate, value->ToString(context).ToLocalChecked());
    if (*baselineStr) {
        auto baseline = ConvertStrToEnum(*baselineStr, BASELINE_TABLE, TextBaseline::ALPHABETIC);
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            offscreenCanvas->SetTextBaseline(baseline);
        }
        args.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__textBaseline").ToLocalChecked(), value)
            .ToChecked();
    }
}

void V8OffscreenCanvasBridge::FontGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__font").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::FontSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    v8::String::Utf8Value fontStr(isolate, value->ToString(context).ToLocalChecked());
    if (!(*fontStr)) {
        return;
    }
    std::vector<std::string> fontProps;
    StringUtils::StringSpliter(*fontStr, ' ', fontProps);
    bool updateFontStyle = false;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (!offscreenCanvas) {
        args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__font").ToLocalChecked(), value).ToChecked();
        return;
    }
    for (const auto& fontProp : fontProps) {
        if (FONT_WEIGHTS.find(fontProp) != FONT_WEIGHTS.end()) {
            auto weight = ConvertStrToFontWeight(fontProp);
            offscreenCanvas->SetFontWeight(weight);
        } else if (FONT_STYLES.find(fontProp) != FONT_STYLES.end()) {
            updateFontStyle = true;
            auto fontStyle = ConvertStrToFontStyle(fontProp);
            offscreenCanvas->SetFontStyle(fontStyle);
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
    if (!updateFontStyle) {
        offscreenCanvas->SetFontStyle(FontStyle::NORMAL);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__font").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::AlphaGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = args.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__globalAlpha").ToLocalChecked())
                                     .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::AlphaSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto alpha = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetAlpha(alpha);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__globalAlpha").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::CompositeOperationGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__globalCompositeOperation").ToLocalChecked())
            .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::CompositeOperationSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

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
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetCompositeType(type);
    }
    args.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__globalCompositeOperation").ToLocalChecked(), value)
        .ToChecked();
}

void V8OffscreenCanvasBridge::LineDashOffsetGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__lineDash").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}
void V8OffscreenCanvasBridge::LineDashOffsetSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto dashoffset = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetLineDashOffset(dashoffset);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__lineDash").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::ShadowBlurGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__shadowBlur").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::ShadowBlurSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto blur = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowBlur(blur);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__shadowBlur").ToLocalChecked(), value).ToChecked();
}

void V8OffscreenCanvasBridge::ShadowColorGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value = args.Holder()
                                     ->Get(context, v8::String::NewFromUtf8(isolate, "__shadowColor").ToLocalChecked())
                                     .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::ShadowColorSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    v8::String::Utf8Value colorStr(isolate, value->ToString(context).ToLocalChecked());
    if (*colorStr) {
        auto color = Color::FromString(*colorStr);
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            offscreenCanvas->SetShadowColor(color);
        }
        args.Holder()
            ->Set(context, v8::String::NewFromUtf8(isolate, "__shadowColor").ToLocalChecked(), value)
            .ToChecked();
    }
}

void V8OffscreenCanvasBridge::ShadowOffsetXGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetX").ToLocalChecked())
            .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::ShadowOffsetXSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto offsetX = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowOffsetX(offsetX);
    }
    args.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetX").ToLocalChecked(), value)
        .ToChecked();
}

void V8OffscreenCanvasBridge::ShadowOffsetYGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    v8::Local<v8::Value> value =
        args.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetY").ToLocalChecked())
            .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::ShadowOffsetYSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto value = args[0];

    auto offsetY = value->NumberValue(context).ToChecked();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetShadowOffsetY(offsetY);
    }
    args.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__shadowOffsetY").ToLocalChecked(), value)
        .ToChecked();
}

void V8OffscreenCanvasBridge::SmoothingEnabledGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> value =
        args.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__imageSmoothingEnabled").ToLocalChecked())
            .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::SmoothingEnabledSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1 || !args[0]->IsBoolean()) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto enabled = args[0]->ToBoolean(isolate)->Value();
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetSmoothingEnabled(enabled);
    }
    args.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__imageSmoothingEnabled").ToLocalChecked(), args[0])
        .ToChecked();
}

void V8OffscreenCanvasBridge::SmoothingQualityGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> value =
        args.Holder()
            ->Get(context, v8::String::NewFromUtf8(isolate, "__imageSmoothingQuality").ToLocalChecked())
            .ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::SmoothingQualitySetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1 || !args[0]->IsString()) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::String::Utf8Value type(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!(*type) || QUALITY_TYPE.find(*type) == QUALITY_TYPE.end()) {
        return;
    }
    std::string quality = *type;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetSmoothingQuality(quality);
    }
    args.Holder()
        ->Set(context, v8::String::NewFromUtf8(isolate, "__imageSmoothingQuality").ToLocalChecked(), args[0])
        .ToChecked();
}

void V8OffscreenCanvasBridge::FilterParamGetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> value =
        args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__filter").ToLocalChecked()).ToLocalChecked();
    args.GetReturnValue().Set(value);
}

void V8OffscreenCanvasBridge::FilterParamSetter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1 || !args[0]->IsString()) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    v8::String::Utf8Value type(isolate, args[0]->ToString(context).ToLocalChecked());
    if (!(*type)) {
        return;
    }
    std::string quality = *type;
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->SetFilterParam(quality);
    }
    args.Holder()->Set(context, v8::String::NewFromUtf8(isolate, "__filter").ToLocalChecked(), args[0]).ToChecked();
}

void V8OffscreenCanvasBridge::IsPointInStroke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 2 && args.Length() != 3) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    bool ret = false;
    if (args.Length() == 2) {
        double x = args[0]->NumberValue(context).ToChecked();
        double y = args[1]->NumberValue(context).ToChecked();
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInStroke(x, y);
        }
    } else if (args.Length() == 3) {
        double x = args[1]->NumberValue(context).ToChecked();
        double y = args[2]->NumberValue(context).ToChecked();
        auto object = args[0]->ToObject(context).ToLocalChecked();
        auto typeVal =
            object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value type(isolate, typeVal->ToString(context).ToLocalChecked());
        if (!(*type)) {
            return;
        }
        if (std::strcmp(*type, "path2d") != 0) {
            LOGE("target is not path.");
            return;
        }
        auto path = GetPath2D(context, object);
        if (path == nullptr) {
            LOGE("target path is null.");
            return;
        }
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInStroke(path, x, y);
        }
    }
    args.GetReturnValue().Set(v8::Integer::New(isolate, ret ? 1 : 0));
}

void V8OffscreenCanvasBridge::IsPointInPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 2 && args.Length() != 3) {
        return;
    }
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();

    bool ret = false;
    if (args.Length() == 2) {
        double x = args[0]->NumberValue(context).ToChecked();
        double y = args[1]->NumberValue(context).ToChecked();
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInPath(x, y);
        }
    } else if (args.Length() == 3) {
        double x = args[1]->NumberValue(context).ToChecked();
        double y = args[2]->NumberValue(context).ToChecked();
        auto object = args[0]->ToObject(context).ToLocalChecked();
        auto typeVal =
            object->Get(context, v8::String::NewFromUtf8(isolate, "__type").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value type(isolate, typeVal->ToString(context).ToLocalChecked());
        if (!(*type)) {
            return;
        }
        if (std::strcmp(*type, "path2d") != 0) {
            LOGE("target is not path.");
            return;
        }
        auto path = GetPath2D(context, object);
        if (path == nullptr) {
            LOGE("target path is null.");
            return;
        }
        auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
        if (offscreenCanvas) {
            ret = offscreenCanvas->IsPointInPath(path, x, y);
        }
    }
    args.GetReturnValue().Set(v8::Integer::New(isolate, ret ? 1 : 0));
}

void V8OffscreenCanvasBridge::ResetTransform(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto offscreenCanvas = GlobalGetOffscreenCanvas(args);
    if (offscreenCanvas) {
        offscreenCanvas->ResetTransform();
    }
}

} // namespace OHOS::Ace::Framework
