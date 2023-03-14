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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ASSET_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ASSET_MANAGER_H

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"

namespace OHOS::Ace {

class Asset : public Referenced {
public:
    ~Asset() override = default;

    virtual size_t GetSize() const = 0;
    virtual const uint8_t* GetData() const = 0;
};

class AssetProvider : public AceType {
    DECLARE_ACE_TYPE(AssetProvider, AceType);

public:
    virtual std::string GetAssetPath(const std::string& assetName) = 0;

    virtual void GetAssetList(const std::string& path, std::vector<std::string>& assetList) = 0;

    virtual bool IsValid() const = 0;
};

class AssetManager : public AceType {
    DECLARE_ACE_TYPE(AssetManager, AceType);

public:
    ~AssetManager() override = default;

    virtual void PushFront(RefPtr<AssetProvider> provider) = 0;

    virtual void PushBack(RefPtr<AssetProvider> provider) = 0;

    virtual RefPtr<Asset> GetAsset(const std::string& assetName) = 0;

    virtual std::string GetAssetPath(const std::string& assetName) = 0;

    virtual void SetLibPath(const std::string& packagePath) = 0;

    virtual std::string GetLibPath() const = 0;

    virtual void GetAssetList(const std::string& path, std::vector<std::string>& assetList) const = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_RESOURCE_ASSET_MANAGER_H
