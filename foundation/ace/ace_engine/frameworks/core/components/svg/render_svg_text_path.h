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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_TEXT_PATH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_TEXT_PATH_H

#include "base/memory/ace_type.h"

#include "frameworks/core/animation/animator.h"
#include "frameworks/core/components/common/properties/svg_paint_state.h"
#include "frameworks/core/components/svg/render_svg_base.h"
#include "frameworks/core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderSvgTextPath : public RenderSvgBase {
    DECLARE_ACE_TYPE(RenderSvgTextPath, RenderSvgBase)

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate) override;

    const std::string& GetTextData() const
    {
        return textData_;
    }

    void SetTextData(const std::string& textData)
    {
        textData_ = textData;
    }

    const std::string& GetPath() const
    {
        return path_;
    }

    void SetPath(const std::string& path)
    {
        path_ = path;
    }

    void SetStartOffset(const Dimension& startOffset)
    {
        startOffset_ = startOffset;
    }

    const Dimension& GetStartOffset() const
    {
        return startOffset_;
    }

protected:
    Dimension startOffset_ = Dimension(0.0);
    std::string path_;
    std::string textData_;

private:
    void PrepareAnimations(const RefPtr<Component>& component);
    bool SetProperty(const std::string& attrName, const Dimension& value);
    bool GetProperty(const std::string& attrName, Dimension& dimension) const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_TEXT_PATH_H
