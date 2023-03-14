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

#include "js_rendering_context.h"
#include "bridge/declarative_frontend/engine/bindings.h"
#include "bridge/declarative_frontend/jsview/js_offscreen_rendering_context.h"

namespace OHOS::Ace::Framework {

JSRenderingContext::JSRenderingContext()
{
}

void JSRenderingContext::JSBind(BindingTarget globalObj)
{
    JSClass<JSRenderingContext>::Declare("CanvasRenderingContext2D");

    JSClass<JSRenderingContext>::CustomMethod("toDataURL", &JSCanvasRenderer::JsToDataUrl);
    JSClass<JSRenderingContext>::CustomProperty("width", &JSRenderingContext::JsGetWidth,
                                                &JSRenderingContext::JsSetWidth);
    JSClass<JSRenderingContext>::CustomProperty("height", &JSRenderingContext::JsGetHeight,
                                                &JSRenderingContext::JsSetHeight);
    JSClass<JSRenderingContext>::CustomMethod("createRadialGradient", &JSCanvasRenderer::JsCreateRadialGradient);
    JSClass<JSRenderingContext>::CustomMethod("fillRect", &JSCanvasRenderer::JsFillRect);
    JSClass<JSRenderingContext>::CustomMethod("strokeRect", &JSCanvasRenderer::JsStrokeRect);
    JSClass<JSRenderingContext>::CustomMethod("clearRect", &JSCanvasRenderer::JsClearRect);
    JSClass<JSRenderingContext>::CustomMethod("createLinearGradient", &JSCanvasRenderer::JsCreateLinearGradient);
    JSClass<JSRenderingContext>::CustomMethod("fillText", &JSCanvasRenderer::JsFillText);
    JSClass<JSRenderingContext>::CustomMethod("strokeText", &JSCanvasRenderer::JsStrokeText);
    JSClass<JSRenderingContext>::CustomMethod("measureText", &JSCanvasRenderer::JsMeasureText);
    JSClass<JSRenderingContext>::CustomMethod("moveTo", &JSCanvasRenderer::JsMoveTo);
    JSClass<JSRenderingContext>::CustomMethod("lineTo", &JSCanvasRenderer::JsLineTo);
    JSClass<JSRenderingContext>::CustomMethod("bezierCurveTo", &JSCanvasRenderer::JsBezierCurveTo);
    JSClass<JSRenderingContext>::CustomMethod("quadraticCurveTo", &JSCanvasRenderer::JsQuadraticCurveTo);
    JSClass<JSRenderingContext>::CustomMethod("arcTo", &JSCanvasRenderer::JsArcTo);
    JSClass<JSRenderingContext>::CustomMethod("arc", &JSCanvasRenderer::JsArc);
    JSClass<JSRenderingContext>::CustomMethod("ellipse", &JSCanvasRenderer::JsEllipse);
    JSClass<JSRenderingContext>::CustomMethod("fill", &JSCanvasRenderer::JsFill);
    JSClass<JSRenderingContext>::CustomMethod("stroke", &JSCanvasRenderer::JsStroke);
    JSClass<JSRenderingContext>::CustomMethod("clip", &JSCanvasRenderer::JsClip);
    JSClass<JSRenderingContext>::CustomMethod("rect", &JSCanvasRenderer::JsRect);
    JSClass<JSRenderingContext>::CustomMethod("beginPath", &JSCanvasRenderer::JsBeginPath);
    JSClass<JSRenderingContext>::CustomMethod("closePath", &JSCanvasRenderer::JsClosePath);
    JSClass<JSRenderingContext>::CustomMethod("restore", &JSCanvasRenderer::JsRestore);
    JSClass<JSRenderingContext>::CustomMethod("save", &JSCanvasRenderer::JsSave);
    JSClass<JSRenderingContext>::CustomMethod("rotate", &JSCanvasRenderer::JsRotate);
    JSClass<JSRenderingContext>::CustomMethod("scale", &JSCanvasRenderer::JsScale);
    JSClass<JSRenderingContext>::CustomMethod("setTransform", &JSCanvasRenderer::JsSetTransform);
    JSClass<JSRenderingContext>::CustomMethod("transform", &JSCanvasRenderer::JsTransform);
    JSClass<JSRenderingContext>::CustomMethod("translate", &JSCanvasRenderer::JsTranslate);
    JSClass<JSRenderingContext>::CustomMethod("setLineDash", &JSCanvasRenderer::JsSetLineDash);
    JSClass<JSRenderingContext>::CustomMethod("drawImage", &JSCanvasRenderer::JsDrawImage);
    JSClass<JSRenderingContext>::CustomMethod("createPattern", &JSCanvasRenderer::JsCreatePattern);
    JSClass<JSRenderingContext>::CustomMethod("createImageData", &JSCanvasRenderer::JsCreateImageData);
    JSClass<JSRenderingContext>::CustomMethod("putImageData", &JSCanvasRenderer::JsPutImageData);
    JSClass<JSRenderingContext>::CustomMethod("getImageData", &JSCanvasRenderer::JsGetImageData);
    JSClass<JSRenderingContext>::CustomMethod("getJsonData", &JSCanvasRenderer::JsGetJsonData);
    JSClass<JSRenderingContext>::CustomMethod("getPixelMap", &JSCanvasRenderer::JsGetPixelMap);

    JSClass<JSRenderingContext>::CustomProperty("fillStyle", &JSCanvasRenderer::JsGetFillStyle,
                                                &JSCanvasRenderer::JsSetFillStyle);
    JSClass<JSRenderingContext>::CustomProperty("strokeStyle", &JSCanvasRenderer::JsGetStrokeStyle,
                                                &JSCanvasRenderer::JsSetStrokeStyle);
    JSClass<JSRenderingContext>::CustomProperty("lineCap", &JSCanvasRenderer::JsGetLineCap,
                                                &JSCanvasRenderer::JsSetLineCap);
    JSClass<JSRenderingContext>::CustomProperty("lineJoin", &JSCanvasRenderer::JsGetLineJoin,
                                                &JSCanvasRenderer::JsSetLineJoin);
    JSClass<JSRenderingContext>::CustomProperty("miterLimit", &JSCanvasRenderer::JsGetMiterLimit,
                                                &JSCanvasRenderer::JsSetMiterLimit);
    JSClass<JSRenderingContext>::CustomProperty("lineWidth", &JSCanvasRenderer::JsGetLineWidth,
                                                &JSCanvasRenderer::JsSetLineWidth);
    JSClass<JSRenderingContext>::CustomProperty("font", &JSCanvasRenderer::JsGetFont,
                                                &JSCanvasRenderer::JsSetFont);
    JSClass<JSRenderingContext>::CustomProperty("textAlign", &JSCanvasRenderer::JsGetTextAlign,
                                                &JSCanvasRenderer::JsSetTextAlign);
    JSClass<JSRenderingContext>::CustomProperty("textBaseline", &JSCanvasRenderer::JsGetTextBaseline,
                                                &JSCanvasRenderer::JsSetTextBaseline);
    JSClass<JSRenderingContext>::CustomProperty("globalAlpha", &JSCanvasRenderer::JsGetGlobalAlpha,
                                                &JSCanvasRenderer::JsSetGlobalAlpha);
    JSClass<JSRenderingContext>::CustomProperty("globalCompositeOperation",
                                                &JSCanvasRenderer::JsGetGlobalCompositeOperation,
                                                &JSCanvasRenderer::JsSetGlobalCompositeOperation);
    JSClass<JSRenderingContext>::CustomProperty("lineDashOffset", &JSCanvasRenderer::JsGetLineDashOffset,
                                                &JSCanvasRenderer::JsSetLineDashOffset);
    JSClass<JSRenderingContext>::CustomProperty("shadowBlur", &JSCanvasRenderer::JsGetShadowBlur,
                                                &JSCanvasRenderer::JsSetShadowBlur);
    JSClass<JSRenderingContext>::CustomProperty("shadowColor", &JSCanvasRenderer::JsGetShadowColor,
                                                &JSCanvasRenderer::JsSetShadowColor);
    JSClass<JSRenderingContext>::CustomProperty("shadowOffsetX", &JSCanvasRenderer::JsGetShadowOffsetX,
                                                &JSCanvasRenderer::JsSetShadowOffsetX);
    JSClass<JSRenderingContext>::CustomProperty("shadowOffsetY", &JSCanvasRenderer::JsGetShadowOffsetY,
                                                &JSCanvasRenderer::JsSetShadowOffsetY);
    JSClass<JSRenderingContext>::CustomProperty("imageSmoothingEnabled", &JSCanvasRenderer::JsGetImageSmoothingEnabled,
                                                &JSCanvasRenderer::JsSetImageSmoothingEnabled);
    JSClass<JSRenderingContext>::CustomProperty("imageSmoothingQuality", &JSCanvasRenderer::JsGetImageSmoothingQuality,
                                                &JSCanvasRenderer::JsSetImageSmoothingQuality);
    JSClass<JSRenderingContext>::CustomMethod("transferFromImageBitmap",
                                              &JSRenderingContext::JsTransferFromImageBitmap);

    JSClass<JSRenderingContext>::Bind(globalObj, JSRenderingContext::Constructor, JSRenderingContext::Destructor);
}

void JSRenderingContext::Constructor(const JSCallbackInfo& args)
{
    auto jsRenderContext = Referenced::MakeRefPtr<JSRenderingContext>();
    jsRenderContext->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsRenderContext));

    if (args.Length() != 0) {
        if (args[0]->IsObject()) {
            JSRenderingContextSettings* jsContextSetting
                = JSRef<JSObject>::Cast(args[0])->Unwrap<JSRenderingContextSettings>();
            if (jsContextSetting == nullptr) {
                LOGE("jsContextSetting is null");
                return;
            }
            bool anti = jsContextSetting->GetAntialias();
            jsRenderContext->SetAnti(anti);
        }
    }
}

