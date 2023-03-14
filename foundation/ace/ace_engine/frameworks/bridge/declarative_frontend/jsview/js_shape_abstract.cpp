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

#include "frameworks/bridge/declarative_frontend/jsview/js_shape_abstract.h"

#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSShapeAbstract::SetStrokeDashArray(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsArray()) {
        LOGE("info is not array");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is null");
        return;
    }

    JSRef<JSArray> array = JSRef<JSArray>::Cast(info[0]);
    int32_t length = static_cast<int32_t>(array->Length());
    if (length < 0) {
        LOGE("info is invalid");
        return;
    } else if (length == 0) {
        component->SetStrokeDashArray(std::vector<Dimension>());
        return;
    }
    std::vector<Dimension> dashArray;
    for (int32_t i = 0; i < length; i++) {
        JSRef<JSVal> value = array->GetValueAt(i);
        Dimension dim;
        if (ParseJsDimensionVp(value, dim)) {
            dashArray.emplace_back(dim);
        }
    }
    if (length != static_cast<int32_t>(dashArray.size())) {
        LOGE("ParseJsDimensionVp failed");
        return;
    }
    // if odd,add twice
    if ((static_cast<uint32_t>(length) & 1)) {
        for (int32_t i = 0; i < length; i++) {
            dashArray.emplace_back(dashArray[i]);
        }
    }
    component->SetStrokeDashArray(dashArray);
}

void JSShapeAbstract::SetStroke(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color strokeColor;
    if (!ParseJsColor(info[0], strokeColor)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStroke(strokeColor, option);
    }
}

void JSShapeAbstract::SetFill(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is null");
        return;
    }
    AnimationOption option = stack->GetImplicitAnimationOption();
    if (info[0]->IsString() && info[0]->ToString() == "none") {
        component->SetFill(Color::TRANSPARENT, option);
    } else {
        Color fillColor;
        if (ParseJsColor(info[0], fillColor)) {
            component->SetFill(fillColor, option);
        }
    }
}

void JSShapeAbstract::SetStrokeDashOffset(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension offset;
    if (!ParseJsDimensionVp(info[0], offset)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStrokeDashOffset(offset, option);
    }
}

void JSShapeAbstract::SetStrokeLineCap(int lineCap)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    if (static_cast<int>(LineCapStyle::SQUARE) == lineCap) {
        component->SetStrokeLineCap(LineCapStyle::SQUARE);
    } else if (static_cast<int>(LineCapStyle::ROUND) == lineCap) {
        component->SetStrokeLineCap(LineCapStyle::ROUND);
    } else {
        component->SetStrokeLineCap(LineCapStyle::BUTT);
    }
}

void JSShapeAbstract::SetStrokeLineJoin(int lineJoin)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    if (static_cast<int>(LineJoinStyle::BEVEL) == lineJoin) {
        component->SetStrokeLineJoin(LineJoinStyle::BEVEL);
    } else if (static_cast<int>(LineJoinStyle::ROUND) == lineJoin) {
        component->SetStrokeLineJoin(LineJoinStyle::ROUND);
    } else {
        component->SetStrokeLineJoin(LineJoinStyle::MITER);
    }
}

void JSShapeAbstract::SetStrokeMiterLimit(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    double miterLimit;
    if (!ParseJsDouble(info[0], miterLimit)) {
        return;
    }
    if (GreatOrEqual(miterLimit, 1.0)) {
        component->SetStrokeMiterLimit(miterLimit);
    }
}

void JSShapeAbstract::SetStrokeOpacity(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    double strokeOpacity;
    if (!ParseJsDouble(info[0], strokeOpacity)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStrokeOpacity(strokeOpacity, option);
    }
}

void JSShapeAbstract::SetFillOpacity(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    double fillOpacity;
    if (!ParseJsDouble(info[0], fillOpacity)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetFillOpacity(fillOpacity, option);
    }
}

void JSShapeAbstract::SetStrokeWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension lineWidth;
    if (!ParseJsDimensionVp(info[0], lineWidth)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    if (GreatOrEqual(lineWidth.Value(), 0.0)) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStrokeWidth(lineWidth, option);
    }
}

void JSShapeAbstract::SetAntiAlias(bool antiAlias)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (component) {
        component->SetAntiAlias(antiAlias);
    }
}

void JSShapeAbstract::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }

    SetWidth(info[0]);
}

void JSShapeAbstract::SetWidth(const JSRef<JSVal>& jsValue)
{
    Dimension dimWidth;
    if (!ParseJsDimensionVp(jsValue, dimWidth)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    AnimationOption option = stack->GetImplicitAnimationOption();
    component->SetWidth(dimWidth, option);
}

void JSShapeAbstract::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }

    SetHeight(info[0]);
}

