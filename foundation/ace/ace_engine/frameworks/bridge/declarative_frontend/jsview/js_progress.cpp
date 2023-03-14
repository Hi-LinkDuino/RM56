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

#include <cmath>
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_progress.h"

#include "core/components/common/properties/color.h"
#include "core/components/progress/progress_component.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/track/track_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSProgress::Create(const JSCallbackInfo& info)
{
    if (info.Length() != 1 || !info[0]->IsObject()) {
        LOGE("create progress fail beacase the param is invalid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);

    auto value = 0;
    auto jsValue = paramObject->GetProperty("value");
    if (jsValue->IsNumber()) {
        value = jsValue->ToNumber<double>();
    } else {
        LOGE("create progress fail because the value is not number");
    }

    auto total = 100;
    auto jsTotal = paramObject->GetProperty("total");
    if (jsTotal->IsNumber()) {
        total = jsTotal->ToNumber<int>();
    } else {
        LOGE("create progress fail because the total is not value");
    }

    if ((value > total) || (value < 0)) {
        value = 0;
    }

    auto progressType = ProgressType::LINEAR;
    auto jsStyle = paramObject->GetProperty("type");
    if (jsStyle->IsNull() || jsStyle->IsUndefined()) {
        jsStyle = paramObject->GetProperty("style");
    }

    auto progressStyle = static_cast<ProgressStyle>(jsStyle->ToNumber<int32_t>());
    if (progressStyle == ProgressStyle::Eclipse) {
        progressType = ProgressType::MOON;
    } else if (progressStyle == ProgressStyle::Ring) {
        progressType = ProgressType::RING;
    } else if (progressStyle == ProgressStyle::ScaleRing) {
        progressType = ProgressType::SCALE;
    } else if (progressStyle == ProgressStyle::Capsule) {
        progressType = ProgressType::CAPSULE;
    }

    auto progressComponent = AceType::MakeRefPtr<OHOS::Ace::ProgressComponent>(0.0, value, 0.0, total, progressType);
    auto theme = GetTheme<ProgressTheme>();

    if (!theme) {
        LOGE("progress Theme is null");
        return;
    }

    progressComponent->InitStyle(theme);

    if (progressStyle == ProgressStyle::ScaleRing) {
        progressComponent->SetScaleNumber(100);
        progressComponent->SetScaleWidth(Dimension(2));
    }

    ViewStackProcessor::GetInstance()->Push(progressComponent);
}

void JSProgress::JSBind(BindingTarget globalObj)
{
    JSClass<JSProgress>::Declare("Progress");
    MethodOptions opt = MethodOptions::NONE;

    JSClass<JSProgress>::StaticMethod("create", &JSProgress::Create, opt);
    JSClass<JSProgress>::StaticMethod("value", &JSProgress::SetValue, opt);
    JSClass<JSProgress>::StaticMethod("color", &JSProgress::SetColor, opt);
    JSClass<JSProgress>::StaticMethod("circularStyle", &JSProgress::SetCircularStyle, opt);
    JSClass<JSProgress>::StaticMethod("cricularStyle", &JSProgress::SetCircularStyle, opt);
    JSClass<JSProgress>::StaticMethod("style", &JSProgress::SetCircularStyle, opt);
    JSClass<JSProgress>::StaticMethod("backgroundColor", &JSProgress::JsBackgroundColor, opt);
    JSClass<JSProgress>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSProgress>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSProgress>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSProgress>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSProgress>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSProgress>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSProgress>::Inherit<JSViewAbstract>();
    JSClass<JSProgress>::Bind(globalObj);
}

void JSProgress::SetValue(double value)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto progress = AceType::DynamicCast<ProgressComponent>(component);
    if (!progress) {
        LOGI("progress component is null.");
        return;
    }
    if (std::isnan(value)) {
        progress->SetValue(0.0);
    } else {
        progress->SetValue(value);
    }
}

void JSProgress::SetColor(const JSCallbackInfo& info)
{
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto progress = AceType::DynamicCast<ProgressComponent>(component);
    if (!progress) {
        LOGI("progress component is null.");
        return;
    }
    RefPtr<TrackComponent> track = progress->GetTrack();

    Color colorVal;
    if (ParseJsColor(info[0], colorVal))  {
        track->SetSelectColor(colorVal);
    }
}

void JSProgress::SetCircularStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto progress = AceType::DynamicCast<ProgressComponent>(component);
    if (!progress) {
        LOGI("progress component is null.");
        return;
    }
    RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();

    Dimension strokeWidthDimension;
    auto jsStrokeWidth = paramObject->GetProperty("strokeWidth");
    if (!ParseJsDimensionVp(jsStrokeWidth, strokeWidthDimension)) {
        LOGI("circular Style error. now use default strokeWidth");
        strokeWidthDimension = theme->GetTrackThickness();
    }

    if (strokeWidthDimension.Value() <= 0.0) {
        strokeWidthDimension = theme->GetTrackThickness();
    }
    progress->SetTrackThickness(strokeWidthDimension);

    auto jsScaleCount = paramObject->GetProperty("scaleCount");
    auto scaleCount = jsScaleCount->IsNumber() ? jsScaleCount->ToNumber<int32_t>() : theme->GetScaleNumber();
    if (scaleCount > 0.0) {
        progress->SetScaleNumber(scaleCount);
    }

    Dimension scaleWidthDimension;
    auto jsScaleWidth = paramObject->GetProperty("scaleWidth");
    if (!ParseJsDimensionVp(jsScaleWidth, scaleWidthDimension)) {
        LOGI("circular Style error. now use default scaleWidth");
        scaleWidthDimension = theme->GetScaleWidth();
    }

    if ((scaleWidthDimension.Value() <= 0.0) || (scaleWidthDimension.Value() > strokeWidthDimension.Value())) {
        scaleWidthDimension = theme->GetScaleWidth();
    }
    progress->SetScaleWidth(scaleWidthDimension);
}

void JSProgress::JsBackgroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    auto component = ViewStackProcessor::GetInstance()->GetMainComponent();
    auto progress = AceType::DynamicCast<ProgressComponent>(component);
    if (!progress) {
        LOGE("progress Component is null");
        return;
    }
    auto track = progress->GetTrack();
    if (!track) {
        LOGE("track Component is null");
        return;
    }

    Color colorVal;
    if (ParseJsColor(info[0], colorVal))  {
        track->SetBackgroundColor(colorVal);
    }
}

} // namespace OHOS::Ace::Framework

