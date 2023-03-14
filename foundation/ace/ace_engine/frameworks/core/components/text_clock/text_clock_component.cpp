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

#include "core/components/text_clock/text_clock_component.h"

#include "core/components/text_clock/render_text_clock.h"
#include "core/components/text_clock/text_clock_element.h"

namespace OHOS::Ace {
RefPtr<Element> TextClockComponent::CreateElement()
{
    return AceType::MakeRefPtr<TextClockElement>();
}

RefPtr<RenderNode> TextClockComponent::CreateRenderNode()
{
    return RenderTextClock::Create();
}

void TextClockComponent::SetHoursWest(double hoursWest)
{
    hoursWest_ = hoursWest;
}

double TextClockComponent::GetHoursWest() const
{
    return hoursWest_;
}

void TextClockComponent::SetFormat(const std::string& format)
{
    format_ = format;
}

std::string TextClockComponent::GetFormat() const
{
    return format_;
}

void TextClockComponent::SetTextClockController(RefPtr<TextClockController> textClockController)
{
    textClockController_ = textClockController;
}

RefPtr<TextClockController> TextClockComponent::GetTextClockController() const
{
    return textClockController_;
}
} // namespace OHOS::Ace