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

#include "core/components/popup/popup_element_v2.h"

#include "base/i18n/localization.h"

namespace OHOS::Ace {

void PopupElementV2::PerformBuild()
{
    PopupElement::PerformBuild();
    auto popupComponentV2 = AceType::DynamicCast<PopupComponentV2>(component_);
    if (!popupComponentV2) {
        return;
    }

    auto themeManager = GetThemeManager();
    if (!themeManager) {
        LOGE("themeManager is null.");
        return;
    }

    popupComponentV2->Initialization(themeManager, context_);
    if (!popupComponentV2->GetOnStateChange().IsEmpty()) {
        onStateChange_ =
            AceAsyncEvent<void(const std::string&)>::Create(popupComponentV2->GetOnStateChange(), context_);
    }

    if (!popupComponentV2->GetChangeEvent().IsEmpty()) {
        changeEvent_ =
            AceAsyncEvent<void(const std::string&)>::Create(popupComponentV2->GetChangeEvent(), context_);
    }

    if (IsDeclarative()) {
        HandleDeclarativePerformBuild();
    }
}

void PopupElementV2::HandleDeclarativePerformBuild()
{
    if (!popup_) {
        LOGE("popup is null.");
        return;
    }

    if (popup_->GetPopupParam()->IsShow()) {
        if (!hasShown_ && ShowPopup()) {
            showId_ = GetId();
            OnStateChange(true);
        }
    } else {
        if (hasShown_ && CancelPopup(showId_)) {
            showId_.clear();
            OnStateChange(false);
        }
    }
}

bool PopupElementV2::IsDeclarative()
{
    auto context = context_.Upgrade();
    if (!context) {
        return false;
    }

    return context->GetIsDeclarative();
}

void PopupElementV2::OnStateChange(bool isVisible)
{
    if (hasShown_ == isVisible) {
        return;
    }
    hasShown_ = isVisible;

    if (changeEvent_ && !isVisible) {
        changeEvent_("false");
        return;
    }

    if (!onStateChange_) {
        return;
    }
    auto json = JsonUtil::Create(true);
    json->Put("isVisible", isVisible);
    onStateChange_(json->ToString());
}

} // namespace OHOS::Ace