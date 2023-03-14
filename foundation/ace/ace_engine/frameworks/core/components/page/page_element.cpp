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

#include "core/components/page/page_element.h"

#include "base/log/dump_log.h"
#include "core/common/frontend.h"
#include "core/common/text_field_manager.h"
#include "core/components/transform/transform_element.h"

namespace OHOS::Ace {

PageElement::PageElement(int32_t pageId, const std::string& pageUrl, const ComposeId& id)
    : ComposedElement(id), pageId_(pageId), pageUrl_(pageUrl)
{}

PageElement::PageElement(
    int32_t pageId, const std::string& pageUrl, const ComposeId& cardComposeId, const ComposeId& id)
    : ComposedElement(id), pageId_(pageId), pageUrl_(pageUrl), cardComposeId_(cardComposeId)
{}

PageElement::~PageElement()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto textFieldManager = context->GetTextFieldManager();
    if (textFieldManager) {
        textFieldManager->RemovePageId(pageId_);
    }
}

bool PageElement::RequestNextFocus(bool vertical, bool reverse, const Rect& rect)
{
    // Do not precess logic.
    return false;
}

void PageElement::RemoveSharedTransition(const ShareId& shareId)
{
    LOGD("Remove shared transition id:%{public}s", shareId.c_str());
    sharedTransitionElementMap_.erase(shareId);
}

void PageElement::AddSharedTransition(const RefPtr<SharedTransitionElement>& shared)
{
    if (!shared) {
        LOGE("Add shared transition failed. element is null.");
        return;
    }
    LOGD("Add shared transition element id:%{public}s", shared->GetShareId().c_str());
    sharedTransitionElementMap_[shared->GetShareId()] = shared;
}

const PageElement::SharedTransitionMap& PageElement::GetSharedTransitionMap() const
{
    return sharedTransitionElementMap_;
}

void PageElement::SetHidden(bool hidden)
{
    auto render = GetRenderNode();
    if (render) {
        auto parent = render->GetParent().Upgrade();
        if (parent) {
            parent->MarkNeedRender();
        }
        render->SetHidden(hidden);
    }

    for (auto&& [id, callback] : hiddenCallbackMap_) {
        callback(hidden);
    }
}

void PageElement::RemoveCardTransition(int32_t retakeId)
{
    cardTransitionMap_.erase(retakeId);
}

void PageElement::AddCardTransition(const RefPtr<TransformElement>& transform)
{
    if (!transform) {
        LOGE("Add transform transition failed. element is null.");
        return;
    }
    cardTransitionMap_[transform->GetRetakeId()] = transform;
}

const PageElement::CardTransitionMap& PageElement::GetCardTransitionMap() const
{
    return cardTransitionMap_;
}

void PageElement::AddGeometryTransition(const std::string& id, WeakPtr<BoxElement>& boxElement, AnimationOption& option)
{
    if (geometryTransitionMap_.find(id) == geometryTransitionMap_.end()) {
        GeometryTransitionInfo sharedInfo;
        sharedInfo.sharedAnimationOption = option;
        sharedInfo.appearElement = boxElement;
        sharedInfo.isNeedCreate = false;
        geometryTransitionMap_.emplace(id, sharedInfo);
    } else {
        if (geometryTransitionMap_[id].appearElement != boxElement && !geometryTransitionMap_[id].isNeedCreate) {
            geometryTransitionMap_[id].disappearElement = geometryTransitionMap_[id].appearElement;
            geometryTransitionMap_[id].appearElement = boxElement;
            geometryTransitionMap_[id].sharedAnimationOption = option;
            geometryTransitionMap_[id].isNeedCreate = true;
        }
    }
}

const PageElement::GeometryTransitionMap& PageElement::GetGeometryTransition() const
{
    return geometryTransitionMap_;
}

void PageElement::RemoveGeometryTransition(const std::string& id)
{
    if (geometryTransitionMap_.find(id) != geometryTransitionMap_.end()) {
        geometryTransitionMap_.erase(id);
    }
}

void PageElement::FinishCreateGeometryTransition(const std::string& id)
{
    geometryTransitionMap_[id].isNeedCreate = false;
}

void PageElement::Dump()
{
    for (const auto& item : geometryTransitionMap_) {
        DumpLog::GetInstance().AddDesc(std::string("geometryTransitionID: ").append(item.first));
        auto element = item.second.appearElement.Upgrade();
        if (!element) {
            continue;
        }
        std::string retakeId = std::to_string(element->GetRetakeId());
        DumpLog::GetInstance().AddDesc(std::string("RetakeId: ").append(retakeId));
    }
}

} // namespace OHOS::Ace