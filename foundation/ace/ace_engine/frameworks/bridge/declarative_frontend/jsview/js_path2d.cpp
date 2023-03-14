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

#include "bridge/declarative_frontend/jsview/js_path2d.h"
#include "bridge/declarative_frontend/jsview/js_canvas_renderer.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

JSPath2D::JSPath2D()
{
}

void JSPath2D::Constructor(const JSCallbackInfo& args)
{
    auto jsPath2d = Referenced::MakeRefPtr<JSPath2D>();
    jsPath2d->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsPath2d));
    jsPath2d->SetCanvasPath2d(JSCanvasRenderer::JsMakePath2D(args));
}

void JSPath2D::Destructor(JSPath2D* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
    }
}

void JSPath2D::JSBind(BindingTarget globalObj)
{
    JSClass<JSPath2D>::Declare("Path2D");
    JSClass<JSPath2D>::CustomMethod("addPath", &JSPath2D::JsPath2DAddPath);
    JSClass<JSPath2D>::CustomMethod("setTransform", &JSCanvasPath::JsPath2DSetTransform);
    JSClass<JSPath2D>::CustomMethod("moveTo", &JSCanvasPath::JsPath2DMoveTo);
    JSClass<JSPath2D>::CustomMethod("lineTo", &JSCanvasPath::JsPath2DLineTo);
    JSClass<JSPath2D>::CustomMethod("arc", &JSCanvasPath::JsPath2DArc);
    JSClass<JSPath2D>::CustomMethod("arcTo", &JSCanvasPath::JsPath2DArcTo);
    JSClass<JSPath2D>::CustomMethod("quadraticCurveTo", &JSCanvasPath::JsPath2DQuadraticCurveTo);
    JSClass<JSPath2D>::CustomMethod("bezierCurveTo", &JSCanvasPath::JsPath2DBezierCurveTo);
    JSClass<JSPath2D>::CustomMethod("ellipse", &JSCanvasPath::JsPath2DEllipse);
    JSClass<JSPath2D>::CustomMethod("rect", &JSCanvasPath::JsPath2DRect);
    JSClass<JSPath2D>::CustomMethod("closePath", &JSCanvasPath::JsPath2DClosePath);
    JSClass<JSPath2D>::Bind(globalObj, JSPath2D::Constructor, JSPath2D::Destructor);
}

void JSPath2D::JsPath2DAddPath(const JSCallbackInfo& info)
{
    JSPath2D* jsPath2d = JSRef<JSObject>::Cast(info[0])->Unwrap<JSPath2D>();
    auto toBeAdd = jsPath2d->GetCanvasPath2d();
    path2d_->AddPath(toBeAdd);
}

} // namespace OHOS::Ace::Framework