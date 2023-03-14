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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_COLORMATRIX_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_COLORMATRIX_DECLARATION_H

#include "core/components/declaration/svg/svg_fe_declaration.h"

namespace OHOS::Ace {

struct SvgFeColorMatrixAttribute : SvgFeAttribute {
    std::string type;
    std::string values;
};

class SvgFeColorMatrixDeclaration : public SvgFeDeclaration {
    DECLARE_ACE_TYPE(SvgFeColorMatrixDeclaration, SvgFeDeclaration);

public:
    SvgFeColorMatrixDeclaration() = default;
    ~SvgFeColorMatrixDeclaration() override = default;
    void InitializeStyle() override;

    void SetType(const std::string& type)
    {
        auto& attribute = MaybeResetAttribute<SvgFeColorMatrixAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.type = type;
    }

    void SetValues(const std::string& values)
    {
        auto& attribute = MaybeResetAttribute<SvgFeColorMatrixAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.values = values;
    }

    const std::string& GetType() const
    {
        auto& attribute = static_cast<SvgFeColorMatrixAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.type;
    }

    const std::string& GetValues() const
    {
        auto& attribute = static_cast<SvgFeColorMatrixAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.values;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_FE_COLORMATRIX_DECLARATION_H
