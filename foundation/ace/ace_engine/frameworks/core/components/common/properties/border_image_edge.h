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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_IMAGE_EDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_IMAGE_EDGE_H

#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"


namespace OHOS::Ace {
class ACE_EXPORT BorderImageEdge final {
public:
    BorderImageEdge() = default;
    ~BorderImageEdge() = default;
    BorderImageEdge(const Dimension& slice, const Dimension& outset,
        const Dimension& border_image_width, BorderImageRepeat repeat);

    bool HasValue() const;

    bool IsValid() const
    {
        return borderImageWidth_.IsValid();
    }
    const Dimension& GetBorderImageSlice() const
    {
        return borderImageSlice_;
    }

    const Dimension&  GetBorderImageOutset() const
    {
        return borderImageOutset_;
    }

    const Dimension& GetBorderImageWidth() const
    {
        return borderImageWidth_;
    }

    BorderImageRepeat GetBorderImageRepeat() const
    {
        return repeat_;
    }

    bool operator==(const BorderImageEdge& borderImageEdge) const
    {
        return NearEqual(borderImageEdge.GetBorderImageSlice().Value(), borderImageSlice_.Value())
               && (borderImageEdge.GetBorderImageSlice().Unit() == borderImageSlice_.Unit())
               && NearEqual(borderImageEdge.GetBorderImageOutset().Value(), borderImageOutset_.Value())
               && (borderImageEdge.GetBorderImageOutset().Unit() == borderImageOutset_.Unit())
               && NearEqual(borderImageEdge.GetBorderImageWidth().Value(), borderImageWidth_.Value())
               && (borderImageEdge.GetBorderImageWidth().Unit() == borderImageWidth_.Unit())
               && (borderImageEdge.GetBorderImageRepeat() == repeat_);
    }
    void SetBorderImageSlice(const Dimension& slice) {
        borderImageSlice_ = slice;
    }
    void SetBorderImageOutset(const Dimension& outset) {
        borderImageOutset_ = outset;
    }
    void SetBorderImageWidth(const Dimension& width) {
        borderImageWidth_ = width;
    }
    void SetBorderImageRepeat(BorderImageRepeat repeat) {
        repeat_ = repeat;
    }

private:
    Dimension borderImageSlice_;
    Dimension borderImageOutset_;
    Dimension borderImageWidth_;
    BorderImageRepeat repeat_ { BorderImageRepeat::STRETCH };
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_BORDER_IMAGE_EDGE_H
