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

#include "frameworks/bridge/declarative_frontend/jsview/js_flex_impl.h"

#include "core/components/flex/flex_component_v2.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

// corresponding to FlexAlign enum
const std::vector<WrapAlignment> WRAP_TABLE = {
    WrapAlignment::START,
    WrapAlignment::START,
    WrapAlignment::CENTER,
    WrapAlignment::END,
    WrapAlignment::STRETCH,
    WrapAlignment::BASELINE,
    WrapAlignment::SPACE_BETWEEN,
    WrapAlignment::SPACE_AROUND,
    WrapAlignment::SPACE_EVENLY,
};

constexpr int32_t MAIN_ALIGN_MAX_VALUE = 8;
constexpr int32_t CROSS_ALIGN_MAX_VALUE = 5;
constexpr int32_t DIRECTION_MAX_VALUE = 3;

} // namespace

void JSFlexImpl::Create(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    if (info.Length() < 1) {
        LOGI("No input args, use default row setting");
        RefPtr<RowComponent> row =
            AceType::MakeRefPtr<OHOS::Ace::RowComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, children);
        row->SetInspectorTag("FlexComponentV2");
        ViewStackProcessor::GetInstance()->Push(row);
        return;
    }
    if (!info[0]->IsObject()) {
        LOGW("arg is not a object, use default row setting");
        RefPtr<RowComponent> row =
            AceType::MakeRefPtr<OHOS::Ace::RowComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, children);
        row->SetInspectorTag("FlexComponentV2");
        ViewStackProcessor::GetInstance()->Push(row);
        return;
    }
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> wrapVal = obj->GetProperty("wrap");
    RefPtr<Component> mainComponent;
    if (wrapVal->IsNumber()) {
        auto wrapNum = wrapVal->ToNumber<int32_t>();
        if (wrapNum == 0) {
            mainComponent = CreateFlexComponent(info);
        } else {
            mainComponent = CreateWrapComponent(info, wrapNum);
        }
    } else {
        mainComponent = CreateFlexComponent(info);
    }
    ViewStackProcessor::GetInstance()->Push(mainComponent);
}

RefPtr<FlexComponent> JSFlexImpl::CreateFlexComponent(const JSCallbackInfo& info)
{
    std::list<RefPtr<Component>> children;
    if (info.Length() < 1) {
        RefPtr<RowComponent> row =
            AceType::MakeRefPtr<OHOS::Ace::RowComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, children);
        return row;
    }
    if (!info[0]->IsObject()) {
        RefPtr<RowComponent> row =
            AceType::MakeRefPtr<OHOS::Ace::RowComponent>(FlexAlign::FLEX_START, FlexAlign::STRETCH, children);
        return row;
    }
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> directionVal = obj->GetProperty("direction");
    JSRef<JSVal> justifyVal = obj->GetProperty("justifyContent");
    JSRef<JSVal> alignItemVal = obj->GetProperty("alignItems");
    auto flex =
        AceType::MakeRefPtr<FlexComponentV2>(FlexDirection::ROW, FlexAlign::FLEX_START, FlexAlign::STRETCH, children);
    if (directionVal->IsNumber()) {
        auto direction = directionVal->ToNumber<int32_t>();
        if (direction >= 0 && direction <= DIRECTION_MAX_VALUE) {
            flex->SetDirection((FlexDirection)direction);
        }
    }
    if (justifyVal->IsNumber()) {
        auto mainAlign = justifyVal->ToNumber<int32_t>();
        if (mainAlign >= 0 && mainAlign <= MAIN_ALIGN_MAX_VALUE) {
            flex->SetMainAxisAlign((FlexAlign)mainAlign);
        }
    }
    if (alignItemVal->IsNumber()) {
        auto crossAlign = alignItemVal->ToNumber<int32_t>();
        if (crossAlign >= 0 && crossAlign <= CROSS_ALIGN_MAX_VALUE) {
            flex->SetCrossAxisAlign((FlexAlign)crossAlign);
        }
    }
    return flex;
}

RefPtr<WrapComponent> JSFlexImpl::CreateWrapComponent(const JSCallbackInfo& info, int32_t wrapVal)
{
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> directionVal = obj->GetProperty("direction");
    JSRef<JSVal> justifyVal = obj->GetProperty("justifyContent");
    JSRef<JSVal> alignItemVal = obj->GetProperty("alignItems");
    JSRef<JSVal> alignContentVal = obj->GetProperty("alignContent");
    std::list<RefPtr<Component>> children;
    auto wrapComponent = AceType::MakeRefPtr<WrapComponent>(children);
    wrapComponent->SetMainAlignment(WrapAlignment::START);
    wrapComponent->SetCrossAlignment(WrapAlignment::STRETCH);
    if (directionVal->IsNumber()) {
        auto direction = directionVal->ToNumber<int32_t>();
        if (direction >= 0 && direction <= DIRECTION_MAX_VALUE) {
            // WrapReverse means wrapVal = 2. Wrap means wrapVal = 1.
            if (direction <= 1) {
                direction += 2 * (wrapVal - 1);
            } else {
                direction -= 2 * (wrapVal - 1);
            }
            wrapComponent->SetDirection((WrapDirection)direction);
        }
    } else {
        // No direction set case: wrapVal == 2 means FlexWrap.WrapReverse.
        WrapDirection wrapDirection = wrapVal == 2 ? WrapDirection::HORIZONTAL_REVERSE : WrapDirection::HORIZONTAL;
        wrapComponent->SetDirection(wrapDirection);
    }
    if (justifyVal->IsNumber()) {
        auto mainAlign = justifyVal->ToNumber<int32_t>();
        if (mainAlign >= 0 && mainAlign <= MAIN_ALIGN_MAX_VALUE) {
            wrapComponent->SetMainAlignment(WRAP_TABLE[mainAlign]);
        }
    }
    if (alignItemVal->IsNumber()) {
        auto crossAlign = alignItemVal->ToNumber<int32_t>();
        if (crossAlign >= 0 && crossAlign <= CROSS_ALIGN_MAX_VALUE) {
            wrapComponent->SetCrossAlignment(WRAP_TABLE[crossAlign]);
        }
    }
    if (alignContentVal->IsNumber()) {
        auto alignContent = alignContentVal->ToNumber<int32_t>();
        if (alignContent >= 0 && alignContent <= MAIN_ALIGN_MAX_VALUE) {
            wrapComponent->SetAlignment(WRAP_TABLE[alignContent]);
        }
    }
    return wrapComponent;
}

