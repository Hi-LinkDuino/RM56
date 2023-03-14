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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_THEME_THEME_MOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_THEME_THEME_MOCK_H

#include "core/components/theme/resource_adapter.h"

namespace OHOS::Ace {

class ResourceAdapterMock : public virtual ResourceAdapter {
    DECLARE_ACE_TYPE(ResourceAdapterMock, ResourceAdapter);

public:
    ResourceAdapterMock();
    ~ResourceAdapterMock() override = default;

    void Initialize();

    RefPtr<ThemeStyle> GetTheme(int32_t themeId) override;

    Color GetColor(uint32_t resId) override;
    Dimension GetDimension(uint32_t resId) override;
    std::string GetString(uint32_t resId) override;
    std::vector<std::string> GetStringArray(uint32_t resId) const override;
    double GetDouble(uint32_t resId) override;
    int32_t GetInt(uint32_t resId) override;

private:
    ACE_DISALLOW_COPY_AND_MOVE(ResourceAdapterMock);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_THEME_THEME_MOCK_H
