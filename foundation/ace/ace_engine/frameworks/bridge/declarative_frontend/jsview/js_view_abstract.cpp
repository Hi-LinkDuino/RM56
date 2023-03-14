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

#include "bridge/declarative_frontend/jsview/js_view_abstract.h"

#include <algorithm>
#include <regex>
#include <vector>

#include "base/json/json_util.h"
#include "bridge/common/utils/utils.h"
#include "bridge/declarative_frontend/engine/functions/js_drag_function.h"
#include "bridge/declarative_frontend/engine/functions/js_focus_function.h"
#include "bridge/declarative_frontend/engine/functions/js_function.h"
#include "bridge/declarative_frontend/engine/functions/js_on_area_change_function.h"
#include "bridge/declarative_frontend/jsview/js_utils.h"
#include "core/components_v2/extensions/events/on_area_change_extension.h"
#ifdef USE_V8_ENGINE
#include "bridge/declarative_frontend/engine/v8/functions/v8_function.h"
#endif
#include "bridge/declarative_frontend/jsview/js_grid_container.h"
#include "bridge/declarative_frontend/jsview/js_view_register.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/common/ace_application_info.h"
#include "core/components/box/box_component_helper.h"
#include "core/components/common/layout/align_declaration.h"
#include "core/components/common/properties/motion_path_option.h"
#include "core/components/menu/menu_component.h"
#include "core/components/option/option_component.h"
#include "core/gestures/long_press_gesture.h"
#include "frameworks/base/memory/referenced.h"
#include "frameworks/bridge/declarative_frontend/engine/functions/js_click_function.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_shape_abstract.h"
#include "frameworks/core/components/text/text_component.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t DEFAULT_DURATION = 1000; // ms
constexpr uint32_t COLOR_ALPHA_OFFSET = 24;
constexpr uint32_t COLOR_ALPHA_VALUE = 0xFF000000;
constexpr int32_t MAX_ALIGN_VALUE = 8;
constexpr int32_t DEFAULT_LONG_PRESS_FINGER = 1;
constexpr int32_t DEFAULT_LONG_PRESS_DURATION = 500;
constexpr double EPSILON = 0.000002f;
const std::regex RESOURCE_APP_STRING_PLACEHOLDER(R"(\%((\d+)(\$)){0,1}([dsf]))", std::regex::icase);
constexpr double FULL_DIMENSION = 100.0;
constexpr double HALF_DIMENSION = 50.0;

bool CheckJSCallbackInfo(const std::string& callerName, const JSCallbackInfo& info,
    std::vector<JSCallbackInfoType>& infoTypes)
{
    if (info.Length() < 1) {
        LOGE("%{public}s: The arg is supposed to have at least one argument", callerName.c_str());
        return false;
    }
    bool typeVerified = false;
    std::string unrecognizedType;
    for (const auto& infoType : infoTypes) {
        switch (infoType) {
            case JSCallbackInfoType::STRING:
                if (info[0]->IsString()) {
                    typeVerified = true;
                } else {
                    unrecognizedType += "string|";
                }
                break;
            case JSCallbackInfoType::NUMBER:
                if (info[0]->IsNumber()) {
                    typeVerified = true;
                } else {
                    unrecognizedType += "number|";
                }
                break;
            case JSCallbackInfoType::OBJECT:
                if (info[0]->IsObject()) {
                    typeVerified = true;
                } else {
                    unrecognizedType += "object|";
                }
                break;
            case JSCallbackInfoType::FUNCTION:
                if (info[0]->IsFunction()) {
                    typeVerified = true;
                } else {
                    unrecognizedType += "Function|";
                }
                break;
            default:
                break;
        }
    }
    if (!typeVerified) {
        LOGE("%{public}s: info[0] is not a [%{public}s]",
            callerName.c_str(), unrecognizedType.substr(0, unrecognizedType.size()-1).c_str());
    }
    return typeVerified || infoTypes.size() == 0;
}

void ParseJsScale(std::unique_ptr<JsonValue>& argsPtrItem, float& scaleX, float& scaleY, float& scaleZ,
    Dimension& centerX, Dimension& centerY)
{
    double xVal = 1.0;
    double yVal = 1.0;
    double zVal = 1.0;
    JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("x"), xVal);
    JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("y"), yVal);
    JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("z"), zVal);
    scaleX = static_cast<float>(xVal);
    scaleY = static_cast<float>(yVal);
    scaleZ = static_cast<float>(zVal);
    // if specify centerX
    Dimension length;
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("centerX"), length)) {
        centerX = length;
    }
    // if specify centerY
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("centerY"), length)) {
        centerY = length;
    }
}

void ParseJsTranslate(
    std::unique_ptr<JsonValue>& argsPtrItem, Dimension& translateX, Dimension& translateY, Dimension& translateZ)
{
    Dimension length;
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("x"), length)) {
        translateX = length;
    }
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("y"), length)) {
        translateY = length;
    }
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("z"), length)) {
        translateZ = length;
    }
}

void ParseJsRotate(std::unique_ptr<JsonValue>& argsPtrItem, float& dx, float& dy, float& dz, Dimension& centerX,
    Dimension& centerY, std::optional<float>& angle)
{
    // default: dx, dy, dz (0.0, 0.0, 0.0)
    double dxVal = 0.0;
    double dyVal = 0.0;
    double dzVal = 0.0;
    JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("x"), dxVal);
    JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("y"), dyVal);
    JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("z"), dzVal);
    dx = static_cast<float>(dxVal);
    dy = static_cast<float>(dyVal);
    dz = static_cast<float>(dzVal);
    // if specify centerX
    Dimension length;
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("centerX"), length)) {
        centerX = length;
    }
    // if specify centerY
    if (JSViewAbstract::ParseJsonDimensionVp(argsPtrItem->GetValue("centerY"), length)) {
        centerY = length;
    }
    // if specify angle
    JSViewAbstract::GetAngle("angle", argsPtrItem, angle);
}

void SetDefaultTransition(TransitionType transitionType)
{
    auto display = ViewStackProcessor::GetInstance()->GetDisplayComponent();
    if (!display) {
        LOGE("display component is null.");
        return;
    }
    LOGI("JsTransition with default");
    display->SetTransition(transitionType, 0.0);
}

bool ParseAndSetOpacityTransition(const std::unique_ptr<JsonValue>& transitionArgs, TransitionType transitionType)
{
    if (transitionArgs->Contains("opacity")) {
        double opacity = 0.0;
        JSViewAbstract::ParseJsonDouble(transitionArgs->GetValue("opacity"), opacity);
        auto display = ViewStackProcessor::GetInstance()->GetDisplayComponent();
        if (!display) {
            LOGE("display component is null.");
            return true;
        }
        LOGI("JsTransition with type: %{public}d, opacity: %{public}.2f", transitionType, opacity);
        display->SetTransition(transitionType, opacity);
        return true;
    }
    return false;
}

bool ParseAndSetRotateTransition(const std::unique_ptr<JsonValue>& transitionArgs, TransitionType transitionType)
{
    if (transitionArgs->Contains("rotate")) {
        auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
        if (!transform) {
            LOGE("transform component is null.");
            return true;
        }
        auto rotateArgs = transitionArgs->GetObject("rotate");
        // default: dx, dy, dz (0.0, 0.0, 0.0)
        float dx = 0.0f;
        float dy = 0.0f;
        float dz = 0.0f;
        // default centerX, centerY 50% 50%;
        Dimension centerX = 0.5_pct;
        Dimension centerY = 0.5_pct;
        std::optional<float> angle;
        ParseJsRotate(rotateArgs, dx, dy, dz, centerX, centerY, angle);
        if (angle) {
            transform->SetRotateTransition(transitionType, dx, dy, dz, angle.value());
            transform->SetOriginDimension(DimensionOffset(centerX, centerY));
            LOGI("JsTransition with type: %{public}d. rotate: [%.2f, %.2f, %.2f] [%.2f, %.2f] %.2f", transitionType, dx,
                dy, dz, centerX.Value(), centerY.Value(), angle.value());
        }
        return true;
    }
    return false;
}

bool ParseAndSetScaleTransition(const std::unique_ptr<JsonValue>& transitionArgs, TransitionType transitionType)
{
    if (transitionArgs->Contains("scale")) {
        auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
        if (!transform) {
            LOGE("transform component is null.");
            return true;
        }
        auto scaleArgs = transitionArgs->GetObject("scale");
        // default: x, y, z (1.0, 1.0, 1.0)
        auto scaleX = 1.0f;
        auto scaleY = 1.0f;
        auto scaleZ = 1.0f;
        // default centerX, centerY 50% 50%;
        Dimension centerX = 0.5_pct;
        Dimension centerY = 0.5_pct;
        ParseJsScale(scaleArgs, scaleX, scaleY, scaleZ, centerX, centerY);
        transform->SetScaleTransition(transitionType, scaleX, scaleY, scaleZ);
        transform->SetOriginDimension(DimensionOffset(centerX, centerY));
        LOGI("JsTransition with type: %{public}d. scale: [%.2f, %.2f, %.2f] [%.2f, %.2f]", transitionType, scaleX,
            scaleY, scaleZ, centerX.Value(), centerY.Value());
        return true;
    }
    return false;
}

bool ParseAndSetTranslateTransition(const std::unique_ptr<JsonValue>& transitionArgs, TransitionType transitionType)
{
    if (transitionArgs->Contains("translate")) {
        auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
        if (!transform) {
            LOGE("transform component is null.");
            return true;
        }
        auto translateArgs = transitionArgs->GetObject("translate");
        // default: x, y, z (0.0, 0.0, 0.0)
        auto translateX = Dimension(0.0);
        auto translateY = Dimension(0.0);
        auto translateZ = Dimension(0.0);
        ParseJsTranslate(translateArgs, translateX, translateY, translateZ);
        transform->SetTranslateTransition(transitionType, translateX, translateY, translateZ);
        LOGI("JsTransition with type: %{public}d. translate: [%.2f, %.2f, %.2f]", transitionType, translateX.Value(),
            translateY.Value(), translateZ.Value());
        return true;
    }
    return false;
}

bool ParseMotionPath(const std::unique_ptr<JsonValue>& argsPtrItem, MotionPathOption& option)
{
    if (argsPtrItem && !argsPtrItem->IsNull()) {
        auto path = argsPtrItem->GetString("path", "");
        if (!path.empty()) {
            option.SetPath(path);
            double from = 0.0;
            double to = 1.0;
            JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("from"), from);
            JSViewAbstract::ParseJsonDouble(argsPtrItem->GetValue("to"), to);
            option.SetBegin(static_cast<float>(from));
            option.SetEnd(static_cast<float>(to));
            option.SetRotate(argsPtrItem->GetBool("rotatable", false));
            return true;
        }
    }
    return false;
}

void SetBgImgPosition(const DimensionUnit& typeX, const DimensionUnit& typeY,
    const double valueX, const double valueY, BackgroundImagePosition& bgImgPosition)
{
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    bgImgPosition.SetSizeX(AnimatableDimension(valueX, typeX, option));
    bgImgPosition.SetSizeY(AnimatableDimension(valueY, typeY, option));
}

std::string GetReplaceContentStr(int pos, const std::string& type, JSRef<JSArray> params, int32_t containCount)
{
    JSRef<JSVal> item = params->GetValueAt(pos + containCount);
    if (type == "d") {
        if (item->IsNumber()) {
            return std::to_string(item->ToNumber<uint32_t>());
        }
    } else if (type == "s") {
        if (item->IsString()) {
            return item->ToString();
        }
    } else if (type == "f") {
        if (item->IsNumber()) {
            return std::to_string(item->ToNumber<float>());
        }
    }
    return std::string();
}

void ReplaceHolder(std::string& originStr, JSRef<JSArray> params, int32_t containCount)
{
    auto size = static_cast<int32_t>(params->Length());
    if (containCount == size) {
        return;
    }
    std::string::const_iterator start = originStr.begin();
    std::string::const_iterator end = originStr.end();
    std::smatch matchs;
    bool shortHolderType = false;
    bool firstMatch = true;
    int searchTime = 0;
    while (std::regex_search(start, end, matchs, RESOURCE_APP_STRING_PLACEHOLDER)) {
        std::string pos = matchs[2];
        std::string type = matchs[4];
        if (firstMatch) {
            firstMatch = false;
            shortHolderType = pos.length() == 0;
        } else {
            if (shortHolderType ^ (pos.length() == 0)) {
                LOGE("wrong place holder,stop parse string");
                return;
            }
        }

        std::string replaceContentStr;
        if (shortHolderType) {
            replaceContentStr = GetReplaceContentStr(searchTime, type, params, containCount);
        } else {
            replaceContentStr = GetReplaceContentStr(StringToInt(pos) - 1, type, params, containCount);
        }

        originStr.replace(matchs[0].first - originStr.begin(), matchs[0].length(), replaceContentStr);
        start = originStr.begin() + matchs.prefix().length() + replaceContentStr.length();
        end = originStr.end();
        searchTime++;
    }
}

bool ParseLocationProps(const JSCallbackInfo& info, AnimatableDimension& x, AnimatableDimension& y)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("ParseLocationProps", info, checkList)) {
        return false;
    }
    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JSRef<JSVal> xVal = sizeObj->GetProperty("x");
    JSRef<JSVal> yVal = sizeObj->GetProperty("y");
    bool hasX = JSViewAbstract::ParseJsAnimatableDimensionVp(xVal, x);
    bool hasY = JSViewAbstract::ParseJsAnimatableDimensionVp(yVal, y);
    return hasX || hasY;
}

#ifndef WEARABLE_PRODUCT
const std::vector<Placement> PLACEMENT = { Placement::LEFT, Placement::RIGHT, Placement::TOP, Placement::BOTTOM,
    Placement::TOP_LEFT, Placement::TOP_RIGHT, Placement::BOTTOM_LEFT, Placement::BOTTOM_RIGHT };

void ParseShowObject(
    const JSCallbackInfo& info, const JSRef<JSObject>& showObj, const RefPtr<PopupComponentV2>& popupComponent)
{
    JSRef<JSVal> changeEventVal = showObj->GetProperty("changeEvent");
    if (changeEventVal->IsFunction()) {
        RefPtr<JsFunction> jsFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(changeEventVal));
        auto eventMarker = EventMarker(
            [execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("Popup.onStateChange");

                if (param != "true" && param != "false") {
                    LOGE("param is not equal true or false, invalid.");
                    return;
                }

                bool newValue = StringToBool(param);
                JSRef<JSVal> newJSVal = JSRef<JSVal>::Make(ToJSValue(newValue));
                func->ExecuteJS(1, &newJSVal);
            });
        popupComponent->SetChangeEvent(eventMarker);
    }
}

void ParsePopupParam(
    const JSCallbackInfo& info, const JSRef<JSObject>& popupObj, const RefPtr<PopupComponentV2>& popupComponent)
{
    JSRef<JSVal> messageVal = popupObj->GetProperty("message");
    popupComponent->SetMessage(messageVal->ToString());

    JSRef<JSVal> placementOnTopVal = popupObj->GetProperty("placementOnTop");
    if (placementOnTopVal->IsBoolean()) {
        popupComponent->SetPlacementOnTop(placementOnTopVal->ToBoolean());
    }

    JSRef<JSVal> onStateChangeVal = popupObj->GetProperty("onStateChange");
    if (onStateChangeVal->IsFunction()) {
        std::vector<std::string> keys = { "isVisible" };
        RefPtr<JsFunction> jsFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onStateChangeVal));
        auto eventMarker = EventMarker(
            [execCtx = info.GetExecutionContext(), func = std::move(jsFunc), keys](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("Popup.onStateChange");
                func->Execute(keys, param);
            });
        popupComponent->SetOnStateChange(eventMarker);
    }

    JSRef<JSVal> primaryButtonVal = popupObj->GetProperty("primaryButton");
    if (primaryButtonVal->IsObject()) {
        ButtonProperties properties;
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(primaryButtonVal);
        JSRef<JSVal> value = obj->GetProperty("value");
        if (value->IsString()) {
            properties.value = value->ToString();
        }

        JSRef<JSVal> actionValue = obj->GetProperty("action");
        if (actionValue->IsFunction()) {
            auto actionFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(actionValue));
            EventMarker actionId([execCtx = info.GetExecutionContext(), func = std::move(actionFunc)]() {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("primaryButton.action");
                func->Execute();
            });
            properties.actionId = actionId;
        }
        properties.showButton = true;
        popupComponent->SetPrimaryButtonProperties(properties);
    }

    JSRef<JSVal> secondaryButtonVal = popupObj->GetProperty("secondaryButton");
    if (secondaryButtonVal->IsObject()) {
        ButtonProperties properties;
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(secondaryButtonVal);
        JSRef<JSVal> value = obj->GetProperty("value");
        if (value->IsString()) {
            properties.value = value->ToString();
        }

        JSRef<JSVal> actionValue = obj->GetProperty("action");
        if (actionValue->IsFunction()) {
            auto actionFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(actionValue));
            EventMarker actionId([execCtx = info.GetExecutionContext(), func = std::move(actionFunc)]() {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("secondaryButton.action");
                func->Execute();
            });
            properties.actionId = actionId;
        }
        properties.showButton = true;
        popupComponent->SetSecondaryButtonProperties(properties);
    }
}

