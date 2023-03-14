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

#include "core/pipeline/base/related_node.h"

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t MAX_DEEP_SEARCH = 10;

}

void RelatedChild::InitRelatedParent(const WeakPtr<RenderNode>& weakParent)
{
    int32_t searchDeep = 0;
    auto parent = weakParent.Upgrade();
    while (searchDeep++ < MAX_DEEP_SEARCH && parent) {
        auto relatedContainer = AceType::DynamicCast<RelatedContainer>(parent);
        if (relatedContainer) {
            LOGD("has related container parent");
            relatedParent_ = AceType::WeakClaim(AceType::RawPtr(relatedContainer));
            return;
        } else {
            parent = parent->GetParent().Upgrade();
        }
    }
}

void RelatedChild::RelatedEventStart()
{
    if (!IsRelatedEventEnable()) {
        return;
    }
    auto parent = relatedParent_.Upgrade();
    parent->OnRelatedStart();
}

bool RelatedChild::RelatedScrollEventPrepare(const Offset& delta)
{
    if (!IsRelatedEventEnable()) {
        return false;
    }
    auto parent = relatedParent_.Upgrade();
    Offset consumed;
    parent->OnRelatedPreScroll(delta, consumed);
    if (!NearZero(consumed.GetY())) {
        return true;
    }
    return false;
}

bool RelatedChild::RelatedScrollEventDoing(const Offset& delta)
{
    if (!IsRelatedEventEnable()) {
        return false;
    }
    auto parent = relatedParent_.Upgrade();
    Offset consumed;
    parent->OnRelatedScroll(delta, consumed);
    if (!NearZero(consumed.GetY())) {
        return true;
    }
    return false;
}

void RelatedChild::RelatedEventEnd()
{
    if (!IsRelatedEventEnable()) {
        return;
    }
    auto parent = relatedParent_.Upgrade();
    parent->OnRelatedEnd();
}

} // namespace OHOS::Ace
