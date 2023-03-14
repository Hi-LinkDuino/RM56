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

#include "frameworks/bridge/declarative_frontend/jsview/js_shape.h"

#include "frameworks/bridge/declarative_frontend/jsview/js_utils.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSShape::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<OHOS::Ace::Component>> componentChildren;
    RefPtr<OHOS::Ace::ShapeContainerComponent> component =
        AceType::MakeRefPtr<OHOS::Ace::ShapeContainerComponent>(componentChildren);
    ViewStackProcessor::GetInstance()->Push(component);
    JSInteractableView::SetFocusable(true);
    InitBox(info);
}

void JSShape::InitBox(const JSCallbackInfo& info)
{
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetOverflow(Overflow::FORCE_CLIP);
    auto clipPath = AceType::MakeRefPtr<ClipPath>();
    clipPath->SetGeometryBoxType(GeometryBoxType::BORDER_BOX);
    box->SetClipPath(clipPath);
    if (info.Length() == 1 && info[0]->IsObject()) {
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        box->SetPixelMap(CreatePixelMapFromNapiValue(info[0]));
#endif
    }
}

void JSShape::SetViewPort(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("shape is null");
        return;
    }
    if (info[0]->IsObject()) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
        JSRef<JSVal> leftValue = obj->GetProperty("x");
        JSRef<JSVal> topValue = obj->GetProperty("y");
        JSRef<JSVal> widthValue = obj->GetProperty("width");
        JSRef<JSVal> heightValue = obj->GetProperty("height");
        AnimationOption option = stack->GetImplicitAnimationOption();
        ShapeViewBox viewBox;
        Dimension dimLeft;
        if (ParseJsDimensionVp(leftValue, dimLeft)) {
            viewBox.SetLeft(dimLeft, option);
        }
        Dimension dimTop;
        if (ParseJsDimensionVp(topValue, dimTop)) {
            viewBox.SetTop(dimTop, option);
        }
        Dimension dimWidth;
        if (ParseJsDimensionVp(widthValue, dimWidth)) {
            viewBox.SetWidth(dimWidth, option);
        }
        Dimension dimHeight;
        if (ParseJsDimensionVp(heightValue, dimHeight)) {
            viewBox.SetHeight(dimHeight, option);
        }
        component->SetViewBox(viewBox);
    }
    info.SetReturnValue(info.This());
}

void JSShape::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSShape::JsWidth(const JSRef<JSVal>& jsValue)
{
    JSViewAbstract::JsWidth(jsValue);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!box->GetWidth().IsValid()) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetWidthFlag(true);
    }
}

void JSShape::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSShape::JsHeight(const JSRef<JSVal>& jsValue)
{
    JSViewAbstract::JsHeight(jsValue);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!box->GetHeight().IsValid()) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetHeightFlag(true);
    }
}

void JSShape::JsSize(const JSCallbackInfo& info)
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
    JsWidth(sizeObj->GetProperty("width"));
    JsHeight(sizeObj->GetProperty("height"));
}

void JSShape::SetStrokeDashArray(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsArray()) {
        LOGE("info is not array");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("component is null");
        return;
    }

    JSRef<JSArray> array = JSRef<JSArray>::Cast(info[0]);
    int32_t length = static_cast<int32_t>(array->Length());
    if (length <= 0) {
        LOGE("info is invalid");
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
    info.SetReturnValue(info.This());
}

void JSShape::SetStroke(const JSCallbackInfo& info)
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
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStroke(strokeColor, option);
    }
}

void JSShape::SetFill(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
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

void JSShape::SetStrokeDashOffset(const JSCallbackInfo& info)
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
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStrokeDashOffset(offset, option);
    }
}

void JSShape::SetStrokeLineCap(int lineCap)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
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

void JSShape::SetStrokeLineJoin(int lineJoin)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
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

void JSShape::SetStrokeMiterLimit(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    double miterLimit;
    if (!ParseJsDouble(info[0], miterLimit)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    if (GreatOrEqual(miterLimit, 1.0)) {
        component->SetStrokeMiterLimit(miterLimit);
    }
}

void JSShape::SetStrokeOpacity(const JSCallbackInfo& info)
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
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStrokeOpacity(strokeOpacity, option);
    }
}

