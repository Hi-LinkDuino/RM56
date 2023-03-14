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

#include "frameworks/bridge/declarative_frontend/jsview/js_rating.h"

#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "core/components/box/box_component.h"
#include "core/components/rating/rating_component.h"
#include "core/components/rating/rating_theme.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSRating::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("piece create error, info is non-valid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto getRating = paramObject->GetProperty("rating");
    auto getIndicator = paramObject->GetProperty("indicator");
    double rating = 0;
    bool indicator = false;
    if (getRating->IsNumber()) {
        rating = getRating->ToNumber<double>();
    } else {
        LOGE("create rating fail because the rating is not value");
    }
    if (getIndicator->IsBoolean()) {
        indicator = getIndicator->ToBoolean();
    }
    auto component = AceType::MakeRefPtr<RatingComponent>();
    component->SetRatingScore(rating);
    component->SetIndicator(indicator);
    ViewStackProcessor::GetInstance()->Push(component);

    auto theme = GetTheme<RatingTheme>();
    if (!theme) {
        LOGE("rating theme is null");
        return;
    }
    auto boxComponent = ViewStackProcessor::GetInstance()->GetBoxComponent();
    if (component->GetIndicator()) {
        if (boxComponent->GetHeightDimension().Value() < 0.0) {
            boxComponent->SetHeight(theme->GetRatingMiniHeight());
        }
        if (boxComponent->GetWidthDimension().Value() < 0.0) {
            boxComponent->SetWidth(theme->GetRatingMiniWidth());
        }
        component->SetPaddingVertical(Dimension());
        component->SetMiniResIdFromTheme(theme);
    } else {
        if (boxComponent->GetHeightDimension().Value() < 0.0) {
            boxComponent->SetHeight(theme->GetRatingHeight());
        }
        if (boxComponent->GetWidthDimension().Value() < 0.0) {
            boxComponent->SetWidth(theme->GetRatingWidth());
        }
        component->SetPaddingVertical(theme->GetPaddingVertical());
        component->SetResIdFromTheme(theme);
    }
    component->SetThemeStyle(theme);
}

void JSRating::JSBind(BindingTarget globalObj)
{
    JSClass<JSRating>::Declare("Rating");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSRating>::StaticMethod("create", &JSRating::Create, opt);
    JSClass<JSRating>::StaticMethod("stars", &JSRating::SetStars, opt);
    JSClass<JSRating>::StaticMethod("stepSize", &JSRating::SetStepSize, opt);
    JSClass<JSRating>::StaticMethod("starStyle", &JSRating::SetStarStyle, opt);
    JSClass<JSRating>::StaticMethod("onChange", &JSRating::SetOnChange);
    JSClass<JSRating>::Inherit<JSViewAbstract>();
    JSClass<JSRating>::Bind<>(globalObj);
}

void JSRating::SetStars(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("the arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!info[0]->IsNumber()) {
        LOGE("arg is not number.");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<RatingComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ratingComponent is null");
        return;
    }
    component->SetStarNum(info[0]->ToNumber<int32_t>());
}

void JSRating::SetStepSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("the arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    if (!info[0]->IsNumber()) {
        LOGE("arg is not number.");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<RatingComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ratingComponent is null");
        return;
    }
    component->SetStepSize(info[0]->ToNumber<double>());
}

void JSRating::SetStarStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("the arg is wrong, it is supposed to have at least 1 arguments");
        return;
    }

    auto stack = ViewStackProcessor::GetInstance();
    auto component = AceType::DynamicCast<RatingComponent>(stack->GetMainComponent());
    if (!component) {
        LOGE("ratingComponent is null");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto getBackgroundUri = paramObject->GetProperty("backgroundUri");
    auto getForegroundUri = paramObject->GetProperty("foregroundUri");
    auto getSecondaryUri = paramObject->GetProperty("secondaryUri");
    std::string backgroundUri;
    std::string foregroundUri;
    std::string secondaryUri;
    if (getBackgroundUri->IsString()) {
        backgroundUri = getBackgroundUri->ToString();
    } else {
        LOGE("backgroundUri error , because the backgroundUri is not string");
    }

    if (getForegroundUri->IsString()) {
        foregroundUri = getForegroundUri->ToString();
    } else {
        LOGE("foregroundUri error , because the foregroundUri is not string");
    }

    if (getSecondaryUri->IsString()) {
        secondaryUri = getSecondaryUri->ToString();
    }

    component->SetBackgroundSrc(backgroundUri);
    component->SetForegroundSrc(foregroundUri);
    component->SetSecondarySrc(secondaryUri);
}

void JSRating::SetOnChange(const JSCallbackInfo& info)
{
    if (!JSViewBindEvent(&RatingComponent::SetOnChange, info)) {
        LOGW("failed to bind event");
    }
    info.ReturnSelf();
}

} // namespace OHOS::Ace::Framework