void ParseCustomPopupParam(
    const JSCallbackInfo& info, const JSRef<JSObject>& popupObj, const RefPtr<PopupComponentV2>& popupComponent)
{
    RefPtr<Component> customComponent;
    auto builderValue = popupObj->GetProperty("builder");
    if (!builderValue->IsObject()) {
        LOGE("builder param is not an object.");
        return;
    }

    JSRef<JSObject> builderObj;
    builderObj = JSRef<JSObject>::Cast(builderValue);
    auto builder = builderObj->GetProperty("builder");
    if (!builder->IsFunction()) {
        LOGE("builder param is not a function.");
        return;
    }
    auto builderFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(builder));
    if (!builderFunc) {
        LOGE("builder function is null.");
        return;
    }
    // use another VSP instance while executing the builder function
    ScopedViewStackProcessor builderViewStackProcessor;
    {
        ACE_SCORING_EVENT("popup.builder");
        builderFunc->Execute();
    }
    customComponent = ViewStackProcessor::GetInstance()->Finish();
    popupComponent->SetCustomComponent(customComponent);

    auto popupParam = popupComponent->GetPopupParam();
    popupParam->SetUseCustomComponent(true);
    auto placementValue = popupObj->GetProperty("placement");
    if (placementValue->IsNumber()) {
        auto placement = placementValue->ToNumber<int32_t>();
        if (placement >= 0 && placement <= static_cast<int32_t>(PLACEMENT.size())) {
            popupParam->SetPlacement(PLACEMENT[placement]);
        }
    }

    auto maskColorValue = popupObj->GetProperty("maskColor");
    Color maskColor;
    if (JSViewAbstract::ParseJsColor(maskColorValue, maskColor)) {
        popupParam->SetMaskColor(maskColor);
    }

    auto popupColorValue = popupObj->GetProperty("popupColor");
    Color backgroundColor;
    if (JSViewAbstract::ParseJsColor(popupColorValue, backgroundColor)) {
        popupParam->SetBackgroundColor(backgroundColor);
    }

    auto enableArrowValue = popupObj->GetProperty("enableArrow");
    if (enableArrowValue->IsBoolean()) {
        popupParam->SetEnableArrow(enableArrowValue->ToBoolean());
    }

    auto autoCancelValue = popupObj->GetProperty("autoCancel");
    if (autoCancelValue->IsBoolean()) {
        popupParam->SetHasAction(!autoCancelValue->ToBoolean());
    }

    JSRef<JSVal> onStateChangeVal = popupObj->GetProperty("onStateChange");
    if (onStateChangeVal->IsFunction()) {
        std::vector<std::string> keys = { "isVisible" };
        RefPtr<JsFunction> jsFunc =
            AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(onStateChangeVal));
        auto eventMarker = EventMarker(
            [execCtx = info.GetExecutionContext(), func = std::move(jsFunc), keys](const std::string& param) {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
                ACE_SCORING_EVENT("popup.onStateChange");
                func->Execute(keys, param);
            });
        popupComponent->SetOnStateChange(eventMarker);
    }
}
#endif

} // namespace

uint32_t ColorAlphaAdapt(uint32_t origin)
{
    uint32_t result = origin;
    if (origin >> COLOR_ALPHA_OFFSET == 0) {
        result = origin | COLOR_ALPHA_VALUE;
    }
    return result;
}

void JSViewAbstract::JsScale(const JSCallbackInfo& info)
{
    LOGD("JsScale");
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER, JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsScale", info, checkList)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("x") || argsPtrItem->Contains("y") || argsPtrItem->Contains("z")) {
            // default: x, y, z (1.0, 1.0, 1.0)
            auto scaleX = 1.0f;
            auto scaleY = 1.0f;
            auto scaleZ = 1.0f;
            // default centerX, centerY 50% 50%;
            Dimension centerX = 0.5_pct;
            Dimension centerY = 0.5_pct;
            ParseJsScale(argsPtrItem, scaleX, scaleY, scaleZ, centerX, centerY);
            transform->Scale(scaleX, scaleY, scaleZ, option);
            transform->SetOriginDimension(DimensionOffset(centerX, centerY));
            return;
        }
    }
    double scale;
    if (ParseJsDouble(info[0], scale)) {
        transform->Scale(scale, option);
    }
}

void JSViewAbstract::JsScaleX(const JSCallbackInfo& info)
{
    LOGD("JsScaleX");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    double scaleVal = 0.0;
    if (!ParseJsDouble(info[0], scaleVal)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->ScaleX(scaleVal, option);
}

void JSViewAbstract::JsScaleY(const JSCallbackInfo& info)
{
    LOGD("JsScaleY");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    double scaleVal = 0.0;
    if (!ParseJsDouble(info[0], scaleVal)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->ScaleY(scaleVal, option);
}

void JSViewAbstract::JsOpacity(const JSCallbackInfo& info)
{
    LOGD("js_opacity");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    double opacity = 0.0;
    if (!ParseJsDouble(info[0], opacity)) {
        return;
    }

    auto display = ViewStackProcessor::GetInstance()->GetDisplayComponent();
    auto stack = ViewStackProcessor::GetInstance();
    auto option = stack->GetImplicitAnimationOption();
    if (!stack->IsVisualStateSet()) {
        display->SetOpacity(opacity, option);
    } else {
        display->GetStateAttributes()->AddAttribute<AnimatableDouble>(DisplayStateAttribute::OPACITY,
            AnimatableDouble(opacity, option), stack->GetVisualState());
        if (!display->GetStateAttributes()->
            HasAttribute(DisplayStateAttribute::OPACITY, VisualState::NORMAL)) {
            display->GetStateAttributes()->AddAttribute<AnimatableDouble>(DisplayStateAttribute::OPACITY,
                AnimatableDouble(display->GetOpacity(), option), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsTranslate(const JSCallbackInfo& info)
{
    LOGD("JsTranslate");
    std::vector<JSCallbackInfoType> checkList { JSCallbackInfoType::STRING, JSCallbackInfoType::NUMBER,
        JSCallbackInfoType::OBJECT };
    if (!CheckJSCallbackInfo("JsTranslate", info, checkList)) {
        return;
    }

    Dimension value;
    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("x") || argsPtrItem->Contains("y") || argsPtrItem->Contains("z")) {
            // default: x, y, z (0.0, 0.0, 0.0)
            auto translateX = Dimension(0.0);
            auto translateY = Dimension(0.0);
            auto translateZ = Dimension(0.0);
            ParseJsTranslate(argsPtrItem, translateX, translateY, translateZ);
            transform->Translate(translateX, translateY, translateZ, option);
            return;
        }
    }
    if (ParseJsDimensionVp(info[0], value)) {
        transform->Translate(value, value, option);
    }
}

void JSViewAbstract::JsTranslateX(const JSCallbackInfo& info)
{
    LOGD("JsTranslateX");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->TranslateX(value, option);
}

void JSViewAbstract::JsTranslateY(const JSCallbackInfo& info)
{
    LOGD("JsTranslateY");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->TranslateY(value, option);
}

void JSViewAbstract::JsRotate(const JSCallbackInfo& info)
{
    LOGD("JsRotate");
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER, JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsRotate", info, checkList)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("x") || argsPtrItem->Contains("y") || argsPtrItem->Contains("z")) {
            // default: dx, dy, dz (0.0, 0.0, 0.0)
            float dx = 0.0f;
            float dy = 0.0f;
            float dz = 0.0f;
            // default centerX, centerY 50% 50%;
            Dimension centerX = 0.5_pct;
            Dimension centerY = 0.5_pct;
            std::optional<float> angle;
            ParseJsRotate(argsPtrItem, dx, dy, dz, centerX, centerY, angle);
            if (angle) {
                transform->Rotate(dx, dy, dz, angle.value(), option);
                transform->SetOriginDimension(DimensionOffset(centerX, centerY));
            } else {
                LOGE("Js JsRotate failed, not specify angle");
            }
            return;
        }
    }
    double rotateZ;
    if (ParseJsDouble(info[0], rotateZ)) {
        transform->RotateZ(rotateZ, option);
    }
}

void JSViewAbstract::JsRotateX(const JSCallbackInfo& info)
{
    LOGD("JsRotateX");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    double rotateVal = 0.0;
    if (!ParseJsDouble(info[0], rotateVal)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->RotateX(rotateVal, option);
}

void JSViewAbstract::JsRotateY(const JSCallbackInfo& info)
{
    LOGD("JsRotateX");
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    double rotateVal = 0.0;
    if (!ParseJsDouble(info[0], rotateVal)) {
        return;
    }

    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->RotateY(rotateVal, option);
}

void JSViewAbstract::JsTransform(const JSCallbackInfo& info)
{
    LOGD("JsTransform");
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsTransform", info, checkList)) {
        return;
    }
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %{public}s", info[0]->ToString().c_str());
        return;
    }
    auto array = argsPtrItem->GetValue("matrix4x4");
    const auto matrix4Len = Matrix4::DIMENSION * Matrix4::DIMENSION;
    if (!array || array->IsNull() || !array->IsArray() || array->GetArraySize() != matrix4Len) {
        LOGE("Js Parse object failed, matrix4x4 is null or not Array");
        return;
    }
    std::vector<float> matrix(matrix4Len);
    for (int32_t i = 0; i < matrix4Len; i++) {
        double value = 0.0;
        ParseJsonDouble(array->GetArrayItem(i), value);
        matrix[i] = static_cast<float>(value);
    }
    auto transform = ViewStackProcessor::GetInstance()->GetTransformComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    transform->Matrix3d(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7],
        matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13], matrix[14], matrix[15], option);
}

void JSViewAbstract::JsTransition(const JSCallbackInfo& info)
{
    LOGD("JsTransition");
    if (info.Length() > 1) {
        LOGE("Too many arguments");
        return;
    }
    if (info.Length() == 0) {
        SetDefaultTransition(TransitionType::ALL);
        return;
    }
    if (!info[0]->IsObject()) {
        LOGE("arg is not Object.");
        return;
    }
    auto transitionArgs = JsonUtil::ParseJsonString(info[0]->ToString());
    ParseAndSetTransitionOption(transitionArgs);
}

void JSViewAbstract::ParseAndSetTransitionOption(std::unique_ptr<JsonValue>& transitionArgs)
{
    TransitionType transitionType = ParseTransitionType(transitionArgs->GetString("type", "All"));
    bool hasEffect = false;
    hasEffect = ParseAndSetOpacityTransition(transitionArgs, transitionType) || hasEffect;
    hasEffect = ParseAndSetTranslateTransition(transitionArgs, transitionType) || hasEffect;
    hasEffect = ParseAndSetScaleTransition(transitionArgs, transitionType) || hasEffect;
    hasEffect = ParseAndSetRotateTransition(transitionArgs, transitionType) || hasEffect;
    if (!hasEffect) {
        SetDefaultTransition(transitionType);
    }
}

void JSViewAbstract::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

bool JSViewAbstract::JsWidth(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return false;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }

    bool isPercentSize = value.Unit() == DimensionUnit::PERCENT ? true : false;
    if (isPercentSize) {
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
        if (renderComponent) {
            renderComponent->SetIsPercentSize(isPercentSize);
        }
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto box = stack->GetBoxComponent();
    auto option = stack->GetImplicitAnimationOption();
    if (!stack->IsVisualStateSet()) {
        box->SetWidth(value, option);
    } else {
        box->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::WIDTH,
            AnimatableDimension(value, option), stack->GetVisualState());
        if (!box->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::WIDTH, VisualState::NORMAL)) {
            box->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::WIDTH,
                AnimatableDimension(box->GetWidth(), option), VisualState::NORMAL);
        }
    }
    return true;
}

void JSViewAbstract::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

bool JSViewAbstract::JsHeight(const JSRef<JSVal>& jsValue)
{
    Dimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return false;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }

    bool isPercentSize = value.Unit() == DimensionUnit::PERCENT ? true : false;
    if (isPercentSize) {
        auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
        auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
        if (renderComponent) {
            renderComponent->SetIsPercentSize(isPercentSize);
        }
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto box = stack->GetBoxComponent();
    auto option = stack->GetImplicitAnimationOption();
    if (!stack->IsVisualStateSet()) {
        box->SetHeight(value, option);
    } else {
        box->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::HEIGHT,
            AnimatableDimension(value, option), stack->GetVisualState());
        if (!box->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::HEIGHT, VisualState::NORMAL)) {
            box->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::HEIGHT,
                AnimatableDimension(box->GetHeight(), option), VisualState::NORMAL);
        }
    }
    return true;
}

void JSViewAbstract::JsResponseRegion(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    std::vector<DimensionRect> result;
    if (!JSViewAbstract::ParseJsResponseRegionArray(info[0], result)) {
        return;
    }

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
    if (renderComponent) {
        renderComponent->SetResponseRegion(result);
        renderComponent->MarkResponseRegion(true);
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetResponseRegion(result);
    box->MarkResponseRegion(true);
    if (ViewStackProcessor::GetInstance()->HasClickGestureListenerComponent()) {
        auto click = ViewStackProcessor::GetInstance()->GetClickGestureListenerComponent();
        click->SetResponseRegion(result);
        click->MarkResponseRegion(true);
    }
    if (ViewStackProcessor::GetInstance()->HasTouchListenerComponent()) {
        auto touch = ViewStackProcessor::GetInstance()->GetTouchListenerComponent();
        touch->SetResponseRegion(result);
        touch->MarkResponseRegion(true);
    }
}

bool JSViewAbstract::ParseJsDimensionRect(const JSRef<JSVal>& jsValue, DimensionRect& result)
{
    if (!jsValue->IsObject()) {
        LOGE("arg is not Object.");
        return false;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> x = obj->GetProperty("x");
    JSRef<JSVal> y = obj->GetProperty("y");
    JSRef<JSVal> width = obj->GetProperty("width");
    JSRef<JSVal> height = obj->GetProperty("height");
    Dimension xDimen = result.GetOffset().GetX();
    Dimension yDimen = result.GetOffset().GetY();
    Dimension widthDimen = result.GetWidth();
    Dimension heightDimen = result.GetHeight();

    if (ParseJsDimension(x, xDimen, DimensionUnit::VP)) {
        auto offset = result.GetOffset();
        offset.SetX(xDimen);
        result.SetOffset(offset);
    }
    if (ParseJsDimension(y, yDimen, DimensionUnit::VP)) {
        auto offset = result.GetOffset();
        offset.SetY(yDimen);
        result.SetOffset(offset);
    }
    if (ParseJsDimension(width, widthDimen, DimensionUnit::VP)) {
        if (widthDimen.Unit() == DimensionUnit::PERCENT && widthDimen.Value() < 0) {
            return true;
        }
        result.SetWidth(widthDimen);
    }
    if (ParseJsDimension(height, heightDimen, DimensionUnit::VP)) {
        if (heightDimen.Unit() == DimensionUnit::PERCENT && heightDimen.Value() < 0) {
            return true;
        }
        result.SetHeight(heightDimen);
    }
    return true;
}

bool JSViewAbstract::ParseJsResponseRegionArray(const JSRef<JSVal>& jsValue, std::vector<DimensionRect>& result)
{
    if (!jsValue->IsArray() && !jsValue->IsObject()) {
        LOGE("arg is not array or Object.");
        return false;
    }

    if (jsValue->IsArray()) {
        JSRef<JSArray> array = JSRef<JSArray>::Cast(jsValue);
        for (size_t i = 0; i < array->Length(); i++) {
            Dimension xDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension yDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension widthDimen = Dimension(1, DimensionUnit::PERCENT);
            Dimension heightDimen = Dimension(1, DimensionUnit::PERCENT);
            DimensionOffset offsetDimen(xDimen, yDimen);
            DimensionRect dimenRect(widthDimen, heightDimen, offsetDimen);
            if (ParseJsDimensionRect(array->GetValueAt(i), dimenRect)) {
                result.emplace_back(dimenRect);
            } else {
                LOGE("Array element is not Object.");
                return false;
            }
        }
        return true;
    }

    Dimension xDimen = Dimension(0.0, DimensionUnit::VP);
    Dimension yDimen = Dimension(0.0, DimensionUnit::VP);
    Dimension widthDimen = Dimension(1, DimensionUnit::PERCENT);
    Dimension heightDimen = Dimension(1, DimensionUnit::PERCENT);
    DimensionOffset offsetDimen(xDimen, yDimen);
    DimensionRect dimenRect(widthDimen, heightDimen, offsetDimen);
    if (ParseJsDimensionRect(jsValue, dimenRect)) {
        result.emplace_back(dimenRect);
        return true;
    } else {
        LOGE("Array element is not Object.");
        return false;
    }
}

void JSViewAbstract::JsSize(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsSize", info, checkList)) {
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JsWidth(sizeObj->GetProperty("width"));
    JsHeight(sizeObj->GetProperty("height"));
}

void JSViewAbstract::JsConstraintSize(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsConstraintSize", info, checkList)) {
        return;
    }

    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);

    JSRef<JSVal> minWidthValue = sizeObj->GetProperty("minWidth");
    Dimension minWidth;
    if (ParseJsDimensionVp(minWidthValue, minWidth)) {
        box->SetMinWidth(minWidth);
        flexItem->SetMinWidth(minWidth);
    }

    JSRef<JSVal> maxWidthValue = sizeObj->GetProperty("maxWidth");
    Dimension maxWidth;
    if (ParseJsDimensionVp(maxWidthValue, maxWidth)) {
        box->SetMaxWidth(maxWidth);
        flexItem->SetMaxWidth(maxWidth);
    }

    JSRef<JSVal> minHeightValue = sizeObj->GetProperty("minHeight");
    Dimension minHeight;
    if (ParseJsDimensionVp(minHeightValue, minHeight)) {
        box->SetMinHeight(minHeight);
        flexItem->SetMinHeight(minHeight);
    }

    JSRef<JSVal> maxHeightValue = sizeObj->GetProperty("maxHeight");
    Dimension maxHeight;
    if (ParseJsDimensionVp(maxHeightValue, maxHeight)) {
        box->SetMaxHeight(maxHeight);
        flexItem->SetMaxHeight(maxHeight);
    }
}

