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

#include "core/components/shape/shape_component.h"

#include "core/components/shape/render_shape.h"
#include "core/components/shape/shape_element.h"

namespace OHOS::Ace {

RefPtr<RenderNode> ShapeComponent::CreateRenderNode()
{
    return RenderShape::Create();
}

RefPtr<Element> ShapeComponent::CreateElement()
{
    return AceType::MakeRefPtr<ShapeElement>();
}

void ShapeComponent::InheritShapeStyle(const FillState& fillState, const StrokeState& strokeState, bool antiAlias)
{
    fillState_.Inherit(fillState);
    strokeState_.Inherit(strokeState);
    if (!antiAlias_.first) {
        antiAlias_.second = antiAlias;
    }
}

} // namespace OHOS::Ace
