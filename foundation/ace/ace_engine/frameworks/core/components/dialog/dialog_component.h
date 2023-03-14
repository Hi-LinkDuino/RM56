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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_DIALOG_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_DIALOG_COMPONENT_H

#include "base/utils/device_type.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/dialog/dialog_properties.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components/dialog_tween/dialog_tween_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/focus_animation/focus_animation_component.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/components/transition/transition_component.h"
#include "core/components/tween/tween_component.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

// constants of dialog component
extern const char CALLBACK_SUCCESS[];
extern const char CALLBACK_CANCEL[];
extern const char CALLBACK_COMPLETE[];
extern const char DIALOG_TWEEN_NAME[];

class DialogComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(DialogComponent, SoleChildComponent);

public:
    DialogComponent();
    ~DialogComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;
    virtual void BuildChild(const RefPtr<ThemeManager>& themeManager);

    void SetTitle(const RefPtr<TextComponent>& title)
    {
        title_ = title;
    }

    void SetTitlePadding(const Edge& titlePadding)
    {
        titlePadding_ = titlePadding;
    }

    void SetContent(const RefPtr<TextComponent>& content)
    {
        content_ = content;
    }

    void SetContentPadding(const Edge& contentPadding)
    {
        contentPadding_ = contentPadding;
    }

    void SetBackgroundColor(const Color& backgroundColor)
    {
        backgroundColor_ = backgroundColor;
    }

    void SetActions(const std::list<RefPtr<ButtonComponent>>& actions)
    {
        actions_ = actions;
    }

    void SetAutoCancel(bool autoCancel)
    {
        autoCancel_ = autoCancel;
    }

    void SetAnimator(const RefPtr<Animator>& animator)
    {
        animator_ = animator;
    }

    void SetDialogTween(const RefPtr<DialogTweenComponent>& dialogTween)
    {
        dialogTween_ = dialogTween;
    }

    void SetDialogTweenBox(const RefPtr<BoxComponent>& dialogTweenBox)
    {
        dialogTweenBox_ = dialogTweenBox;
    }

    void SetOnSuccessId(const EventMarker& onSuccessId)
    {
        // Before update event marker, remove old marker from manager.
        BackEndEventManager<void(int32_t)>::GetInstance().RemoveBackEndEvent(onSuccessId_);
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

    const RefPtr<TextComponent>& GetTitle() const
    {
        return title_;
    }

    const Edge& GetTitlePadding() const
    {
        return titlePadding_;
    }

    const RefPtr<TextComponent>& GetContent() const
    {
        return content_;
    }

    const Edge& GetContentPadding() const
    {
        return contentPadding_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const std::list<RefPtr<ButtonComponent>>& GetActions() const
    {
        return actions_;
    }

    bool GetAutoCancel() const
    {
        return autoCancel_;
    }

    std::vector<EventMarker>& GetMenuSuccessId()
    {
        return menuSuccessId_;
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

    const RefPtr<DialogTweenComponent>& GetDialogTweenComponent() const
    {
        return dialogTween_;
    }

    const RefPtr<BoxComponent>& GetDialogTweenBox() const
    {
        return dialogTweenBox_;
    }

    bool HasCustomChild() const
    {
        return customComponent_ != nullptr;
    }

    void SetCustomChild(const RefPtr<Component>& customChild)
    {
        customComponent_ = customChild;
    }

    int32_t GetDialogId() const
    {
        return dialogId_;
    }

    void SetDialogTweenComposedId(int32_t dialogTweenComposeId)
    {
        dialogTweenComposedId_ = dialogTweenComposeId;
    }

    void SetContext(const WeakPtr<PipelineContext>& context)
    {
        context_ = context;
    }

    void SetData(const std::string& data)
    {
        data_ = data;
    }

    // This method is design for unit test, please call this method seriously.
    void SetDeviceType(DeviceType deviceType)
    {
        deviceType_ = deviceType;
        isDeviceTypeSet_ = true;
    }

    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    void SetMargin(const Edge& margin)
    {
        margin_ = margin;
        isSetMargin_ = true;
    }

    const Edge& GetMargin() const
    {
        return margin_;
    }

    void SetIsDragable(bool isDragable)
    {
        dragable_ = isDragable;
    }

    bool IsDragable() const
    {
        return dragable_;
    }

    bool IsSetMargin() const
    {
        return isSetMargin_;
    }

    // used for inspector node in PC preview
    void SetCustomDialogId(int32_t dialogId)
    {
        customDialogId_ = dialogId;
    }

    void SetIsMenu(bool isMenu)
    {
        isMenu_ = isMenu;
    }

    void SetDialogProperties(const DialogProperties& properties)
    {
        properties_ = properties;
    }

    const DialogProperties& GetDialogProperties() const
    {
        return properties_;
    }

    bool HasStopListenerAdded() const
    {
        return hasStopListenerAdded_;
    }

    void SetHasStopListenerAdded(bool hasStopListenerAdded)
    {
        hasStopListenerAdded_ = hasStopListenerAdded;
    }

protected:
    static int32_t GenerateDialogId();

    RefPtr<BoxComponent> BuildBox(bool& isLimit);
    void BuildFocusChild(const RefPtr<Component>& child, const RefPtr<FocusCollaborationComponent>& collaboration);
    void BuildTitle(const RefPtr<ColumnComponent>& column);
    virtual void BuildContent(const RefPtr<ColumnComponent>& column);
    virtual void BuildActions(const RefPtr<ThemeManager>& themeManager, const RefPtr<ColumnComponent>& column);
    void BuildActionsForWatch(const RefPtr<ColumnComponent>& column);
    virtual void BuildMenu(const RefPtr<ColumnComponent>& column);
    RefPtr<Component> BuildButton(const RefPtr<ButtonComponent>& button, const EventMarker& callbackId,
        const Edge& edge, bool isAutoFocus = false);
    RefPtr<TransitionComponent> BuildAnimation(const RefPtr<BoxComponent>& child);
    RefPtr<TransitionComponent> BuildAnimationForPhone(const RefPtr<Component>& child);
    RefPtr<Component> GenerateComposed(
        const std::string& name, const RefPtr<Component>& child, bool isDialogTweenChild);
    void BuildDialogTween(const RefPtr<TransitionComponent>& transition, bool isLimit, Edge margin);
    RefPtr<Component> BuildDivider(const RefPtr<ThemeManager>& themeManager);
    void BuildDragBar(const RefPtr<FocusCollaborationComponent>& collaboration);

    std::string data_; // date is used for accessibility.
    int32_t dialogId_ = -1;
    int32_t dialogTweenComposedId_ = 0;
    RefPtr<TextComponent> title_;
    Edge titlePadding_;
    RefPtr<TextComponent> content_;
    Edge contentPadding_;
    bool autoCancel_ = true;
    Color backgroundColor_;
    std::list<RefPtr<ButtonComponent>> actions_;
    RefPtr<DialogTheme> dialogTheme_;
    RefPtr<DialogTweenComponent> dialogTween_;
    RefPtr<BoxComponent> dialogTweenBox_;
    RefPtr<Animator> animator_;
    EventMarker onSuccessId_;
    EventMarker onCancelId_;
    EventMarker onCompleteId_;
    EventMarker onPositiveSuccessId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker onNegativeSuccessId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    EventMarker onNeutralSuccessId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
    std::vector<EventMarker> menuSuccessId_;
    WeakPtr<PipelineContext> context_;
    DeviceType deviceType_ = DeviceType::PHONE;
    bool isDeviceTypeSet_ = false;
    bool isMenu_ = false;
    bool dragable_ = false;
    DialogProperties properties_;

    RefPtr<Component> customComponent_;
    Dimension height_;
    Dimension width_;
    Edge margin_;
    bool isSetMargin_ = false;
    bool hasStopListenerAdded_ = false;
    // used for inspector node in PC preview
    int32_t customDialogId_ = -1;
};

class DialogBuilder {
public:
    static RefPtr<DialogComponent> Build(
        const DialogProperties& dialogProperties, const WeakPtr<PipelineContext>& context);

private:
    static RefPtr<DialogComponent> BuildDialogWithType(DialogType type);
    static void BuildTitleAndContent(const RefPtr<DialogComponent>& dialog, const DialogProperties& dialogProperties,
        const RefPtr<DialogTheme>& dialogTheme, std::string& data);
    static void BuildButtons(const RefPtr<ThemeManager>& themeManager, const RefPtr<DialogComponent>& dialog,
        const std::vector<ButtonInfo>& buttons, const RefPtr<DialogTheme>& dialogTheme,
        std::string& data);
    static void BuildButtonsForWatch(
        const RefPtr<ThemeManager>& themeManager, const RefPtr<DialogComponent>& dialog, std::string& data);
    static RefPtr<DialogComponent> BuildAnimation(
        const RefPtr<DialogComponent>& dialogChild, const RefPtr<DialogTheme>& dialogTheme);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_DIALOG_COMPONENT_H
