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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_OFFSET_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_OFFSET_DECLARATION_H

#include "core/components/declaration/svg/svg_fe_declaration.h"

namespace OHOS::Ace {

struct SvgFeOffsetAttribute : SvgFeAttribute {
    Dimension dx;
    Dimension dy;
};

class SvgFeOffsetDeclaration : public SvgFeDeclaration {
    DECLARE_ACE_TYPE(SvgFeOffsetDeclaration, SvgFeDeclaration);

public:
    SvgFeOffsetDeclaration() = default;
    ~SvgFeOffsetDeclaration() override = default;
    void InitializeStyle() override;

    void SetDx(const Dimension& dx)
    {
        auto& attribute = MaybeResetAttribute<SvgFeOffsetAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.dx = dx;
    }

    void SetDy(const Dimension& dy)
    {
        auto& attribute = MaybeResetAttribute<SvgFeOffsetAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.dy = dy;
    }

    double GetDx() const
    {
        auto& attribute = static_cast<SvgFeOffsetAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        if (attribute.dx.Unit() == DimensionUnit::PERCENT) {
            return 0.0;
        }
        return attribute.dx.Value();
    }

    double GetDy() const
    {
        auto& attribute = static_cast<SvgFeOffsetAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        if (attribute.dy.Unit() == DimensionUnit::PERCENT) {
            return 0.0;
        }
        return attribute.dy.Value();
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_OFFSET_DECLARATION_H
