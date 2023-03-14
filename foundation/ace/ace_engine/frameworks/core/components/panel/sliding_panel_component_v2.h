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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_SLIDING_PANEL_COMPONENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_SLIDING_PANEL_COMPONENT_V2_H

#include "core/components/panel/sliding_panel_component.h"

namespace OHOS::Ace {

class ACE_EXPORT SlidingPanelComponentV2 : public SlidingPanelComponent {
    DECLARE_ACE_TYPE(SlidingPanelComponentV2, SlidingPanelComponent);

public:
    void OnChildAdded(const RefPtr<Component>& child) override;

    const RefPtr<BoxComponent>& GetBoxStyle() const
    {
        return boxStyle_;
    }

    void SetBoxStyle(const RefPtr<BoxComponent>& boxStyle)
    {
        boxStyle_ = boxStyle;
    }

    bool HasDecorationStyle() const
    {
        return hasDecorationStyle_;
    }

    void SetHasDecorationStyle(bool hasDecorationStyle)
    {
        hasDecorationStyle_ = hasDecorationStyle;
    }

    bool HasBorderStyle() const
    {
        return hasBorderStyle_;
    }

    void SetHasBorderStyle(bool hasBorderStyle)
    {
        hasBorderStyle_ = hasBorderStyle;
    }

    bool HasBgStyle() const
    {
        return hasBgStyle_;
    }

    void SetHasBgStyle(bool hasBgStyle)
    {
        hasBgStyle_ = hasBgStyle;
    }

protected:
    void BuildInnerChild();

    RefPtr<BoxComponent> boxStyle_;
    bool hasDecorationStyle_ = false;
    bool hasBorderStyle_ = false;
    bool hasBgStyle_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_SLIDING_PANEL_COMPONENT_V2_H
