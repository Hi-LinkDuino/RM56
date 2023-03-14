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

#include "core/common/font_manager.h"

#include "base/utils/system_properties.h"
#include "core/components/text/render_text.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

float FontManager::fontWeightScale_ = 1.0f;

void FontManager::RegisterFont(
    const std::string& familyName, const std::string& familySrc, const RefPtr<PipelineContext>& context)
{
    if (std::find(std::begin(fontNames_), std::end(fontNames_), familyName) == std::end(fontNames_)) {
        fontNames_.emplace_back(familyName);
    }

    for (auto iter = fontLoaders_.begin(); iter != fontLoaders_.end(); ++iter) {
        auto& fontLoader = *iter;
        if (fontLoader->GetFamilyName() == familyName) {
            LOGI("Font is already loaded!");
            return;
        }
    }
    RefPtr<FontLoader> fontLoader = FontLoader::Create(familyName, familySrc);
    fontLoaders_.emplace_back(fontLoader);
    fontLoader->AddFont(context);

    fontLoader->SetVariationChanged([weak = WeakClaim(this), familyName]() {
        auto fontManager = weak.Upgrade();
        if (fontManager) {
            fontManager->VaryFontCollectionWithFontWeightScale();
        }
    });
}

void FontManager::RegisterCallback(
    const WeakPtr<RenderNode>& node, const std::string& familyName, const std::function<void()>& callback)
{
    if (!callback) {
        return;
    }
    for (auto& fontLoader : fontLoaders_) {
        if (fontLoader->GetFamilyName() == familyName) {
            fontLoader->SetOnLoaded(node, callback);
        }
    }
}

const std::vector<std::string>& FontManager::GetFontNames() const
{
    return fontNames_;
}

void FontManager::AddFontNode(const WeakPtr<RenderNode>& node)
{
    if (fontNodes_.find(node) == fontNodes_.end()) {
        fontNodes_.emplace(node);
    }
}

void FontManager::RemoveFontNode(const WeakPtr<RenderNode>& node)
{
    fontNodes_.erase(node);
}

void FontManager::RebuildFontNode()
{
    for (auto iter = fontNodes_.begin(); iter != fontNodes_.end();) {
        auto fontNode = iter->Upgrade();
        if (fontNode) {
            fontNode->MarkNeedLayout();
            ++iter;
        } else {
            iter = fontNodes_.erase(iter);
        }
    }
}

void FontManager::UnRegisterCallback(const WeakPtr<RenderNode>& node)
{
    for (auto& fontLoader : fontLoaders_) {
        fontLoader->RemoveCallback(node);
    }
}

void FontManager::UpdateFontWeightScale()
{
    float fontWeightScale = SystemProperties::GetFontWeightScale();
    if (!NearEqual(fontWeightScale, fontWeightScale_)) {
        fontWeightScale_ = fontWeightScale;
        VaryFontCollectionWithFontWeightScale();
    }
}

void FontManager::AddVariationNode(const WeakPtr<RenderNode>& node)
{
    if (variationNodes_.find(node) == variationNodes_.end()) {
        variationNodes_.emplace(node);
    }
}

void FontManager::RemoveVariationNode(const WeakPtr<RenderNode>& node)
{
    variationNodes_.erase(node);
}

void FontManager::NotifyVariationNodes()
{
    for (const auto& node : variationNodes_) {
        auto refNode = node.Upgrade();
        if (refNode) {
            auto text = DynamicCast<RenderText>(refNode);
            if (text) {
                text->MarkNeedMeasure();
            }
            refNode->MarkNeedLayout();
        }
    }
}

} // namespace OHOS::Ace