void JSViewAbstract::JsLayoutPriority(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::STRING, JSCallbackInfoType::NUMBER};
    if (!CheckJSCallbackInfo("JsLayoutPriority", info, checkList)) {
        return;
    }

    int32_t priority;
    if (info[0]->IsNumber()) {
        priority = info[0]->ToNumber<int32_t>();
    } else {
        priority = static_cast<int32_t>(StringUtils::StringToUint(info[0]->ToString()));
    }

    auto flex = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flex->SetDisplayIndex(priority);
}

void JSViewAbstract::JsLayoutWeight(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::STRING, JSCallbackInfoType::NUMBER};
    if (!CheckJSCallbackInfo("JsLayoutWeight", info, checkList)) {
        return;
    }

    int32_t value;
    if (info[0]->IsNumber()) {
        value = info[0]->ToNumber<int32_t>();
    } else {
        value = static_cast<int32_t>(StringUtils::StringToUint(info[0]->ToString()));
    }

    auto flex = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flex->SetFlexWeight(value);
}

void JSViewAbstract::JsAlign(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER};
    if (!CheckJSCallbackInfo("JsAlign", info, checkList)) {
        return;
    }
    auto value = info[0]->ToNumber<int32_t>();
    Alignment alignment = ParseAlignment(value);
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetAlignment(alignment);
}

void JSViewAbstract::JsPosition(const JSCallbackInfo& info)
{
    AnimatableDimension x;
    AnimatableDimension y;
    if (ParseLocationProps(info, x, y)) {
        auto flexItemComponent = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
        flexItemComponent->SetLeft(x);
        flexItemComponent->SetTop(y);
        flexItemComponent->SetPositionType(PositionType::ABSOLUTE);
    }
}

void JSViewAbstract::JsMarkAnchor(const JSCallbackInfo& info)
{
    AnimatableDimension x;
    AnimatableDimension y;
    if (ParseLocationProps(info, x, y)) {
        auto flexItemComponent = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
        flexItemComponent->SetAnchorX(x);
        flexItemComponent->SetAnchorY(y);
    }
}

void JSViewAbstract::JsOffset(const JSCallbackInfo& info)
{
    AnimatableDimension x;
    AnimatableDimension y;
    if (ParseLocationProps(info, x, y)) {
        auto flexItemComponent = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
        flexItemComponent->SetLeft(x);
        flexItemComponent->SetTop(y);
        flexItemComponent->SetPositionType(PositionType::OFFSET);
    }
}

void JSViewAbstract::JsEnabled(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!info[0]->IsBoolean()) {
        LOGE("arg is not bool.");
        return;
    }

    auto rootComponent = ViewStackProcessor::GetInstance()->GetRootComponent();
    rootComponent->SetDisabledStatus(!(info[0]->ToBoolean()));
}

void JSViewAbstract::JsAspectRatio(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    double value = 0.0;
    if (!ParseJsDouble(info[0], value)) {
        return;
    }
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!boxComponent) {
        LOGE("boxComponent is null");
        return;
    }
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    boxComponent->SetAspectRatio(value, option);
}

void JSViewAbstract::JsOverlay(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::STRING};
    if (!CheckJSCallbackInfo("JsOverlay", info, checkList)) {
        return;
    }

    auto coverageComponent = ViewStackProcessor::GetInstance()->GetCoverageComponent();
    std::string text = info[0]->ToString();
    coverageComponent->SetTextVal(text);
    coverageComponent->SetIsOverLay(true);

    if (info.Length() > 1 && !info[1]->IsNull()) {
        JSRef<JSObject> optionObj = JSRef<JSObject>::Cast(info[1]);
        JSRef<JSVal> alignVal = optionObj->GetProperty("align");
        auto value = alignVal->ToNumber<int32_t>();
        Alignment alignment = ParseAlignment(value);
        coverageComponent->SetAlignment(alignment);

        JSRef<JSVal> val = optionObj->GetProperty("offset");
        if (val->IsObject()) {
            JSRef<JSObject> offsetObj = JSRef<JSObject>::Cast(val);
            JSRef<JSVal> xVal = offsetObj->GetProperty("x");
            Dimension x;
            if (ParseJsDimensionVp(xVal, x)) {
                coverageComponent->SetX(x);
            }
            JSRef<JSVal> yVal = offsetObj->GetProperty("y");
            Dimension y;
            if (ParseJsDimensionVp(yVal, y)) {
                coverageComponent->SetY(y);
            }
        }
    }
}

Alignment JSViewAbstract::ParseAlignment(int32_t align)
{
    Alignment alignment = Alignment::CENTER;
    switch (align) {
        case 0:
            alignment = Alignment::TOP_LEFT;
            break;
        case 1:
            alignment = Alignment::TOP_CENTER;
            break;
        case 2:
            alignment = Alignment::TOP_RIGHT;
            break;
        case 3:
            alignment = Alignment::CENTER_LEFT;
            break;
        case 4:
            alignment = Alignment::CENTER;
            break;
        case 5:
            alignment = Alignment::CENTER_RIGHT;
            break;
        case 6:
            alignment = Alignment::BOTTOM_LEFT;
            break;
        case 7:
            alignment = Alignment::BOTTOM_CENTER;
            break;
        case 8:
            alignment = Alignment::BOTTOM_RIGHT;
            break;
        default:
            LOGE("Invalid value for alignment");
    }
    return alignment;
}

void JSViewAbstract::SetVisibility(int value)
{
    auto display = ViewStackProcessor::GetInstance()->GetDisplayComponent();
    display->SetVisible(VisibleType(value));
}

void JSViewAbstract::JsFlexBasis(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JsFlexBasis: The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flexItem->SetFlexBasis(value);
}

void JSViewAbstract::JsFlexGrow(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JsFlexGrow: The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    double value = 0.0;
    if (!ParseJsDouble(info[0], value)) {
        return;
    }
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flexItem->SetFlexGrow(value);
}

void JSViewAbstract::JsFlexShrink(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JsFlexShrink: The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    double value = 0.0;
    if (!ParseJsDouble(info[0], value)) {
        return;
    }
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flexItem->SetFlexShrink(value);
}

void JSViewAbstract::JsDisplayPriority(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JsDisplayPriority: The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    double value = 0.0;
    if (!ParseJsDouble(info[0], value)) {
        return;
    }
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    flexItem->SetDisplayIndex(value);
}

void JSViewAbstract::JsSharedTransition(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::STRING};
    if (!CheckJSCallbackInfo("JsSharedTransition", info, checkList)) {
        return;
    }
    // id
    auto id = info[0]->ToString();
    if (id.empty()) {
        LOGE("JsSharedTransition: id is empty.");
        return;
    }
    auto sharedTransitionComponent = ViewStackProcessor::GetInstance()->GetSharedTransitionComponent();
    sharedTransitionComponent->SetShareId(id);

    // options
    if (info.Length() > 1 && info[1]->IsObject()) {
        auto optionsArgs = JsonUtil::ParseJsonString(info[1]->ToString());
        // default: duration: 1000; if not specify: duration: 0
        int32_t duration = 0;
        auto durationValue = optionsArgs->GetValue("duration");
        if (durationValue && durationValue->IsNumber()) {
            duration = durationValue->GetInt();
            if (duration < 0) {
                duration = DEFAULT_DURATION;
            }
        }
        // default: delay: 0
        auto delay = optionsArgs->GetInt("delay", 0);
        if (delay < 0) {
            delay = 0;
        }
        // default: LinearCurve
        RefPtr<Curve> curve;
        auto curveArgs = optionsArgs->GetValue("curve");
        if (curveArgs->IsString()) {
            curve = CreateCurve(optionsArgs->GetString("curve", "linear"));
        } else if (curveArgs->IsObject()) {
            auto curveString = curveArgs->GetValue("__curveString");
            if (!curveString) {
                return;
            }
            curve = CreateCurve(curveString->GetString());
        } else {
            curve = AceType::MakeRefPtr<LinearCurve>();
        }
        TweenOption tweenOption;
        tweenOption.SetCurve(curve);
        tweenOption.SetDuration(static_cast<int32_t>(duration));
        tweenOption.SetDelay(static_cast<int32_t>(delay));
        // motionPath

        if (optionsArgs->Contains("motionPath")) {
            MotionPathOption motionPathOption;
            if (ParseMotionPath(optionsArgs->GetValue("motionPath"), motionPathOption)) {
                tweenOption.SetMotionPathOption(motionPathOption);
            }
        }
        // zIndex
        int32_t zIndex = 0;
        if (optionsArgs->Contains("zIndex")) {
            zIndex = optionsArgs->GetInt("zIndex", 0);
        }
        // type
        SharedTransitionEffectType type = SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE;
        if (optionsArgs->Contains("type")) {
            type = static_cast<SharedTransitionEffectType>(
                optionsArgs->GetInt("type", static_cast<int32_t>(SharedTransitionEffectType::SHARED_EFFECT_EXCHANGE)));
        }
        // effect: exchange
        auto sharedTransitionEffect =
            SharedTransitionEffect::GetSharedTransitionEffect(type, sharedTransitionComponent->GetShareId());
        sharedTransitionComponent->SetEffect(sharedTransitionEffect);
        sharedTransitionComponent->SetOption(tweenOption);
        if (zIndex != 0) {
            sharedTransitionComponent->SetZIndex(zIndex);
        }
    }
}

void JSViewAbstract::JsGeometryTransition(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::STRING};
    if (!CheckJSCallbackInfo("JsGeometryTransition", info, checkList)) {
        return;
    }
    // id
    auto id = info[0]->ToString();
    if (id.empty()) {
        LOGE("JsGeometryTransition: id is empty.");
        return;
    }
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!boxComponent) {
        LOGE("boxComponent is null");
        return;
    }
    boxComponent->SetGeometryTransitionId(id);
}

void JSViewAbstract::JsAlignSelf(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER};
    if (!CheckJSCallbackInfo("JsAlignSelf", info, checkList)) {
        return;
    }
    auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
    auto alignVal = info[0]->ToNumber<int32_t>();

    if (alignVal >= 0 && alignVal <= MAX_ALIGN_VALUE) {
        flexItem->SetAlignSelf((FlexAlign)alignVal);
    }
}

void JSViewAbstract::JsBorderColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color borderColor;
    if (!ParseJsColor(info[0], borderColor)) {
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto option = stack->GetImplicitAnimationOption();
    if (!stack->IsVisualStateSet()) {
        BoxComponentHelper::SetBorderColor(GetBackDecoration(), borderColor, option);
    } else {
        auto boxComponent = AceType::DynamicCast<BoxComponent>(stack->GetBoxComponent());
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<AnimatableColor>(BoxStateAttribute::BORDER_COLOR,
            AnimatableColor(borderColor, option), stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::BORDER_COLOR, VisualState::NORMAL)) {
            auto c = BoxComponentHelper::GetBorderColor(GetBackDecoration());
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableColor>(BoxStateAttribute::BORDER_COLOR,
                AnimatableColor(c, option), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsBackgroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color backgroundColor;
    if (!ParseJsColor(info[0], backgroundColor)) {
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto boxComponent = stack->GetBoxComponent();
    if (!boxComponent) {
        LOGE("boxComponent is null");
        return;
    }
    auto option = stack->GetImplicitAnimationOption();
    if (!stack->IsVisualStateSet()) {
        boxComponent->SetColor(backgroundColor, option);
    } else {
        boxComponent->GetStateAttributes()->AddAttribute<AnimatableColor>(BoxStateAttribute::COLOR,
            AnimatableColor(backgroundColor, option), stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::COLOR, VisualState::NORMAL)) {
            Color c = boxComponent->GetColor();
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableColor>(BoxStateAttribute::COLOR,
                AnimatableColor(c, option), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsBackgroundImage(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    std::string src;
    if (info[0]->IsString()) {
        src = info[0]->ToString();
    } else if (!ParseJsMedia(info[0], src)) {
        LOGE("can not parse image src.");
        return;
    }

    auto decoration = GetBackDecoration();
    if (!decoration) {
        LOGE("The decoration is nullptr.");
        return;
    }
    auto image = decoration->GetImage();
    if (!image) {
        image = AceType::MakeRefPtr<BackgroundImage>();
    }

    if (info[0]->IsString()) {
        image->SetSrc(src, GetThemeConstants());
    } else {
        image->SetParsedSrc(src);
    }

    int32_t repeatIndex = 0;
    if (info.Length() == 2 && info[1]->IsNumber()) {
        repeatIndex = info[1]->ToNumber<int32_t>();
    }
    auto repeat = static_cast<ImageRepeat>(repeatIndex);
    image->SetImageRepeat(repeat);
    decoration->SetImage(image);
}

void JSViewAbstract::JsBackgroundImageSize(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER, JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsBackgroundImageSize", info, checkList)) {
        return;
    }
    auto decoration = GetBackDecoration();
    if (!decoration) {
        LOGE("The decoration is nullptr.");
        return;
    }
    auto image = decoration->GetImage();
    if (!image) {
        image = AceType::MakeRefPtr<BackgroundImage>();
    }
    BackgroundImageSize bgImgSize;
    if (info[0]->IsNumber()) {
        auto sizeType = static_cast<BackgroundImageSizeType>(info[0]->ToNumber<int32_t>());
        bgImgSize.SetSizeTypeX(sizeType);
        bgImgSize.SetSizeTypeY(sizeType);
    } else {
        auto imageArgs = JsonUtil::ParseJsonString(info[0]->ToString());
        if (imageArgs->IsNull()) {
            LOGE("Js Parse failed. imageArgs is null.");
            return;
        }
        Dimension width;
        Dimension height;
        ParseJsonDimensionVp(imageArgs->GetValue("width"), width);
        ParseJsonDimensionVp(imageArgs->GetValue("height"), height);
        double valueWidth = width.Value();
        double valueHeight = height.Value();
        BackgroundImageSizeType typeWidth = BackgroundImageSizeType::LENGTH;
        BackgroundImageSizeType typeHeight = BackgroundImageSizeType::LENGTH;
        if (width.Unit() == DimensionUnit::PERCENT) {
            typeWidth = BackgroundImageSizeType::PERCENT;
            valueWidth = width.Value() * FULL_DIMENSION;
        }
        if (height.Unit() == DimensionUnit::PERCENT) {
            typeHeight = BackgroundImageSizeType::PERCENT;
            valueHeight = height.Value() * FULL_DIMENSION;
        }
        bgImgSize.SetSizeTypeX(typeWidth);
        bgImgSize.SetSizeValueX(valueWidth);
        bgImgSize.SetSizeTypeY(typeHeight);
        bgImgSize.SetSizeValueY(valueHeight);
    }
    image->SetImageSize(bgImgSize);
    decoration->SetImage(image);
}

void JSViewAbstract::JsBackgroundImagePosition(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER, JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsBackgroundImagePosition", info, checkList)) {
        return;
    }
    auto decoration = GetBackDecoration();
    if (!decoration) {
        LOGE("The decoration is nullptr.");
        return;
    }
    auto image = decoration->GetImage();
    if (!image) {
        image = AceType::MakeRefPtr<BackgroundImage>();
    }
    BackgroundImagePosition bgImgPosition;
    if (info[0]->IsNumber()) {
        int32_t align = info[0]->ToNumber<int32_t>();
        switch (align) {
            case 0:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    0.0, 0.0, bgImgPosition);
                break;
            case 1:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    HALF_DIMENSION, 0.0, bgImgPosition);
                break;
            case 2:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    FULL_DIMENSION, 0.0, bgImgPosition);
                break;
            case 3:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    0.0, HALF_DIMENSION, bgImgPosition);
                break;
            case 4:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    HALF_DIMENSION, HALF_DIMENSION, bgImgPosition);
                break;
            case 5:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    FULL_DIMENSION, HALF_DIMENSION, bgImgPosition);
                break;
            case 6:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    0.0, FULL_DIMENSION, bgImgPosition);
                break;
            case 7:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    HALF_DIMENSION, FULL_DIMENSION, bgImgPosition);
                break;
            case 8:
                SetBgImgPosition(DimensionUnit::PERCENT, DimensionUnit::PERCENT,
                    FULL_DIMENSION, FULL_DIMENSION, bgImgPosition);
                break;
            default:
                break;
        }
    } else {
        auto imageArgs = JsonUtil::ParseJsonString(info[0]->ToString());
        if (imageArgs->IsNull()) {
            LOGE("Js Parse failed. imageArgs is null.");
            return;
        }
        Dimension x;
        Dimension y;
        ParseJsonDimensionVp(imageArgs->GetValue("x"), x);
        ParseJsonDimensionVp(imageArgs->GetValue("y"), y);
        double valueX = x.Value();
        double valueY = y.Value();
        DimensionUnit typeX = DimensionUnit::PX;
        DimensionUnit typeY = DimensionUnit::PX;
        if (x.Unit() == DimensionUnit::PERCENT) {
            valueX = x.Value() * FULL_DIMENSION;
            typeX = DimensionUnit::PERCENT;
        }
        if (y.Unit() == DimensionUnit::PERCENT) {
            valueY = y.Value() * FULL_DIMENSION;
            typeY = DimensionUnit::PERCENT;
        }
        SetBgImgPosition(typeX, typeY, valueX, valueY, bgImgPosition);
    }
    image->SetImagePosition(bgImgPosition);
    decoration->SetImage(image);
}

