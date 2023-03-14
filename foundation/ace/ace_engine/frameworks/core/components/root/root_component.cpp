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

#include "core/components/root/root_component.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/root/render_root.h"
#include "core/components/root/root_element.h"

namespace OHOS::Ace {

RootComponent::RootComponent(const RefPtr<Component>& child) : child_(child) {}

RefPtr<RenderNode> RootComponent::CreateRenderNode()
{
    LOGI("CreateRenderRoot.");
    return RenderRoot::Create();
}

RefPtr<Element> RootComponent::CreateElement()
{
    return AceType::MakeRefPtr<RootElement>();
}

RefPtr<RootComponent> RootComponent::Create(const RefPtr<Component>& child)
{
    return AceType::MakeRefPtr<RootComponent>(child);
}

RefPtr<RootElement> RootComponent::SetupElementTree(const RefPtr<PipelineContext>& context)
{
    LOGI("First build for element tree.");
    RefPtr<RootElement> rootElement = AceType::DynamicCast<RootElement>(CreateElement());
    rootElement->SetPipelineContext(context);
    rootElement->SetNewComponent(AceType::Claim(this));
    rootElement->Mount(nullptr);
    return rootElement;
}

} // namespace OHOS::Ace
