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

#include "core/components/shared_transition/shared_transition_element.h"

#include "core/components/box/box_element.h"
#include "core/components/box/render_box.h"
#include "core/components/display/display_component.h"
#include "core/components/display/render_display.h"
#include "core/components/image/image_element.h"
#include "core/components/image/render_image.h"
#include "core/components/page/page_element.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
namespace {

constexpr uint8_t MAX_OPACITY = 255;
constexpr uint8_t MIN_OPACITY = 0;

} // namespace

SharedTransitionElement::~SharedTransitionElement()
{
    auto page = pageElement_.Upgrade();
    if (!page) {
        return;
    }

    // shared transition may be updated by other element
    auto& sharedTransitionMap = page->GetSharedTransitionMap();
    auto pos = sharedTransitionMap.find(shareId_);
    if (pos != sharedTransitionMap.end()) {
        auto ptr = pos->second.Upgrade();
        if (ptr == this) {
            page->RemoveSharedTransition(shareId_);
        }
    }
}

void SharedTransitionElement::Update()
{
    ComposedElement::Update();
    const auto sharedComponent = AceType::DynamicCast<SharedTransitionComponent>(component_);
    if (!sharedComponent) {
        LOGE("Get SharedComponent failed. id: %{public}s", GetId().c_str());
        return;
    }
    option_ = sharedComponent->GetOption();
    oldShareId_ = shareId_;
    shareId_ = sharedComponent->GetShareId();
    effect_ = sharedComponent->GetEffect();
    enablePopEnter_ = sharedComponent->IsEnablePopEnter();
    enablePopExit_ = sharedComponent->IsEnablePopExit();
    enablePushEnter_ = sharedComponent->IsEnablePushEnter();
    enablePushExit_ = sharedComponent->IsEnablePushExit();
    opacity_ = sharedComponent->GetOpacity();
    zIndex_ = sharedComponent->GetZIndex();
}

RefPtr<RenderBox> SharedTransitionElement::GetRenderPassengerWithPajamas() const
{
    if (children_.empty()) {
        LOGE("Get render passenger failed. no child. id: %{public}s", GetId().c_str());
        return nullptr;
    }
    const auto& passenger = AceType::DynamicCast<BoxElement>(GetContentElement());
    if (!passenger) {
        LOGE("Get render passenger failed. passenger is null. id: %{public}s", GetId().c_str());
        return nullptr;
    }
    return AceType::DynamicCast<RenderBox>(passenger->GetRenderNode());
}

RefPtr<RenderBox> SharedTransitionElement::GetRenderBox()
{
    const auto& parent = GetElementParent().Upgrade();
    if (!parent) {
        LOGE("GetRenderBox failed, parent is nullptr");
        return nullptr;
    }

    auto box = AceType::DynamicCast<BoxElement>(parent);
    if (!box) {
        LOGE("GetRenderBox failed, parent is not box");
        return nullptr;
    }

    return AceType::DynamicCast<RenderBox>(box->GetRenderNode());
}

Size SharedTransitionElement::GetSuitSize() const
{
    const auto& renderBox = GetRenderPassengerWithPajamas();
    if (!renderBox) {
        LOGE("Get layout size failed. render box not found. id: %{public}s", GetId().c_str());
        return Size();
    }
    return renderBox->GetLayoutSize() - renderBox->GetMarginSize();
}

Offset SharedTransitionElement::GetGlobalOffset() const
{
    const auto& renderBox = GetRenderPassengerWithPajamas();
    if (!renderBox) {
        LOGE("Get Global offset failed. render box not found. id: %{public}s", GetId().c_str());
        return Offset();
    }
    return renderBox->GetGlobalOffset();
}

float SharedTransitionElement::GetOpacity() const
{
    return opacity_;
}

int32_t SharedTransitionElement::GetZIndex() const
{
    return zIndex_;
}

