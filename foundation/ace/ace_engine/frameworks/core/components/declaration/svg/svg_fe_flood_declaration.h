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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_FLOOD_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_FLOOD_DECLARATION_H

#include "core/components/declaration/svg/svg_fe_declaration.h"

namespace OHOS::Ace {

struct SvgFeFloodAttribute : SvgFeAttribute {
    Color floodColor = Color::BLACK;
    double floodOpacity = 1.0;
};

class SvgFeFloodDeclaration : public SvgFeDeclaration {
    DECLARE_ACE_TYPE(SvgFeFloodDeclaration, SvgFeDeclaration);

public:
    SvgFeFloodDeclaration() = default;
    ~SvgFeFloodDeclaration() override = default;
    void InitializeStyle() override;

    void SetFloodColor(const Color& floodColor)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFloodAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.floodColor = floodColor;
    }

    void SetFloodOpacity(const double& floodOpacity)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFloodAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.floodOpacity = floodOpacity;
    }

    const Color& GetFloodColor() const
    {
        auto& attribute = static_cast<SvgFeFloodAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.floodColor;
    }

    double GetFloodOpacity() const
    {
        auto& attribute = static_cast<SvgFeFloodAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.floodOpacity;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_FLOOD_DECLARATION_H
