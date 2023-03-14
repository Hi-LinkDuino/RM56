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

#include "core/components/tab_bar/tab_content_component.h"

#include "core/components/tab_bar/render_tab_content.h"
#include "core/components/tab_bar/tab_content_element.h"
#include "core/components_v2/tabs/tab_content_proxy_element.h"

namespace OHOS::Ace {

TabContentComponent::TabContentComponent(
    const std::list<RefPtr<Component>>& contents, const RefPtr<TabController>& controller)
    : ComponentGroup(contents)
{
    controller_ = controller;
}

RefPtr<Element> TabContentComponent::CreateElement()
{
    if (useProxy_) {
        return AceType::MakeRefPtr<V2::TabContentProxyElement>(GetChildren());
    }
    return AceType::MakeRefPtr<TabContentElement>(GetChildren());
}

RefPtr<RenderNode> TabContentComponent::CreateRenderNode()
{
    return RenderTabContent::Create();
}

} // namespace OHOS::Ace