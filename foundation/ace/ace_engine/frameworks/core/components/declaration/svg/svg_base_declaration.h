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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_BASE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_BASE_DECLARATION_H

#include "core/components/common/properties/svg_paint_state.h"
#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

const char VALUE_NONE[] = "none";

struct SvgBaseAttribute : Attribute {
    bool hasOpacity = false;
    double opacity = 1.0;
    FillState fillState;
    StrokeState strokeState;
    SvgTextStyle textStyle;
    std::string transform;
    std::string transformOrigin;
    std::string filterId;
    std::string maskId;
    std::string href;
    std::string id;
    ClipState clipState;
};

class SvgBaseDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SvgBaseDeclaration, Declaration);

public:
    SvgBaseDeclaration() = default;
    ~SvgBaseDeclaration() override = default;

    const FillState& GetFillState() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.fillState;
    }

    const StrokeState& GetStrokeState() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.strokeState;
    }

    const SvgTextStyle& GetSvgTextStyle() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.textStyle;
    }

    const std::string& GetTransform() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.transform;
    }

    const std::string& GetMaskId() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.maskId;
    }

    const std::string& GetFilterId() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.filterId;
    }

    const std::string& GetTransformOrigin() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.transformOrigin;
    }

    bool HasOpacity() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.hasOpacity;
    }

    double GetOpacity() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.opacity;
    }

    void Inherit(const RefPtr<Declaration>& parent);

    void SetGradient(const Gradient& gradient)
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        attribute.fillState.SetGradient(gradient);
    }

    const std::string& GetClipPathHref() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.clipState.GetHref();
    }

    void SetHref(const std::string& href)
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        attribute.href = href;
    }

    const std::string& GetHref() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.href;
    }

    const ClipState& GetClipState() const
    {
        auto& attribute = static_cast<SvgBaseAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.clipState;
    }

    const std::string& GetId()
    {
        auto& commonAttr = MaybeResetAttribute<CommonAttribute>(AttributeTag::COMMON_ATTR);
        return commonAttr.id;
    }

    LineCapStyle GetLineCapStyle(const std::string& val) const;
    LineJoinStyle GetLineJoinStyle(const std::string& val) const;
    Color GetColor(const std::string& value) const;

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool SetPresentationAttr(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_BASE_DECLARATION_H