bool SharedTransitionElement::AboardShuttle(Offset& ticket)
{
    if (!passengerComponent_ || passengerElement_) {
        LOGE("Aboard Shuttle Component failed. passenger not init or already aboard. id: %{public}s", GetId().c_str());
        return false;
    }
    auto passenger = GetRenderPassengerWithPajamas();
    if (!passenger) {
        LOGE("Aboard Shuttle Component failed. render passenger is null. id: %{public}s", GetId().c_str());
        return false;
    }
    const auto& renderBox = AceType::DynamicCast<RenderBox>(passenger);
    if (!renderBox) {
        LOGE("Aboard Shuttle Component failed. render passenger do not have pajamas. id: %{public}s", GetId().c_str());
        return false;
    }
    LOGD("SharedTransitionElement: aboard shuttle. id: %{public}s", GetId().c_str());
    // save origin width/height and recover it when GetOffShuttle
    passengerWidth_ = passengerComponent_->GetWidthDimension();
    passengerHeight_ = passengerComponent_->GetHeightDimension();
    auto suitSize = GetSuitSize();
    passengerComponent_->SetWidth(suitSize.Width());
    passengerComponent_->SetHeight(suitSize.Height());

    auto parent = GetRenderBox();
    if (parent && parent->GetBackDecoration() != nullptr) {
        passengerComponent_->SetBackDecoration(parent->GetBackDecoration());
    }
    if (parent && parent->GetFrontDecoration() != nullptr) {
        passengerComponent_->SetFrontDecoration(parent->GetFrontDecoration());
    }

    passengerElement_ = AceType::DynamicCast<BoxElement>(GetContentElement());
    if (!passengerElement_) {
        LOGE("Aboard Shuttle Element failed. passenger element is null.");
        return false;
    }
    // passenger goes out and comes to the shuttle port
    // check first child not null when check passenger. no need check again here.
    GetFirstChild()->UpdateChild(passengerElement_, nullptr);
    passengerRender_ = passenger;
    passengerElement_->SetRenderNode(passengerRender_);
    auto placeHolder = AceType::MakeRefPtr<BoxComponent>();
    // passenger measures the size
    auto paintRect = renderBox->GetLayoutSize();
    placeHolder->SetHeight(paintRect.Height());
    placeHolder->SetWidth(paintRect.Width());
    // check first child not null when check passenger. no need check again here.
    GetFirstChild()->UpdateChild(nullptr, placeHolder);
    ticket = renderBox->GetOffsetToStage();
    return true;
}

void SharedTransitionElement::GetOffShuttle()
{
    LOGD("SharedTransitionElement: get off shuttle. id: %{public}s", GetId().c_str());
    auto placeHolder = GetContentElement();
    if (!passengerElement_ || !passengerRender_ || !placeHolder) {
        LOGE("GetOff Shuttle failed. Passenger already Get Off or place holder is null.");
        return;
    }
    if (placeHolder == passengerElement_) {
        LOGI("Passenger already takeoff shuttle. do nothing.");
        return;
    }
    // check first child not null when check placeHolder. no need check again here.
    auto placeHolderRender = placeHolder->GetRenderNode();
    bool placeHolderHidden = placeHolderRender ? placeHolderRender->GetHidden() : true;
    GetFirstChild()->UpdateChild(placeHolder, nullptr);
    auto parent = passengerElement_->GetElementParent().Upgrade();
    if (parent) {
        // Remove passenger from overlay.
        parent->UpdateChild(passengerElement_, nullptr);
    }

    // save origin width/height in AboardShuttle and recover it when GetOffShuttle
    passengerComponent_->SetWidth(passengerWidth_.Value(), passengerWidth_.Unit());
    passengerComponent_->SetHeight(passengerHeight_.Value(), passengerHeight_.Unit());
    passengerComponent_->SetBackDecoration(nullptr);
    passengerComponent_->SetFrontDecoration(nullptr);
    passengerElement_->SetRenderNode(passengerRender_);
    passengerElement_->SetNewComponent(passengerComponent_);
    passengerElement_->Mount(GetFirstChild());
    // Follow place holder's hidden status.
    passengerRender_->SetHidden(placeHolderHidden);
    // Clear RefPtr.
    passengerRender_.Reset();
    passengerElement_.Reset();
}

void SharedTransitionElement::SetVisible(bool visible)
{
    auto displayElement = DynamicCast<DisplayElement>(GetFirstChild());
    if (!displayElement) {
        LOGE("Set visible failed. display element is null. id: %{public}s", GetId().c_str());
        return;
    }
    auto displayRender = DynamicCast<RenderDisplay>(displayElement->GetRenderNode());
    if (!displayRender) {
        LOGE("Set visible failed. display render is null. id: %{public}s", GetId().c_str());
        return;
    }
    if (visible) {
        displayRender->UpdateOpacity(MAX_OPACITY);
    } else {
        displayRender->UpdateOpacity(MIN_OPACITY);
    }
}

void SharedTransitionElement::PerformBuild()
{
    Register();
    ComposedElement::PerformBuild();
}

