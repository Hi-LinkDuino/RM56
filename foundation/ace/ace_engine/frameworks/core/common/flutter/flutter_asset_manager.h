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

#ifndef FOUNDATION_ACE_FRAMEWORKS_COMMON_FLUTTER_FLUTTER_ASSET_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_COMMON_FLUTTER_FLUTTER_ASSET_MANAGER_H

#include <deque>
#include <vector>

#include "flutter/assets/asset_resolver.h"
#include "flutter/fml/mapping.h"

#include "base/log/event_report.h"
#include "base/memory/ace_type.h"
#include "base/resource/asset_manager.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

class FlutterAsset final : public Asset {
public:
    explicit FlutterAsset(std::unique_ptr<fml::Mapping> mapping) : mapping_(std::move(mapping)) {}
    ~FlutterAsset() override = default;

    size_t GetSize() const override
    {
        return mapping_ ? mapping_->GetSize() : 0;
    }

    const uint8_t* GetData() const override
    {
        return mapping_ ? mapping_->GetMapping() : nullptr;
    }

private:
    std::unique_ptr<fml::Mapping> mapping_;
};

class FlutterAssetProvider : public AssetProvider {
    DECLARE_ACE_TYPE(FlutterAssetProvider, AssetProvider);

public:
    virtual std::unique_ptr<fml::Mapping> GetAsMapping(const std::string& assetName) const = 0;
};

class ACE_EXPORT FlutterAssetManager final : public AssetManager {
    DECLARE_ACE_TYPE(FlutterAssetManager, AssetManager);

public:
    FlutterAssetManager() = default;
    ~FlutterAssetManager() override = default;

    void PushFront(RefPtr<AssetProvider> provider) override
    {
        if (!provider || !provider->IsValid()) {
            return;
        }
        providers_.push_front(std::move(provider));
    }

    void PushBack(RefPtr<AssetProvider> provider) override
    {
        if (!provider || !provider->IsValid()) {
            return;
        }
        providers_.push_back(std::move(provider));
    }

    RefPtr<Asset> GetAsset(const std::string& assetName) override;

    std::string GetAssetPath(const std::string& assetName) override;

    void SetLibPath(const std::string& packagePath) override
    {
        packagePath_ = packagePath;
    }

    std::string GetLibPath() const override
    {
        return packagePath_;
    }

    void GetAssetList(const std::string& path, std::vector<std::string>& assetList) const override;

private:
    std::deque<RefPtr<AssetProvider>> providers_;
    std::string packagePath_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_COMMON_FLUTTER_FLUTTER_ASSET_MANAGER_H
