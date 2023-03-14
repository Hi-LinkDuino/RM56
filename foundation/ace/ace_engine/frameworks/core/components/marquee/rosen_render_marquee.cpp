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

#include "core/components/marquee/rosen_render_marquee.h"

#include "flutter/third_party/icu/source/common/unicode/uchar.h"
#include "render_service_client/core/ui/rs_node.h"

#include "base/utils/string_utils.h"

namespace OHOS::Ace {

void RosenRenderMarquee::Update(const RefPtr<Component>& component)
{
    RenderMarquee::Update(component);
    if (auto rsNode = GetRSNode()) {
        rsNode->SetClipToFrame(true);
    }
}

TextDirection RosenRenderMarquee::GetTextDirection(const std::string& text) const
{
    auto wstring = StringUtils::ToWstring(text);
    // Find first strong direction char.
    for (const auto& charInStr : wstring) {
        auto direction = u_charDirection(charInStr);
        if (direction == UCharDirection::U_LEFT_TO_RIGHT) {
            return TextDirection::LTR;
        }
        if (direction == UCharDirection::U_RIGHT_TO_LEFT || direction == UCharDirection::U_RIGHT_TO_LEFT_ARABIC) {
            return TextDirection::RTL;
        }
    }
    return TextDirection::INHERIT;
}

} // namespace OHOS::Ace