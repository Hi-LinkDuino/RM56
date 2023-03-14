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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FONT_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FONT_MANAGER_H

#include <list>
#include <set>
#include <vector>

#include "base/memory/ace_type.h"
#include "core/common/font_loader.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class FontManager : public virtual AceType {
    DECLARE_ACE_TYPE(FontManager, AceType);

public:
    FontManager() = default;
    ~FontManager() override = default;

    virtual void VaryFontCollectionWithFontWeightScale() = 0;

    virtual void LoadSystemFont() = 0;

    static RefPtr<FontManager> Create();

    void RegisterFont(
        const std::string& familyName, const std::string& familySrc, const RefPtr<PipelineContext>& context);
    void RegisterCallback(
        const WeakPtr<RenderNode>& node, const std::string& familyName, const std::function<void()>& callback);
    void UnRegisterCallback(const WeakPtr<RenderNode>& node);
    const std::vector<std::string>& GetFontNames() const;
    void AddFontNode(const WeakPtr<RenderNode>& node);
    void RemoveFontNode(const WeakPtr<RenderNode>& node);
    void RebuildFontNode();
    void UpdateFontWeightScale();
    void AddVariationNode(const WeakPtr<RenderNode>& node);
    void RemoveVariationNode(const WeakPtr<RenderNode>& node);
    void NotifyVariationNodes();

protected:
    static float fontWeightScale_;

private:
    std::list<RefPtr<FontLoader>> fontLoaders_;
    std::vector<std::string> fontNames_;
    std::set<WeakPtr<RenderNode>> fontNodes_;
    // Render nodes need to layout when wght scale is changed.
    std::set<WeakPtr<RenderNode>> variationNodes_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FONT_MANAGER_H
