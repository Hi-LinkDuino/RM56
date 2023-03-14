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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SPAN_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SPAN_H

#include "core/components/declaration/span/span_declaration.h"
#include "core/components/text_span/text_span_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace::Framework {

class DOMSpan final : public DOMNode {
    DECLARE_ACE_TYPE(DOMSpan, DOMNode);

public:
    DOMSpan(NodeId nodeId, const std::string& nodeName);
    ~DOMSpan() override = default;

    bool HasSetFontStyle() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetFontStyle() : false;
    }

    bool HasSetFontColor() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetFontColor() : false;
    }

    bool HasSetFontWeight() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetFontWeight() : false;
    }

    bool HasSetFontSize() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetFontSize() : false;
    }

    bool HasSetFontFamily() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetFontFamily() : false;
    }

    bool HasSetTextDecoration() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetTextDecoration() : false;
    }

    bool HasSetAllowScale() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetAllowScale() : false;
    }

    bool HasSetFontFeatures() const
    {
        auto declaration = AceType::DynamicCast<SpanDeclaration>(declaration_);
        return declaration ? declaration->HasSetFontFeatures() : false;
    }

    void SetTextStyle(const TextStyle& spanStyle)
    {
        if (declaration_) {
            auto& specializedStyle = declaration_->MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
            if (specializedStyle.IsValid()) {
                specializedStyle.spanStyle = spanStyle;
            }
        }
    }

    RefPtr<Component> GetSpecializedComponent() override
    {
        return textSpanChild_;
    }

protected:
    void PrepareSpecializedComponent() override;
    void OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot) override;
    void OnChildNodeRemoved(const RefPtr<DOMNode>& child) override;
    void ResetInitializedStyle() override;

private:
    void CheckAndSetCurrentSpanStyle(
        const RefPtr<DOMSpan>& domSpan, TextStyle& currentStyle, const TextStyle& parentStyle);

    std::list<RefPtr<DOMNode>> children_;
    RefPtr<TextSpanComponent> textSpanChild_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_SPAN_H
