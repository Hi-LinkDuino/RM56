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

#include "core/components/test/unittest/video/texture_test_utils.h"

#include <cmath>

#include "core/components/text/text_component.h"

namespace OHOS::Ace {

const Offset MockRenderTexture::GetImageOffset() const
{
    return Offset(alignmentX_, alignmentY_);
}

const Size& MockRenderTexture::GetImageSize() const
{
    return drawSize_;
}

bool IsNearEqual(const Offset& left, const Offset& right)
{
    return std::abs(left.GetX() - right.GetX()) < 1.0 && std::abs(left.GetY() - right.GetY()) < 1.0;
}

bool IsNearEqual(const Size& left, const Size& right)
{
    return std::abs(left.Width() - right.Width()) < 1.0 && std::abs(left.Height() - right.Height()) < 1.0;
}

} // namespace OHOS::Ace
