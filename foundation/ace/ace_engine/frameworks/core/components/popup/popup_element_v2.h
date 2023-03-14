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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_ELEMENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_ELEMENT_V2_H

#include "core/components/popup/popup_component_v2.h"
#include "core/components/popup/popup_element.h"

namespace OHOS::Ace {

class PopupElementV2 : public PopupElement {
    DECLARE_ACE_TYPE(PopupElementV2, PopupElement);

public:
    explicit PopupElementV2(const ComposeId& id) : PopupElement(id) {}
    ~PopupElementV2() override = default;

    void PerformBuild() override;
    bool IsDeclarative();
    void OnStateChange(bool isVisible) override;

    std::string GetMessage() const
    {
        auto popupComponentV2 = AceType::DynamicCast<PopupComponentV2>(popup_);
        if (popupComponentV2) {
            return popupComponentV2->GetMessage();
        }
        return "";
    }

    bool GetPlacementOnTop()
    {
        auto popupComponentV2 = AceType::DynamicCast<PopupComponentV2>(popup_);
        if (popupComponentV2) {
            return popupComponentV2->GetPlacementOnTop();
        }
        return false;
    }

    std::string GetPrimaryButtonValue() const
    {
        auto popupComponentV2 = AceType::DynamicCast<PopupComponentV2>(popup_);
        if (popupComponentV2) {
            return popupComponentV2->GetPrimaryButtonValue();
        }
        return "";
    }

    std::string GetSecondaryButtonValue() const
    {
        auto popupComponentV2 = AceType::DynamicCast<PopupComponentV2>(popup_);
        if (popupComponentV2) {
            return popupComponentV2->GetSecondaryButtonValue();
        }
        return "";
    }

    bool CanUpdate(const RefPtr<Component>& newComponent) override
    {
        return Element::CanUpdate(newComponent);
    }

private:
    void HandleDeclarativePerformBuild();
    bool hasShown_ = false;
    ComposeId showId_;
    std::function<void(const std::string&)> onStateChange_;
    std::function<void(const std::string&)> changeEvent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_POPUP_POPUP_ELEMENT_V2_H