void JSShapeAbstract::SetHeight(const JSRef<JSVal>& jsValue)
{
    Dimension dimHeight;
    if (!ParseJsDimensionVp(jsValue, dimHeight)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    AnimationOption option = stack->GetImplicitAnimationOption();
    component->SetHeight(dimHeight, option);
}

void JSShapeAbstract::JsSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not Object or String.");
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    SetWidth(sizeObj->GetProperty("width"));
    SetHeight(sizeObj->GetProperty("height"));
}

void JSShapeAbstract::ObjectWidth(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }

    ObjectWidth(info[0]);
}

void JSShapeAbstract::ObjectWidth(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        LOGE("Value is less than zero");
        return;
    }
    if (basicShape_) {
        basicShape_->SetWidth(value);
    }
}

void JSShapeAbstract::ObjectHeight(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }

    ObjectHeight(info[0]);
}

void JSShapeAbstract::ObjectHeight(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        LOGE("Value is less than zero");
        return;
    }
    if (basicShape_) {
        basicShape_->SetHeight(value);
    }
}

void JSShapeAbstract::ObjectSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not Object or String.");
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    ObjectWidth(sizeObj->GetProperty("width"));
    ObjectHeight(sizeObj->GetProperty("height"));
}

void JSShapeAbstract::ObjectOffset(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> xVal = sizeObj->GetProperty("x");
        JSRef<JSVal> yVal = sizeObj->GetProperty("y");
        Dimension x;
        Dimension y;
        if (basicShape_ && ParseJsDimensionVp(xVal, x) && ParseJsDimensionVp(yVal, y)) {
            basicShape_->SetOffset(DimensionOffset(x, y));
        }
    }
}

void JSShapeAbstract::ObjectFill(const JSCallbackInfo& info)
{
    info.ReturnSelf();
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    Color color;
    if (ParseJsColor(info[0], color) && basicShape_) {
        basicShape_->SetColor(color);
    }
}

void JSShapeAbstract::JSBind()
{
    JSClass<JSShapeAbstract>::Declare("JSShapeAbstract");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSShapeAbstract>::StaticMethod("stroke", &JSShapeAbstract::SetStroke, opt);
    JSClass<JSShapeAbstract>::StaticMethod("fill", &JSShapeAbstract::SetFill, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeDashOffset", &JSShapeAbstract::SetStrokeDashOffset, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeDashArray", &JSShapeAbstract::SetStrokeDashArray);
    JSClass<JSShapeAbstract>::StaticMethod("strokeLineCap", &JSShapeAbstract::SetStrokeLineCap, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeLineJoin", &JSShapeAbstract::SetStrokeLineJoin, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeMiterLimit", &JSShapeAbstract::SetStrokeMiterLimit, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeOpacity", &JSShapeAbstract::SetStrokeOpacity, opt);
    JSClass<JSShapeAbstract>::StaticMethod("fillOpacity", &JSShapeAbstract::SetFillOpacity, opt);
    JSClass<JSShapeAbstract>::StaticMethod("strokeWidth", &JSShapeAbstract::SetStrokeWidth, opt);
    JSClass<JSShapeAbstract>::StaticMethod("antiAlias", &JSShapeAbstract::SetAntiAlias, opt);
    JSClass<JSShapeAbstract>::StaticMethod("width", &JSShapeAbstract::JsWidth, opt);
    JSClass<JSShapeAbstract>::StaticMethod("height", &JSShapeAbstract::JsHeight, opt);
    JSClass<JSShapeAbstract>::StaticMethod("size", &JSShapeAbstract::JsSize, opt);
    JSClass<JSShapeAbstract>::Inherit<JSViewAbstract>();
}

void JSShapeAbstract::SetSize(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsObject()) {
        auto stack = ViewStackProcessor::GetInstance();
        auto component = AceType::DynamicCast<OHOS::Ace::ShapeComponent>(stack->GetMainComponent());
        if (!component) {
            LOGE("ShapeComponent is null");
            return;
        }
        AnimationOption option = stack->GetImplicitAnimationOption();
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> width = obj->GetProperty("width");
        Dimension dimWidth;
        if (ParseJsDimensionVp(width, dimWidth)) {
            component->SetWidth(dimWidth, option);
        }
        JSRef<JSVal> height = obj->GetProperty("height");
        Dimension dimHeight;
        if (ParseJsDimensionVp(height, dimHeight)) {
            component->SetHeight(dimHeight, option);
        }
    }
}

} // namespace OHOS::Ace::Framework
