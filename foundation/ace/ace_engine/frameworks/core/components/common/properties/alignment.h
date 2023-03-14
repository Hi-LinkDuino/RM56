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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_ALIGNMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_ALIGNMENT_H

#include <string>

#include "base/geometry/offset.h"
#include "base/geometry/size.h"
#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"

namespace OHOS::Ace {

class ACE_EXPORT Alignment final {
public:
    Alignment() = default;
    ~Alignment() = default;

    double GetHorizontal() const
    {
        return horizontal_;
    }

    double GetVertical() const
    {
        return vertical_;
    }

    bool operator==(const Alignment& other) const
    {
        return NearEqual(horizontal_, other.horizontal_) && NearEqual(vertical_, other.vertical_);
    }

    bool operator!=(const Alignment& other) const
    {
        return !operator==(other);
    }

    static const Offset GetAlignPosition(const Size& parentSize, const Size& childSize, const Alignment& alignment);
    static const Alignment TOP_LEFT;
    static const Alignment TOP_CENTER;
    static const Alignment TOP_RIGHT;
    static const Alignment CENTER_LEFT;
    static const Alignment CENTER;
    static const Alignment CENTER_RIGHT;
    static const Alignment BOTTOM_LEFT;
    static const Alignment BOTTOM_CENTER;
    static const Alignment BOTTOM_RIGHT;

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Alignment (" << std::fixed << std::setprecision(1) << horizontal_ << ", " << vertical_ << ")";
        std::string output = ss.str();
        return output;
    }

    std::string GetAlignmentStr(TextDirection direction) const;

private:
    friend class AlignCreator;
    Alignment(double horizontal, double vertical) : horizontal_(horizontal), vertical_(vertical) {}
    double horizontal_ = 0.0;
    double vertical_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_ALIGNMENT_H
