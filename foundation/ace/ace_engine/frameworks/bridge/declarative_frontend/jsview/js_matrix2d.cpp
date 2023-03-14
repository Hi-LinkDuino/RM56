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

#include "bridge/declarative_frontend/jsview/js_matrix2d.h"
#include "bridge/declarative_frontend/jsview/js_rendering_context.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

JSMatrix2d::JSMatrix2d()
{
}

void JSMatrix2d::Constructor(const JSCallbackInfo& args)
{
    auto jsCalendarController = Referenced::MakeRefPtr<JSMatrix2d>();
    jsCalendarController->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsCalendarController));
}

void JSMatrix2d::Destructor(JSMatrix2d* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
    }
}

void JSMatrix2d::JSBind(BindingTarget globalObj)
{
    JSClass<JSMatrix2d>::Declare("Matrix2D");
    JSClass<JSMatrix2d>::CustomMethod("scaleX", &JSMatrix2d::JsSetScaleX);
    JSClass<JSMatrix2d>::CustomMethod("rotateY", &JSMatrix2d::JsSetRotateY);
    JSClass<JSMatrix2d>::CustomMethod("rotateX", &JSMatrix2d::JsSetRotateX);
    JSClass<JSMatrix2d>::CustomMethod("scaleY", &JSMatrix2d::JsSetScaleY);
    JSClass<JSMatrix2d>::CustomMethod("translateX", &JSMatrix2d::JsSetTranslateX);
    JSClass<JSMatrix2d>::CustomMethod("translateY", &JSMatrix2d::JsSetTranslateY);

    JSClass<JSMatrix2d>::Bind(globalObj, JSMatrix2d::Constructor, JSMatrix2d::Destructor);
}

void JSMatrix2d::JsSetScaleX(const JSCallbackInfo& info)
{
    double scaleX;
    if (info[0]->IsNumber()) {
        JSViewAbstract::ParseJsDouble(info[0], scaleX);
        scaleX_ = scaleX;
    }
}

void JSMatrix2d::JsSetRotateY(const JSCallbackInfo& info)
{
    double rotateY;
    if (info[0]->IsNumber()) {
        JSViewAbstract::ParseJsDouble(info[0], rotateY);
        rotateY_ = rotateY;
    }
}

void JSMatrix2d::JsSetRotateX(const JSCallbackInfo& info)
{
    double rotateX;
    if (info[0]->IsNumber()) {
        JSViewAbstract::ParseJsDouble(info[0], rotateX);
        rotateX_ = rotateX;
    }
}

void JSMatrix2d::JsSetScaleY(const JSCallbackInfo& info)
{
    double scaleY;
    if (info[0]->IsNumber()) {
        JSViewAbstract::ParseJsDouble(info[0], scaleY);
        scaleY_ = scaleY;
    }
}

void JSMatrix2d::JsSetTranslateX(const JSCallbackInfo& info)
{
    double translateX;
    if (info[0]->IsNumber()) {
        JSViewAbstract::ParseJsDouble(info[0], translateX);
        translateX = SystemProperties::Vp2Px(translateX);
        translateX_ = translateX;
    }
}

void JSMatrix2d::JsSetTranslateY(const JSCallbackInfo& info)
{
    double translateY;
    if (info[0]->IsNumber()) {
        JSViewAbstract::ParseJsDouble(info[0], translateY);
        translateY = SystemProperties::Vp2Px(translateY);
        translateY_ = translateY;
    }
}

double JSMatrix2d::JsGetScaleX()
{
    return scaleX_;
}

double JSMatrix2d::JsGetRotateY()
{
    return rotateY_;
}

double JSMatrix2d::JsGetRotateX()
{
    return rotateX_;
}

double JSMatrix2d::JsGetScaleY()
{
    return scaleY_;
}

double JSMatrix2d::JsGetTranslateX()
{
    return translateX_;
}

double JSMatrix2d::JsGetTranslateY()
{
    return translateY_;
}

} // namespace OHOS::Ace::Framework