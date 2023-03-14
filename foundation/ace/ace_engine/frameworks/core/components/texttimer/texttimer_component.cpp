/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components/texttimer/texttimer_component.h"

#include "base/i18n/localization.h"
#include "core/components/texttimer/render_texttimer.h"
#include "core/components/texttimer/texttimer_element.h"

namespace OHOS::Ace {
TextTimerComponent::TextTimerComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<TextTimerDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> TextTimerComponent::CreateRenderNode()
{
    return RenderTextTimer::Create();
}

RefPtr<Element> TextTimerComponent::CreateElement()
{
    return AceType::MakeRefPtr<TextTimerElement>();
}

double TextTimerComponent::GetInputCount() const
{
    return declaration_->GetInputCount();
}

void TextTimerComponent::SetInputCount(double value)
{
    declaration_->SetInputCount(value);
}

bool TextTimerComponent::GetIsCountDown() const
{
    return declaration_->GetIsCountDown();
}

void TextTimerComponent::SetIsCountDown(bool isCountDown)
{
    declaration_->SetIsCountDown(isCountDown);
}

const std::string& TextTimerComponent::GetFormat() const
{
    return declaration_->GetFormat();
}

void TextTimerComponent::SetFormat(const std::string& value)
{
    declaration_->SetFormat(value);
}

const TextStyle& TextTimerComponent::GetTextStyle() const
{
    return declaration_->GetTextStyle();
}

void TextTimerComponent::SetTextStyle(const TextStyle& style)
{
    declaration_->SetTextStyle(style);
}

RefPtr<TextTimerController> TextTimerComponent::GetTextTimerController() const
{
    return declaration_->GetTextTimerController();
}
} // namespace OHOS::Ace