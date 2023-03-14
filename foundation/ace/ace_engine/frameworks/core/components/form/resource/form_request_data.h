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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_REQUEST_DATA_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_REQUEST_DATA_H

#include <string>

namespace OHOS::Ace {

struct RequestFormInfo {
    int32_t id = 0;
    std::string cardName;
    std::string bundleName;
    std::string abilityName;
    std::string moduleName;
    bool temporary = false;
    int32_t dimension = -1;
    bool allowUpate = true;
    Dimension width;
    Dimension height;
    uint64_t index = GetNanoseconds();

    std::string ToString() const
    {
        std::stringstream paramStream;
        paramStream << bundleName << abilityName << moduleName << cardName << dimension << index << temporary;
        return paramStream.str();
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_RESOURCE_FORM_REQUEST_DATA_H