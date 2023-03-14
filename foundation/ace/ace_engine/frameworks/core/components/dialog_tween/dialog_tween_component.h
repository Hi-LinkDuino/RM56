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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_TWEEN_DIALOG_TWEEN_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_TWEEN_DIALOG_TWEEN_COMPONENT_H

#include "core/animation/animator.h"
#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class DialogTweenComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(DialogTweenComponent, SoleChildComponent);

public:
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void SetAutoCancel(bool autoCancel)
    {
        autoCancel_ = autoCancel;
    }

    void SetOnSuccessId(const EventMarker& onSuccessId)
    {
        onSuccessId_ = onSuccessId;
    }

    void SetOnCancelId(const EventMarker& onCancelId)
    {
        onCancelId_ = onCancelId;
    }

    void SetOnCompleteId(const EventMarker& onCompleteId)
    {
        onCompleteId_ = onCompleteId;
    }

    void SetOnPositiveSuccessId(const EventMarker& onPositiveSuccessId)
    {
        onPositiveSuccessId_ = onPositiveSuccessId;
    }

    void SetOnNegativeSuccessId(const EventMarker& onNegativeSuccessId)
    {
        onNegativeSuccessId_ = onNegativeSuccessId;
    }

    void SetOnNeutralSuccessId(const EventMarker& onNeutralSuccessId)
    {
        onNeutralSuccessId_ = onNeutralSuccessId;
    }

    void SetAnimator(const RefPtr<Animator>& animator)
    {
        animator_ = animator;
    }

    void SetParentAnimator(const RefPtr<Animator>& parentAnimator)
    {
        parentAnimator_ = parentAnimator;
    }

    bool GetAutoCancel() const
    {
        return autoCancel_;
    }

    const EventMarker& GetOnSuccessId() const
    {
        return onSuccessId_;
    }

    const EventMarker& GetOnCancelId() const
    {
        return onCancelId_;
    }

    const EventMarker& GetOnCompleteId() const
    {
        return onCompleteId_;
    }

    const EventMarker& GetOnPositiveSuccessId() const
    {
        return onPositiveSuccessId_;
    }

    const EventMarker& GetOnNegativeSuccessId() const
    {
        return onNegativeSuccessId_;
    }

    const EventMarker& GetOnNeutralSuccessId() const
    {
        return onNeutralSuccessId_;
    }

    const RefPtr<Animator>& GetAnimator() const
    {
        return animator_;
    }

    const RefPtr<Animator>& GetParentAnimator() const
    {
        return parentAnimator_;
    }

    void SetComposedId(int32_t composedId)
    {
        composedId_ = composedId;
    }

    int32_t GetComposedId() const
    {
        return composedId_;
    }

    // used for inspector node in PC preview
    void SetCustomDialogId(int32_t dialogId)
    {
        customDialogId_ = dialogId;
    }

    // used for inspector node in PC preview
    int32_t GetCustomDialogId()
    {
        return customDialogId_;
    }

    void SetDialogId(int32_t dialogId)
    {
        dialogId_ = dialogId;
    }

    int32_t GetDialogId()
    {
        return dialogId_;
    }

    void SetData(const std::string& data)
    {
        data_ = data;
    }

    const std::string& GetData() const
    {
        return data_;
    }

    void SetMargin(const Edge& edge)
    {
        margin_ = edge;
        isSetMargin_ = true;
    }

    const Edge& GetMargin() const
    {
        return margin_;
    }

    void SetDialogLimit(bool flag)
    {
        isLimit_ = flag;
    }

    bool GetDialogLimit() const
    {
        return isLimit_;
    }

    bool IsSetMargin() const
    {
        return isSetMargin_;
    }

    void SetDragable(bool dragable)
    {
        isDragable_ = dragable;
    }

    bool IsDragable() const
    {
        return isDragable_;
    }

    void SetIsMenu(bool isMenu)
    {
        isMenu_ = isMenu;
    }

    bool IsMenu() const
    {
        return isMenu_;
    }

    void SetMenuSuccessId(const std::vector<EventMarker>& menuSuccessId)
    {
        menuSuccessId_ = menuSuccessId;
    }

    std::vector<EventMarker>& GetMenuSuccessId()
    {
        return menuSuccessId_;
    }

    void SetOnStatusChanged(const std::function<void(bool)>& onStatusChanged)
    {
        onStatusChanged_ = onStatusChanged;
    }

    const std::function<void(bool)>& GetOnStatusChanged() const
    {
        return onStatusChanged_;
    }

    void SetAlignment(DialogAlignment alignment)
    {
        alignment_ = alignment;
    }

    DialogAlignment GetAlignment() const
    {
        return alignment_;
    }

    void SetOffset(const DimensionOffset& offset)
    {
        offset_ = offset;
    }

    const DimensionOffset& GetOffset() const
    {
        return offset_;
    }

    void SetGridCount(int32_t gridCount)
    {
        gridCount_ = gridCount;
    }

    int32_t GetGridCount() const
    {
        return gridCount_;
    }

private:
    bool autoCancel_ = true;
    RefPtr<Animator> animator_;
    RefPtr<Animator> parentAnimator_;
    EventMarker onSuccessId_;
    EventMarker onCancelId_;
    EventMarker onCompleteId_;
    EventMarker onPositiveSuccessId_;
    EventMarker onNegativeSuccessId_;
    EventMarker onNeutralSuccessId_;
    std::vector<EventMarker> menuSuccessId_;
    int32_t composedId_ = 0;
    // used for inspector node in PC preview
    int32_t customDialogId_ = -1;
    // Used when pop dialog.
    int32_t dialogId_ = -1;
    int32_t gridCount_ = 0;
    std::string data_;
    std::function<void(bool)> onStatusChanged_;

    // Alignment and offset can control the position of dialog.
    DialogAlignment alignment_ = DialogAlignment::DEFAULT;
    DimensionOffset offset_;

    Edge margin_;
    bool isSetMargin_ = false;
    bool isLimit_ = true;
    bool isMenu_ = false;
    bool isDragable_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_TWEEN_DIALOG_TWEEN_COMPONENT_H
