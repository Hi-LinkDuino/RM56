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

#include "core/components_v2/swiper/swiper_element.h"

#include "core/components/swiper/render_swiper.h"

namespace OHOS::Ace::V2 {

RefPtr<RenderNode> SwiperElement::CreateRenderNode()
{
    auto render = RenderElement::CreateRenderNode();
    RefPtr<RenderSwiper> renderSwiper = AceType::DynamicCast<RenderSwiper>(render);
    if (renderSwiper) {
        renderSwiper->SetBuildChildByIndex([weak = WeakClaim(this)](int32_t index) {
            auto element = weak.Upgrade();
            if (!element) {
                return false;
            }
            return element->BuildChildByIndex(index);
        });

        renderSwiper->SetDeleteChildByIndex([weak = WeakClaim(this)](int32_t index) {
            auto element = weak.Upgrade();
            if (!element) {
                return;
            }
            element->DeleteChildByIndex(index);
        });
    }

    return render;
}

void SwiperElement::PerformBuild()
{
    auto component = AceType::DynamicCast<SwiperComponent>(component_);
    ACE_DCHECK(component); // MUST be SwiperComponent
    V2::ElementProxyHost::UpdateChildren(component->GetChildren());
}

bool SwiperElement::BuildChildByIndex(int32_t index)
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
    RefPtr<RenderSwiper> swiper = AceType::DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        return false;
    }
    swiper->AddChildByIndex(index, renderNode);
    return true;
}

void SwiperElement::DeleteChildByIndex(int32_t index)
{
    ReleaseElementByIndex(index);
    RefPtr<RenderSwiper> swiper = AceType::DynamicCast<RenderSwiper>(renderNode_);
    if (!swiper) {
        return;
    }
    swiper->RemoveChildByIndex(index);
}

bool SwiperElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    return true;
}

void SwiperElement::ApplyRenderChild(const RefPtr<RenderElement>& renderChild)
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

RefPtr<Element> SwiperElement::OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component)
{
    return UpdateChild(element, component);
}

RefPtr<Component> SwiperElement::OnMakeEmptyComponent()
{
    return nullptr;
}

void SwiperElement::OnDataSourceUpdated(size_t startIndex)
{
    RefPtr<RenderSwiper> render = AceType::DynamicCast<RenderSwiper>(renderNode_);
    if (!render) {
        return;
    }
    render->OnDataSourceUpdated(ElementProxyHost::TotalCount(), startIndex);
    auto context = context_.Upgrade();
    if (context) {
        context->AddPostFlushListener(AceType::Claim(this));
    }
}

void SwiperElement::OnPostFlush()
{
    ReleaseRedundantComposeIds();
}

} // namespace OHOS::Ace::V2
