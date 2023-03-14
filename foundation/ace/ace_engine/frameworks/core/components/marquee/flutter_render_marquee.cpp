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

#include "core/components/marquee/flutter_render_marquee.h"

#include "flutter/third_party/icu/source/common/unicode/uchar.h"

#include "base/utils/string_utils.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderMarquee::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(
            0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderMarquee::Paint(RenderContext& context, const Offset& offset)
{
    layer_->SetClip(0.0, GetLayoutSize().Width(), 0.0, GetLayoutSize().Height(), Clip::HARD_EDGE);
    RenderNode::Paint(context, offset);
}

TextDirection FlutterRenderMarquee::GetTextDirection(const std::string& text) const
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