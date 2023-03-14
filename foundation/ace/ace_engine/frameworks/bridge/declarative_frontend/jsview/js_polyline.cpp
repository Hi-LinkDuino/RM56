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

#include "frameworks/bridge/declarative_frontend/jsview/js_polyline.h"

#include "core/components/shape/shape_component.h"
#include "frameworks/base/memory/referenced.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSPolyline::Create(const JSCallbackInfo& info)
{
    RefPtr<Component> polylineComponent = AceType::MakeRefPtr<OHOS::Ace::ShapeComponent>(ShapeType::POLYLINE);
    ViewStackProcessor::GetInstance()->Push(polylineComponent);
    JSShapeAbstract::SetSize(info);
}

void JSPolyline::JSBind(BindingTarget globalObj)
{
    JSClass<JSPolyline>::Declare("Polyline");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSPolyline>::StaticMethod("create", &JSPolyline::Create, opt);

    JSClass<JSPolyline>::StaticMethod("width", &JSShapeAbstract::JsWidth);
    JSClass<JSPolyline>::StaticMethod("height", &JSShapeAbstract::JsHeight);
    JSClass<JSPolyline>::StaticMethod("points", &JSPolyline::JSPoints);
    JSClass<JSPolyline>::Inherit<JSShapeAbstract>();
    JSClass<JSPolyline>::Bind<>(globalObj);
}

void JSPolyline::JSPoints(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    auto polyline = AceType::DynamicCast<ShapeComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    ShapePoint point;
    ShapePoints points;
    JSRef<JSArray> pointsArray = JSRef<JSArray>::Cast(info[0]);
    if (!polyline) {
        LOGE("polylineComponent is null.");
        return;
    }
    if (pointsArray->Length() < 2) {
        LOGE("Polyline  have at least 2 points");
        return;
    } else {
        for (size_t i = 0; i < pointsArray->Length(); i++) {
            JSRef<JSArray> pointArray = pointsArray->GetValueAt(i);
            if (pointArray->GetValueAt(0)->IsNumber()) {
                point.first = Dimension(pointArray->GetValueAt(0)->ToNumber<double>(), DimensionUnit::VP);
            } else if (pointArray->GetValueAt(0)->IsString()) {
                point.first = StringUtils::StringToDimension(pointArray->GetValueAt(0)->ToString(), true);
            } else {
                LOGE("Polyline point should be Number or String");
                return;
            }
            if (pointArray->GetValueAt(1)->IsNumber()) {
                point.second = Dimension(pointArray->GetValueAt(1)->ToNumber<double>(), DimensionUnit::VP);
            } else if (pointArray->GetValueAt(1)->IsString()) {
                point.second = StringUtils::StringToDimension(pointArray->GetValueAt(1)->ToString(), true);
            } else {
                LOGE("Polyline point should be Number or String");
                return;
            }
            points.push_back(point);
        }
        polyline->SetPoints(points);
    }
}

} // namespace OHOS::Ace::Framework
