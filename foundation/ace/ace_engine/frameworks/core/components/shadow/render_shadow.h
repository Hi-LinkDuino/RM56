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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHADOW_RENDER_SHADOW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHADOW_RENDER_SHADOW_H

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderShadow : public RenderNode {
    DECLARE_ACE_TYPE(RenderShadow, RenderNode);

public:
    RenderShadow() : RenderNode(true) {}
    ~RenderShadow() override = default;

    static RefPtr<RenderNode> Create();
    void PerformLayout() override;
    void Update(const RefPtr<Component>& component) override {};
    void SetShadowProperties(const RRect& rrect, const Offset& offset);
    void SetShadowProperties(const RRect& rrect, const Offset& offset, const Rect& clipRect);
    void CancelShadow();
    bool MouseHoverTest(const Point& parentLocalPoint) override
    {
        // this node is not required hover state.
        return false;
    }

protected:
    Rect clipRect_;
    RRect rrect_;
    Offset offset_;
    bool isDisplay_ = false;
    bool isNeedClip_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHADOW_RENDER_SHADOW_H