void JSRenderingContext::Destructor(JSRenderingContext* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
    }
}

void JSRenderingContext::JsGetWidth(const JSCallbackInfo& info)
{
    double width = 0.0;
    width = pool_->GetWidth();
    width = SystemProperties::Px2Vp(width);
    auto returnValue = JSVal(ToJSValue(width));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    info.SetReturnValue(returnPtr);
}

void JSRenderingContext::JsSetWidth(const JSCallbackInfo& info)
{
    return;
}

void JSRenderingContext::JsSetHeight(const JSCallbackInfo& info)
{
    return;
}

void JSRenderingContext::JsGetHeight(const JSCallbackInfo& info)
{
    double height = 0.0;
    height = pool_->GetHeight();
    height = SystemProperties::Px2Vp(height);
    auto returnValue = JSVal(ToJSValue(height));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    info.SetReturnValue(returnPtr);
}
void JSRenderingContext::JsTransferFromImageBitmap(const JSCallbackInfo& info)
{
    if (info.Length() != 0) {
        if (info[0]->IsObject()) {
            uint32_t id = 0;
            JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
            JSRef<JSVal> widthId = obj->GetProperty("__id");
            JSViewAbstract::ParseJsInteger(widthId, id);
            RefPtr<OffscreenCanvas> offscreenCanvas = JSOffscreenRenderingContext::GetOffscreenCanvas(id);
            pool_->TransferFromImageBitmap(offscreenCanvas);
        }
    }
}

} // namespace OHOS::Ace::Framework
