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

#include "core/components/text/text_component_v2.h"

#include "core/components/declaration/span/span_declaration.h"
#include "core/components/text_span/text_span_component.h"

namespace OHOS::Ace {

void TextComponentV2::OnChildInserted(const RefPtr<Component>& child, int32_t position)
{
    CheckAndSetChildStyle(child);
}

void TextComponentV2::OnChildAppended(const RefPtr<Component>& child)
{
    CheckAndSetChildStyle(child);
}

void TextComponentV2::CheckAndSetChildStyle(const RefPtr<Component>& child)
{
    // If span component has no developer-set styles, then set text styles to span
    auto spanComponent = AceType::DynamicCast<TextSpanComponent>(child);
    if (!spanComponent) {
        LOGW("child of text is not valid.");
        return;
    }
    auto spanDeclaration = AceType::DynamicCast<SpanDeclaration>(spanComponent->GetDeclaration());
    if (!spanDeclaration) {
        LOGW("declaration of span is not valid.");
        return;
    }
    auto spanStyle = spanComponent->GetTextStyle();
    auto textStyle = GetTextStyle();

    if (!spanDeclaration->HasSetFontSize()) {
        spanStyle.SetFontSize(textStyle.GetFontSize());
    }
    if (!spanDeclaration->HasSetFontStyle()) {
        spanStyle.SetFontStyle(textStyle.GetFontStyle());
    }
    if (!spanDeclaration->HasSetFontColor()) {
        spanStyle.SetTextColor(textStyle.GetTextColor());
    }
    if (!spanDeclaration->HasSetFontWeight()) {
        spanStyle.SetFontWeight(textStyle.GetFontWeight());
    }
    if (!spanDeclaration->HasSetTextDecoration()) {
        spanStyle.SetTextDecoration(textStyle.GetTextDecoration());
        spanStyle.SetTextDecorationColor(textStyle.GetTextDecorationColor());
    }
    if (!spanDeclaration->HasSetFontFamily()) {
        spanStyle.SetFontFamilies(textStyle.GetFontFamilies());
    }
    if (!spanDeclaration->HasSetAllowScale()) {
        spanStyle.SetAllowScale(textStyle.IsAllowScale());
    }
    if (!spanDeclaration->HasSetFontFeatures()) {
        spanStyle.SetFontFeatures(textStyle.GetFontFeatures());
    }
    if (!spanDeclaration->HasSetLetterSpacing()) {
        spanStyle.SetLetterSpacing(textStyle.GetLetterSpacing());
    }
    if (!spanDeclaration->HasSetTextCase()) {
        spanStyle.SetTextCase(textStyle.GetTextCase());
    }
    // Span don't support developer-set line-height.
    spanStyle.SetLineHeight(textStyle.GetLineHeight(), textStyle.HasHeightOverride());
    spanComponent->SetTextStyle(spanStyle);
}

} // namespace OHOS::Ace
