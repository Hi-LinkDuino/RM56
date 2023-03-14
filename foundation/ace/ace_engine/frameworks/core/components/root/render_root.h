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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_RENDER_ROOT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_RENDER_ROOT_H

#include "core/animation/animator.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderRoot : public RenderNode {
    DECLARE_ACE_TYPE(RenderRoot, RenderNode);

public:
    ~RenderRoot() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    // Set the design size directly proportional to the real physical pixels.
    void SetScale(float scale)
    {
        scale_ = scale;
    }

    float GetScale() const
    {
        return scale_;
    }

    virtual void DumpLayerTree();

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    void SetReset(bool reset)
    {
        isReset_ = reset;
    }

    void AnimateToShow(int32_t duration);

    void AnimateToHide(int32_t duration);

    void SetBgColor(const Color& color);

    void SetDefaultBgColor();

    void NotifyOnShow() const
    {
        for (const auto& child : GetChildren()) {
            child->OnAppShow();
        }
    }

    void NotifyOnHide() const
    {
        for (const auto& child : GetChildren()) {
            child->OnAppHide();
        }
    }

protected:
    RenderRoot();

    float scale_ = 1.0f;
    Color bgColor_;
    bool forceColor_ = false;
    bool isBgColorInit_ = false;
    bool isReset_ = false;
    RefPtr<Animator> controller_;
    bool isContextMenu_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_RENDER_ROOT_H
