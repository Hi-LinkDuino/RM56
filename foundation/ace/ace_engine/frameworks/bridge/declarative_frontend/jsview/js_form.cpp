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

#include "frameworks/bridge/declarative_frontend/jsview/js_form.h"

#include "base/geometry/dimension.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#if !(defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM) || defined(WEARABLE_PRODUCT))
#include "frameworks/core/components/form/form_component.h"
#else
#include "frameworks/core/components/box/box_component.h"
#endif

namespace OHOS::Ace::Framework {
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
namespace {
constexpr Dimension DEFAULT_FONT_SIZE = 30.0_px;
}
#endif

void JSForm::Create(const JSCallbackInfo& info)
{
    if (info.Length() == 0 || !info[0]->IsObject()) {
        LOGE("form create fail due to FormComponent construct param is empty or type is not Object");
        return;
    }
    auto obj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> id = obj->GetProperty("id");
    JSRef<JSVal> name = obj->GetProperty("name");
    JSRef<JSVal> bundle = obj->GetProperty("bundle");
    JSRef<JSVal> ability = obj->GetProperty("ability");
    JSRef<JSVal> module = obj->GetProperty("module");
    JSRef<JSVal> dimension = obj->GetProperty("dimension");
    JSRef<JSVal> temporary = obj->GetProperty("temporary");

    LOGD("js form create id:%{public}d, name:%{public}s, bundle:%{public}s, ability:%{public}s, module:%{public}s, "
         "temporary:%{public}s",
        id->ToNumber<int32_t>(), name->ToString().c_str(), bundle->ToString().c_str(), ability->ToString().c_str(),
        module->ToString().c_str(), temporary->ToString().c_str());

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    RequestFormInfo fomInfo;
    fomInfo.id = id->ToNumber<int32_t>();
    fomInfo.cardName = name->ToString();
    fomInfo.bundleName = bundle->ToString();
    fomInfo.abilityName = ability->ToString();
    fomInfo.moduleName = module->ToString();
    fomInfo.dimension = dimension->ToNumber<int32_t>();
    fomInfo.temporary = temporary->ToBoolean();

    RefPtr<FormComponent> form = AceType::MakeRefPtr<OHOS::Ace::FormComponent>();
    form->SetFormRequestionInfo(fomInfo);
#else
    // for PC Preview
    RefPtr<ComposedComponent> form = AceType::MakeRefPtr<OHOS::Ace::ComposedComponent>("", "card");
    RefPtr<BoxComponent> mountBox = AceType::MakeRefPtr<OHOS::Ace::BoxComponent>();
    auto textComponent = AceType::MakeRefPtr<TextComponent>("This component is not supported on PC Preview.");
    auto textStyle = textComponent->GetTextStyle();
    textStyle.SetFontSize(DEFAULT_FONT_SIZE);
    textComponent->SetTextStyle(textStyle);
    mountBox->SetColor(Color::FromString("#808080"));

    mountBox->SetParent(form);
    textComponent->SetParent(mountBox);

    mountBox->SetChild(textComponent);
    form->SetChild(mountBox);
#endif
    form->SetInspectorTag("FormComponent");
    ViewStackProcessor::GetInstance()->Push(form, false);
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    boxComponent->SetMouseAnimationType(HoverAnimationType::SCALE);
}

void JSForm::SetSize(const JSCallbackInfo& info)
{
    if (info.Length() == 0 || !info[0]->IsObject()) {
        LOGW("form set size fail due to FormComponent construct param is empty or type is not Object");
    }
    Dimension width = 0.0_vp;
    Dimension height = 0.0_vp;

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> widthValue = sizeObj->GetProperty("width");
    if (!widthValue->IsNull() && !widthValue->IsEmpty()) {
        if (widthValue->IsNumber()) {
            width = Dimension(widthValue->ToNumber<double>(), DimensionUnit::VP);
        } else if (widthValue->IsString()) {
            width = StringUtils::StringToDimension(widthValue->ToString(), true);
        }
    }

    JSRef<JSVal> heightValue = sizeObj->GetProperty("height");
    if (!heightValue->IsNull() && !heightValue->IsEmpty()) {
        if (heightValue->IsNumber()) {
            height = Dimension(heightValue->ToNumber<double>(), DimensionUnit::VP);
        } else if (heightValue->IsString()) {
            height = StringUtils::StringToDimension(heightValue->ToString(), true);
        }
    }
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (form) {
        form->SetCardSize(width.IsValid() ? width : 0.0_vp, height.IsValid() ? height : 0.0_vp);
    }
#else
    // for PC Preview
    auto form = AceType::DynamicCast<ComposedComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (form) {
        RefPtr<BoxComponent> mountBox = AceType::DynamicCast<BoxComponent>(form->GetChild());
        if (mountBox) {
            mountBox->SetWidth(width.Value(), width.Unit());
            mountBox->SetHeight(height.Value(), height.Unit());
        }
    }
#endif
}

void JSForm::SetDimension(int32_t value)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (form) {
        form->SetDimension(value);
    }
#endif
}

