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

#include "core/components/rich_text/rich_text_element.h"

#include "core/components/rich_text/rich_text_component.h"

namespace OHOS::Ace {

RichTextElement::~RichTextElement()
{
    if (delegate_) {
        delegate_->ReleasePlatformResource();
    }
}

void RichTextElement::Prepare(const WeakPtr<Element>& parent)
{
    RenderElement::Prepare(parent);

    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    pipelineContext->AddPageTransitionListener([weak = AceType::WeakClaim(this)]
        (const TransitionEvent& event, const WeakPtr<PageElement>& in, const WeakPtr<PageElement>& out) {
        if (event == TransitionEvent::POP_START) {
            LOGE("Handle pop transition event failed. page out is null.");
            auto element = weak.Upgrade();
            if (element) {
                if (element->GetDelegate()) {
                    element->GetDelegate()->HideRichText();
                }
            }
        }
    });
}

void RichTextElement::SetNewComponent(const RefPtr<Component>& newComponent)
{
    if (newComponent == nullptr) {
        Element::SetNewComponent(newComponent);
        return;
    }
    auto webComponent = AceType::DynamicCast<RichTextComponent>(newComponent);
    if (webComponent) {
        if (richTextData_.compare(webComponent->GetData()) != 0) {
            richTextData_.assign(webComponent->GetData());
            if (delegate_) {
                delegate_->UpdateRichTextData(richTextData_);
            }
        }
        Element::SetNewComponent(webComponent);
    }
}

} // namespace OHOS::Ace
