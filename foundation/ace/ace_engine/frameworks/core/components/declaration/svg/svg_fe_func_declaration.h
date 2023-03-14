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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_FUNC_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_FUNC_DECLARATION_H

#include "core/components/declaration/svg/svg_fe_declaration.h"

namespace OHOS::Ace {

enum class FeFuncType {
    DISCRETE,
    GAMMA,
    IDENTITY,
    LINEAR,
    TABLE,
};

struct SvgFeFuncAttribute : SvgFeAttribute {
    FeFuncType type = FeFuncType::IDENTITY;
    std::vector<double> tableValues; // list of number between 0 and 1
    double slope = 0.0;
    double intercept = 0.0;
    double amplitude = 1.0;
    double exponent = 1.0;
    double offset = 0.0;
};

class SvgFeFuncDeclaration : public SvgFeDeclaration {
    DECLARE_ACE_TYPE(SvgFeFuncDeclaration, SvgFeDeclaration);

public:
    SvgFeFuncDeclaration() = default;
    ~SvgFeFuncDeclaration() override = default;
    void InitializeStyle() override;

    void SetAmplitude(double amplitude)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.amplitude = amplitude;
    }

    void SetExponent(double exponent)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.exponent = exponent;
    }

    void SetIntercept(double intercept)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.intercept = intercept;
    }

    void SetOffset(double offset)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.offset = offset;
    }

    void SetSlope(double slope)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.slope = slope;
    }

    void SetTableValues(const std::string& tableValues)
    {
        auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
        StringUtils::StringSpliter(tableValues, ' ', attribute.tableValues);
    }

    void SetType(const std::string& type)
    {
        static const LinearMapNode<FeFuncType> FE_FUNC_TABLE[] = {
            { "discrete", FeFuncType::DISCRETE },
            { "gamma", FeFuncType::GAMMA },
            { "identity", FeFuncType::IDENTITY },
            { "linear", FeFuncType::LINEAR },
            { "table", FeFuncType::TABLE },
        };
        int64_t inIndex = BinarySearchFindIndex(FE_FUNC_TABLE, ArraySize(FE_FUNC_TABLE), type.c_str());
        if (inIndex != -1) {
            auto& attribute = MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
            attribute.type = FE_FUNC_TABLE[inIndex].value;
        }
    }

    FeFuncType GetType() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.type;
    }

    double GetAmplitude() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.amplitude;
    }

    double GetExponent() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.exponent;
    }

    double GetIntercept() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.intercept;
    }

    double GetOffset() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.offset;
    }

    double GetSlope() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.slope;
    }

    const std::vector<double>& GetTableValues() const
    {
        auto& attribute = static_cast<SvgFeFuncAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.tableValues;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_FUNC_DECLARATION_H
