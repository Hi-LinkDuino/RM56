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

#include "frameworks/bridge/common/dom/dom_span.h"

#include "core/common/ace_application_info.h"
#include "core/components/declaration/span/span_declaration.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMSpan::DOMSpan(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    textSpanChild_ = AceType::MakeRefPtr<TextSpanComponent>("");
    // span has no box component.
    boxComponent_.Reset();
}

void DOMSpan::ResetInitializedStyle()
{
    for (const auto& child : GetChildList()) {
        auto domSpan = AceType::DynamicCast<DOMSpan>(child);
        if (!domSpan) {
            LOGW("span only support span child");
            continue;
        }
        auto spanComponent = AceType::DynamicCast<TextSpanComponent>(domSpan->GetSpecializedComponent());
        if (!spanComponent) {
            LOGW("span only support span child");
            continue;
        }

        // If span component has no developer-set styles, then set text styles to span
        TextStyle spanStyle = spanComponent->GetTextStyle();
        CheckAndSetCurrentSpanStyle(domSpan, spanStyle, textSpanChild_->GetTextStyle());
        domSpan->SetTextStyle(spanStyle);
        spanComponent->SetTextStyle(spanStyle);
    }
}

void DOMSpan::CheckAndSetCurrentSpanStyle(
    const RefPtr<DOMSpan>& domSpan, TextStyle& currentStyle, const TextStyle& parentStyle)
{
    if (!domSpan->HasSetFontColor()) {
        currentStyle.SetTextColor(parentStyle.GetTextColor());
    }
    if (!domSpan->HasSetFontSize()) {
        currentStyle.SetFontSize(parentStyle.GetFontSize());
    }
    if (!domSpan->HasSetFontStyle()) {
        currentStyle.SetFontStyle(parentStyle.GetFontStyle());
    }
    if (!domSpan->HasSetFontWeight()) {
        currentStyle.SetFontWeight(parentStyle.GetFontWeight());
    }
    if (!domSpan->HasSetTextDecoration()) {
        currentStyle.SetTextDecoration(parentStyle.GetTextDecoration());
    }
    if (!domSpan->HasSetFontFamily()) {
        currentStyle.SetFontFamilies(parentStyle.GetFontFamilies());
    }
    if (!domSpan->HasSetAllowScale()) {
        currentStyle.SetAllowScale(parentStyle.IsAllowScale());
    }
    if (!domSpan->HasSetFontFeatures()) {
        currentStyle.SetFontFeatures(parentStyle.GetFontFeatures());
    }
    currentStyle.SetLetterSpacing(parentStyle.GetLetterSpacing());
    currentStyle.SetLineHeight(parentStyle.GetLineHeight(), parentStyle.HasHeightOverride());
}

void DOMSpan::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        return;
    }

    if (child->GetTag() == DOM_NODE_TAG_SPAN) {
        // Get current span's parent (also span) styles, if no user setting styles
        // on this span, use its parent styles.
        TextStyle parentSpanStyle = textSpanChild_->GetTextStyle();
        auto spanComponent = AceType::DynamicCast<TextSpanComponent>(child->GetSpecializedComponent());
        if (!spanComponent) {
            LOGE("DOMSpan: span is null");
            return;
        }

        // Get current span's styles to compare with its parent span.
        TextStyle currentSpanStyle = spanComponent->GetTextStyle();
        auto domSpan = AceType::DynamicCast<DOMSpan>(child);
        CheckAndSetCurrentSpanStyle(domSpan, currentSpanStyle, parentSpanStyle);
        domSpan->SetTextStyle(currentSpanStyle);
        spanComponent->SetTextStyle(currentSpanStyle);
        textSpanChild_->InsertChild(slot, child->GetRootComponent());
    }
}

void DOMSpan::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child) {
        return;
    }

    textSpanChild_->RemoveChild(child->GetRootComponent());
}

void DOMSpan::PrepareSpecializedComponent()
{
    auto spanDeclaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
    if (!spanDeclaration) {
        return;
    }
    auto& specializedStyle = declaration_->MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
    if (specializedStyle.IsValid()) {
        bool isCard = AceApplicationInfo::GetInstance().GetIsCardType();
        if (isCard) {
            specializedStyle.spanStyle.SetAllowScale(false);
            if (specializedStyle.spanStyle.GetFontSize().Unit() == DimensionUnit::FP) {
                specializedStyle.spanStyle.SetAllowScale(true);
            }
        }
    }

    textSpanChild_->SetDeclaration(spanDeclaration);
}

} // namespace OHOS::Ace::Framework
