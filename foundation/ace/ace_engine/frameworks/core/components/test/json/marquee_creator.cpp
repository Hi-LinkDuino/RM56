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

#include "core/components/test/json/marquee_creator.h"

#include "base/log/log.h"
#include "core/components/marquee/marquee_theme.h"
#include "core/components/test/json/text_style_creator.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> MarqueeCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    auto marquee = AceType::MakeRefPtr<MarqueeComponent>();
    InitStyle(marquee);
    if (componentJson.Contains(MARQUEE_VALUE) && componentJson.GetValue(MARQUEE_VALUE)->IsString()) {
        auto textValue = componentJson.GetValue(MARQUEE_VALUE)->GetString();
        marquee->SetValue(textValue);
    }
    if (componentJson.Contains(MARQUEE_SCROLL_AMOUNT) && componentJson.GetValue(MARQUEE_SCROLL_AMOUNT)->IsNumber()) {
        double scrollAmount = componentJson.GetValue(MARQUEE_SCROLL_AMOUNT)->GetDouble();
        marquee->SetScrollAmount(scrollAmount);
    }
    if (componentJson.Contains(MARQUEE_LOOP) && componentJson.GetValue(MARQUEE_LOOP)->IsNumber()) {
        int32_t loop = componentJson.GetValue(MARQUEE_LOOP)->GetInt();
        marquee->SetLoop(loop);
    }
    if (componentJson.Contains(MARQUEE_DIRECTION) && componentJson.GetValue(MARQUEE_DIRECTION)->IsString()) {
        auto direction = componentJson.GetValue(MARQUEE_DIRECTION)->GetString();
        if (direction == MARQUEE_DIRECTION_LEFT) {
            marquee->SetDirection(MarqueeDirection::LEFT);
        } else if (direction == MARQUEE_DIRECTION_RIGHT) {
            marquee->SetDirection(MarqueeDirection::RIGHT);
        } else {
            LOGE("direction error: %{public}s", direction.c_str());
        }
    }
    if (componentJson.Contains(TEXT_STYLE) && componentJson.GetValue(TEXT_STYLE)->IsObject()) {
        TextStyle textStyle = TextStyleCreator::CreateFromJson(*componentJson.GetValue(TEXT_STYLE));
        marquee->SetTextStyle(textStyle);
    }
    if (componentJson.Contains(MARQUEE_BOUNCE) && componentJson.GetValue(MARQUEE_BOUNCE)->IsString()) {
        auto bounce = componentJson.GetValue(MARQUEE_BOUNCE)->GetString();
        marquee->SetBounceEventId(EventMarker(bounce));
    }
    if (componentJson.Contains(MARQUEE_FINISH) && componentJson.GetValue(MARQUEE_FINISH)->IsString()) {
        auto finish = componentJson.GetValue(MARQUEE_FINISH)->GetString();
        marquee->SetFinishEventId(EventMarker(finish));
    }
    if (componentJson.Contains(MARQUEE_START) && componentJson.GetValue(MARQUEE_START)->IsString()) {
        auto start = componentJson.GetValue(MARQUEE_START)->GetString();
        marquee->SetStartEventId(EventMarker(start));
    }
    return marquee;
}

void MarqueeCreator::InitStyle(RefPtr<MarqueeComponent> marquee)
{
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    auto theme = themeManager->GetTheme<MarqueeTheme>();
    TextStyle textStyle;
    textStyle.SetFontSize(theme->GetFontSize());
    textStyle.SetTextColor(theme->GetTextColor());
    marquee->SetTextStyle(textStyle);
}

} // namespace OHOS::Ace