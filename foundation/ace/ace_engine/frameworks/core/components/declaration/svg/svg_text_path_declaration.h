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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_TEXT_PATH_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_TEXT_PATH_DECLARATION_H

#include "core/components/declaration/svg/svg_base_declaration.h"

namespace OHOS::Ace {

struct SvgTextPathAttribute : SvgBaseAttribute {
    Dimension startOffset;
    std::string path;
    std::string value;
};

class SvgTextPathDeclaration : public SvgBaseDeclaration {
    DECLARE_ACE_TYPE(SvgTextPathDeclaration, SvgBaseDeclaration);

public:
    SvgTextPathDeclaration() = default;
    ~SvgTextPathDeclaration() override = default;
    void InitializeStyle() override;

    const std::string& GetTextData() const
    {
        auto& attribute = static_cast<SvgTextPathAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.value;
    }

    void SetTextData(const std::string& textData)
    {
        auto& attribute = MaybeResetAttribute<SvgTextPathAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.value = textData;
    }

    const std::string& GetPath() const
    {
        auto& attribute = static_cast<SvgTextPathAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.path;
    }

    void SetPath(const std::string& path)
    {
        auto& attribute = MaybeResetAttribute<SvgTextPathAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.path = path;
    }

    void SetStartOffset(const Dimension& startOffset)
    {
        auto& attribute = MaybeResetAttribute<SvgTextPathAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.startOffset = startOffset;
    }

    const Dimension& GetStartOffset() const
    {
        auto& attribute = static_cast<SvgTextPathAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.startOffset;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

private:
    bool SetSpecializedValue(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_TEXT_PATH_DECLARATION_H
