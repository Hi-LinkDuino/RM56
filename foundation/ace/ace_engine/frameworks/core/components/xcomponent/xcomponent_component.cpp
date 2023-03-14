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

#include "core/components/xcomponent/xcomponent_component.h"

#include <iomanip>
#include <sstream>

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/log/log.h"
#include "core/components/xcomponent/render_xcomponent.h"
#include "core/components/xcomponent/resource/xcomponent_delegate.h"
#include "core/components/xcomponent/xcomponent_element.h"

namespace OHOS::Ace {
XComponentComponent::XComponentComponent(const std::string& type) : type_(type)
{
    ACE_DCHECK(!type_.empty());
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<XComponentDeclaration>();
        declaration_->Init();
    }
    pool_ = AceType::MakeRefPtr<XComponentTaskPool>();
    pool_->component_ = AceType::WeakClaim(this);
    xcomponentController_ = AceType::MakeRefPtr<XComponentController>();
}

RefPtr<RenderNode> XComponentComponent::CreateRenderNode()
{
    RefPtr<RenderNode> renderNode = RenderXComponent::Create();
    delegate_ = AceType::MakeRefPtr<XComponentDelegate>(AceType::WeakClaim<XComponentComponent>(this),
                                                 renderNode->GetContext(),
                                                 std::move(errorCallback_), type_);
    if (createdCallback_ != nullptr) {
        delegate_->AddCreatedCallback(createdCallback_);
    }
    auto renderXComponent = AceType::DynamicCast<RenderXComponent>(renderNode);
    delegate_->AddCreatedCallback([renderXComponent, this]() {
        if (!renderXComponent) {
            LOGE("renderXComponent is null");
            return;
        }
        auto pipelineContext = renderXComponent->GetContext().Upgrade();
        if (!pipelineContext) {
            LOGE("fail to create Update due to context is null");
            return;
        }
        auto uiTaskExecutor = SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(),
                                                       TaskExecutor::TaskType::UI);
        uiTaskExecutor.PostTask([renderXComponent, this] {
            if (renderXComponent) {
                renderXComponent->Update(AceType::Claim<Component>(this));
            }
        });
    });
    renderXComponent->SetDelegate(delegate_);
    return renderNode;
}

RefPtr<Element> XComponentComponent::CreateElement()
{
    return AceType::MakeRefPtr<XComponentElement>();
}

void XComponentTaskPool::SetRenderNode(const WeakPtr<RenderXComponent>& renderNode)
{
    renderNode_ = renderNode;
}

void XComponentTaskPool::NativeXComponentInit(
    OH_NativeXComponent* nativeXComponent,
    WeakPtr<NativeXComponentImpl> nativeXComponentImpl)
{
    auto renderNode = renderNode_.Upgrade();
    if (!renderNode) {
        LOGE("XComponentTaskPool::NativeXComponentInit invalid renderNode");
        return;
    }
    renderNode->NativeXComponentInit(nativeXComponent, nativeXComponentImpl);
}
} // namespace OHOS::Ace
