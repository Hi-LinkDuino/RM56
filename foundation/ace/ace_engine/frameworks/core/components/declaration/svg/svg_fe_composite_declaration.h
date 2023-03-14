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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_COMPOSITE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_COMPOSITE_DECLARATION_H

#include "core/components/declaration/svg/svg_fe_declaration.h"

namespace OHOS::Ace {

enum class FeOperatorType {
    FE_ARITHMETIC,
    FE_ATOP,
    FE_IN,
    FE_LIGHTER,
    FE_OUT,
    FE_OVER,
    FE_XOR
};

struct SvgFeCompositeAttribute : SvgFeAttribute {
    FeInType in2 = FeInType::PRIMITIVE;
    FeOperatorType operatorType = FeOperatorType::FE_OVER;
    double k1 = 0.0;
    double k2 = 0.0;
    double k3 = 0.0;
    double k4 = 0.0;
};

class SvgFeCompositeDeclaration : public SvgFeDeclaration {
    DECLARE_ACE_TYPE(SvgFeCompositeDeclaration, SvgFeDeclaration);

public:
    SvgFeCompositeDeclaration() = default;
    ~SvgFeCompositeDeclaration() override = default;
    void InitializeStyle() override;

    void SetK1(double k1)
    {
        auto& attribute = MaybeResetAttribute<SvgFeCompositeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.k1 = k1;
    }

    void SetK2(double k2)
    {
        auto& attribute = MaybeResetAttribute<SvgFeCompositeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.k2 = k2;
    }

    void SetK3(double k3)
    {
        auto& attribute = MaybeResetAttribute<SvgFeCompositeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.k3 = k3;
    }

    void SetK4(double k4)
    {
        auto& attribute = MaybeResetAttribute<SvgFeCompositeAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.k4 = k4;
    }

    void SetIn2(const std::string& In2)
    {
        static const LinearMapNode<FeInType> IN_TABLE[] = {
            { "BackgroundAlpha", FeInType::BACKGROUND_ALPHA },
            { "BackgroundImage", FeInType::BACKGROUND_IMAGE },
            { "FillPaint", FeInType::FILL_PAINT },
            { "SourceAlpha", FeInType::SOURCE_ALPHA },
            { "SourceGraphic", FeInType::SOURCE_GRAPHIC },
            { "StrokePaint", FeInType::STROKE_PAINT },
        };
        int64_t inIndex = BinarySearchFindIndex(IN_TABLE, ArraySize(IN_TABLE), In2.c_str());
        if (inIndex != -1) {
            auto& attribute = MaybeResetAttribute<SvgFeCompositeAttribute>(AttributeTag::SPECIALIZED_ATTR);
            attribute.in2 = IN_TABLE[inIndex].value;
        }
    }

    void SetOperatorType(const std::string& type)
    {
        static const LinearMapNode<FeOperatorType> FE_OPERATOR_TABLE[] = {
            { "arithmetic", FeOperatorType::FE_ARITHMETIC },
            { "atop", FeOperatorType::FE_ATOP },
            { "in", FeOperatorType::FE_IN },
            { "lighter", FeOperatorType::FE_LIGHTER },
            { "out", FeOperatorType::FE_OUT },
            { "over", FeOperatorType::FE_OVER },
            { "xor", FeOperatorType::FE_XOR },
        };
        int64_t inIndex = BinarySearchFindIndex(FE_OPERATOR_TABLE, ArraySize(FE_OPERATOR_TABLE), type.c_str());
        if (inIndex != -1) {
            auto& attribute = MaybeResetAttribute<SvgFeCompositeAttribute>(AttributeTag::SPECIALIZED_ATTR);
            attribute.operatorType = FE_OPERATOR_TABLE[inIndex].value;
        }
    }

    double GetK1() const
    {
        auto& attribute = static_cast<SvgFeCompositeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.k1;
    }

    double GetK2() const
    {
        auto& attribute = static_cast<SvgFeCompositeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.k2;
    }

    double GetK3() const
    {
        auto& attribute = static_cast<SvgFeCompositeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.k3;
    }

    double GetK4() const
    {
        auto& attribute = static_cast<SvgFeCompositeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.k4;
    }

    const FeInType& GetIn2() const
    {
        auto& attribute = static_cast<SvgFeCompositeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.in2;
    }

    const FeOperatorType& GetOperatorType() const
    {
        auto& attribute = static_cast<SvgFeCompositeAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.operatorType;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_COMPOSITE_DECLARATION_H
