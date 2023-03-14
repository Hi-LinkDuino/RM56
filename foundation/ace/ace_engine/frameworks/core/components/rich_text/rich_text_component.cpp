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

#include "core/components/rich_text/rich_text_component.h"

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/log/log.h"
#include "core/components/rich_text/render_rich_text.h"
#include "core/components/rich_text/resource/rich_text_delegate.h"
#include "core/components/rich_text/rich_text_element.h"

#include <iomanip>
#include <sstream>

namespace OHOS::Ace {

RichTextComponent::RichTextComponent(const std::string& type) : type_(type)
{
    ACE_DCHECK(!type_.empty());
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<RichTextDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> RichTextComponent::CreateRenderNode()
{
    RefPtr<RenderNode> renderNode = RenderRichText::Create();
    if (!delegate_) {
        delegate_ = AceType::MakeRefPtr<RichTextDelegate>(AceType::WeakClaim<RichTextComponent>(this),
                                                 renderNode->GetContext(),
                                                 std::move(errorCallback_), type_);
    }
    auto renderRichText = AceType::DynamicCast<RenderRichText>(renderNode);

    delegate_->AddWebLayoutChangeCallback(
        [renderRichText, weak = WeakClaim(this)](int32_t width, int32_t height, int32_t contentHeight) {
            if (!renderRichText) {
                LOGE("renderRichText is null");
                return;
            }
            auto pipelineContext = renderRichText->GetContext().Upgrade();
            if (!pipelineContext) {
                LOGE("fail to create Update due to context is null");
                return;
            }
            auto uiTaskExecutor = SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(),
                TaskExecutor::TaskType::UI);
            auto weakRender = AceType::WeakClaim(AceType::RawPtr(renderRichText));
            uiTaskExecutor.PostTask([weakRender, width, height, contentHeight] {
                auto renderRichText = weakRender.Upgrade();
                if (renderRichText) {
                    renderRichText->UpdateLayoutParams(width, height, contentHeight);
                }
            });
    });

    renderRichText->SetDelegate(delegate_);
    return renderNode;
}

RefPtr<Element> RichTextComponent::CreateElement()
{
    RefPtr<RichTextElement> elementNode = AceType::MakeRefPtr<RichTextElement>();
    if (!delegate_) {
        delegate_ = AceType::MakeRefPtr<RichTextDelegate>(AceType::WeakClaim<RichTextComponent>(this),
                                                     elementNode->GetContext(),
                                                     std::move(errorCallback_), type_);
    }
    elementNode->SetDelegate(delegate_);
    return elementNode;
}



} // namespace OHOS::Ace
