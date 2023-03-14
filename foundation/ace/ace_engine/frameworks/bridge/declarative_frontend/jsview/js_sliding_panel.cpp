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

#include "frameworks/bridge/declarative_frontend/jsview/js_sliding_panel.h"

#include <algorithm>
#include <iterator>

#include "core/components/panel/sliding_panel_component_v2.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {
namespace {

JSRef<JSVal> SlidingPanelSizeChangeEventToJSValue(const SlidingPanelSizeChangeEvent& eventInfo)
{
    JSRef<JSObject> obj = JSRef<JSObject>::New();
    std::string modeStr = "half";
    const PanelMode& mode = eventInfo.GetMode();
    if (mode == PanelMode::FULL) {
        modeStr = "full";
    } else if (mode == PanelMode::MINI) {
        modeStr = "mini";
    }
    obj->SetProperty<std::string>("mode", modeStr.c_str());
    obj->SetProperty<double>("width", eventInfo.GetWidth());
    obj->SetProperty<double>("height", eventInfo.GetHeight());
    return JSRef<JSVal>::Cast(obj);
}

} // namespace

void JSSlidingPanel::Create(const JSCallbackInfo& info)
{
    auto slidingPanel = AceType::MakeRefPtr<SlidingPanelComponentV2>();
    slidingPanel->SetHasDragBar(true);
    ViewStackProcessor::GetInstance()->Push(slidingPanel);
    if (info.Length() > 0 && info[0]->IsBoolean()) {
        auto isShow = info[0]->ToBoolean();
        auto component = ViewStackProcessor::GetInstance()->GetDisplayComponent();
        auto display = AceType::DynamicCast<DisplayComponent>(component);
        if (!display) {
            LOGE("display is null");
            return;
        }
        display->SetVisible(isShow ? VisibleType::VISIBLE : VisibleType::GONE);
    }
}

void JSSlidingPanel::JSBind(BindingTarget globalObj)
{
    JSClass<JSSlidingPanel>::Declare("Panel");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSSlidingPanel>::StaticMethod("create", &JSSlidingPanel::Create, opt);
    JSClass<JSSlidingPanel>::StaticMethod("dragBar", &JSSlidingPanel::SetHasDragBar, opt);
    JSClass<JSSlidingPanel>::StaticMethod("show", &JSSlidingPanel::SetShow, opt);
    JSClass<JSSlidingPanel>::StaticMethod("mode", &JSSlidingPanel::SetPanelMode, opt);
    JSClass<JSSlidingPanel>::StaticMethod("type", &JSSlidingPanel::SetPanelType, opt);
    JSClass<JSSlidingPanel>::StaticMethod("fullHeight", &JSSlidingPanel::SetFullHeight, opt);
    JSClass<JSSlidingPanel>::StaticMethod("halfHeight", &JSSlidingPanel::SetHalfHeight, opt);
    JSClass<JSSlidingPanel>::StaticMethod("miniHeight", &JSSlidingPanel::SetMiniHeight, opt);
    JSClass<JSSlidingPanel>::StaticMethod("backgroundColor", JsBackgroundColor);
    JSClass<JSSlidingPanel>::StaticMethod("border", JsPanelBorder);
    JSClass<JSSlidingPanel>::StaticMethod("borderWidth", JsPanelBorderWidth);
    JSClass<JSSlidingPanel>::StaticMethod("borderColor", JsPanelBorderColor);
    JSClass<JSSlidingPanel>::StaticMethod("borderStyle", JsPanelBorderStyle);
    JSClass<JSSlidingPanel>::StaticMethod("borderRadius", JsPanelBorderRadius);

    JSClass<JSSlidingPanel>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSSlidingPanel>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSSlidingPanel>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSSlidingPanel>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSSlidingPanel>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSSlidingPanel>::StaticMethod("onChange", &JSSlidingPanel::SetOnSizeChange);

    JSClass<JSSlidingPanel>::Inherit<JSContainerBase>();
    JSClass<JSSlidingPanel>::Inherit<JSViewAbstract>();
    JSClass<JSSlidingPanel>::Bind<>(globalObj);
}

