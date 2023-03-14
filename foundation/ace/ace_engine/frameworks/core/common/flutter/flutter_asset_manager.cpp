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

#include "core/common/flutter/flutter_asset_manager.h"

namespace OHOS::Ace {

RefPtr<Asset> FlutterAssetManager::GetAsset(const std::string& assetName)
{
    if (assetName.empty()) {
        return nullptr;
    }

    for (const auto& provider : providers_) {
        auto fileProvider = AceType::DynamicCast<FlutterAssetProvider>(provider);
        if (fileProvider) {
            auto mapping = fileProvider->GetAsMapping(assetName);
            if (mapping) {
                return AceType::MakeRefPtr<FlutterAsset>(std::move(mapping));
            }
        }
    }
    LOGW("find asset failed, assetName = %{public}s", assetName.c_str());
    return nullptr;
}

std::string FlutterAssetManager::GetAssetPath(const std::string& assetName)
{
    for (const auto& provider : providers_) {
        std::string path = provider->GetAssetPath(assetName);
        if (!path.empty()) {
            return path;
        }
    }
    return "";
}

void FlutterAssetManager::GetAssetList(const std::string& path, std::vector<std::string>& assetList) const
{
    for (const auto& provider : providers_) {
        provider->GetAssetList(path, assetList);
    }
}

} // namespace OHOS::Ace
