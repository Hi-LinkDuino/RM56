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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_PANEL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_PANEL_COMPONENT_H

#include <functional>

#include "core/components/box/box_component.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

using BuildPanelImpl = std::function<void()>;

// PanelController is used in PanelElement to build a panel
class PanelController : public virtual AceType {
    DECLARE_ACE_TYPE(PanelController, AceType);

public:
    void ShowPanel()
    {
        if (showPanelImpl_) {
            showPanelImpl_();
        }
    }

    void ClosePanel()
    {
        if (closePanelImpl_) {
            closePanelImpl_();
        }
    }

    void SetShowPanelImpl(const BuildPanelImpl& showPanelImpl)
    {
        showPanelImpl_ = showPanelImpl;
    }

    void SetClosePanelImpl(const BuildPanelImpl& closePanelImpl)
    {
        closePanelImpl_ = closePanelImpl;
    }

private:
    BuildPanelImpl showPanelImpl_;
    BuildPanelImpl closePanelImpl_;
};

class ACE_EXPORT PanelComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(PanelComponent, ComposedComponent);

public:
    PanelComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(id, name, child)
    {
        panelController_ = AceType::MakeRefPtr<PanelController>();
    }

    PanelComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name)
    {
        panelController_ = AceType::MakeRefPtr<PanelController>();
    }

    ~PanelComponent() override = default;

    RefPtr<Element> CreateElement() override;

    const RefPtr<PanelController>& GetPanelController() const
    {
        return panelController_;
    }

    const EventMarker& GetOnSizeChanged() const
    {
        return onSizeChanged_;
    }

    void SetOnSizeChanged(const EventMarker& value)
    {
        onSizeChanged_ = value;
    }

    PanelMode GetPanelMode() const
    {
        return mode_;
    }

    void SetPanelMode(PanelMode mode)
    {
        mode_ = mode;
    }

    PanelType GetPanelType() const
    {
        return type_;
    }

    void SetPanelType(PanelType type)
    {
        type_ = type;
    }

    void SetBoxStyle(const RefPtr<BoxComponent>& style)
    {
        boxStyle_ = style;
    }

    RefPtr<BoxComponent> GetBoxStyle() const
    {
        return boxStyle_;
    }

    bool HasBoxStyle() const
    {
        return hasBoxStyle_;
    }

    void SetHasBoxStyle(bool hasBoxStyle)
    {
        hasBoxStyle_ = hasBoxStyle;
    }

    bool HasDecorationStyle() const
    {
        return hasDecorationStyle_;
    }

    void SetHasDecorationStyle(bool hasDecorationStyle)
    {
        hasDecorationStyle_ = hasDecorationStyle;
    }

    bool HasDragBar() const
    {
        return hasDragBar_;
    }

    void SetHasDragBar(bool hasDragBar)
    {
        hasDragBar_ = hasDragBar;
    }

    bool HasBackgroundColor() const
    {
        return hasBackgroundColor_;
    }

    void SetHasBackgroundColor(bool hasBackground)
    {
        hasBackgroundColor_ = hasBackground;
    }

    bool HasBorderStyle() const
    {
        return hasBorderStyle_;
    }

    void SetHasBorderStyle(bool borderStyle)
    {
        hasBorderStyle_ = borderStyle;
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

private:
    PanelType type_ = PanelType::FOLDABLE_BAR;
    PanelMode mode_ = PanelMode::FULL;
    RefPtr<PanelController> panelController_;
    EventMarker onSizeChanged_;

    RefPtr<BoxComponent> boxStyle_;
    bool hasBoxStyle_ = false;
    bool hasDecorationStyle_ = false;
    bool hasDragBar_ = true;
    bool hasBackgroundColor_ = false;
    bool hasBorderStyle_ = false;
    std::pair<Dimension, bool> miniHeight_ = { 0.0_vp, false };
    std::pair<Dimension, bool> halfHeight_ = { 0.0_vp, false };
    std::pair<Dimension, bool> fullHeight_ = { 0.0_vp, false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PANEL_PANEL_COMPONENT_H