void JSViewAbstract::JsBindMenu(const JSCallbackInfo& info)
{
    ViewStackProcessor::GetInstance()->GetCoverageComponent();
    auto menuComponent = ViewStackProcessor::GetInstance()->GetMenuComponent(true);
    if (!menuComponent) {
        return;
    }
    auto click = ViewStackProcessor::GetInstance()->GetBoxComponent();
    RefPtr<Gesture> tapGesture = AceType::MakeRefPtr<TapGesture>();
    tapGesture->SetOnActionId([weak = WeakPtr<OHOS::Ace::MenuComponent>(menuComponent)](const GestureEvent& info) {
        auto refPtr = weak.Upgrade();
        if (!refPtr) {
            return;
        }
        auto showDialog = refPtr->GetTargetCallback();
        showDialog("BindMenu", info.GetGlobalLocation());
    });
    click->SetOnClick(tapGesture);
    auto menuTheme = GetTheme<SelectTheme>();
    menuComponent->SetTheme(menuTheme);

    if (info[0]->IsArray()) {
        auto context = info.GetExecutionContext();
        auto paramArray = JSRef<JSArray>::Cast(info[0]);
        size_t size = paramArray->Length();
        for (size_t i = 0; i < size; i++) {
            std::string value;
            auto indexObject = JSRef<JSObject>::Cast(paramArray->GetValueAt(i));
            auto menuValue = indexObject->GetProperty("value");
            auto menuAction = indexObject->GetProperty("action");
            ParseJsString(menuValue, value);
            auto action = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(menuAction));

            auto optionTheme = GetTheme<SelectTheme>();
            auto optionComponent = AceType::MakeRefPtr<OHOS::Ace::OptionComponent>(optionTheme);
            auto textComponent = AceType::MakeRefPtr<OHOS::Ace::TextComponent>(value);

            optionComponent->SetTheme(optionTheme);
            optionComponent->SetText(textComponent);
            optionComponent->SetValue(value);
            optionComponent->SetCustomizedCallback([action, context] {
                JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(context);
                ACE_SCORING_EVENT("menu.action");
                action->Execute();
            });
            menuComponent->AppendOption(optionComponent);
        }
    } else {
        JSRef<JSObject> menuObj;
        if (info[0]->IsObject()) {
            menuObj = JSRef<JSObject>::Cast(info[0]);
        } else {
            LOGE("No param object.");
            return;
        }

        auto builder = menuObj->GetProperty("builder");
        if (!builder->IsFunction()) {
            LOGE("builder param is not a function.");
            return;
        }
        auto builderFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(builder));
        if (!builderFunc) {
            LOGE("builder function is null.");
            return;
        }
        // use another VSP instance while executing the builder function
        ScopedViewStackProcessor builderViewStackProcessor;
        {
            ACE_SCORING_EVENT("menu.builder");
            builderFunc->Execute();
        }
        auto customComponent = ViewStackProcessor::GetInstance()->Finish();
        if (!customComponent) {
            LOGE("Custom component is null.");
            return;
        }

        auto optionTheme = GetTheme<SelectTheme>();
        auto optionComponent = AceType::MakeRefPtr<OHOS::Ace::OptionComponent>(optionTheme);
        optionComponent->SetCustomComponent(customComponent);

        menuComponent->AppendOption(optionComponent);
    }
}

void JSViewAbstract::JsPadding(const JSCallbackInfo& info)
{
    ParseMarginOrPadding(info, false);
}

void JSViewAbstract::JsMargin(const JSCallbackInfo& info)
{
    JSViewAbstract::ParseMarginOrPadding(info, true);
}

void JSViewAbstract::ParseMarginOrPadding(const JSCallbackInfo& info, bool isMargin)
{
    std::vector<JSCallbackInfoType> checkList { JSCallbackInfoType::STRING, JSCallbackInfoType::NUMBER,
        JSCallbackInfoType::OBJECT };
    if (!CheckJSCallbackInfo("ParseMarginOrPadding", info, checkList)) {
        return;
    }

    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("top") || argsPtrItem->Contains("bottom") || argsPtrItem->Contains("left") ||
            argsPtrItem->Contains("right")) {
            Dimension topDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension bottomDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension leftDimen = Dimension(0.0, DimensionUnit::VP);
            Dimension rightDimen = Dimension(0.0, DimensionUnit::VP);
            ParseJsonDimensionVp(argsPtrItem->GetValue("top"), topDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("bottom"), bottomDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("left"), leftDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("right"), rightDimen);
            if (isMargin) {
                SetMargins(topDimen, bottomDimen, leftDimen, rightDimen);
            } else {
                SetPaddings(topDimen, bottomDimen, leftDimen, rightDimen);
            }
            return;
        }
    }
    AnimatableDimension length;
    if (!ParseJsAnimatableDimensionVp(info[0], length)) {
        return;
    }
    if (isMargin) {
        SetMargin(length);
    } else {
        SetPadding(length);
    }
}

void JSViewAbstract::JsBorder(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsBorder", info, checkList)) {
        return;
    }
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        info.ReturnSelf();
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    AnimationOption option = stack->GetImplicitAnimationOption();
    auto boxComponent = AceType::DynamicCast<BoxComponent>(stack->GetBoxComponent());
    if (!boxComponent) {
        LOGE("boxComponent is null");
        return;
    }
    Dimension width;
    if (argsPtrItem->Contains("width") && ParseJsonDimensionVp(argsPtrItem->GetValue("width"), width)) {
        if (!stack->IsVisualStateSet()) {
            BoxComponentHelper::SetBorderWidth(GetBackDecoration(), width, option);
        } else {
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>
                (BoxStateAttribute::BORDER_WIDTH, AnimatableDimension(width, option), stack->GetVisualState());
            if (!boxComponent->GetStateAttributes()->
                HasAttribute(BoxStateAttribute::BORDER_WIDTH, VisualState::NORMAL)) {
                boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::BORDER_WIDTH,
                    AnimatableDimension(BoxComponentHelper::GetBorderWidth(GetBackDecoration()), option),
                    VisualState::NORMAL);
            }
        }
    }
    Color color;
    if (argsPtrItem->Contains("color") && ParseJsonColor(argsPtrItem->GetValue("color"), color)) {
        if (!stack->IsVisualStateSet()) {
            BoxComponentHelper::SetBorderColor(GetBackDecoration(), color, option);
        } else {
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableColor>(BoxStateAttribute::BORDER_COLOR,
                AnimatableColor(color, option), stack->GetVisualState());
            if (!boxComponent->GetStateAttributes()->
                HasAttribute(BoxStateAttribute::BORDER_COLOR, VisualState::NORMAL)) {
                Color c = BoxComponentHelper::GetBorderColor(GetBackDecoration());
                boxComponent->GetStateAttributes()->AddAttribute<AnimatableColor>
                    (BoxStateAttribute::BORDER_COLOR, AnimatableColor(c, option), VisualState::NORMAL);
            }
        }
    }
    Dimension radius;
    if (argsPtrItem->Contains("radius") && ParseJsonDimensionVp(argsPtrItem->GetValue("radius"), radius)) {
        if (!stack->IsVisualStateSet()) {
            BoxComponentHelper::SetBorderRadius(GetBackDecoration(), radius, option);
        } else {
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::BORDER_RADIUS,
                AnimatableDimension(radius, option), stack->GetVisualState());
            if (!boxComponent->GetStateAttributes()->
                HasAttribute(BoxStateAttribute::BORDER_RADIUS, VisualState::NORMAL)) {
                boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::BORDER_RADIUS,
                    AnimatableDimension(BoxComponentHelper::GetBorderRadius(GetBackDecoration()).GetX(), option),
                    VisualState::NORMAL);
            }
        }
    }
    if (argsPtrItem->Contains("style")) {
        auto borderStyle = argsPtrItem->GetInt("style", static_cast<int32_t>(BorderStyle::SOLID));
        SetBorderStyle(borderStyle);
    }
    info.ReturnSelf();
}

void JSViewAbstract::JsBorderWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension borderWidth;
    if (!ParseJsDimensionVp(info[0], borderWidth)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    AnimationOption option = stack->GetImplicitAnimationOption();
    if (!ViewStackProcessor::GetInstance()->IsVisualStateSet()) {
        BoxComponentHelper::SetBorderWidth(GetBackDecoration(), borderWidth, option);
    } else {
        auto boxComponent = AceType::DynamicCast<BoxComponent>(stack->GetBoxComponent());
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>
            (BoxStateAttribute::BORDER_WIDTH, AnimatableDimension(borderWidth, option), stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::BORDER_WIDTH, VisualState::NORMAL)) {
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>(
                BoxStateAttribute::BORDER_WIDTH,
                AnimatableDimension(BoxComponentHelper::GetBorderWidth(GetBackDecoration()), option),
                VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsBorderRadius(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Dimension borderRadius;
    if (!ParseJsDimensionVp(info[0], borderRadius)) {
        return;
    }
    auto stack = ViewStackProcessor::GetInstance();
    AnimationOption option = stack->GetImplicitAnimationOption();
    if (!stack->IsVisualStateSet()) {
        SetBorderRadius(borderRadius, option);
    } else {
        auto boxComponent = AceType::DynamicCast<BoxComponent>(stack->GetBoxComponent());
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::BORDER_RADIUS,
            AnimatableDimension(borderRadius, option), stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::BORDER_RADIUS, VisualState::NORMAL)) {
            boxComponent->GetStateAttributes()->AddAttribute<AnimatableDimension>(BoxStateAttribute::BORDER_RADIUS,
            AnimatableDimension(
                BoxComponentHelper::GetBorderRadius(GetBackDecoration()).GetX(), option), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsBlur(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }

    double blur = 0.0;
    if (!ParseJsDouble(info[0], blur)) {
        return;
    }
    SetBlur(blur);
    info.SetReturnValue(info.This());
}

void JSViewAbstract::JsColorBlend(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color colorBlend;
    if (!ParseJsColor(info[0], colorBlend)) {
        return;
    }
    SetColorBlend(colorBlend);
    info.SetReturnValue(info.This());
}

void JSViewAbstract::JsBackdropBlur(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }

    double blur = 0.0;
    if (!ParseJsDouble(info[0], blur)) {
        return;
    }
    SetBackdropBlur(blur);
    info.SetReturnValue(info.This());
}

void JSViewAbstract::JsWindowBlur(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsWindowBlur", info, checkList)) {
        return;
    }

    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        return;
    }
    double progress = 0.0;
    ParseJsonDouble(argsPtrItem->GetValue("percent"), progress);
    auto style = argsPtrItem->GetInt("style", static_cast<int32_t>(WindowBlurStyle::STYLE_BACKGROUND_SMALL_LIGHT));

    progress = std::clamp(progress, 0.0, 1.0);
    style = std::clamp(style, static_cast<int32_t>(WindowBlurStyle::STYLE_BACKGROUND_SMALL_LIGHT),
        static_cast<int32_t>(WindowBlurStyle::STYLE_BACKGROUND_XLARGE_DARK));

    SetWindowBlur(static_cast<float>(progress), static_cast<WindowBlurStyle>(style));
    info.SetReturnValue(info.This());
}

bool JSViewAbstract::ParseJsDimension(const JSRef<JSVal>& jsValue, Dimension& result, DimensionUnit defaultUnit)
{
    if (!jsValue->IsNumber() && !jsValue->IsString() && !jsValue->IsObject()) {
        LOGE("arg is not Number, String or Object.");
        return false;
    }

    if (jsValue->IsNumber()) {
        result = Dimension(jsValue->ToNumber<double>(), defaultUnit);
        return true;
    }
    if (jsValue->IsString()) {
        result = StringUtils::StringToDimensionWithUnit(jsValue->ToString(), defaultUnit);
        return true;
    }
    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGE("themeConstants is nullptr");
        return false;
    }
    result = themeConstants->GetDimension(resId->ToNumber<uint32_t>());
    return true;
}

bool JSViewAbstract::ParseJsDimensionVp(const JSRef<JSVal>& jsValue, Dimension& result)
{
    // 'vp' -> the value varies with pixel density of device.
    return ParseJsDimension(jsValue, result, DimensionUnit::VP);
}

bool JSViewAbstract::ParseJsAnimatableDimensionVp(const JSRef<JSVal>& jsValue, AnimatableDimension& result)
{
    if (ParseJsDimensionVp(jsValue, result)) {
        AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
        result.SetAnimationOption(option);
        return true;
    }
    return false;
}

bool JSViewAbstract::ParseJsDimensionFp(const JSRef<JSVal>& jsValue, Dimension& result)
{
    // the 'fp' unit is used for text scenes.
    return ParseJsDimension(jsValue, result, DimensionUnit::FP);
}

bool JSViewAbstract::ParseJsDimensionPx(const JSRef<JSVal>& jsValue, Dimension& result)
{
    return ParseJsDimension(jsValue, result, DimensionUnit::PX);
}

bool JSViewAbstract::ParseJsDouble(const JSRef<JSVal>& jsValue, double& result)
{
    if (!jsValue->IsNumber() && !jsValue->IsString() && !jsValue->IsObject()) {
        LOGE("arg is not Number, String or Object.");
        return false;
    }
    if (jsValue->IsNumber()) {
        result = jsValue->ToNumber<double>();
        return true;
    }
    if (jsValue->IsString()) {
        result = StringUtils::StringToDouble(jsValue->ToString());
        return true;
    }
    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }
    result = themeConstants->GetDouble(resId->ToNumber<uint32_t>());
    return true;
}

bool JSViewAbstract::ParseJsColor(const JSRef<JSVal>& jsValue, Color& result)
{
    if (!jsValue->IsNumber() && !jsValue->IsString() && !jsValue->IsObject()) {
        LOGE("arg is not Number, String or Object.");
        return false;
    }
    if (jsValue->IsNumber()) {
        result = Color(ColorAlphaAdapt(jsValue->ToNumber<uint32_t>()));
        return true;
    }
    if (jsValue->IsString()) {
        result = Color::FromString(jsValue->ToString());
        return true;
    }
    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }
    result = themeConstants->GetColor(resId->ToNumber<uint32_t>());
    return true;
}

