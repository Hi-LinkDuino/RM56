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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_COMPONENT_H

#include "core/components/common/properties/border.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/placement.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

using PopupImpl = std::function<void()>;

class PopupParam : public AceType {
    DECLARE_ACE_TYPE(PopupParam, AceType)

public:
    PopupParam() = default;
    ~PopupParam() override = default;

    void SetIsShow(bool isShow)
    {
        isShow_ = isShow;
    }

    bool IsShow() const
    {
        return isShow_;
    }

    void SetHasAction(bool hasAction)
    {
        hasAction_ = hasAction;
    }

    bool HasAction() const
    {
        return hasAction_;
    }

    void SetPlacement(const Placement placement)
    {
        placement_ = placement;
    }

    void SetMaskColor(const Color& maskColor)
    {
        maskColor_ = maskColor;
        isMaskColorSetted_ = true;
    }

    void SetBackgroundColor(const Color& backgroundColor)
    {
        backgroundColor_ = backgroundColor;
        isBackgrouhdColorSetted_ = true;
    }

    void SetOnVisibilityChange(const EventMarker& onVisibilityChange)
    {
        onVisibilityChange_ = onVisibilityChange;
    }

    Placement GetPlacement() const
    {
        return placement_;
    }

    const Color& GetMaskColor() const
    {
        return maskColor_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const EventMarker& GetOnVisibilityChange() const
    {
        return onVisibilityChange_;
    }

    const Edge& GetMargin() const
    {
        return margin_;
    }

    void SetMargin(const Edge& margin)
    {
        margin_ = margin;
    }

    const Edge& GetTargetMargin() const
    {
        return targetMargin_;
    }

    void SetTargetMargin(const Edge& targetMargin)
    {
        targetMargin_ = targetMargin;
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

    void SetPadding(const Edge& padding)
    {
        padding_ = padding;
    }

    const Border& GetBorder() const
    {
        return border_;
    }

    void SetBorder(const Border& border)
    {
        border_ = border;
    }

    const Dimension& GetArrowOffset() const
    {
        return arrowOffset_;
    }

    void SetArrowOffset(const Dimension& arrowOffset)
    {
        arrowOffset_ = arrowOffset;
    }

    const ComposeId& GetTargetId() const
    {
        return targetId_;
    }

    void SetTargetId(const ComposeId& targetId)
    {
        targetId_ = targetId;
    }

    bool IsMaskColorSetted() const
    {
        return isMaskColorSetted_;
    }

    bool IsBackgroundColorSetted() const
    {
        return isBackgrouhdColorSetted_;
    }

    bool EnableArrow() const
    {
        return enableArrow_;
    }

    void SetEnableArrow(bool enableArrow)
    {
        enableArrow_ = enableArrow;
    }

    bool IsUseCustom() const
    {
        return useCustom_;
    }

    void SetUseCustomComponent(bool useCustom)
    {
        useCustom_ = useCustom;
    }

private:
    bool isShow_ = true;
    bool hasAction_ = false;
    bool enableArrow_ = true;
    bool isMaskColorSetted_ = false;
    bool isBackgrouhdColorSetted_ = false;
    bool useCustom_ = false;
    Color maskColor_;
    Color backgroundColor_;
    Placement placement_ = Placement::BOTTOM;
    EventMarker onVisibilityChange_;
    Edge padding_;
    Edge margin_;
    Edge targetMargin_;
    Border border_;
    Dimension arrowOffset_;
    ComposeId targetId_;
};

class PopupController : public virtual AceType {
    DECLARE_ACE_TYPE(PopupController, AceType);

public:
    void ShowPopup()
    {
        if (showPopupImpl_) {
            showPopupImpl_();
        }
    }

    void SetShowPopupImpl(const PopupImpl& showPopupImpl)
    {
        showPopupImpl_ = showPopupImpl;
    }

    void CancelPopup()
    {
        if (cancelPopupImpl_) {
            cancelPopupImpl_();
        }
    }

    void SetCancelPopupImpl(const PopupImpl& cancelPopupImpl)
    {
        cancelPopupImpl_ = cancelPopupImpl;
    }

private:
    PopupImpl showPopupImpl_;
    PopupImpl cancelPopupImpl_;
};

class PopupComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(PopupComponent, ComposedComponent);

public:
    PopupComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : ComposedComponent(id, name, child)
    {
        popupParam_ = MakeRefPtr<PopupParam>();
        popupController_ = MakeRefPtr<PopupController>();
    };
    PopupComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name)
    {
        popupParam_ = MakeRefPtr<PopupParam>();
        popupController_ = MakeRefPtr<PopupController>();
    };
    ~PopupComponent() override = default;

    RefPtr<Element> CreateElement() override;

    const RefPtr<PopupController>& GetPopupController() const
    {
        return popupController_;
    }

    const RefPtr<PopupParam>& GetPopupParam() const
    {
        return popupParam_;
    }

    bool IsShow() const
    {
        if (popupParam_) {
            return popupParam_->IsShow();
        }
        return true;
    }

private:
    RefPtr<PopupParam> popupParam_;
    RefPtr<PopupController> popupController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_COMPONENT_H
