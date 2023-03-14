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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_DIV_DIV_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_DIV_DIV_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/core/components/scroll/scroll_position_controller.h"

namespace OHOS::Ace {

struct DivAttribute : Attribute {
    bool isCard = false;
    bool isCardBlur = false;
};

struct DivStyle : Style {
    FlexDirection flexDirection = FlexDirection::ROW;
    bool isFlexWrap = false;
    std::string justifyContent = DOM_JUSTIFY_CONTENT_START;
    std::string alignItems = DOM_ALIGN_ITEMS_STRETCH;
    std::string alignContent = DOM_ALIGN_CONTENT_START;
    std::string columnsArgs;
    std::string rowsArgs;
    double columnGap = 0.0;
    double rowGap = 0.0;
    int32_t rowStart = -1;
    int32_t rowEnd = -1;
    int32_t columnStart = -1;
    int32_t columnEnd = -1;
    std::string direction = DOM_FLEX_ROW;
    std::pair<bool, Color> scrollBarColor;
    std::pair<bool, Dimension> scrollBarWidth;
    EdgeEffect edgeEffect = EdgeEffect::NONE;
};

class DivDeclaration : public Declaration {
    DECLARE_ACE_TYPE(DivDeclaration, Declaration);

public:
    DivDeclaration() = default;
    ~DivDeclaration() override = default;

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_DIV_DIV_DECLARATION_H
