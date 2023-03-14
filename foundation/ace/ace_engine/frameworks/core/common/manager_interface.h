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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_MANAGER_INTERFACE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_MANAGER_INTERFACE_H

#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "core/components/scroll/scroll_element.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Ace {

class ManagerInterface : public AceType {
    DECLARE_ACE_TYPE(ManagerInterface, AceType);

public:
    virtual void SetClickPosition(const Offset& position) {}
    virtual const Offset& GetClickPosition()
    {
        static Offset empty;
        return empty;
    }
    virtual void MovePage(int32_t pageId, const Offset& rootRect, double offsetHeight) {}
    virtual void SetScrollElement(int32_t pageId, const WeakPtr<ScrollElement>& scrollElement) {}
    virtual void RemovePageId(int32_t pageId) {}
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_MANAGER_INTERFACE_H
