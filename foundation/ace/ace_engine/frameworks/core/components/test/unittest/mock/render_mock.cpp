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

#include "core/components/test/unittest/mock/render_mock.h"

#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "core/components/dialog_modal/dialog_modal_component.h"
#include "core/components/semi_modal/semi_modal_component.h"
#include "core/image/image_object.h"

namespace OHOS::Ace {
namespace {

MockRenderDisplay::HookRenderDisplay g_hookRenderDisplay;
MockRenderDropFilter::HookRenderDropFilter g_hookRenderDropfilter;

} // namespace

RefPtr<RenderNode> RenderDisplay::Create()
{
    LOGD("Mock RenderDisplay Create");
    const auto& display = AceType::MakeRefPtr<MockRenderDisplay>();
    if (g_hookRenderDisplay) {
        LOGD("execute hook");
        g_hookRenderDisplay(display);
    }
    return display;
}

void MockRenderDisplay::SetMockHook(const HookRenderDisplay& hook)
{
    LOGD("set hook");
    g_hookRenderDisplay = hook;
}

RefPtr<RenderNode> RenderRoot::Create()
{
    return AceType::MakeRefPtr<MockRenderRoot>();
}

RefPtr<RenderNode> RenderStack::Create()
{
    return AceType::MakeRefPtr<MockRenderStack>();
}

RefPtr<RenderNode> RenderBox::Create()
{
    return AceType::MakeRefPtr<MockRenderBox>();
}

RefPtr<RenderNode> RenderClip::Create()
{
    return AceType::MakeRefPtr<MockRenderClip>();
}

RefPtr<RenderNode> RenderImage::Create()
{
    return AceType::MakeRefPtr<MockRenderImage>();
}

RefPtr<ImageCache> ImageCache::Create()
{
    return AceType::MakeRefPtr<MockImageCache>();
}

void ImageCache::Purge() {}

RefPtr<RenderNode> RenderDropFilter::Create()
{
    LOGD("Mock RenderDropFilter Create");
    const auto& dropFilter = AceType::MakeRefPtr<MockRenderDropFilter>();
    if (g_hookRenderDropfilter) {
        LOGD("execute hook");
        g_hookRenderDropfilter(dropFilter);
    }
    return dropFilter;
}

void MockRenderDropFilter::SetMockHook(const HookRenderDropFilter& hook)
{
    LOGD("set hook");
    g_hookRenderDropfilter = hook;
}

} // namespace OHOS::Ace