void JSShape::SetFillOpacity(const JSCallbackInfo& info)
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
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetFillOpacity(fillOpacity, option);
    }
}

void JSShape::SetStrokeWidth(const JSCallbackInfo& info)
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
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ShapeComponent is null");
        return;
    }
    if (GreatOrEqual(lineWidth.Value(), 0.0)) {
        AnimationOption option = stack->GetImplicitAnimationOption();
        component->SetStrokeWidth(lineWidth, option);
    }
}

void JSShape::SetAntiAlias(bool antiAlias)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetAntiAlias(antiAlias);
    }
}

void JSShape::SetBitmapMesh(const JSCallbackInfo& info)
{
    if (info.Length() != 3) {
        LOGE("The arg is wrong, it is supposed to have at least 3 argument");
        return;
    }
    std::vector<double> mesh;
    JSRef<JSVal> meshValue = info[0];

    if (meshValue->IsObject()) {
        JSRef<JSObject> meshObj = JSRef<JSObject>::Cast(meshValue);
        JSRef<JSArray> array = meshObj->GetPropertyNames();
        for (size_t i = 0; i < array->Length(); i++) {
            JSRef<JSVal> value = array->GetValueAt(i);
            if (value->IsString()) {
                std::string valueStr;
                if (ParseJsString(value, valueStr)) {
                    double vert;
                    if (ParseJsDouble(meshObj->GetProperty(valueStr.c_str()), vert)) {
                        mesh.push_back(vert);
                    }
                }
            }
        }
    }
    uint32_t column = 0;
    uint32_t row = 0;
    JSRef<JSVal> columnValue = info[1];
    JSRef<JSVal> rowValue = info[2];
    if (!ParseJsInteger(columnValue, column)) {
        return;
    }
    if (!ParseJsInteger(rowValue, row)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<OHOS::Ace::ShapeContainerComponent>(stack->GetMainComponent());
    if (component) {
        component->SetBitmapMesh(mesh, (int32_t)column, (int32_t)row);
    }
}

void JSShape::JSBind(BindingTarget globalObj)
{
    JSClass<JSShape>::Declare("Shape");
    JSClass<JSShape>::StaticMethod("create", &JSShape::Create);
    JSClass<JSShape>::StaticMethod("viewPort", &JSShape::SetViewPort);

    JSClass<JSShape>::StaticMethod("width", &JSShape::JsWidth);
    JSClass<JSShape>::StaticMethod("height", &JSShape::JsHeight);
    JSClass<JSShape>::StaticMethod("size", &JSShape::JsSize);

    JSClass<JSShape>::StaticMethod("stroke", &JSShape::SetStroke);
    JSClass<JSShape>::StaticMethod("fill", &JSShape::SetFill);
    JSClass<JSShape>::StaticMethod("strokeDashOffset", &JSShape::SetStrokeDashOffset);
    JSClass<JSShape>::StaticMethod("strokeDashArray", &JSShape::SetStrokeDashArray);
    JSClass<JSShape>::StaticMethod("strokeLineCap", &JSShape::SetStrokeLineCap);
    JSClass<JSShape>::StaticMethod("strokeLineJoin", &JSShape::SetStrokeLineJoin);
    JSClass<JSShape>::StaticMethod("strokeMiterLimit", &JSShape::SetStrokeMiterLimit);
    JSClass<JSShape>::StaticMethod("strokeOpacity", &JSShape::SetStrokeOpacity);
    JSClass<JSShape>::StaticMethod("fillOpacity", &JSShape::SetFillOpacity);
    JSClass<JSShape>::StaticMethod("strokeWidth", &JSShape::SetStrokeWidth);
    JSClass<JSShape>::StaticMethod("antiAlias", &JSShape::SetAntiAlias);
    JSClass<JSShape>::StaticMethod("mesh", &JSShape::SetBitmapMesh);

    JSClass<JSShape>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSShape>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSShape>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSShape>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSShape>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSShape>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSShape>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSShape>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSShape>::Inherit<JSContainerBase>();
    JSClass<JSShape>::Inherit<JSViewAbstract>();
    JSClass<JSShape>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
