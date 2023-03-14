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

#include "bridge/declarative_frontend/jsview/js_image_animator.h"

#include "bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

constexpr uint32_t DEFAULT_DURATION = 1000; // ms
constexpr uint32_t DEFAULT_ITERATIONS = 1;

void JSImageAnimator::Create()
{
    RefPtr<ImageAnimatorComponent> imageAnimator = AceType::MakeRefPtr<ImageAnimatorComponent>("ImageAnimator");
    imageAnimator->SetIteration(DEFAULT_ITERATIONS);
    imageAnimator->SetDuration(DEFAULT_DURATION);
    ViewStackProcessor::GetInstance()->Push(imageAnimator);
    // Init Common Styles for ImageAnimator
    ViewStackProcessor::GetInstance()->GetBoxComponent();
}

void JSImageAnimator::JSBind(BindingTarget globalObj)
{
    JSClass<JSImageAnimator>::Declare("ImageAnimator");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSImageAnimator>::StaticMethod("create", &JSImageAnimator::Create, opt);
    JSClass<JSImageAnimator>::StaticMethod("images", &JSImageAnimator::SetImages, opt);
    JSClass<JSImageAnimator>::StaticMethod("state", &JSImageAnimator::SetState, opt);
    JSClass<JSImageAnimator>::StaticMethod("duration", &JSImageAnimator::SetDuration, opt);
    JSClass<JSImageAnimator>::StaticMethod("iterations", &JSImageAnimator::SetIteration, opt);
    JSClass<JSImageAnimator>::StaticMethod("reverse", &JSImageAnimator::SetIsReverse, opt);
    JSClass<JSImageAnimator>::StaticMethod("fixedSize", &JSImageAnimator::SetFixedSize, opt);
    JSClass<JSImageAnimator>::StaticMethod("fillMode", &JSImageAnimator::SetFillMode, opt);
    JSClass<JSImageAnimator>::StaticMethod("preDecode", &JSImageAnimator::SetPreDecode, opt);

    JSClass<JSImageAnimator>::StaticMethod("onStart", &JSImageAnimator::OnStart, opt);
    JSClass<JSImageAnimator>::StaticMethod("onPause", &JSImageAnimator::OnPause, opt);
    JSClass<JSImageAnimator>::StaticMethod("onRepeat", &JSImageAnimator::OnRepeat, opt);
    JSClass<JSImageAnimator>::StaticMethod("onCancel", &JSImageAnimator::OnCancel, opt);
    JSClass<JSImageAnimator>::StaticMethod("onFinish", &JSImageAnimator::OnFinish, opt);

    JSClass<JSImageAnimator>::Inherit<JSContainerBase>();
    JSClass<JSImageAnimator>::Inherit<JSViewAbstract>();
    JSClass<JSImageAnimator>::Bind<>(globalObj);
}

void JSImageAnimator::SetImages(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("JSImageAnimator: The arg is wrong, it is supposed to have one object argument.");
        return;
    }
    if (info[0]->IsNull()) {
        LOGE("JSImageAnimator: info is null.");
        return;
    }
    if (!info[0]->IsArray()) {
        LOGE("JSImageAnimator: info is not array.");
        return;
    }
    JSRef<JSArray> imageArray = JSRef<JSArray>::Cast(info[0]);
    std::vector<ImageProperties> images;
    for (uint32_t i = 0; i < imageArray->Length(); ++i) {
        auto imageArgs = JsonUtil::ParseJsonString(imageArray->GetValueAt(i)->ToString());
        ImageProperties imageProperties;
        ParseImages(imageArgs, imageProperties);
        images.push_back(imageProperties);
    }

    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetImageProperties(images);
    }
}

void JSImageAnimator::SetState(int32_t state)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetStatus(static_cast<Animator::Status>(state));
    }
}

void JSImageAnimator::SetDuration(int32_t duration)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetDuration(duration);
    }
}

void JSImageAnimator::SetIteration(int32_t iteration)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetIteration(iteration);
    }
}

void JSImageAnimator::SetFillMode(int32_t fillMode)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetFillMode(static_cast<FillMode>(fillMode));
    }
}

void JSImageAnimator::SetPreDecode(int32_t preDecode)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetPreDecode(preDecode);
    }
}

void JSImageAnimator::SetIsReverse(bool isReverse)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetIsReverse(isReverse);
    }
}

void JSImageAnimator::SetFixedSize(bool fixedSize)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (imageAnimator) {
        imageAnimator->SetIsFixedSize(fixedSize);
    }
}

void JSImageAnimator::OnStart(const JSCallbackInfo& info)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!imageAnimator) {
        return;
    }
    const auto& controller = imageAnimator->GetImageAnimatorController();
    if (controller) {
        auto startEvent = GetEventMarker(info);
        controller->SetStartEvent(startEvent);
    }
}

void JSImageAnimator::OnPause(const JSCallbackInfo& info)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!imageAnimator) {
        return;
    }
    const auto& controller = imageAnimator->GetImageAnimatorController();
    if (controller) {
        auto pauseEvent = GetEventMarker(info);
        controller->SetPauseEvent(pauseEvent);
    }
}

void JSImageAnimator::OnRepeat(const JSCallbackInfo& info)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!imageAnimator) {
        return;
    }
    const auto& controller = imageAnimator->GetImageAnimatorController();
    if (controller) {
        auto repeatEvent = GetEventMarker(info);
        controller->SetRepeatEvent(repeatEvent);
    }
}

void JSImageAnimator::OnCancel(const JSCallbackInfo& info)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!imageAnimator) {
        return;
    }
    const auto& controller = imageAnimator->GetImageAnimatorController();
    if (controller) {
        auto cancelEvent = GetEventMarker(info);
        controller->SetCancelEvent(cancelEvent);
    }
}

void JSImageAnimator::OnFinish(const JSCallbackInfo& info)
{
    auto imageAnimator =
        AceType::DynamicCast<ImageAnimatorComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!imageAnimator) {
        return;
    }
    const auto& controller = imageAnimator->GetImageAnimatorController();
    if (controller) {
        auto finishEvent = GetEventMarker(info);
        controller->SetStopEvent(finishEvent);
    }
}

EventMarker JSImageAnimator::GetEventMarker(const JSCallbackInfo& info)
{
    if (!info[0]->IsFunction()) {
        LOGE("info[0] is not a function.");
        return EventMarker();
    }
    RefPtr<JsFunction> jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(info[0]));
    auto eventMarker = EventMarker([execCtx = info.GetExecutionContext(), func = std::move(jsFunc)]() {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("ImageAnimator.onClick");
        func->Execute();
    });
    return eventMarker;
}

void JSImageAnimator::ParseImages(const std::unique_ptr<JsonValue>& image, ImageProperties& imageProperties)
{
    imageProperties.src = image->GetString("src");
    if (!ParseJsonDimensionVp(image->GetValue("width"), imageProperties.width)) {
        LOGW("parse image property width failed!");
    }
    if (!ParseJsonDimensionVp(image->GetValue("height"), imageProperties.height)) {
        LOGW("parse image property height failed!");
    }
    if (!ParseJsonDimensionVp(image->GetValue("top"), imageProperties.top)) {
        LOGW("parse image property top failed!");
    }
    if (!ParseJsonDimensionVp(image->GetValue("left"), imageProperties.left)) {
        LOGW("parse image property left failed!");
    }
    imageProperties.duration = image->GetInt("duration");
}

} // namespace OHOS::Ace::Framework
