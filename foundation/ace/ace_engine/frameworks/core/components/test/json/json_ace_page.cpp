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

#include "core/common/json_ace_page.h"

#include "core/components/page/page_component.h"
#include "core/components/page_transition/page_transition_component.h"
#include "core/dsl/component_factory.h"

namespace OHOS::Ace {

JsonAcePage::JsonAcePage(int32_t pageId) : AcePage(pageId) {}

RefPtr<PageComponent> JsonAcePage::BuildPage(const std::string& content)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> pageContent = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);

    RefPtr<PageTransitionComponent> transitionStack = AceType::MakeRefPtr<PageTransitionComponent>();
    transitionStack->SetContent(pageContent);

    RefPtr<PageComponent> page = AceType::MakeRefPtr<PageComponent>(GetPageId(), "JsonPage", transitionStack);
    return page;
}

RefPtr<ComposedComponent> JsonAcePage::BuildPagePatch(const std::string& content)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> patchContent = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    RefPtr<ComposedComponent> patch = AceType::DynamicCast<ComposedComponent>(patchContent);
    return patch;
}

} // namespace OHOS::Ace
