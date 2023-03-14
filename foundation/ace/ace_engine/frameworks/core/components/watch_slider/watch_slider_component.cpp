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

#include "core/components/watch_slider/watch_slider_component.h"

#include "core/components/watch_slider/render_watch_slider.h"
#include "core/components/watch_slider/watch_slider_element.h"

namespace OHOS::Ace {

RefPtr<Element> WatchSliderComponent::CreateElement()
{
    return AceType::MakeRefPtr<WatchSliderElement>();
}

RefPtr<RenderNode> WatchSliderComponent::CreateRenderNode()
{
    return RenderWatchSlider::Create();
}

} // namespace OHOS::Ace
