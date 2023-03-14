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

#include "core/components/grid_layout/grid_layout_element.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

void GridLayoutElement::Update()
{
    ComponentGroupElement::Update();
}

bool GridLayoutElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    RefPtr<RenderGridLayout> grid = AceType::DynamicCast<RenderGridLayout>(renderNode_);
    if (!grid) {
        LOGE("Render grid is null.");
        return false;
    }
    LOGI("RequestNextFocus vertical:%{public}d reverse:%{public}d.", vertical, reverse);
    bool ret = false;
    while (!ret) {
        int32_t focusIndex = grid->RequestNextFocus(vertical, reverse);
        auto size = static_cast<int32_t>(GetChildrenList().size());
        if (focusIndex < 0 || focusIndex >= size) {
            return false;
        }
        auto iter = GetChildrenList().begin();
        std::advance(iter, focusIndex);
        auto focusNode = *iter;
        if (!focusNode) {
            LOGE("Target focus node is null.");
            return false;
        }
        // If current Node can not obtain focus, move to next.
        ret = focusNode->RequestFocusImmediately();
    }
    return ret;
}

void GridLayoutElement::ApplyRenderChild(const RefPtr<RenderElement>& renderChild)
{
    if (!renderChild) {
        LOGE("Element child is null");
        return;
    }

    if (!renderNode_) {
        LOGE("RenderElement don't have a render node");
        return;
    }

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        ComponentGroupElement::ApplyRenderChild(renderChild);
        return;
    }

    auto proxy = RenderItemProxy::Create();
    if (!proxy) {
        LOGE("fail to create proxy node");
        return;
    }
    renderNode_->AddChild(proxy);
    proxy->AddChild(renderChild->GetRenderNode());
    proxy->Attach(context_);
}

void GridLayoutElement::PerformBuild()
{
    ComponentGroupElement::PerformBuild();
}

} // namespace OHOS::Ace