bool JSViewAbstract::ParseJsFontFamilies(const JSRef<JSVal>& jsValue, std::vector<std::string>& result)
{
    result.clear();
    if (!jsValue->IsString() && !jsValue->IsObject()) {
        LOGE("arg is not String or Object.");
        return false;
    }
    if (jsValue->IsString()) {
        result = ConvertStrToFontFamilies(jsValue->ToString());
        return true;
    }
    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }
    result.emplace_back(themeConstants->GetString(resId->ToNumber<uint32_t>()));
    return true;
}

bool JSViewAbstract::ParseJsString(const JSRef<JSVal>& jsValue, std::string& result)
{
    if (!jsValue->IsString() && !jsValue->IsObject()) {
        LOGE("arg is not String or Object.");
        return false;
    }

    if (jsValue->IsString()) {
        LOGD("jsValue->IsString()");
        result = jsValue->ToString();
        return true;
    }

    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> type = jsObj->GetProperty("type");
    if (!type->IsNumber()) {
        LOGW("type is not number");
        return false;
    }

    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }

    JSRef<JSVal> args = jsObj->GetProperty("params");
    if (!args->IsArray()) {
        LOGW("args is not array");
        return false;
    }

    JSRef<JSArray> params = JSRef<JSArray>::Cast(args);
    if (type->ToNumber<uint32_t>() == static_cast<uint32_t>(ResourceType::STRING)) {
        auto originStr = themeConstants->GetString(resId->ToNumber<uint32_t>());
        ReplaceHolder(originStr, params, 0);
        result = originStr;
    } else if (type->ToNumber<uint32_t>() == static_cast<uint32_t>(ResourceType::PLURAL)) {
        auto countJsVal = params->GetValueAt(0);
        int count = 0;
        if (!countJsVal->IsNumber()) {
            LOGW("pluralString, pluralnumber is not number");
            return false;
        }
        count = countJsVal->ToNumber<int>();
        auto pluralStr = themeConstants->GetPluralString(resId->ToNumber<uint32_t>(), count);
        ReplaceHolder(pluralStr, params, 1);
        result = pluralStr;
    } else {
        return false;
    }

    return true;
}

bool JSViewAbstract::ParseJsMedia(const JSRef<JSVal>& jsValue, std::string& result)
{
    if (!jsValue->IsObject() && !jsValue->IsString()) {
        LOGE("arg is not Object and String.");
        return false;
    }
    if (jsValue->IsString()) {
        result = jsValue->ToString();
        return true;
    }
    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> type = jsObj->GetProperty("type");
    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNull() && !type->IsNull() && type->IsNumber() && resId->IsNumber()) {
        auto themeConstants = GetThemeConstants();
        if (!themeConstants) {
            LOGW("themeConstants is nullptr");
            return false;
        }
        auto typeInteger = type->ToNumber<int32_t>();
        if (typeInteger == static_cast<int>(ResourceType::MEDIA)) {
            result = themeConstants->GetMediaPath(resId->ToNumber<uint32_t>());
            return true;
        }
        if (typeInteger == static_cast<int>(ResourceType::RAWFILE)) {
            JSRef<JSVal> args = jsObj->GetProperty("params");
            if (!args->IsArray()) {
                LOGW("args is not Array");
                return false;
            }
            JSRef<JSArray> params = JSRef<JSArray>::Cast(args);
            auto fileName = params->GetValueAt(0);
            if (!fileName->IsString()) {
                LOGW("fileName is not String");
                return false;
            }
            result = themeConstants->GetRawfile(fileName->ToString());
            return true;
        }
        LOGE("JSImage::Create ParseJsMedia type is wrong");
        return false;
    }
    LOGI("input value is not string or number, using PixelMap");
    return false;
}

bool JSViewAbstract::ParseJsBool(const JSRef<JSVal>& jsValue, bool& result)
{
    if (!jsValue->IsBoolean() && !jsValue->IsObject()) {
        LOGE("arg is not bool or Object.");
        return false;
    }

    if (jsValue->IsBoolean()) {
        LOGD("jsValue->IsBoolean()");
        result = jsValue->ToBoolean();
        return true;
    }

    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> type = jsObj->GetProperty("type");
    if (!type->IsNumber()) {
        LOGW("type is not number");
        return false;
    }

    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }

    if (type->ToNumber<uint32_t>() == static_cast<uint32_t>(ResourceType::BOOLEAN)) {
        result = themeConstants->GetBoolean(resId->ToNumber<uint32_t>());
        return true;
    } else {
        return false;
    }
}

bool JSViewAbstract::ParseJsInteger(const JSRef<JSVal>& jsValue, uint32_t& result)
{
    if (!jsValue->IsNumber() && !jsValue->IsObject()) {
        LOGE("arg is not number or Object.");
        return false;
    }

    if (jsValue->IsNumber()) {
        LOGD("jsValue->IsNumber()");
        result = jsValue->ToNumber<uint32_t>();
        return true;
    }

    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> type = jsObj->GetProperty("type");
    if (!type->IsNumber()) {
        LOGW("type is not number");
        return false;
    }

    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }

    if (type->ToNumber<uint32_t>() == static_cast<uint32_t>(ResourceType::INTEGER)) {
        result = static_cast<uint32_t>(themeConstants->GetInt(resId->ToNumber<uint32_t>()));
        return true;
    } else {
        return false;
    }
}

bool JSViewAbstract::ParseJsIntegerArray(const JSRef<JSVal>& jsValue, std::vector<uint32_t>& result)
{
    if (!jsValue->IsArray() && !jsValue->IsObject()) {
        LOGE("arg is not array or Object.");
        return false;
    }

    if (jsValue->IsArray()) {
        JSRef<JSArray> array = JSRef<JSArray>::Cast(jsValue);
        for (size_t i = 0; i < array->Length(); i++) {
            JSRef<JSVal> value = array->GetValueAt(i);
            if (value->IsNumber()) {
                result.emplace_back(value->ToNumber<uint32_t>());
            } else if (value->IsObject()) {
                uint32_t singleResInt;
                if (ParseJsInteger(value, singleResInt)) {
                    result.emplace_back(singleResInt);
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    }

    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> type = jsObj->GetProperty("type");
    if (!type->IsNumber()) {
        LOGW("type is not number");
        return false;
    }

    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }

    if (type->ToNumber<uint32_t>() == static_cast<uint32_t>(ResourceType::INTARRAY)) {
        result = themeConstants->GetIntArray(resId->ToNumber<uint32_t>());
        return true;
    } else {
        return false;
    }
}

bool JSViewAbstract::ParseJsStrArray(const JSRef<JSVal>& jsValue, std::vector<std::string>& result)
{
    if (!jsValue->IsArray() && !jsValue->IsObject()) {
        LOGE("arg is not array or Object.");
        return false;
    }

    if (jsValue->IsArray()) {
        JSRef<JSArray> array = JSRef<JSArray>::Cast(jsValue);
        for (size_t i = 0; i < array->Length(); i++) {
            JSRef<JSVal> value = array->GetValueAt(i);
            if (value->IsString()) {
                result.emplace_back(value->ToString());
            } else if (value->IsObject()) {
                std::string singleResStr;
                if (ParseJsString(value, singleResStr)) {
                    result.emplace_back(singleResStr);
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        return true;
    }

    JSRef<JSObject> jsObj = JSRef<JSObject>::Cast(jsValue);
    JSRef<JSVal> type = jsObj->GetProperty("type");
    if (!type->IsNumber()) {
        LOGW("type is not number");
        return false;
    }

    JSRef<JSVal> resId = jsObj->GetProperty("id");
    if (!resId->IsNumber()) {
        LOGW("resId is not number");
        return false;
    }

    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }

    if (type->ToNumber<uint32_t>() == static_cast<uint32_t>(ResourceType::STRARRAY)) {
        result = themeConstants->GetStringArray(resId->ToNumber<uint32_t>());
        return true;
    } else {
        return false;
    }
}

std::pair<Dimension, Dimension> JSViewAbstract::ParseSize(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("ParseSize", info, checkList)) {
        return std::pair<Dimension, Dimension>();
    }
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        info.SetReturnValue(info.This());
        return std::pair<Dimension, Dimension>();
    }
    Dimension width;
    Dimension height;
    if (!ParseJsonDimensionVp(argsPtrItem->GetValue("width"), width) ||
        !ParseJsonDimensionVp(argsPtrItem->GetValue("height"), height)) {
        return std::pair<Dimension, Dimension>();
    }
    LOGD("JsSize width = %lf unit = %d, height = %lf unit = %d", width.Value(), width.Unit(), height.Value(),
        height.Unit());
    info.SetReturnValue(info.This());
    return std::pair<Dimension, Dimension>(width, height);
}

void JSViewAbstract::JsUseAlign(const JSCallbackInfo& info)
{
    if (info.Length() < 2) {
        LOGE("The arg is wrong, it is supposed to have atleast 2 arguments");
        return;
    }

    if (!info[0]->IsObject() && !info[1]->IsObject()) {
        LOGE("arg is not IsObject.");
        return;
    }

    AlignDeclaration* declaration = JSRef<JSObject>::Cast(info[0])->Unwrap<AlignDeclaration>();
    if (declaration == nullptr) {
        LOGE("declaration is nullptr");
        return;
    }

    JSRef<JSObject> obj = JSRef<JSObject>::Cast(info[1]);
    JSRef<JSVal> side = obj->GetProperty("side");
    JSRef<JSVal> offset = obj->GetProperty("offset");

    if (!side->IsNumber()) {
        LOGE("side is not Number [%s]", side->ToString().c_str());
        return;
    }

    auto sideValue = side->ToNumber<int32_t>();

    if (declaration->GetDeclarationType() == AlignDeclaration::DeclarationType::HORIZONTAL) {
        if (sideValue < static_cast<int32_t>(AlignDeclaration::Edge::START) ||
            sideValue > static_cast<int32_t>(AlignDeclaration::Edge::END)) {
            LOGE("side should be Edge.Start Edge.Middle or Edge.End with HorizontalAlignDeclaration");
            return;
        }
    } else if (declaration->GetDeclarationType() == AlignDeclaration::DeclarationType::VERTICAL) {
        if (sideValue < static_cast<int32_t>(AlignDeclaration::Edge::TOP) ||
            sideValue > static_cast<int32_t>(AlignDeclaration::Edge::BASELINE)) {
            LOGE("side should be Edge.Top Edge.Center Edge.Bottom or Edge.Baseline with VerticalAlignDeclaration");
            return;
        }
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetAlignDeclarationPtr(declaration);
    box->SetUseAlignSide(static_cast<AlignDeclaration::Edge>(sideValue));
    Dimension offsetDimension;
    if (ParseJsDimensionVp(offset, offsetDimension)) {
        box->SetUseAlignOffset(offsetDimension);
    }
}

void JSViewAbstract::JsGridSpan(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::NUMBER};
    if (!CheckJSCallbackInfo("JsGridSpan", info, checkList)) {
        return;
    }
    auto gridContainerInfo = JSGridContainer::GetContainer();
    if (gridContainerInfo != nullptr) {
        auto span = info[0]->ToNumber<uint32_t>();
        auto builder = ViewStackProcessor::GetInstance()->GetBoxComponent()->GetGridColumnInfoBuilder();
        builder->SetParent(gridContainerInfo);
        builder->SetColumns(span);
    }
}

void JSViewAbstract::JsGridOffset(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    auto gridContainerInfo = JSGridContainer::GetContainer();
    if (info[0]->IsNumber() && gridContainerInfo != nullptr) {
        auto builder = ViewStackProcessor::GetInstance()->GetBoxComponent()->GetGridColumnInfoBuilder();
        builder->SetParent(gridContainerInfo);
        int32_t offset = info[0]->ToNumber<int32_t>();
        builder->SetOffset(offset);
    }
}

static bool ParseSpanAndOffset(const JSRef<JSVal>& val, uint32_t& span, int32_t& offset)
{
    // {lg: 4}
    if (val->IsNumber()) {
        span = val->ToNumber<uint32_t>();
        return true;
    }

    if (!val->IsObject()) {
        LOGE("The argument is not object or number.");
        return false;
    }

    // {lg: {span: 1, offset: 2}}
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(val);
    span = obj->GetProperty("span")->ToNumber<uint32_t>();
    offset = obj->GetProperty("offset")->ToNumber<int32_t>();
    return true;
}

void JSViewAbstract::JsUseSizeType(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsUseSizeType", info, checkList)) {
        return;
    }
    auto gridContainerInfo = JSGridContainer::GetContainer();
    if (gridContainerInfo == nullptr) {
        LOGE("No valid grid container.");
        return;
    }
    auto builder = ViewStackProcessor::GetInstance()->GetBoxComponent()->GetGridColumnInfoBuilder();
    builder->SetParent(gridContainerInfo);

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    // keys order must be strictly refer to GridSizeType
    const char* keys[] = { "", "xs", "sm", "md", "lg" };
    for (uint32_t i = 1; i < sizeof(keys) / sizeof(const char*); i++) {
        JSRef<JSVal> val = sizeObj->GetProperty(keys[i]);
        if (val->IsNull() || val->IsEmpty()) {
            continue;
        }
        uint32_t span = 0;
        int32_t offset = 0;
        if (ParseSpanAndOffset(val, span, offset)) {
            builder->SetSizeColumn(static_cast<GridSizeType>(i), span);
            builder->SetOffset(offset, static_cast<GridSizeType>(i));
        }
    }
}

void JSViewAbstract::JsZIndex(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    int zIndex = 0;
    if (info[0]->IsNumber()) {
        zIndex = info[0]->ToNumber<int>();
    }
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto renderComponent = AceType::DynamicCast<RenderComponent>(component);
    if (renderComponent) {
        renderComponent->SetZIndex(zIndex);
    }
}

void JSViewAbstract::Pop()
{
    ViewStackProcessor::GetInstance()->Pop();
}

void JSViewAbstract::JsOnDragStart(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::FUNCTION};
    if (!CheckJSCallbackInfo("JsOnDragStart", info, checkList)) {
        return;
    }

    RefPtr<JsDragFunction> jsOnDragStartFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragStartId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragStartFunc)](
                        const RefPtr<DragEvent>& info, const std::string &extraParams) -> DragItemInfo {
        DragItemInfo itemInfo;
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, itemInfo);

        auto ret = func->Execute(info, extraParams);
        if (!ret->IsObject()) {
            LOGE("builder param is not an object.");
            return itemInfo;
        }
        auto component = ParseDragItemComponent(ret);
        if (component) {
            LOGI("use custom builder param.");
            itemInfo.customComponent = component;
            return itemInfo;
        }

        auto builderObj = JSRef<JSObject>::Cast(ret);
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        auto pixmap = builderObj->GetProperty("pixelMap");
        itemInfo.pixelMap = CreatePixelMapFromNapiValue(pixmap);
#endif
        auto extraInfo = builderObj->GetProperty("extraInfo");
        ParseJsString(extraInfo, itemInfo.extraInfo);
        component = ParseDragItemComponent(builderObj->GetProperty("builder"));
        itemInfo.customComponent = component;
        return itemInfo;
    };
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetOnDragStartId(onDragStartId);
}

RefPtr<Component> JSViewAbstract::ParseDragItemComponent(const JSRef<JSVal>& info)
{
    JSRef<JSVal> builder;
    if (info->IsObject()) {
        auto builderObj = JSRef<JSObject>::Cast(info);
        builder = builderObj->GetProperty("builder");
    } else if (info->IsFunction()) {
        builder = info;
    } else {
        return nullptr;
    }
    if (!builder->IsFunction()) {
        return nullptr;
    }
    auto builderFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(builder));
    if (!builderFunc) {
        return nullptr;
    }
    // use another VSP instance while executing the builder function
    ScopedViewStackProcessor builderViewStackProcessor;
    {
        ACE_SCORING_EVENT("onDragStart.builder");
        builderFunc->Execute();
    }
    auto component = ViewStackProcessor::GetInstance()->Finish();
    if (!component) {
        LOGE("Custom component is null.");
    }
    return component;
}

void JSViewAbstract::JsOnDragEnter(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::FUNCTION};
    if (!CheckJSCallbackInfo("JsOnDragEnter", info, checkList)) {
        return;
    }
    RefPtr<JsDragFunction> jsOnDragEnterFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragEnterId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragEnterFunc)](
                            const RefPtr<DragEvent>& info, const std::string &extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDragEnter");
        func->Execute(info, extraParams);
    };
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetOnDragEnterId(onDragEnterId);
}

void JSViewAbstract::JsOnDragMove(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::FUNCTION};
    if (!CheckJSCallbackInfo("JsOnDragMove", info, checkList)) {
        return;
    }
    RefPtr<JsDragFunction> jsOnDragMoveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragMoveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragMoveFunc)](
                            const RefPtr<DragEvent>& info, const std::string &extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDragMove");
        func->Execute(info, extraParams);
    };
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetOnDragMoveId(onDragMoveId);
}

