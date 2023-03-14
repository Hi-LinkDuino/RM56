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

#include "core/components/font/flutter_font_loader.h"

#include "flutter/lib/ui/text/font_collection.h"

#include "base/network/download_manager.h"
#include "core/components/font/flutter_font_collection.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

FlutterFontLoader::FlutterFontLoader(const std::string& familyName, const std::string& familySrc)
    : FontLoader(familyName, familySrc) {}

void FlutterFontLoader::AddFont(const RefPtr<PipelineContext>& context)
{
    if (familySrc_.empty()) {
        return;
    }

    if (familySrc_.substr(0, 4) == FONT_SRC_NETWORK) {
        // Get font from NetWork.
        LoadFromNetwork(context);
    } else {
        // Get font from asset.
        LoadFromAsset(context);
    }
}

void FlutterFontLoader::LoadFromNetwork(const OHOS::Ace::RefPtr<OHOS::Ace::PipelineContext>& context)
{
    auto weakContext = AceType::WeakClaim(AceType::RawPtr(context));
    context->GetTaskExecutor()->PostTask([weak = AceType::WeakClaim(this), weakContext] {
        auto fontLoader = weak.Upgrade();
        auto context = weakContext.Upgrade();
        if (!fontLoader || !context) {
            return;
        }
        std::vector<uint8_t> fontData;
        if (!DownloadManager::GetInstance().Download(fontLoader->familySrc_, fontData) || fontData.empty()) {
            return;
        }
        context->GetTaskExecutor()->PostTask([fontData, weak] {
            auto fontLoader = weak.Upgrade();
            if (!fontLoader) {
                return;
            }
            // Load font.
            FlutterFontCollection::GetInstance().LoadFontFromList(
                fontData.data(), fontData.size(), fontLoader->familyName_);
            fontLoader->isLoaded_ = true;

            // When font is already loaded, notify all which used this font.
            for (const auto& [node, callback] : fontLoader->callbacks_) {
                if (callback) {
                    callback();
                }
            }
            fontLoader->callbacks_.clear();
            if (fontLoader->variationChanged_) {
                fontLoader->variationChanged_();
            }
        }, TaskExecutor::TaskType::UI);
    }, TaskExecutor::TaskType::BACKGROUND);
}

void FlutterFontLoader::LoadFromAsset(const OHOS::Ace::RefPtr<OHOS::Ace::PipelineContext>& context)
{
    auto weakContext = AceType::WeakClaim(AceType::RawPtr(context));
    context->GetTaskExecutor()->PostTask([weak = AceType::WeakClaim(this), weakContext] {
        auto fontLoader = weak.Upgrade();
        auto context = weakContext.Upgrade();
        if (!fontLoader || !context) {
            return;
        }
        auto assetManager = context->GetAssetManager();
        if (!assetManager) {
            LOGE("No asset manager!");
            return;
        }
        std::string assetSrc(fontLoader->familySrc_);
        if (assetSrc[0] == '/') {
            assetSrc = assetSrc.substr(1); // get the asset src without '/'.
        } else if (assetSrc[0] == '.' && assetSrc.size() > 2 && assetSrc[1] == '/') {
            assetSrc = assetSrc.substr(2); // get the asset src without './'.
        }
        auto assetData = assetManager->GetAsset(assetSrc);
        if (!assetData) {
            LOGE("No asset data!");
            return;
        }

        context->GetTaskExecutor()->PostTask([assetData, weak] {
            auto fontLoader = weak.Upgrade();
            if (!fontLoader) {
                return;
            }
            // Load font.
            FlutterFontCollection::GetInstance().LoadFontFromList(
                    assetData->GetData(), assetData->GetSize(), fontLoader->familyName_);
            fontLoader->isLoaded_ = true;

            for (const auto& [node, callback] : fontLoader->callbacks_) {
                if (callback) {
                    callback();
                }
            }
            fontLoader->callbacks_.clear();
            if (fontLoader->variationChanged_) {
                fontLoader->variationChanged_();
            }
        }, TaskExecutor::TaskType::UI);
    }, TaskExecutor::TaskType::BACKGROUND);
}

} // namespace OHOS::Ace
