/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_LAYOUT_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_LAYOUT_INFO_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

enum class GridSizeType {
    UNDEFINED = 0,
    XS,
    SM,
    MD,
    LG,
    XL,
};

enum class GridTemplateType {
    NORMAL = 0,
    GRID,
};

enum class GridColumnType {
    NONE = 0,
    BUBBLE_TYPE,
    BUTTON_TYPE,
    CARD_TYPE,
    CONTENT_TYPE,
    MENU,
    TOAST,
    PANEL,
    TOOL_BAR,
    DIALOG,
    CAR_DIALOG,
};

class GridLayoutInfo : public virtual AceType {
    DECLARE_ACE_TYPE(GridLayoutInfo, AceType);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_GRID_LAYOUT_INFO_H
