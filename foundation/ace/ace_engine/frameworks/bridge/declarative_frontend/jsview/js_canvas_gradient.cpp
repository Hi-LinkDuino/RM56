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

#include "bridge/declarative_frontend/jsview/js_canvas_gradient.h"
#include "bridge/declarative_frontend/jsview/js_rendering_context.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

JSCanvasGradient::JSCanvasGradient()
{
}

void JSCanvasGradient::Constructor(const JSCallbackInfo& args)
{
    auto jsCanvasGradient = Referenced::MakeRefPtr<JSCanvasGradient>();
    jsCanvasGradient->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsCanvasGradient));
}

void JSCanvasGradient::Destructor(JSCanvasGradient* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
        delete controller->GetGradient();
    }
}

void JSCanvasGradient::JSBind(BindingTarget globalObj)
{
    JSClass<JSCanvasGradient>::Declare("CanvasGradient");
    JSClass<JSCanvasGradient>::CustomMethod("addColorStop", &JSCanvasGradient::addColorStop);
    JSClass<JSCanvasGradient>::Bind(globalObj, JSCanvasGradient::Constructor, JSCanvasGradient::Destructor);
}

void JSCanvasGradient::addColorStop(const JSCallbackInfo& info)
{
    if (info[0]->IsNumber() && info[1]->IsString()) {
        std::string jsColor = "";
        double offset = 0.0;
        GradientColor color;
        JSViewAbstract::ParseJsDouble(info[0], offset);
        JSViewAbstract::ParseJsString(info[1], jsColor);
        color.SetColor(Color::FromString(jsColor));
        color.SetDimension(offset);
        if (gradient_) {
            gradient_->AddColor(color);
        }
    }
}

} // namespace OHOS::Ace::Framework
