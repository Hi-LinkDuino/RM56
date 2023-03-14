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

#include "core/components/drop_filter/drop_filter_component.h"

#include "core/components/drop_filter/drop_filter_element.h"
#include "core/components/drop_filter/render_drop_filter.h"

namespace OHOS::Ace {

DropFilterComponent::DropFilterComponent(const RefPtr<Component>& child, double sigmaX, double sigmaY)
    : SoleChildComponent(child), sigmaX_(sigmaX), sigmaY_(sigmaY) {}

RefPtr<RenderNode> DropFilterComponent::CreateRenderNode()
{
    return RenderDropFilter::Create();
}

RefPtr<Element> DropFilterComponent::CreateElement()
{
    return AceType::MakeRefPtr<DropFilterElement>();
}

} // namespace OHOS::Ace
