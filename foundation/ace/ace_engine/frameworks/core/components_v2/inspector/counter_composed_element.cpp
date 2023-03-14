/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/inspector/counter_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/counter/counter_element.h"

namespace OHOS::Ace::V2 {
void CounterComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto counterElement = GetContentElement<CounterElement>(CounterElement::TypeId());
    if (!counterElement) {
        LOGE("get GetCounterElement failed");
        return;
    }
    counterElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    counterElement->MarkDirty();
}

void CounterComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto counterElement = GetContentElement<CounterElement>(CounterElement::TypeId());
    if (!counterElement) {
        LOGE("get GetCounterElement failed");
        return;
    }
    auto element = GetInspectorComposedElementParent(counterElement);
    if (!element) {
        LOGE("counterElement get GetInspectorComposedElementParent failed");
        return;
    }
    auto child = GetElementChildBySlot(element, slot);
    if (!child) {
        LOGE("get GetElementChildBySlot failed");
        return;
    }
    element->UpdateChildWithSlot(child, newComponent, slot, slot);
    element->MarkDirty();
}

void CounterComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto counterElement = GetContentElement<CounterElement>(CounterElement::TypeId());
    if (!counterElement) {
        LOGE("get GetCounterElement failed");
        return;
    }
    auto element = GetInspectorComposedElementParent(counterElement);
    if (!element) {
        LOGE("counterElement get GetInspectorComposedElementParent failed");
        return;
    }
    auto child = GetElementChildBySlot(element, slot);
    if (!child) {
        LOGE("get GetElementChildBySlot failed");
        return;
    }
    element->UpdateChildWithSlot(child, nullptr, slot, slot);
    element->MarkDirty();
}
}
