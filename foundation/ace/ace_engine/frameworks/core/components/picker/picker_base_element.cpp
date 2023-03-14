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

#include "core/components/picker/picker_base_element.h"

#include "core/components/picker/picker_base_component.h"
#include "core/components/picker/render_picker_base.h"

namespace OHOS::Ace {

void PickerBaseElement::Update()
{
    SoleChildElement::Update();
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    context->AddPostFlushListener(AceType::Claim(this));
}

void PickerBaseElement::OnPostFlush()
{
    auto render = AceType::DynamicCast<RenderPickerBase>(renderNode_);
    if (!render) {
        return;
    }
    render->StartAnimation();
}

void PickerBaseElement::PerformBuild()
{
    auto context = context_.Upgrade();
    if (context) {
        auto picker = AceType::DynamicCast<PickerBaseComponent>(component_);
        auto accessibilityManager = context->GetAccessibilityManager();
        if (picker && accessibilityManager) {
            picker->Initialize(accessibilityManager, GetThemeManager());
        } else {
            LOGE("can not get accessibility manager or picker is null.");
        }
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        if (accessibilityManager && picker) {
            auto node = accessibilityManager->GetAccessibilityNodeById(picker->GetPickerBaseId());
            auto stack = picker->GetStack();
            if (node && stack) {
                node->SetZIndexToChild(stack->GetChildrenSize());
                accessibilityManager->ClearNodeRectInfo(node, false);
            }
        }
#endif
    }

    SoleChildElement::PerformBuild();
    auto render = AceType::DynamicCast<RenderPickerBase>(renderNode_);
    if (render) {
        render->UpdateRenders();
        render->FlushColumnsOptions();
    } else {
        LOGE("render picker base is null.");
    }
}

} // namespace OHOS::Ace
