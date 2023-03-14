/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_SUBWINDOW_OHOS_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_SUBWINDOW_OHOS_H

#include "wm/window.h"

#include "base/subwindow/subwindow.h"
#include "base/subwindow/subwindow_manager.h"
#include "core/components/select_popup/select_popup_component.h"
#include "core/components/stack/stack_element.h"

namespace OHOS::Rosen {
class Window;
class WindowOption;
class RSUIDirector;
} // namespace OHOS::Rosen

namespace OHOS::Ace {

class SubwindowOhos : public Subwindow {
    DECLARE_ACE_TYPE(SubwindowOhos, Subwindow)

public:
    explicit SubwindowOhos(int32_t instanceId);
    ~SubwindowOhos() = default;

    void InitContainer() override;
    void ShowMenu(const RefPtr<Component>& newComponent) override;
    void CloseMenu() override;
    void ClearMenu() override;

private:
    RefPtr<StackElement> GetStack();
    void AddMenu(const RefPtr<Component>& newComponent);
    void ShowWindow();
    void HideWindow();

    static int32_t id_;
    int32_t windowId_ = 0;
    int32_t parentContainerId_ = -1;
    int32_t childContainerId_ = -1;
    std::shared_ptr<OHOS::Rosen::RSUIDirector> rsUiDirector;
    sptr<OHOS::Rosen::Window> window_ = nullptr;
    RefPtr<SelectPopupComponent> popup_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_SUBWINDOW_OHOS_H