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

#include "core/components/picker/picker_value_element.h"

#include "core/components/box/box_element.h"
#include "core/components/stack/stack_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

void PickerValueElement::OnClick()
{
    auto render = AceType::DynamicCast<RenderPickerValue>(renderNode_);
    if (!render) {
        LOGE("render is null.");
        return;
    }
    render->HandleClickCallback();
}

RefPtr<RenderBox> PickerValueElement::GetRenderBox() const
{
    auto element = GetElementParent().Upgrade();
    while (element && !AceType::InstanceOf<ComposedElement>(element)) {
        auto box = AceType::DynamicCast<BoxElement>(element);
        if (box) {
            return AceType::DynamicCast<RenderBox>(box->GetRenderNode());
        }
        element = element->GetElementParent().Upgrade();
    }
    return nullptr;
}

void PickerValueElement::HandleSelfFocus(bool focus)
{
    auto render = AceType::DynamicCast<RenderPickerValue>(renderNode_);
    if (!render) {
        LOGE("render is null.");
        return;
    }

    auto box = GetRenderBox();
    if (!box) {
        LOGE("box is null.");
        return;
    }

    render->HandleFocus(box, focus);
}

void PickerValueElement::HandleSelfFocus()
{
    auto render = AceType::DynamicCast<RenderPickerValue>(renderNode_);
    if (!render) {
        LOGE("render is null.");
        return;
    }

    auto box = GetRenderBox();
    if (!box) {
        LOGE("box is null.");
        return;
    }

    render->HandleFocus(box);
}

} // namespace OHOS::Ace