void JSSlidingPanel::JsBackgroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color backgroundColor;
    if (!ParseJsColor(info[0], backgroundColor)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    auto box = JSSlidingPanel::GetPanelBox();
    if (!panel || !box) {
        LOGE("Not valid type for SlidingPanel");
        return;
    }
    box->SetColor(backgroundColor);
    panel->SetHasBgStyle(true);
    panel->SetHasDecorationStyle(true);
}

RefPtr<BoxComponent> JSSlidingPanel::GetPanelBox()
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    if (!panel) {
        return nullptr;
    }
    if (panel->HasBoxStyle()) {
        return panel->GetBoxStyle();
    } else {
        panel->SetHasBoxStyle(true);
        auto box = AceType::MakeRefPtr<BoxComponent>();
        panel->SetBoxStyle(box);
        return box;
    }
}

RefPtr<Decoration> JSSlidingPanel::GetPanelDecoration()
{
    auto box = JSSlidingPanel::GetPanelBox();
    if (!box) {
        return nullptr;
    }
    auto decoration = box->GetBackDecoration();
    if (!decoration) {
        decoration = AceType::MakeRefPtr<Decoration>();
        box->SetBackDecoration(decoration);
    }
    return decoration;
}

void JSSlidingPanel::JsPanelBorderColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color borderColor;
    if (!ParseJsColor(info[0], borderColor)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    auto decoration = JSSlidingPanel::GetPanelDecoration();
    if (!panel || !decoration) {
        return;
    }
    auto border = decoration->GetBorder();
    border.SetColor(borderColor);
    decoration->SetBorder(border);
    panel->SetHasBorderStyle(true);
    panel->SetHasDecorationStyle(true);
}

void JSSlidingPanel::JsPanelBorderRadius(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension radiusVal;
    if (!ParseJsDimensionVp(info[0], radiusVal)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    auto decoration = JSSlidingPanel::GetPanelDecoration();
    if (!panel || !decoration) {
        return;
    }
    auto border = decoration->GetBorder();
    border.SetBorderRadius(Radius(radiusVal));
    decoration->SetBorder(border);
    panel->SetHasBorderStyle(true);
    panel->SetHasDecorationStyle(true);
}

void JSSlidingPanel::JsPanelBorderWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension borderWidth;
    if (!ParseJsDimensionVp(info[0], borderWidth)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    auto decoration = JSSlidingPanel::GetPanelDecoration();
    if (!panel || !decoration) {
        return;
    }
    auto border = decoration->GetBorder();
    border.SetWidth(borderWidth);
    decoration->SetBorder(border);
    panel->SetHasBorderStyle(true);
    panel->SetHasDecorationStyle(true);
}

void JSSlidingPanel::JsPanelBorderStyle(int32_t style)
{
    BorderStyle borderStyle = BorderStyle::SOLID;
    if (style > 0 && style < 4) {
        borderStyle = (BorderStyle)style;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    auto decoration = JSSlidingPanel::GetPanelDecoration();
    if (!panel || !decoration) {
        return;
    }
    auto border = decoration->GetBorder();
    border.SetStyle(borderStyle);
    decoration->SetBorder(border);
    panel->SetHasBorderStyle(true);
    panel->SetHasDecorationStyle(true);
}

void JSSlidingPanel::JsPanelBorder(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    if (!info[0]->IsObject()) {
        LOGE("arg is not a object.");
        return;
    }

    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        info.ReturnSelf();
        return;
    }
    Dimension width = Dimension(0.0, DimensionUnit::VP);
    Dimension radius = Dimension(0.0, DimensionUnit::VP);
    ParseJsonDimensionVp(argsPtrItem->GetValue("width"), width);
    ParseJsonDimensionVp(argsPtrItem->GetValue("radius"), radius);
    auto borderStyle = argsPtrItem->GetInt("style", static_cast<int32_t>(BorderStyle::SOLID));
    auto style = (borderStyle > 0 && borderStyle < 4) ? (BorderStyle)borderStyle : BorderStyle::SOLID;
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponentV2>(component);
    auto decoration = JSSlidingPanel::GetPanelDecoration();
    if (!panel || !decoration) {
        return;
    }
    auto border = decoration->GetBorder();
    border.SetStyle(style);
    border.SetWidth(width);
    Color color;
    if (ParseJsonColor(argsPtrItem->GetValue("color"), color)) {
        border.SetColor(color);
    }
    border.SetBorderRadius(Radius(radius));
    decoration->SetBorder(border);
    panel->SetHasBorderStyle(true);
    panel->SetHasDecorationStyle(true);
}

void JSSlidingPanel::SetOnSizeChange(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onSizeChangeFunc = AceType::MakeRefPtr<JsEventFunction<SlidingPanelSizeChangeEvent, 1>>(
            JSRef<JSFunc>::Cast(args[0]), SlidingPanelSizeChangeEventToJSValue);
        auto onSizeChange = EventMarker(
            [execCtx = args.GetExecutionContext(), func = std::move(onSizeChangeFunc)](const BaseEventInfo* param) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                auto sizeChange = TypeInfoHelper::DynamicCast<SlidingPanelSizeChangeEvent>(param);
                if (!sizeChange) {
                    LOGE("HandleSizeChangeEvent, sizeChange == nullptr");
                    return;
                }
                ACE_SCORING_EVENT("SlidingPanel.OnSizeChange");
                func->Execute(*sizeChange);
            });
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
        if (panel) {
            panel->SetOnSizeChanged(onSizeChange);
        }
    }
    args.ReturnSelf();
}

