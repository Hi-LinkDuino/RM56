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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_STACK_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_STACK_ELEMENT_H

#include "core/pipeline/base/component_group_element.h"

namespace OHOS::Ace {

class ACE_EXPORT StackElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(StackElement, ComponentGroupElement, FocusGroup);

    enum class Operation {
        NONE,
        DIRECT_PUSH,
        TOAST_PUSH,
        TOAST_POP,
        DIALOG_PUSH,
        DIALOG_POP,
        TEXT_OVERLAY_POP,
        POPUP_POP,
        PANEL_PUSH,
        PANEL_POP,
        DIRECT_POP,
        MENU_POP,
    };

    struct ToastInfo {
        int32_t toastId = -1;
        RefPtr<Element> child;
    };

    struct PopupComponentInfo {
        int32_t popId = -1;
        ComposeId id = "-1";
        Operation operation = Operation::NONE;
        RefPtr<Component> component;
        bool IsValid()
        {
            return component != nullptr;
        }
    };

public:
    void PerformBuild() override;
    void PushComponent(const RefPtr<Component>& newComponent, bool disableTouchEvent = true);
    void PopComponent();
    void PushToastComponent(const RefPtr<Component>& newComponent, int32_t toastId);
    void PopToastComponent(int32_t toastPopId);
    void PushPanel(const RefPtr<Component>& newComponent, bool disableTouchEvent = false);
    void PopPanel();
    bool PushDialog(const RefPtr<Component>& newComponent, bool disableTouchEvent = true);
    bool PopDialog(int32_t id = -1);
    void PopTextOverlay();
    void PopPopup(const ComposeId& id);
    void PopMenu();
    void PopInstant();
    void PushInstant(const RefPtr<Component>& newComponent, bool disableTouchEvent = true);
    virtual bool isPageElement()
    {
        return false;
    }

protected:
    void OnFocus() override;
    void OnBlur() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

private:
    void PerformPopupChild(PopupComponentInfo& popupComponentInfo);
    void PerformPushChild(PopupComponentInfo& popupComponentInfo);
    void PerformPushToast(PopupComponentInfo& popupComponentInfo);
    void PerformPopToastById(int32_t toastId);
    void PerformPopToast();
    void PerformPopDialog(int32_t id = -1);
    void PerformPopDialogById(int32_t id);
    void PerformPopTextOverlay();
    void PerformPopPopup(const ComposeId& id);
    void PerformPopMenu();
    void PerformDirectPop();
    void PerformPopupChild();
    void EnableTouchEventAndRequestFocus();

    void CreateInspectorComponent(PopupComponentInfo& componentInfo) const;

    std::list<PopupComponentInfo> popupComponentInfos_;
    std::vector<ToastInfo> toastStack_;
    bool disableTouchEvent_ = true;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STACK_STACK_ELEMENT_H
