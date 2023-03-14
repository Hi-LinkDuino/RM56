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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_POSITIONED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_POSITIONED_ELEMENT_H

#include "base/log/log.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class PositionedElement : public SoleChildElement, public FocusGroup {
    DECLARE_ACE_TYPE(PositionedElement, SoleChildElement, FocusGroup);

public:
    bool GetIsAutoFocus() const
    {
        return isAutoFocus_;
    }

    void SetIsAutoFocus(bool isAutoFocus)
    {
        isAutoFocus_ = isAutoFocus;
    }

    void PerformBuild() override
    {
        SoleChildElement::PerformBuild();
        auto focusScope = AceType::DynamicCast<PositionedElement>(this);
        if (!focusScope) {
            LOGE("PositionedElement can not DynamicCast to FocusGroup");
            return;
        }

        if (focusScope->GetIsAutoFocus()) {
            RequestFocusImmediately();
        }
    }

protected:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override
    {
        return false;
    }

private:
    bool isAutoFocus_ { false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POSITIONED_POSITIONED_ELEMENT_H