void JSForm::AllowUpdate(const JSCallbackInfo& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (info.Length() > 0 && info[0]->IsBoolean()) {
        auto allowUpdate = info[0]->ToBoolean();
        auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (form) {
            form->SetAllowUpdate(allowUpdate);
        }
    }
#endif
}

void JSForm::SetVisibility(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsNumber()) {
        auto type = info[0]->ToNumber<int32_t>();
        auto component = ViewStackProcessor::GetInstance()->GetDisplayComponent();
        auto display = AceType::DynamicCast<DisplayComponent>(component);
        display->SetVisible(VisibleType(type));
    }
}

void JSForm::SetModuleName(const JSCallbackInfo& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (info.Length() > 0 && info[0]->IsString()) {
        auto moduleName = info[0]->ToString();
        auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
        if (form) {
            form->SetModuleName(moduleName);
        }
    }
#endif
}

void JSForm::JsOnAcquired(const JSCallbackInfo& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());

        auto onAppearId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                LOGI("onAcquire send:%{public}s", param.c_str());
                std::vector<std::string> keys = { "id" };
                ACE_SCORING_EVENT("Form.onAcquired");
                func->Execute(keys, param);
            });
        form->SetOnAcquireFormEventId(onAppearId);
    }
#endif
}

void JSForm::JsOnError(const JSCallbackInfo& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());

        auto onErrorId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                LOGI("onError send:%{public}s", param.c_str());
                std::vector<std::string> keys = { "errcode", "msg" };
                ACE_SCORING_EVENT("Form.onError");
                func->Execute(keys, param);
            });

        form->SetOnErrorEventId(onErrorId);
    }
#endif
}

void JSForm::JsOnUninstall(const JSCallbackInfo& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());

        auto onUninstallId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                LOGI("onUninstall send:%{public}s", param.c_str());
                std::vector<std::string> keys = { "id" };
                ACE_SCORING_EVENT("Form.onUninstall");
                func->Execute(keys, param);
            });

        form->SetOnUninstallEventId(onUninstallId);
    }
#endif
}

void JSForm::JsOnRouter(const JSCallbackInfo& info)
{
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
    if (info[0]->IsFunction()) {
        RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
        auto form = AceType::DynamicCast<FormComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());

        auto onRouterId =
            EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE(execCtx);
                LOGI("onRouter send:%{public}s", param.c_str());
                std::vector<std::string> keys = { "action" };
                ACE_SCORING_EVENT("Form.onRouter");
                func->Execute(keys, param);
            });

        form->SetOnRouterEventId(onRouterId);
    }
#endif
}

void JSForm::JSBind(BindingTarget globalObj)
{
    JSClass<JSForm>::Declare("FormComponent");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSForm>::StaticMethod("create", &JSForm::Create, opt);
    JSClass<JSForm>::StaticMethod("size", &JSForm::SetSize, opt);
    JSClass<JSForm>::StaticMethod("dimension", &JSForm::SetDimension, opt);
    JSClass<JSForm>::StaticMethod("allowUpdate", &JSForm::AllowUpdate, opt);
    JSClass<JSForm>::StaticMethod("visibility", &JSForm::SetVisibility, opt);
    JSClass<JSForm>::StaticMethod("clip", &JSViewAbstract::JsClip, opt);

    JSClass<JSForm>::StaticMethod("onAcquired", &JSForm::JsOnAcquired);
    JSClass<JSForm>::StaticMethod("onError", &JSForm::JsOnError);
    JSClass<JSForm>::StaticMethod("onUninstall", &JSForm::JsOnUninstall);
    JSClass<JSForm>::StaticMethod("onRouter", &JSForm::JsOnRouter);
    JSClass<JSForm>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSForm>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSForm>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSForm>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSForm>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSForm>::StaticMethod("onClick", &JSInteractableView::JsOnClick);

    JSClass<JSForm>::Inherit<JSViewAbstract>();
    JSClass<JSForm>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework
