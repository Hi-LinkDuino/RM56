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

#include "frameworks/bridge/declarative_frontend/jsview/js_line.h"

#include "core/components/shape/shape_component.h"
#include "frameworks/base/memory/referenced.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_function.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSLine::JSBind(BindingTarget globalObj)
{
    JSClass<JSLine>::Declare("Line");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSLine>::StaticMethod("create", &JSLine::Create, opt);

    JSClass<JSLine>::StaticMethod("width", &JSShapeAbstract::JsWidth);
    JSClass<JSLine>::StaticMethod("height", &JSShapeAbstract::JsHeight);
    JSClass<JSLine>::StaticMethod("startPoint", &JSLine::SetStart);
    JSClass<JSLine>::StaticMethod("endPoint", &JSLine::SetEnd);

    JSClass<JSLine>::Inherit<JSShapeAbstract>();
    JSClass<JSLine>::Bind<>(globalObj);
}

void JSLine::Create(const JSCallbackInfo& info)
{
    RefPtr<ShapeComponent> lineComponent = AceType::MakeRefPtr<OHOS::Ace::ShapeComponent>(ShapeType::LINE);
    lineComponent->SetStroke(Color::BLACK);
    ViewStackProcessor::GetInstance()->Push(lineComponent);
    JSShapeAbstract::SetSize(info);
}

void JSLine::SetStart(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    auto line = AceType::DynamicCast<ShapeComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!line) {
        LOGE("startPointComponent is null.");
        return;
    }
    JSRef<JSArray> pointArray = JSRef<JSArray>::Cast(info[0]);
    ShapePoint startPoint;
    SetPoint(pointArray, startPoint);
    line->SetStart(startPoint);
}

void JSLine::SetEnd(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    auto line = AceType::DynamicCast<ShapeComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!line) {
        LOGE("startPointComponent is null.");
        return;
    }
    JSRef<JSArray> pointArray = JSRef<JSArray>::Cast(info[0]);
    ShapePoint endPoint;
    SetPoint(pointArray, endPoint);
    line->SetEnd(endPoint);
}

void JSLine::SetPoint(const JSRef<JSArray>& array, ShapePoint& point)
{
    if (array->Length() < 1) {
        LOGE("The starting point is one");
        return;
    } else {
        if (array->GetValueAt(0)->IsNumber()) {
            point.first = Dimension(array->GetValueAt(0)->ToNumber<double>(), DimensionUnit::VP);
        } else if (array->GetValueAt(0)->IsString()) {
            point.first = StringUtils::StringToDimension(array->GetValueAt(0)->ToString(), true);
        } else {
            LOGE("Line point should be Number or String");
        }
        if (array->GetValueAt(1)->IsNumber()) {
            point.second = Dimension(array->GetValueAt(1)->ToNumber<double>(), DimensionUnit::VP);
        } else if (array->GetValueAt(0)->IsString()) {
            point.second = StringUtils::StringToDimension(array->GetValueAt(1)->ToString(), true);
        } else {
            LOGE("Line point should be Number or String");
        }
    }
}

} // namespace OHOS::Ace::Framework