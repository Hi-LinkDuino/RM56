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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_SLIDING_PANEL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_SLIDING_PANEL_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/panel/panel_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ACE_EXPORT SlidingPanelComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(SlidingPanelComponent, SoleChildComponent);

public:
    static RefPtr<Component> Create(const RefPtr<PanelComponent>& component);

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    PanelMode GetMode() const
    {
        return mode_;
    }

    void SetMode(PanelMode mode)
    {
        mode_ = mode;
    }

    void SetHasBoxStyle(bool hasBoxStyle)
    {
        hasBoxStyle_ = hasBoxStyle;
    }

    bool HasBoxStyle() const
    {
        return hasBoxStyle_;
    }

    const EventMarker& GetOnSizeChanged() const
    {
        return onSizeChanged_;
    }

    void SetOnSizeChanged(const EventMarker& value)
    {
        onSizeChanged_ = value;
    }

    const std::pair<Dimension, bool>& GetMiniHeight() const
    {
        return miniHeight_;
    }

    void SetMiniHeight(const std::pair<Dimension, bool>& miniHeight)
    {
        miniHeight_ = miniHeight;
    }

    const std::pair<Dimension, bool>& GetHalfHeight() const
    {
        return halfHeight_;
    }

    void SetHalfHeight(const std::pair<Dimension, bool>& halfHeight)
    {
        halfHeight_ = halfHeight;
    }

    const std::pair<Dimension, bool>& GetFullHeight() const
    {
        return fullHeight_;
    }

    void SetFullHeight(const std::pair<Dimension, bool>& fullHeight)
    {
        fullHeight_ = fullHeight;
    }

    PanelType GetType() const
    {
        return type_;
    }

    void SetType(PanelType type)
    {
        type_ = type;
    }

    // used for inspector node in PC preview
    int32_t GetPanelId() const
    {
        return panelId_;
    }

    void SetPanelId(int32_t panelId)
    {
        panelId_ = panelId;
    }

    void SetHasDragBar(bool hasDrag)
    {
        hasDragBar_ = hasDrag;
    }

    bool HasDragBar() const
    {
        return hasDragBar_;
    }

protected:
    void BuildInnerChild(const RefPtr<BoxComponent>& boxStyle, const RefPtr<PanelComponent>& panel);

    PanelMode mode_ = PanelMode::HALF;
    PanelType type_ = PanelType::FOLDABLE_BAR;
    bool hasBoxStyle_ = false;
    // used for inspector node in PC preview
    int32_t panelId_ = -1;
    EventMarker onSizeChanged_;
    bool hasDragBar_ = false;
    std::pair<Dimension, bool> miniHeight_ = { 0.0_vp, false };
    std::pair<Dimension, bool> halfHeight_ = { 0.0_vp, false };
    std::pair<Dimension, bool> fullHeight_ = { 0.0_vp, false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_SLIDING_PANEL_COMPONENT_H
