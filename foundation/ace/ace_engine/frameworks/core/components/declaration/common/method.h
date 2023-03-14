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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_METHOD_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_METHOD_H

#include "core/components/declaration/common/controllers.h"
#include "frameworks/core/components/scroll/scroll_position_controller.h"

namespace OHOS::Ace {

enum class MethodTag {
    COMMON_METHOD = 0,
    SPECIALIZED_METHOD,
    UNKNOWN,
    DEFAULT,
};

struct Method {
    bool IsValid() const
    {
        return tag != MethodTag::UNKNOWN;
    }

    bool IsShared() const
    {
        return isShared;
    }

    bool isShared = true;
    MethodTag tag = MethodTag::DEFAULT;
};

struct CommonMethod : Method {
    void Focus(const RefPtr<FocusableController>& focusableController, bool shouldFocus) const
    {
        if (focusableController) {
            focusableController->RequestFocus(shouldFocus);
        }
    }

    void ScrollBy(const RefPtr<ScrollPositionController>& positionController, double dx, double dy, bool isSmooth) const
    {
        if (positionController) {
            positionController->ScrollBy(dx, dy, isSmooth);
        }
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_COMMON_METHOD_H
