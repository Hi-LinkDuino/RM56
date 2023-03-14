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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_JSON_ACE_PAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_JSON_ACE_PAGE_H

#include <string>

#include "base/utils/macros.h"
#include "core/common/ace_page.h"

namespace OHOS::Ace {

class ACE_EXPORT JsonAcePage : public AcePage {
    DECLARE_ACE_TYPE(JsonAcePage, AcePage);

public:
    explicit JsonAcePage(int32_t pageId);
    ~JsonAcePage() override = default;

    RefPtr<PageComponent> BuildPage(const std::string& content) override;
    RefPtr<ComposedComponent> BuildPagePatch(const std::string& content);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_JSON_ACE_PAGE_H
