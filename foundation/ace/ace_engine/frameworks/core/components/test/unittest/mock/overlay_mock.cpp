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

#include "core/components/test/unittest/mock/overlay_mock.h"

#include "core/components/overlay/overlay_component.h"
#include "core/components/stack/flutter_render_stack.h"

namespace OHOS::Ace {

namespace {

MockOverlayElement::HookOverlay g_hookOverlay;

} // namespace

void MockOverlayElement::SetMockHook(const HookOverlay& hook)
{
    LOGD("set hook.");
    g_hookOverlay = std::move(hook);
}

RefPtr<Element> OverlayComponent::CreateElement()
{
    auto overlay = AceType::MakeRefPtr<MockOverlayElement>();
    if (g_hookOverlay) {
        LOGD("execute hook.");
        g_hookOverlay(overlay);
    }
    return overlay;
}

RefPtr<RenderNode> OverlayComponent::CreateRenderNode()
{
    auto renderNode = AceType::DynamicCast<FlutterRenderStack>(StackComponent::CreateRenderNode());
    if (renderNode) {
        renderNode->SetVisible(false);
        renderNode->SetBoundary();
    }
    return renderNode;
}

} // namespace OHOS::Ace