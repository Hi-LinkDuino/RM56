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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_BORDER_CREATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_BORDER_CREATOR_H

#include "base/json/json_util.h"
#include "core/components/common/properties/border.h"

namespace OHOS::Ace {

// Creator for creating Border property from json.
class BorderCreator final {
public:
    static std::unique_ptr<Border> CreateFromJson(const JsonValue& json);

private:
    static void UpdateRadiusFromJson(const std::unique_ptr<Border>& border, const JsonValue& json);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_BORDER_CREATOR_H
