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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_ALIGN_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_ALIGN_DECLARATION_H

#include "base/memory/ace_type.h"
#include "core/components/common/layout/constants.h"

namespace OHOS::Ace {

class AlignDeclaration : public virtual AceType {
    DECLARE_ACE_TYPE(AlignDeclaration, AceType);

public:
    enum class Edge : uint32_t {
        TOP = 0,
        CENTER,
        BOTTOM,
        BASELINE,
        START,
        MIDDLE,
        END,
        AUTO,
    };

    enum class DeclarationType : uint32_t {
        VERTICAL = 0,
        HORIZONTAL,
    };

    AlignDeclaration() = default;
    ~AlignDeclaration() override = default;

    virtual DeclarationType GetDeclarationType() = 0;

    virtual HorizontalAlign GetHorizontalAlign() const
    {
        return HorizontalAlign::CENTER;
    }

    virtual VerticalAlign GetVerticalAlign() const
    {
        return VerticalAlign::CENTER;
    }
};

using AlignDeclarationPtr = AlignDeclaration*;

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_LAYOUT_ALIGN_DECLARATION_H
