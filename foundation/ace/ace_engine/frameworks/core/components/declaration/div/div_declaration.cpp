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

#include "core/components/declaration/div/div_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void DivDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_DIV_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_DIV_STYLE);
}

bool DivDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_DIV_CARD_TYPE) {
        auto& divAttr = MaybeResetAttribute<DivAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (divAttr.IsValid()) {
            divAttr.isCard = StringToBool(attr.second);
        }
        return true;
    } else if (attr.first == DOM_DIV_CARD_BLUR) {
        auto& divAttr = MaybeResetAttribute<DivAttribute>(AttributeTag::SPECIALIZED_ATTR);
        if (divAttr.IsValid()) {
            divAttr.isCardBlur = StringToBool(attr.second);
        }
        return true;
    }
    return false;
}

bool DivDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DivDeclaration&)> styleSetters[] {
        { DOM_ALIGN_CONTENT,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.alignContent = value;
                }
            } },
        { DOM_ALIGN_ITEMS,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.alignItems = value;
                }
            } },
        { DOM_FLEX_DIRECTION,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.direction = value;
                }
            } },
        { DOM_FLEX_WRAP,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.isFlexWrap = (value == DOM_WRAP);
                }
            } },
        { DOM_GRID_AUTO_FLOW,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.direction = value;
                }
            } },
        { DOM_GRID_COLUMN_END,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.columnEnd = StringToInt(value);
                }
            } },
        { DOM_GRID_COLUMN_START,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.columnStart = StringToInt(value);
                }
            } },
        { DOM_GRID_COLUMN_GAP,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.columnGap = StringToDouble(value);
                }
            } },
        { DOM_GRID_ROW_END,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.rowEnd = StringToInt(value);
                }
            } },
        { DOM_GRID_ROW_START,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.rowStart = StringToInt(value);
                }
            } },
        { DOM_GRID_ROW_GAP,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.rowGap = StringToDouble(value);
                }
            } },
        { DOM_GRID_TEMPLATE_COLUMNS,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.columnsArgs = value;
                }
            } },
        { DOM_GRID_TEMPLATE_ROWS,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.rowsArgs = value;
                }
            } },
        { DOM_JUSTIFY_CONTENT,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.justifyContent = value;
                }
            } },
        { DOM_SCROLL_OVER_SCROLL_EFFECT,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    if (value == DOM_SCROLL_EFFECT_SPRING) {
                        divStyle.edgeEffect = EdgeEffect::SPRING;
                    } else if (value == DOM_SCROLL_EFFECT_FADE) {
                        divStyle.edgeEffect = EdgeEffect::FADE;
                    } else {
                        divStyle.edgeEffect = EdgeEffect::NONE;
                    }
                }
            } },
        { DOM_SCROLL_SCROLLBAR_COLOR,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.scrollBarColor.first = true;
                    divStyle.scrollBarColor.second = declaration.ParseColor(value);
                }
            } },
        { DOM_SCROLL_SCROLLBAR_WIDTH,
            [](const std::string& value, DivDeclaration& declaration) {
                auto& divStyle = declaration.MaybeResetStyle<DivStyle>(StyleTag::SPECIALIZED_STYLE);
                if (divStyle.IsValid()) {
                    divStyle.scrollBarWidth.first = true;
                    auto width = declaration.ParseDimension(value);
                    divStyle.scrollBarWidth.second = width.IsValid() ? width : Dimension();
                }
            } },
    };
    auto operatorIter = BinarySearchFindIndex(styleSetters, ArraySize(styleSetters), style.first.c_str());
    if (operatorIter != -1) {
        styleSetters[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