RefPtr<Component> SharedTransitionElement::BuildChild()
{
    auto shared = AceType::DynamicCast<SharedTransitionComponent>(component_);
    if (shared) {
        auto passengerComponent = ComposedElement::BuildChild();
        passengerComponent_ = AceType::DynamicCast<BoxComponent>(passengerComponent);
        if (!passengerComponent_) {
            passengerComponent_ = AceType::MakeRefPtr<BoxComponent>();
            passengerComponent_->SetChild(passengerComponent);
        }
        auto displayComponent_ = AceType::MakeRefPtr<DisplayComponent>();
        if (passengerElement_) {
            displayComponent_->SetChild(AceType::MakeRefPtr<BoxComponent>());
        } else {
            displayComponent_->SetChild(passengerComponent_);
        }
        Component::MergeRSNode(displayComponent_->GetChild(), displayComponent_);
        return displayComponent_;
    } else {
        LOGE("Build child failed. no shared transition component found. id: %{public}s", GetId().c_str());
        return ComposedElement::BuildChild();
    }
}

void SharedTransitionElement::Register()
{
    auto page = SearchParentPage();
    if (!page) {
        LOGE("No parent page found.");
        return;
    }
    LOGD("SharedTransitionElement Register shareId: %{public}s, id: %{public}s", shareId_.c_str(), GetId().c_str());
    pageElement_ = page;
    if (!oldShareId_.empty()) {
        auto& sharedTransitionElementMap = page->GetSharedTransitionMap();
        auto oldSharedIter = sharedTransitionElementMap.find(oldShareId_);
        if (oldSharedIter != sharedTransitionElementMap.end()) {
            auto oldWeak = oldSharedIter->second;
            auto oldShared = oldWeak.Upgrade();
            if (oldShared == this) {
                page->RemoveSharedTransition(oldShareId_);
            }
        }
    }
    page->AddSharedTransition(AceType::Claim(this));
}

RefPtr<PageElement> SharedTransitionElement::SearchParentPage() const
{
    auto parent = GetElementParent().Upgrade();
    while (parent) {
        auto page = AceType::DynamicCast<PageElement>(parent);
        if (page) {
            LOGD("Find parent page. page id: %{public}d, id: %{public}s", page->GetPageId(), GetId().c_str());
            return page;
        }
        parent = parent->GetElementParent().Upgrade();
    }
    LOGD("No parent page found. shareId: %{public}s, id: %{public}s", shareId_.c_str(), GetId().c_str());
    return nullptr;
}

const ShareId& SharedTransitionElement::GetShareId() const
{
    return shareId_;
}

const RefPtr<SharedTransitionEffect>& SharedTransitionElement::GetEffect() const
{
    return effect_;
}

bool SharedTransitionElement::IsEnablePopEnter() const
{
    return enablePopEnter_;
}

bool SharedTransitionElement::IsEnablePushEnter() const
{
    return enablePushEnter_;
}

bool SharedTransitionElement::IsEnablePopExit() const
{
    return enablePopExit_;
}

bool SharedTransitionElement::IsEnablePushExit() const
{
    return enablePushExit_;
}

RefPtr<Element> SharedTransitionElement::GetContentElement() const
{
    auto display = GetFirstChild();
    if (!display) {
        LOGE("Get Content Element failed. display is null.");
        return nullptr;
    }
    return display->GetFirstChild();
}

void SharedTransitionElement::SetSizeModified(SizeModifiedCallback&& sizeModifiedCallback)
{
    auto boxBaseElement = DynamicCast<BoxBaseElement>(GetContentElement());
    if (!boxBaseElement) {
        LOGE("Set Size Modified failed. image element not found.");
        return;
    }
    auto boxBaseRender = DynamicCast<RenderBoxBase>(boxBaseElement->GetRenderNode());
    if (!boxBaseRender) {
        LOGE("Set Size Modified failed. image render not found.");
        return;
    }
    sizeModifiedCallback_ = sizeModifiedCallback;
    if (!sizeModifiedCallback_) {
        boxBaseRender->SetLayoutCallback(nullptr);
        return;
    }
    boxBaseRender->SetLayoutCallback([sharedWeak = AceType::WeakClaim(this)]() {
        auto shared = sharedWeak.Upgrade();
        if (!shared) {
            LOGE("Layout callback is failed. shared is null");
            return;
        }
        auto context = shared->context_.Upgrade();
        if (!context) {
            LOGE("Layout callback is failed. context is null");
            return;
        }
        // Re-create shared transition after paint.
        context->AddPostFlushListener(shared);
    });
}

void SharedTransitionElement::OnPostFlush()
{
    if (sizeModifiedCallback_) {
        sizeModifiedCallback_();
    }
}

} // namespace OHOS::Ace
