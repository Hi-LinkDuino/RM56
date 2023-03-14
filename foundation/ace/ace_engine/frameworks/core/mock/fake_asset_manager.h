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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_MOCK_FAKE_ASSET_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_MOCK_FAKE_ASSET_MANAGER_H

#include "base/resource/asset_manager.h"

namespace OHOS::Ace {

class FakeAsset final : public Asset {
public:
    ~FakeAsset() override = default;

    size_t GetSize() const override
    {
        return 0;
    }

    const uint8_t* GetData() const override
    {
        return nullptr;
    }
};

class FakeAssetManager final : public AssetManager {
public:
    ~FakeAssetManager() override = default;

    void PushFront(RefPtr<AssetProvider> provider) override {}

    void PushBack(RefPtr<AssetProvider> provider) override {}

    RefPtr<Asset> GetAsset(const std::string& assetName) override
    {
        return nullptr;
    }

    std::string GetAssetPath(const std::string& assetName) override
    {
        return "";
    }

    void SetLibPath(const std::string& packagePath) override {}

    std::string GetLibPath() const override
    {
        return "";
    }

    void GetAssetList(const std::string& path, std::vector<std::string>& assetList) const override
    {
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_MOCK_FAKE_ASSET_MANAGER_H
