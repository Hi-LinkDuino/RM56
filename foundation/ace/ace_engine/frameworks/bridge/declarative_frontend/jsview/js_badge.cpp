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

#include "bridge/declarative_frontend/jsview/js_badge.h"

#include "base/log/ace_trace.h"
#include "core/components/badge/badge_component.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSBadge::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("The argv is wrong, it is supposed to be a object");
        return;
    }

    auto badge = AceType::MakeRefPtr<OHOS::Ace::BadgeComponent>();
    SetDefaultTheme(badge);

    auto obj = JSRef<JSObject>::Cast(info[0]);
    SetCustomizedTheme(obj, badge);

    ViewStackProcessor::GetInstance()->Push(badge);
}

void JSBadge::JSBind(BindingTarget globalObj)
{
    JSClass<JSBadge>::Declare("Badge");

    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSBadge>::StaticMethod("create", &JSBadge::Create, opt);

    JSClass<JSBadge>::Inherit<JSViewAbstract>();
    JSClass<JSBadge>::Bind(globalObj);
}

void JSBadge::SetDefaultTheme(OHOS::Ace::RefPtr<OHOS::Ace::BadgeComponent>& badge)
{
    auto badgeTheme = GetTheme<BadgeTheme>();
    if (!badgeTheme) {
        LOGE("Get badge theme error");
        return;
    }

    badge->SetBadgeColor(badgeTheme->GetBadgeColor());
    badge->SetMessageCount(badgeTheme->GetMessageCount());
    badge->SetBadgePosition(badgeTheme->GetBadgePosition());
    badge->SetBadgeTextColor(badgeTheme->GetBadgeTextColor());
    badge->SetBadgeFontSize(badgeTheme->GetBadgeFontSize());
}

void JSBadge::SetCustomizedTheme(const JSRef<JSObject>& obj, OHOS::Ace::RefPtr<OHOS::Ace::BadgeComponent>& badge)
{
    auto count = obj->GetProperty("count");
    if (!count->IsNull() && count->IsNumber()) {
        auto value = count->ToNumber<int32_t>();
        badge->SetMessageCount(value);
    }

    auto position = obj->GetProperty("position");
    if (!position->IsNull() && position->IsNumber()) {
        auto value = position->ToNumber<int32_t>();
        badge->SetBadgePosition(static_cast<BadgePosition>(value));
    }

    auto maxCount = obj->GetProperty("maxCount");
    if (!maxCount->IsNull() && maxCount->IsNumber()) {
        auto value = maxCount->ToNumber<int32_t>();
        badge->SetMaxCount(value);
    }

    auto style = obj->GetProperty("style");
    if (!style->IsNull() && style->IsObject()) {
        auto value = JSRef<JSObject>::Cast(style);
        JSRef<JSVal> colorValue = value->GetProperty("color");
        JSRef<JSVal> fontSizeValue = value->GetProperty("fontSize");
        JSRef<JSVal> badgeSizeValue = value->GetProperty("badgeSize");
        JSRef<JSVal> badgeColorValue = value->GetProperty("badgeColor");

        Color colorVal;
        if (ParseJsColor(colorValue, colorVal))  {
            badge->SetBadgeTextColor(colorVal);
        }

        Dimension fontSize;
        if (ParseJsDimensionFp(fontSizeValue, fontSize)) {
            badge->SetBadgeFontSize(fontSize);
        }

        Dimension badgeSize;
        if (ParseJsDimensionFp(badgeSizeValue, badgeSize)) {
            badge->SetBadgeCircleSize(badgeSize);
        }

        Color badgeColor;
        if (ParseJsColor(badgeColorValue, badgeColor)) {
            badge->SetBadgeColor(badgeColor);
        }
    }

    auto value = obj->GetProperty("value");
    if (!value->IsNull() && value->IsString()) {
        auto label = value->ToString();
        badge->SetBadgeLabel(label);
    }
}

} // namespace OHOS::Ace::Framework