void JSFlexImpl::JsFlexWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    JsFlexWidth(info[0]);
}

void JSFlexImpl::JsFlexWidth(const JSRef<JSVal>& jsValue)
{
    JSViewAbstract::JsWidth(jsValue);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto widthVal = box->GetWidth();
    auto mainComponent = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto flex = AceType::DynamicCast<FlexComponent>(mainComponent);
    if (flex) {
        if (flex->GetDirection() == FlexDirection::ROW || flex->GetDirection() == FlexDirection::ROW_REVERSE) {
            flex->SetMainAxisSize(widthVal.Value() < 0.0 ? MainAxisSize::MIN : MainAxisSize::MAX);
        } else {
            flex->SetCrossAxisSize(widthVal.Value() < 0.0 ? CrossAxisSize::MIN : CrossAxisSize::MAX);
        }
    } else {
        auto wrap = AceType::DynamicCast<WrapComponent>(mainComponent);
        if (wrap) {
            wrap->SetHorizontalMeasure(widthVal.Value() < 0.0 ? MeasureType::CONTENT : MeasureType::PARENT);
        }
    }
}

void JSFlexImpl::JsFlexHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    JsFlexHeight(info[0]);
}

void JSFlexImpl::JsFlexHeight(const JSRef<JSVal>& jsValue)
{
    JSViewAbstract::JsHeight(jsValue);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto heightVal = box->GetHeight();
    auto mainComponent = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto flex = AceType::DynamicCast<FlexComponent>(mainComponent);
    if (flex) {
        if (flex->GetDirection() == FlexDirection::COLUMN || flex->GetDirection() == FlexDirection::COLUMN_REVERSE) {
            flex->SetMainAxisSize(heightVal.Value() < 0.0 ? MainAxisSize::MIN : MainAxisSize::MAX);
        } else {
            flex->SetCrossAxisSize(heightVal.Value() < 0.0 ? CrossAxisSize::MIN : CrossAxisSize::MAX);
        }
    } else {
        auto wrap = AceType::DynamicCast<WrapComponent>(mainComponent);
        if (wrap) {
            wrap->SetVerticalMeasure(heightVal.Value() < 0.0 ? MeasureType::CONTENT : MeasureType::PARENT);
        }
    }
}

void JSFlexImpl::JsFlexSize(const JSCallbackInfo& info)
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
    JsFlexWidth(sizeObj->GetProperty("width"));
    JsFlexHeight(sizeObj->GetProperty("height"));
}

void JSFlexImpl::JSBind(BindingTarget globalObj)
{
    JSClass<JSFlexImpl>::Declare("Flex");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSFlexImpl>::StaticMethod("create", &JSFlexImpl::Create, opt);

    JSClass<JSFlexImpl>::StaticMethod("width", &JSFlexImpl::JsFlexWidth);
    JSClass<JSFlexImpl>::StaticMethod("height", &JSFlexImpl::JsFlexHeight);
    JSClass<JSFlexImpl>::StaticMethod("size", &JSFlexImpl::JsFlexSize);

    JSClass<JSFlexImpl>::StaticMethod("fillParent", &JSFlex::SetFillParent, opt);
    JSClass<JSFlexImpl>::StaticMethod("wrapContent", &JSFlex::SetWrapContent, opt);
    JSClass<JSFlexImpl>::StaticMethod("justifyContent", &JSFlex::SetJustifyContent, opt);
    JSClass<JSFlexImpl>::StaticMethod("alignItems", &JSFlex::SetAlignItems, opt);
    JSClass<JSFlexImpl>::StaticMethod("alignContent", &JSFlex::SetAlignContent, opt);
    JSClass<JSFlexImpl>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSFlexImpl>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSFlexImpl>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSFlexImpl>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSFlexImpl>::StaticMethod("onPan", &JSInteractableView::JsOnPan);
    JSClass<JSFlexImpl>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSFlexImpl>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSFlexImpl>::Inherit<JSContainerBase>();
    JSClass<JSFlexImpl>::Inherit<JSViewAbstract>();
    JSClass<JSFlexImpl>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
