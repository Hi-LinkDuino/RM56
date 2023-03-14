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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_H

#include "frameworks/core/components/svg/render_svg_base.h"
#include "frameworks/core/components/svg/svg_component.h"

namespace OHOS::Ace {

class RenderSvg : public RenderSvgBase {
    DECLARE_ACE_TYPE(RenderSvg, RenderSvgBase);

public:
    static RefPtr<RenderNode> Create();
    ~RenderSvg() override;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool PrepareSelfAnimation(const RefPtr<SvgAnimate>& svgAnimate) override;

    const Rect& GetViewBox() const
    {
        return viewBox_;
    }

    const Dimension& GetX() const
    {
        return x_;
    }

    const Dimension& GetY() const
    {
        return y_;
    }

    const Dimension& GetWidth() const
    {
        return width_;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }

    bool IsRoot() const
    {
        return isRoot_;
    }

    void AddHrefNode(const std::string& id, const RefPtr<RenderSvgBase>& mask)
    {
        if (id.empty() || mask == nullptr) {
            return;
        }
        svgHrefNodes_[id] = mask;
    }

    RefPtr<RenderSvgBase> GetHrefNode(const std::string& id)
    {
        if (svgHrefNodes_.find(id) == svgHrefNodes_.end()) {
            return nullptr;
        }
        return svgHrefNodes_[id].Upgrade();
    }

    void AddHrefComponent(const std::string& id, const RefPtr<Component>& component)
    {
        if (id.empty() || component == nullptr) {
            return;
        }
        svgHrefComponents_[id] = component;
    }

    RefPtr<Component> GetHrefComponent(const std::string& id)
    {
        if (svgHrefComponents_.find(id) == svgHrefComponents_.end()) {
            return nullptr;
        }
        return svgHrefComponents_[id].Upgrade();
    }

    void AddHrefDeclaration(const std::string& id, const RefPtr<SvgBaseDeclaration>& declaration)
    {
        if (id.empty() || declaration == nullptr) {
            return;
        }
        svgHrefDeclarations_[id] = declaration;
    }

    RefPtr<SvgBaseDeclaration> GetHrefDeclaration(const std::string& id)
    {
        if (svgHrefDeclarations_.find(id) == svgHrefDeclarations_.end()) {
            return nullptr;
        }
        return svgHrefDeclarations_[id].Upgrade();
    }

    void MarkIsFixSize(bool isFixSize)
    {
        isFixSize_ = isFixSize;
    }

    void SetRootOpacity(uint8_t alpha)
    {
        rootOpacity_ = alpha;
    }

    void SetRootRotate(double rotate)
    {
        rootRotate_ = rotate;
    }

protected:
    Rect viewBox_;
    bool isRoot_ = false;
    uint8_t rootOpacity_ = 255;
    double rootRotate_ = 0.0;

private:
    void PrepareAnimations();
    void AddSvgAnimations(const RefPtr<SvgComponent>& svgComponent);
    bool SetProperty(const std::string& attrName, const Dimension& value);
    bool GetProperty(const std::string& attrName, Dimension& dimension) const;
    void SetOpacityCallback();
    bool OpacityAnimation(const RefPtr<SvgAnimate>& svgAnimate);
    void UpdateTransform();

    Dimension x_;
    Dimension y_;
    Dimension width_ = Dimension(-1.0);
    Dimension height_ = Dimension(-1.0);
    bool autoMirror_ = false;
    std::vector<RefPtr<SvgAnimate>> svgAnimates_;
    bool hasUpdated_ = false;
    std::function<void(double)> opacityCallback_;
    std::map<std::string, WeakPtr<RenderSvgBase>> svgHrefNodes_;
    std::map<std::string, WeakPtr<Component>> svgHrefComponents_;
    std::map<std::string, WeakPtr<SvgBaseDeclaration>> svgHrefDeclarations_;
    bool isFixSize_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_RENDER_SVG_H
