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

#include "core/components/text_span/text_span_component.h"

#include "core/components/text_span/render_text_span.h"
#include "core/components/text_span/text_span_element.h"

namespace OHOS::Ace {

TextSpanComponent::TextSpanComponent(const std::string& spanData) : ComponentGroup(std::list<RefPtr<Component>>())
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SpanDeclaration>();
        declaration_->Init();
    }
    SetSpanData(spanData);
}

RefPtr<Element> TextSpanComponent::CreateElement()
{
    return AceType::MakeRefPtr<TextSpanElement>();
}

RefPtr<RenderNode> TextSpanComponent::CreateRenderNode()
{
    return RenderTextSpan::Create();
}

const std::string& TextSpanComponent::GetSpanData() const
{
    return declaration_->GetSpanData();
}

void TextSpanComponent::SetSpanData(const std::string& data)
{
    declaration_->SetSpanData(data);
}

const TextStyle& TextSpanComponent::GetTextStyle() const
{
    return declaration_->GetSpanStyle();
}

void TextSpanComponent::SetTextStyle(const TextStyle& spanStyle)
{
    declaration_->SetTextStyle(spanStyle);
    hasNewStyle_ = true;
}

bool TextSpanComponent::IsShow() const
{
    return declaration_->IsShow();
}

void TextSpanComponent::SetIsShow(bool isShow)
{
    declaration_->SetIsShow(isShow);
}

void TextSpanComponent::SetOnClick(const EventMarker& onClick)
{
    declaration_->SetClickEvent(onClick);
}

void TextSpanComponent::SetRemoteMessageEventId(const EventMarker& eventId)
{
    declaration_->SetRemoteMessageEvent(eventId);
}

void TextSpanComponent::SetDeclaration(const RefPtr<SpanDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

const RefPtr<SpanDeclaration>& TextSpanComponent::GetDeclaration() const
{
    return declaration_;
}

} // namespace OHOS::Ace