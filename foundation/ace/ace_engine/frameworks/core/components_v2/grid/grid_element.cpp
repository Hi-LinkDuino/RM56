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

#include "core/components_v2/grid/grid_element.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/grid_layout/grid_layout_component.h"
#include "core/components/grid_layout/grid_layout_item_component.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/components_v2/grid/render_grid_scroll.h"

namespace OHOS::Ace::V2 {

void GridElement::Update()
{
    RenderElement::Update();
    RefPtr<RenderGridScroll> render = AceType::DynamicCast<RenderGridScroll>(renderNode_);
    if (!render) {
        return;
    }
    render->OnDataSourceUpdated(0);
}

RefPtr<RenderNode> GridElement::CreateRenderNode()
{
    auto render = RenderElement::CreateRenderNode();
    RefPtr<RenderGridScroll> renderGrid = AceType::DynamicCast<RenderGridScroll>(render);
    if (renderGrid) {
        renderGrid->SetBuildChildByIndex([weak = WeakClaim(this)](int32_t index) {
            auto element = weak.Upgrade();
            if (!element) {
                return false;
            }
            return element->BuildChildByIndex(index);
        });

        renderGrid->SetDeleteChildByIndex([weak = WeakClaim(this)](int32_t index) {
            auto element = weak.Upgrade();
            if (!element) {
                return;
            }
            element->DeleteChildByIndex(index);
        });

        renderGrid->SetGetChildSpanByIndex([weak = WeakClaim(this)](int32_t index, bool isHorizontal, int32_t& itemMain,
                                               int32_t& itemCross, int32_t& itemMainSpan, int32_t& itemCrossSpan) {
            auto element = weak.Upgrade();
            if (!element) {
                return false;
            }
            return element->GetItemSpanByIndex(index, isHorizontal, itemMain, itemCross, itemMainSpan, itemCrossSpan);
        });
    }

    return render;
}

void GridElement::PerformBuild()
{
    auto component = AceType::DynamicCast<GridLayoutComponent>(component_);
    ACE_DCHECK(component); // MUST be GridComponent
    V2::ElementProxyHost::UpdateChildren(component->GetChildren());
}

bool GridElement::BuildChildByIndex(int32_t index)
{
    if (index < 0) {
        return false;
    }
    auto element = GetElementByIndex(index);
    if (!element) {
        LOGE("GetElementByIndex failed index=[%d]", index);
        return false;
    }
    auto renderNode = element->GetRenderNode();
    if (!renderNode) {
        LOGE("GetRenderNode failed");
        return false;
    }
    RefPtr<RenderGridScroll> grid = AceType::DynamicCast<RenderGridScroll>(renderNode_);
    if (!grid) {
        return false;
    }
    grid->AddChildByIndex(index, renderNode);
    return true;
}

bool GridElement::GetItemSpanByIndex(int32_t index, bool isHorizontal, int32_t& itemMain, int32_t& itemCross,
    int32_t& itemMainSpan, int32_t& itemCrossSpan)
{
    if (index < 0) {
        return false;
    }
    auto component = GetComponentByIndex(index);
    auto gridItem = AceType::DynamicCast<GridLayoutItemComponent>(component);

    if (!gridItem) {
        return false;
    }
    if (isHorizontal) {
        itemMain = gridItem->GetColumnIndex();
        itemCross = gridItem->GetRowIndex();
        itemMainSpan = gridItem->GetColumnSpan();
        itemCrossSpan = gridItem->GetRowSpan();
    } else {
        itemMain = gridItem->GetRowIndex();
        itemCross = gridItem->GetColumnIndex();
        itemMainSpan = gridItem->GetRowSpan();
        itemCrossSpan = gridItem->GetColumnSpan();
    }
    return true;
}

void GridElement::DeleteChildByIndex(int32_t index)
{
    ReleaseElementByIndex(index);
}

bool GridElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
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
    renderNode_->AddChild(renderChild->GetRenderNode());
}

RefPtr<Element> GridElement::OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component)
{
    return UpdateChild(element, component);
}

RefPtr<Component> GridElement::OnMakeEmptyComponent()
{
    return AceType::MakeRefPtr<GridLayoutItemComponent>();
}

void GridElement::OnDataSourceUpdated(size_t startIndex)
{
    auto context = context_.Upgrade();
    if (context) {
        context->AddPostFlushListener(AceType::Claim(this));
    }
    RefPtr<RenderGridScroll> render = AceType::DynamicCast<RenderGridScroll>(renderNode_);
    if (!render) {
        return;
    }
    render->OnDataSourceUpdated(static_cast<int32_t>(startIndex));
    render->SetTotalCount(ElementProxyHost::TotalCount());
}

void GridElement::OnPostFlush()
{
    ReleaseRedundantComposeIds();
}

size_t GridElement::GetReloadedCheckNum()
{
    RefPtr<RenderGridScroll> render = AceType::DynamicCast<RenderGridScroll>(renderNode_);
    if (render) {
        size_t cachedNum = render->GetCachedSize();
        if (cachedNum > 0) {
            return cachedNum;
        }
    }
    return ElementProxyHost::GetReloadedCheckNum();
}

void GridElement::Dump()
{
    DumpProxy();
}

} // namespace OHOS::Ace::V2