void JSViewAbstract::JsOnDragLeave(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::FUNCTION};
    if (!CheckJSCallbackInfo("JsOnDragLeave", info, checkList)) {
        return;
    }
    RefPtr<JsDragFunction> jsOnDragLeaveFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDragLeaveId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDragLeaveFunc)](
                            const RefPtr<DragEvent>& info, const std::string &extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDragLeave");
        func->Execute(info, extraParams);
    };
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetOnDragLeaveId(onDragLeaveId);
}

void JSViewAbstract::JsOnDrop(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::FUNCTION};
    if (!CheckJSCallbackInfo("JsOnDrop", info, checkList)) {
        return;
    }
    RefPtr<JsDragFunction> jsOnDropFunc = AceType::MakeRefPtr<JsDragFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onDropId = [execCtx = info.GetExecutionContext(), func = std::move(jsOnDropFunc)](
                        const RefPtr<DragEvent>& info, const std::string &extraParams) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onDrop");
        func->Execute(info, extraParams);
    };
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    box->SetOnDropId(onDropId);
}

void JSViewAbstract::JsOnAreaChange(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::FUNCTION};
    if (!CheckJSCallbackInfo("JsOnAreaChange", info, checkList)) {
        return;
    }
    auto jsOnAreaChangeFunction = AceType::MakeRefPtr<JsOnAreaChangeFunction>(JSRef<JSFunc>::Cast(info[0]));
    auto onAreaChangeCallback = [execCtx = info.GetExecutionContext(), func = std::move(jsOnAreaChangeFunction)](
                                    const Rect& oldRect, const Offset& oldOrigin, const Rect& rect,
                                    const Offset& origin) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("onAreaChange");
        func->Execute(oldRect, oldOrigin, rect, origin);
    };
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!boxComponent) {
        LOGE("boxComponent is null");
        return;
    }
    boxComponent->GetEventExtensions()->GetOnAreaChangeExtension()->AddOnAreaChangeEvent(
        std::move(onAreaChangeCallback));
}

#ifndef WEARABLE_PRODUCT
void JSViewAbstract::JsBindPopup(const JSCallbackInfo& info)
{
    if (info.Length() < 2) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!info[0]->IsBoolean() && !info[0]->IsObject()) {
        LOGE("The first param type is not bool or object, invalid.");
        return;
    }

    if (!info[1]->IsObject()) {
        LOGE("The second param type is not object, invalid.");
        return;
    }

    ViewStackProcessor::GetInstance()->GetCoverageComponent();
    auto popupComponent = ViewStackProcessor::GetInstance()->GetPopupComponent(true);
    if (!popupComponent) {
        return;
    }
    auto popupParam = popupComponent->GetPopupParam();
    if (!popupParam) {
        return;
    }

    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("this component does not have inspector");
        return;
    }
    popupParam->SetTargetId(inspector->GetId());
    if (info[0]->IsBoolean()) {
        popupParam->SetIsShow(info[0]->ToBoolean());
    } else {
        JSRef<JSObject> showObj = JSRef<JSObject>::Cast(info[0]);
        ParseShowObject(info, showObj, popupComponent);
        popupParam->SetIsShow(showObj->GetProperty("value")->ToBoolean());
    }

    JSRef<JSObject> popupObj = JSRef<JSObject>::Cast(info[1]);
    if (popupObj->GetProperty("message")->IsString()) {
        ParsePopupParam(info, popupObj, popupComponent);
    } else {
        ParseCustomPopupParam(info, popupObj, popupComponent);
    }
}
#endif

void JSViewAbstract::JsLinearGradient(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
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
    Gradient lineGradient;
    lineGradient.SetType(GradientType::LINEAR);
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    // angle
    std::optional<float> degree;
    GetAngle("angle", argsPtrItem, degree);
    if (degree) {
        lineGradient.GetLinearGradient().angle = AnimatableDimension(degree.value(), DimensionUnit::PX, option);
        degree.reset();
    }
    // direction
    auto direction =
        static_cast<GradientDirection>(argsPtrItem->GetInt("direction", static_cast<int32_t>(GradientDirection::NONE)));
    switch (direction) {
        case GradientDirection::LEFT:
            lineGradient.GetLinearGradient().linearX = GradientDirection::LEFT;
            break;
        case GradientDirection::RIGHT:
            lineGradient.GetLinearGradient().linearX = GradientDirection::RIGHT;
            break;
        case GradientDirection::TOP:
            lineGradient.GetLinearGradient().linearY = GradientDirection::TOP;
            break;
        case GradientDirection::BOTTOM:
            lineGradient.GetLinearGradient().linearY = GradientDirection::BOTTOM;
            break;
        case GradientDirection::LEFT_TOP:
            lineGradient.GetLinearGradient().linearX = GradientDirection::LEFT;
            lineGradient.GetLinearGradient().linearY = GradientDirection::TOP;
            break;
        case GradientDirection::LEFT_BOTTOM:
            lineGradient.GetLinearGradient().linearX = GradientDirection::LEFT;
            lineGradient.GetLinearGradient().linearY = GradientDirection::BOTTOM;
            break;
        case GradientDirection::RIGHT_TOP:
            lineGradient.GetLinearGradient().linearX = GradientDirection::RIGHT;
            lineGradient.GetLinearGradient().linearY = GradientDirection::TOP;
            break;
        case GradientDirection::RIGHT_BOTTOM:
            lineGradient.GetLinearGradient().linearX = GradientDirection::RIGHT;
            lineGradient.GetLinearGradient().linearY = GradientDirection::BOTTOM;
            break;
        case GradientDirection::NONE:
        case GradientDirection::START_TO_END:
        case GradientDirection::END_TO_START:
        default:
            break;
    }
    // repeating
    auto repeating = argsPtrItem->GetBool("repeating", false);
    lineGradient.SetRepeat(repeating);
    // color stops
    GetGradientColorStops(lineGradient, argsPtrItem->GetValue("colors"));

    auto stack = ViewStackProcessor::GetInstance();
    if (!stack->IsVisualStateSet()) {
        auto decoration = GetBackDecoration();
        if (decoration) {
            decoration->SetGradient(lineGradient);
        }
    } else {
        auto boxComponent = stack->GetBoxComponent();
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<Gradient>
            (BoxStateAttribute::GRADIENT, lineGradient, stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::GRADIENT, VisualState::NORMAL)) {
            boxComponent->GetStateAttributes()->AddAttribute<Gradient>(BoxStateAttribute::GRADIENT,
                GetBackDecoration()->GetGradient(), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsRadialGradient(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsRadialGradient", info, checkList)) {
        return;
    }

    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        info.ReturnSelf();
        return;
    }

    Gradient radialGradient;
    radialGradient.SetType(GradientType::RADIAL);
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    // center
    auto center = argsPtrItem->GetValue("center");
    if (center && !center->IsNull() && center->IsArray() && center->GetArraySize() == 2) {
        Dimension value;
        if (ParseJsonDimensionVp(center->GetArrayItem(0), value)) {
            radialGradient.GetRadialGradient().radialCenterX = AnimatableDimension(value, option);
            if (value.Unit() == DimensionUnit::PERCENT) {
                // [0,1] -> [0, 100]
                radialGradient.GetRadialGradient().radialCenterX =
                    AnimatableDimension(value.Value() * 100.0, DimensionUnit::PERCENT, option);
            }
        }
        if (ParseJsonDimensionVp(center->GetArrayItem(1), value)) {
            radialGradient.GetRadialGradient().radialCenterY = AnimatableDimension(value, option);
            if (value.Unit() == DimensionUnit::PERCENT) {
                // [0,1] -> [0, 100]
                radialGradient.GetRadialGradient().radialCenterY =
                    AnimatableDimension(value.Value() * 100.0, DimensionUnit::PERCENT, option);
            }
        }
    }
    // radius
    Dimension radius;
    if (ParseJsonDimensionVp(argsPtrItem->GetValue("radius"), radius)) {
        radialGradient.GetRadialGradient().radialVerticalSize = AnimatableDimension(radius, option);
        radialGradient.GetRadialGradient().radialHorizontalSize = AnimatableDimension(radius, option);
    }
    // repeating
    auto repeating = argsPtrItem->GetBool("repeating", false);
    radialGradient.SetRepeat(repeating);
    // color stops
    GetGradientColorStops(radialGradient, argsPtrItem->GetValue("colors"));

    auto stack = ViewStackProcessor::GetInstance();
    if (!stack->IsVisualStateSet()) {
        auto decoration = GetBackDecoration();
        if (decoration) {
            decoration->SetGradient(radialGradient);
        }
    } else {
        auto boxComponent = stack->GetBoxComponent();
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<Gradient>
            (BoxStateAttribute::GRADIENT, radialGradient, stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::GRADIENT, VisualState::NORMAL)) {
            boxComponent->GetStateAttributes()->AddAttribute<Gradient>(BoxStateAttribute::GRADIENT,
                GetBackDecoration()->GetGradient(), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsSweepGradient(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsSweepGradient", info, checkList)) {
        return;
    }

    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        info.ReturnSelf();
        return;
    }

    Gradient sweepGradient;
    sweepGradient.SetType(GradientType::SWEEP);
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    // center
    auto center = argsPtrItem->GetValue("center");
    if (center && !center->IsNull() && center->IsArray() && center->GetArraySize() == 2) {
        Dimension value;
        if (ParseJsonDimensionVp(center->GetArrayItem(0), value)) {
            sweepGradient.GetSweepGradient().centerX = AnimatableDimension(value, option);
            if (value.Unit() == DimensionUnit::PERCENT) {
                // [0,1] -> [0, 100]
                sweepGradient.GetSweepGradient().centerX =
                    AnimatableDimension(value.Value() * 100.0, DimensionUnit::PERCENT, option);
            }
        }
        if (ParseJsonDimensionVp(center->GetArrayItem(1), value)) {
            sweepGradient.GetSweepGradient().centerY = AnimatableDimension(value, option);
            if (value.Unit() == DimensionUnit::PERCENT) {
                // [0,1] -> [0, 100]
                sweepGradient.GetSweepGradient().centerY =
                    AnimatableDimension(value.Value() * 100.0, DimensionUnit::PERCENT, option);
            }
        }
    }
    std::optional<float> degree;
    // start
    GetAngle("start", argsPtrItem, degree);
    if (degree) {
        sweepGradient.GetSweepGradient().startAngle = AnimatableDimension(degree.value(), DimensionUnit::PX, option);
        degree.reset();
    }
    // end
    GetAngle("end", argsPtrItem, degree);
    if (degree) {
        sweepGradient.GetSweepGradient().endAngle = AnimatableDimension(degree.value(), DimensionUnit::PX, option);
        degree.reset();
    }
    // rotation
    GetAngle("rotation", argsPtrItem, degree);
    if (degree) {
        sweepGradient.GetSweepGradient().rotation = AnimatableDimension(degree.value(), DimensionUnit::PX, option);
        degree.reset();
    }
    // repeating
    auto repeating = argsPtrItem->GetBool("repeating", false);
    sweepGradient.SetRepeat(repeating);
    // color stops
    GetGradientColorStops(sweepGradient, argsPtrItem->GetValue("colors"));

    auto stack = ViewStackProcessor::GetInstance();
    if (!stack->IsVisualStateSet()) {
        auto decoration = GetBackDecoration();
        if (decoration) {
            decoration->SetGradient(sweepGradient);
        }
    } else {
        auto boxComponent = stack->GetBoxComponent();
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<Gradient>
            (BoxStateAttribute::GRADIENT, sweepGradient, stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::GRADIENT, VisualState::NORMAL)) {
            boxComponent->GetStateAttributes()->AddAttribute<Gradient>(BoxStateAttribute::GRADIENT,
                GetBackDecoration()->GetGradient(), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::JsMotionPath(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsMotionPath", info, checkList)) {
        return;
    }
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    MotionPathOption motionPathOption;
    if (ParseMotionPath(argsPtrItem, motionPathOption)) {
        if (motionPathOption.GetRotate()) {
            ViewStackProcessor::GetInstance()->GetTransformComponent();
        }
        auto flexItem = ViewStackProcessor::GetInstance()->GetFlexItemComponent();
        flexItem->SetMotionPathOption(motionPathOption);
    } else {
        LOGE("parse motionPath failed. %{public}s", info[0]->ToString().c_str());
    }
}

void JSViewAbstract::JsShadow(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::OBJECT};
    if (!CheckJSCallbackInfo("JsShadow", info, checkList)) {
        return;
    }
    auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
    if (!argsPtrItem || argsPtrItem->IsNull()) {
        LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
        info.ReturnSelf();
        return;
    }
    double radius = 0.0;
    ParseJsonDouble(argsPtrItem->GetValue("radius"), radius);
    if (LessOrEqual(radius, 0.0)) {
        LOGE("JsShadow Parse radius failed, radius = %{public}lf", radius);
        return;
    }
    std::vector<Shadow> shadows(1);
    shadows.begin()->SetBlurRadius(radius);
    Dimension offsetX;
    if (ParseJsonDimensionVp(argsPtrItem->GetValue("offsetX"), offsetX)) {
        shadows.begin()->SetOffsetX(offsetX.Value());
    }
    Dimension offsetY;
    if (ParseJsonDimensionVp(argsPtrItem->GetValue("offsetY"), offsetY)) {
        shadows.begin()->SetOffsetY(offsetY.Value());
    }
    Color color;
    if (ParseJsonColor(argsPtrItem->GetValue("color"), color)) {
        shadows.begin()->SetColor(color);
    }
    auto backDecoration = GetBackDecoration();
    backDecoration->SetShadows(shadows);
}

void JSViewAbstract::JsGrayScale(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }

    if (GreatNotEqual(value.Value(), 1.0)) {
        value.SetValue(1.0);
    }

    auto frontDecoration = GetFrontDecoration();
    frontDecoration->SetGrayScale(value);
}

void JSViewAbstract::JsBrightness(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (value.Value() == 0) {
        value.SetValue(EPSILON);
    }
    auto frontDecoration = GetFrontDecoration();
    frontDecoration->SetBrightness(value);
}

void JSViewAbstract::JsContrast(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (value.Value() == 0) {
        value.SetValue(EPSILON);
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    auto frontDecoration = GetFrontDecoration();
    frontDecoration->SetContrast(value);
}

void JSViewAbstract::JsSaturate(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (value.Value() == 0) {
        value.SetValue(EPSILON);
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    auto frontDecoration = GetFrontDecoration();
    frontDecoration->SetSaturate(value);
}

void JSViewAbstract::JsSepia(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }

    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    auto frontDecoration = GetFrontDecoration();
    frontDecoration->SetSepia(value);
}

void JSViewAbstract::JsInvert(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Dimension value;
    if (!ParseJsDimensionVp(info[0], value)) {
        return;
    }
    if (LessNotEqual(value.Value(), 0.0)) {
        value.SetValue(0.0);
    }
    auto frontDecoration = GetFrontDecoration();
    frontDecoration->SetInvert(value);
}

void JSViewAbstract::JsHueRotate(const JSCallbackInfo& info)
{
    std::optional<float> degree;
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }
    if (info[0]->IsString()) {
        degree = static_cast<float>(StringUtils::StringToDegree(info[0]->ToString()));
    } else if (info[0]->IsNumber()) {
        degree = static_cast<float>(info[0]->ToNumber<uint32_t>());
    } else {
        LOGE("Invalid value type");
    }
    float deg = 0.0;
    if (degree) {
        deg = degree.value();
        degree.reset();
    }
    auto decoration = GetFrontDecoration();
    if (decoration) {
        decoration->SetHueRotate(deg);
    }
}

void JSViewAbstract::JsClip(const JSCallbackInfo& info)
{
    if (info.Length() > 0) {
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        if (info[0]->IsObject()) {
            JSShapeAbstract* clipShape = JSRef<JSObject>::Cast(info[0])->Unwrap<JSShapeAbstract>();
            if (clipShape == nullptr) {
                LOGE("clipShape is null.");
                return;
            }
            auto clipPath = AceType::MakeRefPtr<ClipPath>();
            clipPath->SetBasicShape(clipShape->GetBasicShape());
            box->SetClipPath(clipPath);
        } else if (info[0]->IsBoolean()) {
            box->SetBoxClipFlag(info[0]->ToBoolean());
        }
    }
}

void JSViewAbstract::JsMask(const JSCallbackInfo& info)
{
    if (info.Length() > 0 && info[0]->IsObject()) {
        JSShapeAbstract* maskShape = JSRef<JSObject>::Cast(info[0])->Unwrap<JSShapeAbstract>();
        if (maskShape == nullptr) {
            return;
        }
        auto maskPath = AceType::MakeRefPtr<MaskPath>();
        maskPath->SetBasicShape(maskShape->GetBasicShape());
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        box->SetMask(maskPath);
    }
}

void JSViewAbstract::JsFocusable(const JSCallbackInfo& info)
{
    if (!info[0]->IsBoolean()) {
        LOGE("The info is wrong, it is supposed to be an boolean");
        return;
    }

    auto focusComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
    if (!focusComponent) {
        LOGE("The focusComponent is null");
        return;
    } else {
        focusComponent->SetFocusable(info[0]->ToBoolean());
    }
}

void JSViewAbstract::JsOnFocusMove(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        RefPtr<JsFocusFunction> jsOnFocusMove = AceType::MakeRefPtr<JsFocusFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onFocusMove = [execCtx = args.GetExecutionContext(), func = std::move(jsOnFocusMove)](int info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("onFocusMove");
            func->Execute(info);
        };
        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
        focusableComponent->SetOnFocusMove(onFocusMove);
    }
}

void JSViewAbstract::JsOnFocus(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        RefPtr<JsFocusFunction> jsOnFocus = AceType::MakeRefPtr<JsFocusFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onFocus = [execCtx = args.GetExecutionContext(), func = std::move(jsOnFocus)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("onFocus");
            func->Execute();
        };
        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
        focusableComponent->SetOnFocus(onFocus);
    }
}

