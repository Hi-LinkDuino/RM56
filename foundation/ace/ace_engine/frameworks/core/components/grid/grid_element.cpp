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

#include "core/components/grid/grid_element.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/grid/grid_component.h"
#include "core/components/grid/render_grid.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {
namespace {

constexpr double DOUBLE_FACTOR = 2.0;

std::string BuildEventParam(int32_t index, int32_t count)
{
    return std::string("{\"index\":")
        .append(std::to_string(index))
        .append(",\"count\":")
        .append(std::to_string(count))
        .append("}");
}

} // namespace

RefPtr<RenderNode> GridElement::CreateRenderNode()
{
    RefPtr<RenderNode> node = ComponentGroupElement::CreateRenderNode();
    RefPtr<RenderGrid> grid = AceType::DynamicCast<RenderGrid>(node);
    if (grid) {
        SetOnFocusCallback([weakGridElement = AceType::WeakClaim(this)](void) {
            auto gridElement = weakGridElement.Upgrade();
            if (gridElement) {
                gridElement->HandleOnFocus();
            }
        });
        SetOnBlurCallback([weakGridElement = AceType::WeakClaim(this)](void) {
            auto gridElement = weakGridElement.Upgrade();
            if (gridElement) {
                gridElement->HandleOnBlur();
            }
        });
        grid->RegisterCallback([weakGridElement = AceType::WeakClaim(this)](int32_t index, int32_t count) {
            auto gridElement = weakGridElement.Upgrade();
            if (gridElement) {
                gridElement->BuildGridData(index, count);
            }
        });
    }
    return node;
}

void GridElement::BuildGridData(int32_t index, int32_t count)
{
    auto pipelineContext = context_.Upgrade();
    if (pipelineContext == nullptr) {
        LOGE("Failed to get page context");
        return;
    }

    LOGD("BuildGridData(index = %{public}d, count = %{public}d)", index, count);
    std::string result;
    if (requestItem_) {
        requestItem_(BuildEventParam(index, count), result);
    }
    if (!result.empty()) {
        if (newGridItems_.empty()) {
            LOGE("No new items, index = %{public}d, count = %{public}d", index, count);
        } else {
            pipelineContext->AddDirtyElement(AceType::Claim(this));
        }
    }
}

void GridElement::PerformBuild()
{
    if (newGridItems_.empty() || component_) {
        ComponentGroupElement::PerformBuild();
        return;
    }

    LOGD("GridElement::PerformBuild");
    for (const auto& child : newGridItems_) {
        UpdateChild(nullptr, child);
    }

    newGridItems_.clear();
    // New child added, mark need layout.
    if (renderNode_) {
        renderNode_->MarkNeedLayout();
    }
}

void GridElement::Update()
{
    ComponentGroupElement::Update();

    RefPtr<GridComponent> grid = AceType::DynamicCast<GridComponent>(component_);
    if (grid) {
        requestItem_ = AceSyncEvent<void(const std::string&, std::string&)>::Create(grid->GetOnRequestItem(), context_);
    }
}

bool GridElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    RefPtr<RenderGrid> grid = AceType::DynamicCast<RenderGrid>(renderNode_);
    if (!grid) {
        LOGE("Render grid is null.");
        return false;
    }
    Offset center = Offset(rect.Left() + rect.Width() / DOUBLE_FACTOR,
        rect.Top() + rect.Height() / DOUBLE_FACTOR);
    LOGD("RequestNextFocus vertical:%{public}d reverse:%{public}d pos:%{public}lf %{public}lf.",
        vertical, reverse, center.GetX(), center.GetY());
    bool ret = false;
    while (!ret) {
        int32_t focusIndex = grid->RequestNextFocus(vertical, reverse, center);
        int32_t size = static_cast<int32_t>(GetChildrenList().size());
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

void GridElement::HandleOnFocus()
{
    RefPtr<RenderGrid> grid = AceType::DynamicCast<RenderGrid>(renderNode_);
    if (!grid) {
        LOGE("Render grid is null.");
        return;
    }
    int32_t focusIndex = 0;
    for (auto focusNode : GetChildrenList()) {
        if (focusNode->IsCurrentFocus()) {
            break;
        }
        ++focusIndex;
    }
    grid->HandleOnFocus(focusIndex);
}

void GridElement::HandleOnBlur()
{
    RefPtr<RenderGrid> grid = AceType::DynamicCast<RenderGrid>(renderNode_);
    if (!grid) {
        LOGE("Render grid is null.");
        return;
    }
    grid->HandleOnBlur();
}

void GridElement::ApplyRenderChild(const RefPtr<RenderElement>& renderChild)
{
    if (!renderChild) {
        LOGE("Element child is null");
        return;
    }

    if (!renderNode_) {
        LOGE("RenderElement don't have a render node");
        return;
    }

    auto proxy = RenderItemProxy::Create();
    proxy->AddChild(renderChild->GetRenderNode());
    proxy->Attach(context_);
    renderNode_->AddChild(proxy);
}

} // namespace OHOS::Ace
