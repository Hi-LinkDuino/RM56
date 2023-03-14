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

#include "core/components/web/web_element.h"

#include "base/log/log.h"
#include "core/components/web/web_component.h"

namespace OHOS::Ace {

void WebElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (newComponent == nullptr) {
        Element::SetNewComponent(newComponent);
        return;
    }
    auto webComponent = AceType::DynamicCast<WebComponent>(newComponent);
    if (webComponent) {
        if (!webSrc_.empty() && webSrc_ != webComponent->GetSrc()) {
            WebClient::GetInstance().UpdateWebviewUrl(webComponent->GetSrc());
        }
        webSrc_ = webComponent->GetSrc();
        Element::SetNewComponent(webComponent);
        webComp_ = std::move(webComponent);
    }
}

void WebElement::Update()
{
    RenderElement::Update();
    if (webComp_) {
        webComp_->SetFocusElement(AceType::WeakClaim(this));
    }
}

void WebElement::OnFocus()
{
    FocusNode::OnFocus();
    LOGI("web element onfocus");
    if (webComp_) {
        auto controller = webComp_->GetController();
        if (controller) {
            controller->OnFocus();
        }
    }
}

} // namespace OHOS::Ace