void JSViewAbstract::JsOnBlur(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        RefPtr<JsFocusFunction> jsOnBlur = AceType::MakeRefPtr<JsFocusFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onBlur_ = [execCtx = args.GetExecutionContext(), func = std::move(jsOnBlur)]() {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            ACE_SCORING_EVENT("onBlur");
            func->Execute();
        };
        auto focusableComponent = ViewStackProcessor::GetInstance()->GetFocusableComponent();
        focusableComponent->SetOnBlur(onBlur_);
    }
}

void JSViewAbstract::JsKey(const std::string& key)
{
    auto component = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (component) {
        component->SetInspectorKey(key);
    }
}

void JSViewAbstract::JsId(const std::string& id)
{
    JsKey(id);
}

void JSViewAbstract::JsRestoreId(int32_t restoreId)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    if (component) {
        component->SetRestoreId(restoreId);
    }
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void JSViewAbstract::JsDebugLine(const JSCallbackInfo& info)
{
    std::vector<JSCallbackInfoType> checkList {JSCallbackInfoType::STRING};
    if (!CheckJSCallbackInfo("JsDebugLine", info, checkList)) {
        return;
    }

    auto component = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (component) {
        component->SetDebugLine(info[0]->ToString());
    }
}
#endif

void JSViewAbstract::JsOpacityPassThrough(const JSCallbackInfo& info)
{
    JSViewAbstract::JsOpacity(info);
    if (ViewStackProcessor::GetInstance()->HasDisplayComponent()) {
        auto display = ViewStackProcessor::GetInstance()->GetDisplayComponent();
        display->DisableLayer(true);
    }
}

void JSViewAbstract::JsTransitionPassThrough(const JSCallbackInfo& info)
{
    JSViewAbstract::JsTransition(info);
    if (ViewStackProcessor::GetInstance()->HasDisplayComponent()) {
        auto display = ViewStackProcessor::GetInstance()->GetDisplayComponent();
        display->DisableLayer(true);
    }
}

void JSViewAbstract::JsAccessibilityGroup(bool accessible)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("this component does not have inspector");
        return;
    }
    inspector->SetAccessibilityGroup(accessible);
}

void JSViewAbstract::JsAccessibilityText(const std::string& text)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("this component does not have inspector");
        return;
    }
    inspector->SetAccessibilitytext(text);
}

void JSViewAbstract::JsAccessibilityDescription(const std::string& description)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("this component does not have inspector");
        return;
    }
    inspector->SetAccessibilityDescription(description);
}

void JSViewAbstract::JsAccessibilityImportance(const std::string& importance)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("this component does not have inspector");
        return;
    }
    inspector->SetAccessibilityImportance(importance);
}

void JSViewAbstract::JsBindContextMenu(const JSCallbackInfo& info)
{
    ViewStackProcessor::GetInstance()->GetCoverageComponent();
    auto menuComponent = ViewStackProcessor::GetInstance()->GetMenuComponent(true);
    if (!menuComponent) {
        return;
    }
#if defined(MULTIPLE_WINDOW_SUPPORTED)
    menuComponent->SetIsContextMenu(true);
#endif
    int32_t responseType = static_cast<int32_t>(ResponseType::LONGPRESS);
    if (info.Length() == 2 && info[1]->IsNumber()) {
        responseType = info[1]->ToNumber<int32_t>();
        LOGI("Set the responseType is %d.", responseType);
    }

    if (responseType == static_cast<int32_t>(ResponseType::RIGHT_CLICK)) {
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        box->SetOnMouseId([weak = WeakPtr<OHOS::Ace::MenuComponent>(menuComponent)](MouseInfo& info) {
            auto refPtr = weak.Upgrade();
            if (!refPtr) {
                return;
            }
            if (info.GetButton() == MouseButton::RIGHT_BUTTON && info.GetAction() == MouseAction::RELEASE) {
                auto showMenu = refPtr->GetTargetCallback();
#if defined(MULTIPLE_WINDOW_SUPPORTED)
                showMenu("", info.GetScreenLocation());
#else
                showMenu("", info.GetGlobalLocation());
#endif
            }
        });
    } else if (responseType == static_cast<int32_t>(ResponseType::LONGPRESS)) {
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        RefPtr<Gesture> longGesture =
            AceType::MakeRefPtr<LongPressGesture>(DEFAULT_LONG_PRESS_FINGER, false, DEFAULT_LONG_PRESS_DURATION);
        longGesture->SetOnActionId([weak = WeakPtr<OHOS::Ace::MenuComponent>(menuComponent)](const GestureEvent& info) {
            auto refPtr = weak.Upgrade();
            if (!refPtr) {
                return;
            }
            auto showMenu = refPtr->GetTargetCallback();
#if defined(MULTIPLE_WINDOW_SUPPORTED)
            showMenu("", info.GetScreenLocation());
#else
            showMenu("", info.GetGlobalLocation());
#endif
        });
        box->SetOnLongPress(longGesture);
    } else {
        LOGE("The arg responseType is invalid.");
        return;
    }
    auto menuTheme = GetTheme<SelectTheme>();
    menuComponent->SetTheme(menuTheme);

    if (info[0]->IsObject()) {
        JSRef<JSObject> menuObj = JSRef<JSObject>::Cast(info[0]);

        auto builder = menuObj->GetProperty("builder");
        if (!builder->IsFunction()) {
            LOGE("builder param is not a function.");
            return;
        }
        auto builderFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSFunc>::Cast(builder));
        if (!builderFunc) {
            LOGE("builder function is null.");
            return;
        }
        // use another VSP instance while executing the builder function
        ScopedViewStackProcessor builderViewStackProcessor;
        {
            ACE_SCORING_EVENT("contextMenu.builder");
            builderFunc->Execute();
        }
        auto customComponent = ViewStackProcessor::GetInstance()->Finish();
        if (!customComponent) {
            LOGE("Custom component is null.");
            return;
        }

        auto optionTheme = GetTheme<SelectTheme>();
        auto optionComponent = AceType::MakeRefPtr<OHOS::Ace::OptionComponent>(optionTheme);
        optionComponent->SetCustomComponent(customComponent);
        menuComponent->ClearOptions();
        menuComponent->AppendOption(optionComponent);
    } else {
        LOGE("Param is invalid");
        return;
    }
}

void JSViewAbstract::JSBind()
{
    JSClass<JSViewAbstract>::Declare("JSViewAbstract");

    // staticmethods
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSViewAbstract>::StaticMethod("pop", &JSViewAbstract::Pop, opt);

    JSClass<JSViewAbstract>::StaticMethod("width", &JSViewAbstract::JsWidth);
    JSClass<JSViewAbstract>::StaticMethod("height", &JSViewAbstract::JsHeight);
    JSClass<JSViewAbstract>::StaticMethod("responseRegion", &JSViewAbstract::JsResponseRegion);
    JSClass<JSViewAbstract>::StaticMethod("size", &JSViewAbstract::JsSize);
    JSClass<JSViewAbstract>::StaticMethod("constraintSize", &JSViewAbstract::JsConstraintSize);
    JSClass<JSViewAbstract>::StaticMethod("layoutPriority", &JSViewAbstract::JsLayoutPriority);
    JSClass<JSViewAbstract>::StaticMethod("layoutWeight", &JSViewAbstract::JsLayoutWeight);

    JSClass<JSViewAbstract>::StaticMethod("margin", &JSViewAbstract::JsMargin);
    JSClass<JSViewAbstract>::StaticMethod("marginTop", &JSViewAbstract::SetMarginTop, opt);
    JSClass<JSViewAbstract>::StaticMethod("marginBottom", &JSViewAbstract::SetMarginBottom, opt);
    JSClass<JSViewAbstract>::StaticMethod("marginLeft", &JSViewAbstract::SetMarginLeft, opt);
    JSClass<JSViewAbstract>::StaticMethod("marginRight", &JSViewAbstract::SetMarginRight, opt);

    JSClass<JSViewAbstract>::StaticMethod("padding", &JSViewAbstract::JsPadding);
    JSClass<JSViewAbstract>::StaticMethod("paddingTop", &JSViewAbstract::SetPaddingTop, opt);
    JSClass<JSViewAbstract>::StaticMethod("paddingBottom", &JSViewAbstract::SetPaddingBottom, opt);
    JSClass<JSViewAbstract>::StaticMethod("paddingLeft", &JSViewAbstract::SetPaddingLeft, opt);
    JSClass<JSViewAbstract>::StaticMethod("paddingRight", &JSViewAbstract::SetPaddingRight, opt);

    JSClass<JSViewAbstract>::StaticMethod("backgroundColor", &JSViewAbstract::JsBackgroundColor);
    JSClass<JSViewAbstract>::StaticMethod("backgroundImage", &JSViewAbstract::JsBackgroundImage);
    JSClass<JSViewAbstract>::StaticMethod("backgroundImageSize", &JSViewAbstract::JsBackgroundImageSize);
    JSClass<JSViewAbstract>::StaticMethod("backgroundImagePosition", &JSViewAbstract::JsBackgroundImagePosition);
    JSClass<JSViewAbstract>::StaticMethod("borderStyle", &JSViewAbstract::SetBorderStyle, opt);
    JSClass<JSViewAbstract>::StaticMethod("borderColor", &JSViewAbstract::JsBorderColor);
    JSClass<JSViewAbstract>::StaticMethod("border", &JSViewAbstract::JsBorder);
    JSClass<JSViewAbstract>::StaticMethod("borderWidth", &JSViewAbstract::JsBorderWidth);
    JSClass<JSViewAbstract>::StaticMethod("borderRadius", &JSViewAbstract::JsBorderRadius);

    JSClass<JSViewAbstract>::StaticMethod("scale", &JSViewAbstract::JsScale);
    JSClass<JSViewAbstract>::StaticMethod("scaleX", &JSViewAbstract::JsScaleX);
    JSClass<JSViewAbstract>::StaticMethod("scaleY", &JSViewAbstract::JsScaleY);
    JSClass<JSViewAbstract>::StaticMethod("opacity", &JSViewAbstract::JsOpacity);
    JSClass<JSViewAbstract>::StaticMethod("rotate", &JSViewAbstract::JsRotate);
    JSClass<JSViewAbstract>::StaticMethod("rotateX", &JSViewAbstract::JsRotateX);
    JSClass<JSViewAbstract>::StaticMethod("rotateY", &JSViewAbstract::JsRotateY);
    JSClass<JSViewAbstract>::StaticMethod("translate", &JSViewAbstract::JsTranslate);
    JSClass<JSViewAbstract>::StaticMethod("translateX", &JSViewAbstract::JsTranslateX);
    JSClass<JSViewAbstract>::StaticMethod("translateY", &JSViewAbstract::JsTranslateY);
    JSClass<JSViewAbstract>::StaticMethod("transform", &JSViewAbstract::JsTransform);
    JSClass<JSViewAbstract>::StaticMethod("transition", &JSViewAbstract::JsTransition);

    JSClass<JSViewAbstract>::StaticMethod("align", &JSViewAbstract::JsAlign);
    JSClass<JSViewAbstract>::StaticMethod("position", &JSViewAbstract::JsPosition);
    JSClass<JSViewAbstract>::StaticMethod("markAnchor", &JSViewAbstract::JsMarkAnchor);
    JSClass<JSViewAbstract>::StaticMethod("offset", &JSViewAbstract::JsOffset);
    JSClass<JSViewAbstract>::StaticMethod("enabled", &JSViewAbstract::JsEnabled);
    JSClass<JSViewAbstract>::StaticMethod("aspectRatio", &JSViewAbstract::JsAspectRatio);
    JSClass<JSViewAbstract>::StaticMethod("overlay", &JSViewAbstract::JsOverlay);

    JSClass<JSViewAbstract>::StaticMethod("blur", &JSViewAbstract::JsBlur);
    JSClass<JSViewAbstract>::StaticMethod("colorBlend", &JSViewAbstract::JsColorBlend);
    JSClass<JSViewAbstract>::StaticMethod("backdropBlur", &JSViewAbstract::JsBackdropBlur);
    JSClass<JSViewAbstract>::StaticMethod("windowBlur", &JSViewAbstract::JsWindowBlur);
    JSClass<JSViewAbstract>::StaticMethod("visibility", &JSViewAbstract::SetVisibility);
    JSClass<JSViewAbstract>::StaticMethod("flexBasis", &JSViewAbstract::JsFlexBasis);
    JSClass<JSViewAbstract>::StaticMethod("flexGrow", &JSViewAbstract::JsFlexGrow);
    JSClass<JSViewAbstract>::StaticMethod("flexShrink", &JSViewAbstract::JsFlexShrink);
    JSClass<JSViewAbstract>::StaticMethod("alignSelf", &JSViewAbstract::JsAlignSelf);
    JSClass<JSViewAbstract>::StaticMethod("displayPriority", &JSViewAbstract::JsDisplayPriority);
    JSClass<JSViewAbstract>::StaticMethod("useAlign", &JSViewAbstract::JsUseAlign);
    JSClass<JSViewAbstract>::StaticMethod("zIndex", &JSViewAbstract::JsZIndex);
    JSClass<JSViewAbstract>::StaticMethod("sharedTransition", &JSViewAbstract::JsSharedTransition);
    JSClass<JSViewAbstract>::StaticMethod("direction", &JSViewAbstract::SetDirection, opt);
#ifndef WEARABLE_PRODUCT
    JSClass<JSViewAbstract>::StaticMethod("bindPopup", &JSViewAbstract::JsBindPopup);
#endif

    JSClass<JSViewAbstract>::StaticMethod("bindMenu", &JSViewAbstract::JsBindMenu);
    JSClass<JSViewAbstract>::StaticMethod("bindContextMenu", &JSViewAbstract::JsBindContextMenu);
    JSClass<JSViewAbstract>::StaticMethod("onDragStart", &JSViewAbstract::JsOnDragStart);
    JSClass<JSViewAbstract>::StaticMethod("onDragEnter", &JSViewAbstract::JsOnDragEnter);
    JSClass<JSViewAbstract>::StaticMethod("onDragMove", &JSViewAbstract::JsOnDragMove);
    JSClass<JSViewAbstract>::StaticMethod("onDragLeave", &JSViewAbstract::JsOnDragLeave);
    JSClass<JSViewAbstract>::StaticMethod("onDrop", &JSViewAbstract::JsOnDrop);

    JSClass<JSViewAbstract>::StaticMethod("linearGradient", &JSViewAbstract::JsLinearGradient);
    JSClass<JSViewAbstract>::StaticMethod("sweepGradient", &JSViewAbstract::JsSweepGradient);
    JSClass<JSViewAbstract>::StaticMethod("radialGradient", &JSViewAbstract::JsRadialGradient);
    JSClass<JSViewAbstract>::StaticMethod("motionPath", &JSViewAbstract::JsMotionPath);
    JSClass<JSViewAbstract>::StaticMethod("gridSpan", &JSViewAbstract::JsGridSpan);
    JSClass<JSViewAbstract>::StaticMethod("gridOffset", &JSViewAbstract::JsGridOffset);
    JSClass<JSViewAbstract>::StaticMethod("useSizeType", &JSViewAbstract::JsUseSizeType);
    JSClass<JSViewAbstract>::StaticMethod("shadow", &JSViewAbstract::JsShadow);
    JSClass<JSViewAbstract>::StaticMethod("grayscale", &JSViewAbstract::JsGrayScale);
    JSClass<JSViewAbstract>::StaticMethod("focusable", &JSViewAbstract::JsFocusable);
    JSClass<JSViewAbstract>::StaticMethod("onFocusMove", &JSViewAbstract::JsOnFocusMove);
    JSClass<JSViewAbstract>::StaticMethod("onFocus", &JSViewAbstract::JsOnFocus);
    JSClass<JSViewAbstract>::StaticMethod("onBlur", &JSViewAbstract::JsOnBlur);
    JSClass<JSViewAbstract>::StaticMethod("brightness", &JSViewAbstract::JsBrightness);
    JSClass<JSViewAbstract>::StaticMethod("contrast", &JSViewAbstract::JsContrast);
    JSClass<JSViewAbstract>::StaticMethod("saturate", &JSViewAbstract::JsSaturate);
    JSClass<JSViewAbstract>::StaticMethod("sepia", &JSViewAbstract::JsSepia);
    JSClass<JSViewAbstract>::StaticMethod("invert", &JSViewAbstract::JsInvert);
    JSClass<JSViewAbstract>::StaticMethod("hueRotate", &JSViewAbstract::JsHueRotate);
    JSClass<JSViewAbstract>::StaticMethod("clip", &JSViewAbstract::JsClip);
    JSClass<JSViewAbstract>::StaticMethod("mask", &JSViewAbstract::JsMask);
    JSClass<JSViewAbstract>::StaticMethod("key", &JSViewAbstract::JsKey);
    JSClass<JSViewAbstract>::StaticMethod("id", &JSViewAbstract::JsId);
    JSClass<JSViewAbstract>::StaticMethod("restoreId", &JSViewAbstract::JsRestoreId);
    JSClass<JSViewAbstract>::StaticMethod("hoverEffect", &JSViewAbstract::JsHoverEffect);
    JSClass<JSViewAbstract>::StaticMethod("onMouse", &JSViewAbstract::JsOnMouse);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    JSClass<JSViewAbstract>::StaticMethod("debugLine", &JSViewAbstract::JsDebugLine);
#endif
    JSClass<JSViewAbstract>::StaticMethod("geometryTransition", &JSViewAbstract::JsGeometryTransition);
    JSClass<JSViewAbstract>::StaticMethod("onAreaChange", &JSViewAbstract::JsOnAreaChange);
    JSClass<JSViewAbstract>::StaticMethod("touchable", &JSInteractableView::JsTouchable);

    JSClass<JSViewAbstract>::StaticMethod("accessibilityGroup", &JSViewAbstract::JsAccessibilityGroup);
    JSClass<JSViewAbstract>::StaticMethod("accessibilityText", &JSViewAbstract::JsAccessibilityText);
    JSClass<JSViewAbstract>::StaticMethod("accessibilityDescription", &JSViewAbstract::JsAccessibilityDescription);
    JSClass<JSViewAbstract>::StaticMethod("accessibilityImportance", &JSViewAbstract::JsAccessibilityImportance);
    JSClass<JSViewAbstract>::StaticMethod("onAccessibility", &JSInteractableView::JsOnAccessibility);
}

