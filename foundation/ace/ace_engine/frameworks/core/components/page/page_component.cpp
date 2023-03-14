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

#include "core/components/page/page_component.h"

#include "core/components/page/page_element.h"

namespace OHOS::Ace {
namespace {

const ComposeId PAGE_COMPOSE_ID_PREFIX = "Page-";

}

PageComponent::PageComponent(int32_t pageId, const std::string& pageUrl, const RefPtr<Component>& child)
    : ComposedComponent(
          PAGE_COMPOSE_ID_PREFIX + std::to_string(pageId), PAGE_COMPOSE_ID_PREFIX + std::to_string(pageId), child),
      pageId_(pageId), pageUrl_(pageUrl)
{}

PageComponent::PageComponent(
    int32_t pageId, const std::string& pageUrl, const ComposeId& cardComposeId, const RefPtr<Component>& child)
    : ComposedComponent(
          PAGE_COMPOSE_ID_PREFIX + std::to_string(pageId), PAGE_COMPOSE_ID_PREFIX + std::to_string(pageId), child),
      pageId_(pageId), pageUrl_(pageUrl), cardComposeId_(cardComposeId)
{}

RefPtr<Element> PageComponent::CreateElement()
{
    if (!cardComposeId_.empty()) {
        return AceType::MakeRefPtr<PageElement>(pageId_, pageUrl_, cardComposeId_, GetId());
    }
    return AceType::MakeRefPtr<PageElement>(pageId_, pageUrl_, GetId());
}

} // namespace OHOS::Ace