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

#include "frameworks/core/components/shape/shape_container_component.h"

#include "frameworks/core/components/shape/render_shape_container.h"
#include "frameworks/core/components/shape/shape_container_element.h"

namespace OHOS::Ace {

RefPtr<Element> ShapeContainerComponent::CreateElement()
{
    return AceType::MakeRefPtr<ShapeContainerElement>();
}

RefPtr<RenderNode> ShapeContainerComponent::CreateRenderNode()
{
    return RenderShapeContainer::Create();
}

void ShapeContainerComponent::InheritShapeStyle(const FillState& fillState, const StrokeState& strokeState,
    bool antiAlias)
{
    fillState_.Inherit(fillState);
    strokeState_.Inherit(strokeState);
    if (!antiAlias_.first) {
        antiAlias_.second = antiAlias;
    }
}

} // namespace OHOS::Ace
