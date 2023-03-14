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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUSABLE_FOCUSABLE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUSABLE_FOCUSABLE_COMPONENT_H

#include <string>

#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/declaration/common/controllers.h"
#include "core/components/focusable/focusable_element.h"
#include "core/event/ace_event_handler.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ACE_EXPORT FocusableComponent final : public SoleChildComponent {
    DECLARE_ACE_TYPE(FocusableComponent, SoleChildComponent);

public:
    FocusableComponent()
    {
        focusableController_ = AceType::MakeRefPtr<FocusableController>();
    }
    explicit FocusableComponent(const RefPtr<Component>& child) : SoleChildComponent(child)
    {
        focusableController_ = AceType::MakeRefPtr<FocusableController>();
    }
    ~FocusableComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return MakeRefPtr<FocusableElement>();
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderBox::Create();
    }

    const EventMarker& GetOnClickId() const
    {
        return onClickId_;
    }
    void SetOnClickId(const EventMarker& onClickId)
    {
        onClickId_ = onClickId;
    }

    const EventMarker& GetOnFocusId() const
    {
        return onFocusId_;
    }
    void SetOnFocusId(const EventMarker& onFocusId)
    {
        onFocusId_ = onFocusId;
    }

    const EventMarker& GetOnBlurId() const
    {
        return onBlurId_;
    }
    void SetOnBlurId(const EventMarker& onBlurId)
    {
        onBlurId_ = onBlurId;
    }

    std::function<void(int)> GetOnFocusMove() const
    {
        return onFocusMove_;
    }
    void SetOnFocusMove(const std::function<void(int)>& onFocusMove)
    {
        onFocusMove_ = onFocusMove;
    }

    std::function<void()> GetOnFocus() const
    {
        return onFocus_;
    }
    void SetOnFocus(const std::function<void()>& onFocus)
    {
        onFocus_ = onFocus;
    }

    std::function<void()> GetOnBlur() const
    {
        return onBlur_;
    }
    void SetOnBlur(const std::function<void()>& onBlur)
    {
        onBlur_ = onBlur;
    }

    const EventMarker& GetOnKeyId() const
    {
        return onKeyId_;
    }
    void SetOnKeyId(const EventMarker& onKeyId)
    {
        onKeyId_ = onKeyId;
    }

    const EventMarker& GetOnDeleteId() const
    {
        return onDeleteId_;
    }
    void SetOnDeleteId(const EventMarker& onDeleteId)
    {
        onDeleteId_ = onDeleteId;
    }

    RefPtr<BoxComponent> GetBoxStyle() const
    {
        return boxStyle_;
    }
    void SetBoxStyle(const RefPtr<BoxComponent>& boxStyle)
    {
        boxStyle_ = boxStyle;
    }

    RefPtr<BoxComponent> GetFocusedBoxStyle() const
    {
        return focusedBoxStyle_;
    }
    void SetFocusedBoxStyle(const RefPtr<BoxComponent>& boxStyle)
    {
        focusedBoxStyle_ = boxStyle;
    }

    bool GetAutoFocused() const
    {
        return autoFocused_;
    }
    void SetAutoFocused(bool autoFocused)
    {
        autoFocused_ = autoFocused;
    }
    bool IsFocusNode() const
    {
        return focusNode_;
    }
    void SetFocusNode(bool focusNode)
    {
        focusNode_ = focusNode;
    }

    bool IsFocusable() const
    {
        return focusable_;
    }
    void SetFocusable(bool focusable)
    {
        focusable_ = focusable;
    }

    bool CanShow() const
    {
        return show_;
    }
    void SetShow(bool show)
    {
        show_ = show;
    }

    RefPtr<FocusableController> GetFocusableController() const
    {
        return focusableController_;
    }

    bool IsDeleteDisabled() {
        return deleteDisabled_;
    }
    void SetDeleteDisabled(bool deleteDisabled) {
        deleteDisabled_ = deleteDisabled;
    }

private:
    EventMarker onClickId_;
    EventMarker onFocusId_;
    EventMarker onBlurId_;
    EventMarker onKeyId_;
    EventMarker onDeleteId_;

    RefPtr<BoxComponent> boxStyle_;
    RefPtr<BoxComponent> focusedBoxStyle_;
    RefPtr<FocusableController> focusableController_;

    bool autoFocused_ { false };
    bool focusNode_ { false };
    bool focusable_ { false };
    bool show_ { true };
    bool deleteDisabled_ { false };

    std::function<void(int)> onFocusMove_;
    std::function<void()> onFocus_;
    std::function<void()> onBlur_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUSABLE_FOCUSABLE_COMPONENT_H