RefPtr<Decoration> JSViewAbstract::GetFrontDecoration()
{
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto decoration = box->GetFrontDecoration();
    if (!decoration) {
        decoration = AceType::MakeRefPtr<Decoration>();
        box->SetFrontDecoration(decoration);
    }

    return decoration;
}

RefPtr<Decoration> JSViewAbstract::GetBackDecoration()
{
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    auto decoration = box->GetBackDecoration();
    if (!decoration) {
        decoration = AceType::MakeRefPtr<Decoration>();
        box->SetBackDecoration(decoration);
    }
    return decoration;
}

void JSViewAbstract::SetBorderRadius(const Dimension& value, const AnimationOption& option)
{
    BoxComponentHelper::SetBorderRadius(GetBackDecoration(), value, option);
}

void JSViewAbstract::SetBorderStyle(int32_t style)
{
    BorderStyle borderStyle = BorderStyle::SOLID;

    if (static_cast<int32_t>(BorderStyle::SOLID) == style) {
        borderStyle = BorderStyle::SOLID;
    } else if (static_cast<int32_t>(BorderStyle::DASHED) == style) {
        borderStyle = BorderStyle::DASHED;
    } else if (static_cast<int32_t>(BorderStyle::DOTTED) == style) {
        borderStyle = BorderStyle::DOTTED;
    } else {
        borderStyle = BorderStyle::NONE;
    }

    auto stack = ViewStackProcessor::GetInstance();
    if (!stack->IsVisualStateSet()) {
        BoxComponentHelper::SetBorderStyle(GetBackDecoration(), borderStyle);
    } else {
        auto boxComponent = AceType::DynamicCast<BoxComponent>(stack->GetBoxComponent());
        if (!boxComponent) {
            LOGE("boxComponent is null");
            return;
        }
        boxComponent->GetStateAttributes()->AddAttribute<BorderStyle>
            (BoxStateAttribute::BORDER_STYLE, borderStyle, stack->GetVisualState());
        if (!boxComponent->GetStateAttributes()->
            HasAttribute(BoxStateAttribute::BORDER_STYLE, VisualState::NORMAL)) {
            boxComponent->GetStateAttributes()->AddAttribute<BorderStyle>(BoxStateAttribute::BORDER_STYLE,
                BoxComponentHelper::GetBorderStyle(GetBackDecoration()), VisualState::NORMAL);
        }
    }
}

void JSViewAbstract::SetMarginTop(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge margin = box->GetMargin();
    margin.SetTop(value);
    box->SetMargin(margin);
}

void JSViewAbstract::SetMarginBottom(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge margin = box->GetMargin();
    margin.SetBottom(value);
    box->SetMargin(margin);
}

void JSViewAbstract::SetMarginLeft(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge margin = box->GetMargin();
    margin.SetLeft(value);
    box->SetMargin(margin);
}

void JSViewAbstract::SetMarginRight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge margin = box->GetMargin();
    margin.SetRight(value);
    box->SetMargin(margin);
}

void JSViewAbstract::SetMargins(
    const Dimension& top, const Dimension& bottom, const Dimension& left, const Dimension& right)
{
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    Edge margin(left, top, right, bottom, option);
    box->SetMargin(margin);
}

void JSViewAbstract::SetPaddingTop(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge padding = box->GetPadding();
    padding.SetTop(value);
    box->SetPadding(padding);
}

void JSViewAbstract::SetPaddingBottom(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge padding = box->GetPadding();
    padding.SetBottom(value);
    box->SetPadding(padding);
}

void JSViewAbstract::SetPaddingLeft(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge padding = box->GetPadding();
    padding.SetLeft(value);
    box->SetPadding(padding);
}

void JSViewAbstract::SetPaddingRight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have at least 1 argument");
        return;
    }
    AnimatableDimension value;
    if (!ParseJsAnimatableDimensionVp(info[0], value)) {
        return;
    }
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    Edge padding = box->GetPadding();
    padding.SetRight(value);
    box->SetPadding(padding);
}

void JSViewAbstract::SetPadding(const Dimension& value)
{
    SetPaddings(value, value, value, value);
}

void JSViewAbstract::SetPaddings(
    const Dimension& top, const Dimension& bottom, const Dimension& left, const Dimension& right)
{
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
    Edge padding(left, top, right, bottom, option);
    box->SetPadding(padding);
}

void JSViewAbstract::SetMargin(const Dimension& value)
{
    SetMargins(value, value, value, value);
}

void JSViewAbstract::SetBlur(float radius)
{
    auto decoration = GetFrontDecoration();
    SetBlurRadius(decoration, radius);
}

void JSViewAbstract::SetColorBlend(Color color)
{
    auto decoration = GetFrontDecoration();
    if (decoration) {
        decoration->SetColorBlend(color);
    }
}

void JSViewAbstract::SetBackdropBlur(float radius)
{
    auto decoration = GetBackDecoration();
    SetBlurRadius(decoration, radius);
}

void JSViewAbstract::SetBlurRadius(const RefPtr<Decoration>& decoration, float radius)
{
    if (decoration) {
        AnimationOption option = ViewStackProcessor::GetInstance()->GetImplicitAnimationOption();
        decoration->SetBlurRadius(AnimatableDimension(radius, DimensionUnit::PX, option));
    }
}

void JSViewAbstract::SetWindowBlur(float progress, WindowBlurStyle blurStyle)
{
    auto decoration = GetBackDecoration();
    if (decoration) {
        decoration->SetWindowBlurProgress(progress);
        decoration->SetWindowBlurStyle(blurStyle);
    }
}

bool JSViewAbstract::ParseJsonDimension(
    const std::unique_ptr<JsonValue>& jsonValue, Dimension& result, DimensionUnit defaultUnit)
{
    if (!jsonValue || jsonValue->IsNull()) {
        LOGD("invalid json value");
        return false;
    }
    if (!jsonValue->IsNumber() && !jsonValue->IsString() && !jsonValue->IsObject()) {
        LOGE("json value is not number, string or object");
        return false;
    }
    if (jsonValue->IsNumber()) {
        result = Dimension(jsonValue->GetDouble(), defaultUnit);
        return true;
    }
    if (jsonValue->IsString()) {
        result = StringUtils::StringToDimensionWithUnit(jsonValue->GetString(), defaultUnit);
        return true;
    }
    auto resVal = JsonUtil::ParseJsonString(jsonValue->ToString());
    auto resId = resVal->GetValue("id");
    if (!resId || !resId->IsNumber()) {
        LOGE("invalid resource id");
        return false;
    }
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGE("themeConstants is nullptr");
        return false;
    }
    result = themeConstants->GetDimension(resId->GetUInt());
    return true;
}

bool JSViewAbstract::ParseJsonDimensionVp(const std::unique_ptr<JsonValue>& jsonValue, Dimension& result)
{
    return ParseJsonDimension(jsonValue, result, DimensionUnit::VP);
}

bool JSViewAbstract::ParseJsonDouble(const std::unique_ptr<JsonValue>& jsonValue, double& result)
{
    if (!jsonValue || jsonValue->IsNull()) {
        LOGD("invalid json value");
        return false;
    }
    if (!jsonValue->IsNumber() && !jsonValue->IsString() && !jsonValue->IsObject()) {
        LOGE("json value is not number, string or object");
        return false;
    }
    if (jsonValue->IsNumber()) {
        result = jsonValue->GetDouble();
        return true;
    }
    if (jsonValue->IsString()) {
        result = StringUtils::StringToDouble(jsonValue->GetString());
        return true;
    }
    auto resVal = JsonUtil::ParseJsonString(jsonValue->ToString());
    auto resId = resVal->GetValue("id");
    if (!resId || !resId->IsNumber()) {
        LOGE("invalid resource id");
        return false;
    }
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }
    result = themeConstants->GetDouble(resId->GetUInt());
    return true;
}

bool JSViewAbstract::ParseJsonColor(const std::unique_ptr<JsonValue>& jsonValue, Color& result)
{
    if (!jsonValue || jsonValue->IsNull()) {
        LOGD("invalid json value");
        return false;
    }
    if (!jsonValue->IsNumber() && !jsonValue->IsString() && !jsonValue->IsObject()) {
        LOGE("json value is not number, string or object");
        return false;
    }
    if (jsonValue->IsNumber()) {
        result = Color(ColorAlphaAdapt(jsonValue->GetUInt()));
        return true;
    }
    if (jsonValue->IsString()) {
        result = Color::FromString(jsonValue->GetString());
        return true;
    }
    auto resVal = JsonUtil::ParseJsonString(jsonValue->ToString());
    auto resId = resVal->GetValue("id");
    if (!resId || !resId->IsNumber()) {
        LOGE("invalid resource id");
        return false;
    }
    auto themeConstants = GetThemeConstants();
    if (!themeConstants) {
        LOGW("themeConstants is nullptr");
        return false;
    }
    result = themeConstants->GetColor(resId->GetUInt());
    return true;
}

void JSViewAbstract::GetAngle(
    const std::string& key, const std::unique_ptr<JsonValue>& jsonValue, std::optional<float>& angle)
{
    auto value = jsonValue->GetValue(key);
    if (value && value->IsString()) {
        angle = static_cast<float>(StringUtils::StringToDegree(value->GetString()));
    } else if (value && value->IsNumber()) {
        angle = static_cast<float>(value->GetDouble());
    } else {
        LOGE("Invalid value type");
    }
}

void JSViewAbstract::GetGradientColorStops(Gradient& gradient, const std::unique_ptr<JsonValue>& colorStops)
{
    if (!colorStops || colorStops->IsNull() || !colorStops->IsArray()) {
        return;
    }

    for (int32_t i = 0; i < colorStops->GetArraySize(); i++) {
        GradientColor gradientColor;
        auto item = colorStops->GetArrayItem(i);
        if (item && !item->IsNull() && item->IsArray() && item->GetArraySize() >= 1) {
            auto colorParams = item->GetArrayItem(0);
            // color
            Color color;
            if (!ParseJsonColor(colorParams, color)) {
                LOGE("parse colorParams failed");
                continue;
            }
            gradientColor.SetColor(color);
            gradientColor.SetHasValue(false);
            // stop value
            if (item->GetArraySize() <= 1) {
                continue;
            }
            auto stopValue = item->GetArrayItem(1);
            double value = 0.0;
            if (ParseJsonDouble(stopValue, value)) {
                value = std::clamp(value, 0.0, 1.0);
                gradientColor.SetHasValue(true);
                //  [0, 1] -> [0, 100.0];
                gradientColor.SetDimension(Dimension(value * 100.0, DimensionUnit::PERCENT));
            }

            gradient.AddColor(gradientColor);
        }
    }
}

void JSViewAbstract::SetDirection(const std::string& dir)
{
    auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!box) {
        return;
    }

    if (dir == "Ltr") {
        box->SetTextDirection(TextDirection::LTR);
        box->SetInspectorDirection(TextDirection::LTR);
    } else if (dir == "Rtl") {
        box->SetTextDirection(TextDirection::RTL);
        box->SetInspectorDirection(TextDirection::RTL);
    } else if (dir == "Auto") {
        box->SetTextDirection(
            AceApplicationInfo::GetInstance().IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
        box->SetInspectorDirection(TextDirection::AUTO);
    }
}

RefPtr<ThemeConstants> JSViewAbstract::GetThemeConstants()
{
    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return nullptr;
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        LOGE("pipelineContext is null!");
        return nullptr;
    }
    auto themeManager = pipelineContext->GetThemeManager();
    if (!themeManager) {
        LOGE("themeManager is null!");
        return nullptr;
    }
    return themeManager->GetThemeConstants();
}

void JSViewAbstract::JsHoverEffect(const JSCallbackInfo& info)
{
    if (!info[0]->IsNumber()) {
        LOGE("info[0] is not a number");
        return;
    }
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (!boxComponent) {
        LOGE("boxComponent is null");
        return;
    }
    boxComponent->SetMouseAnimationType(static_cast<HoverAnimationType>(info[0]->ToNumber<int32_t>()));
}

RefPtr<Gesture> JSViewAbstract::GetTapGesture(const JSCallbackInfo& info, int32_t countNum, int32_t fingerNum)
{
    auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
    if (!inspector) {
        LOGE("fail to get inspector for on touch event");
        return nullptr;
    }
    auto impl = inspector->GetInspectorFunctionImpl();
    RefPtr<Gesture> tapGesture = AceType::MakeRefPtr<TapGesture>(countNum, fingerNum);
    RefPtr<JsClickFunction> jsOnClickFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(info[0]));
    tapGesture->SetOnActionId(
        [execCtx = info.GetExecutionContext(), func = std::move(jsOnClickFunc), impl](GestureEvent& info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            if (impl) {
                impl->UpdateEventInfo(info);
            }
            ACE_SCORING_EVENT("onClick");
            func->Execute(info);
        });
    return tapGesture;
}

void JSViewAbstract::JsOnMouse(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto inspector = ViewStackProcessor::GetInstance()->GetInspectorComposedComponent();
        if (!inspector) {
            LOGE("fail to get inspector for on mouse event");
            return;
        }
        auto impl = inspector->GetInspectorFunctionImpl();
        RefPtr<JsClickFunction> jsOnMouseFunc = AceType::MakeRefPtr<JsClickFunction>(JSRef<JSFunc>::Cast(args[0]));
        auto onMouseId = [execCtx = args.GetExecutionContext(), func = std::move(jsOnMouseFunc), impl](
                             MouseInfo& info) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            if (impl) {
                impl->UpdateEventInfo(info);
            }
            ACE_SCORING_EVENT("onMouse");
            func->Execute(info);
        };
        auto box = ViewStackProcessor::GetInstance()->GetBoxComponent();
        box->SetOnMouseId(onMouseId);
    }
}

} // namespace OHOS::Ace::Framework
