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

#include "core/components/common/properties/edge.h"

#include <vector>

#include "frameworks/base/utils/string_utils.h"

namespace OHOS::Ace {
namespace {

double GetValueInPx(const Dimension& value, double dipScale)
{
    if (value.Unit() == DimensionUnit::VP || value.Unit() == DimensionUnit::FP) {
        return value.Value() * dipScale;
    } else {
        return value.Value();
    }
}

} // namespace

const Edge Edge::NONE = Edge(0.0, 0.0, 0.0, 0.0);

bool Edge::FromString(const std::string& value, Edge& edge)
{
    bool result = true;
    std::vector<std::string> offsets;
    StringUtils::StringSpliter(value, ' ', offsets);
    switch (offsets.size()) {
        case 1: // example: 1px
            edge.top_ = StringUtils::StringToDimension(offsets[0]);
            edge.right_ = StringUtils::StringToDimension(offsets[0]);
            edge.bottom_ = StringUtils::StringToDimension(offsets[0]);
            edge.left_ = StringUtils::StringToDimension(offsets[0]);
            break;
        case 2: // example: 1px 2px
            edge.top_ = StringUtils::StringToDimension(offsets[0]);
            edge.right_ = StringUtils::StringToDimension(offsets[1]);
            edge.bottom_ = StringUtils::StringToDimension(offsets[0]);
            edge.left_ = StringUtils::StringToDimension(offsets[1]);
            break;
        case 3: // example: 1px 2px 3px
            edge.top_ = StringUtils::StringToDimension(offsets[0]);
            edge.right_ = StringUtils::StringToDimension(offsets[1]);
            edge.bottom_ = StringUtils::StringToDimension(offsets[2]);
            edge.left_ = StringUtils::StringToDimension(offsets[1]);
            break;
        case 4: // example: 1px 2px 3px 4px
            edge.top_ = StringUtils::StringToDimension(offsets[0]);
            edge.right_ = StringUtils::StringToDimension(offsets[1]);
            edge.bottom_ = StringUtils::StringToDimension(offsets[2]);
            edge.left_ = StringUtils::StringToDimension(offsets[3]);
            break;
        default:
            result = false;
            break;
    }
    return result;
}

bool Edge::IsValid() const
{
    return left_.Value() >= 0.0 && top_.Value() >= 0.0 && right_.Value() >= 0.0 && bottom_.Value() >= 0.0;
}

bool Edge::IsEffective() const
{
    return left_.Value() > 0.0 || top_.Value() > 0.0 || right_.Value() > 0.0 || bottom_.Value() > 0.0;
}

Size Edge::GetLayoutSizeInPx(double dipScale) const
{
    double width = GetValueInPx(left_, dipScale) + GetValueInPx(right_, dipScale);
    double height = GetValueInPx(top_, dipScale) + GetValueInPx(bottom_, dipScale);
    return Size(width, height);
}

Offset Edge::GetOffsetInPx(double dipScale) const
{
    return Offset(GetValueInPx(left_, dipScale), GetValueInPx(top_, dipScale));
}

double Edge::HorizontalInPx(double dipScale) const
{
    return GetValueInPx(left_, dipScale) + GetValueInPx(right_, dipScale);
}

double Edge::VerticalInPx(double dipScale) const
{
    return GetValueInPx(top_, dipScale) + GetValueInPx(bottom_, dipScale);
}

} // namespace OHOS::Ace