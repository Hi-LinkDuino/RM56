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

#include "base/geometry/dimension.h"
#include "base/utils/string_utils.h"

namespace OHOS::Ace {
    std::string Dimension::ToString() const
    {
        static std::string units[6] = {"px", "vp", "fp", "%", "lpx", "auto"};
        if (units[static_cast<int>(unit_)] == units[3]) {
            return StringUtils::DoubleToString(value_ * 100).append(units[static_cast<int>(unit_)]);
        }
        return StringUtils::DoubleToString(value_).append(units[static_cast<int>(unit_)]);
    }
} // namespace OHOS::Ace