void JSSlidingPanel::SetHasDragBar(bool hasDragBar)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (panel) {
        panel->SetHasDragBar(hasDragBar);
    }
}

void JSSlidingPanel::SetShow(bool isShow)
{
    auto component = ViewStackProcessor::GetInstance()->GetDisplayComponent();
    auto display = AceType::DynamicCast<DisplayComponent>(component);
    if (!display) {
        LOGE("display is null");
        return;
    }
    display->SetVisible(isShow ? VisibleType::VISIBLE : VisibleType::GONE);
}

void JSSlidingPanel::SetPanelMode(int32_t mode)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (panel) {
        if (static_cast<int32_t>(PanelMode::HALF) == mode) {
            panel->SetMode(PanelMode::HALF);
        } else if (static_cast<int32_t>(PanelMode::MINI) == mode) {
            panel->SetMode(PanelMode::MINI);
        } else {
            panel->SetMode(PanelMode::FULL);
        }
    }
}

void JSSlidingPanel::SetPanelType(int32_t type)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (panel) {
        if (static_cast<int32_t>(PanelType::MINI_BAR) == type) {
            panel->SetType(PanelType::MINI_BAR);
        } else if (static_cast<int32_t>(PanelType::TEMP_DISPLAY) == type) {
            panel->SetType(PanelType::TEMP_DISPLAY);
        } else {
            panel->SetType(PanelType::FOLDABLE_BAR);
        }
    }
}

void JSSlidingPanel::SetMiniHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::pair<Dimension, bool> minHeight;
    minHeight.second = true;
    if (!ParseJsDimensionVp(info[0], minHeight.first)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (panel) {
        panel->SetMiniHeight(minHeight);
    }
}

void JSSlidingPanel::SetHalfHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::pair<Dimension, bool> halfHeight;
    halfHeight.second = true;
    if (!ParseJsDimensionVp(info[0], halfHeight.first)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (panel) {
        panel->SetHalfHeight(halfHeight);
    }
}

void JSSlidingPanel::SetFullHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    std::pair<Dimension, bool> fullHeight;
    fullHeight.second = true;
    if (!ParseJsDimensionVp(info[0], fullHeight.first)) {
        return;
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto panel = AceType::DynamicCast<SlidingPanelComponent>(component);
    if (panel) {
        panel->SetFullHeight(fullHeight);
    }
}

} // namespace OHOS::Ace::Framework
