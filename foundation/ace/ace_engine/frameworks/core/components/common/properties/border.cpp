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

#include "core/components/common/properties/border.h"

namespace OHOS::Ace {

Border::Border(const BorderEdge& left, const BorderEdge& top, const BorderEdge& right, const BorderEdge& bottom)
    : left_(left), top_(top), right_(right), bottom_(bottom)
{}

Border::Border(const BorderImageEdge& leftImage, const BorderImageEdge& topImage,
    const BorderImageEdge& rightImage, const BorderImageEdge& bottomImage)
    : borderImageLeft_(leftImage), borderImageTop_(topImage),
    borderImageRight_(rightImage), borderImageBottom_(bottomImage)
{}

bool Border::IsAllEqual() const
{
    return (left_ == top_) && (top_ == right_) && (right_ == bottom_);
}

bool Border::HasValue() const
{
    return left_.HasValue() || top_.HasValue() || right_.HasValue() || bottom_.HasValue();
}

bool Border::HasRadius() const
{
    return topLeftRadius_.HasValue() || topRightRadius_.HasValue() ||
        bottomLeftRadius_.HasValue() || bottomRightRadius_.HasValue();
}

Offset Border::GetOffset(double dipScale) const
{
    return Offset(left_.GetWidthInPx(dipScale), top_.GetWidthInPx(dipScale));
}

double Border::HorizontalWidth(double dipScale) const
{
    return left_.GetWidthInPx(dipScale) + right_.GetWidthInPx(dipScale);
}

double Border::VerticalWidth(double dipScale) const
{
    return top_.GetWidthInPx(dipScale) + bottom_.GetWidthInPx(dipScale);
}

Size Border::GetLayoutSize(double dipScale) const
{
    return Size(HorizontalWidth(dipScale), VerticalWidth(dipScale));
}

BorderEdge Border::GetValidEdge() const
{
    if (left_.HasValue()) {
        return left_;
    }
    if (top_.HasValue()) {
        return top_;
    }
    if (right_.HasValue()) {
        return right_;
    }
    if (bottom_.HasValue()) {
        return bottom_;
    }
    return BorderEdge();
}

} // namespace OHOS::Ace
