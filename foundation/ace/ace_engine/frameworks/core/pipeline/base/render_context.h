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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_CONTEXT_H

#include "base/geometry/offset.h"
#include "base/geometry/rect.h"
#include "base/memory/ace_type.h"
#include "core/pipeline/base/render_layer.h"

namespace OHOS::Ace {

template<typename T>
T* CastLayerAs(RenderLayer layer)
{
    return static_cast<T*>(layer);
}

class RenderNode;
class RenderContext : public virtual AceType {
    DECLARE_ACE_TYPE(RenderContext, AceType);

public:
    ~RenderContext() override = default;

    static RefPtr<RenderContext> Create();

    virtual void Repaint(const RefPtr<RenderNode>&) = 0;
    virtual void PaintChild(const RefPtr<RenderNode>&, const Offset& offset) = 0;
    virtual bool IsIntersectWith(const RefPtr<RenderNode>& child, Offset& offset) { return true; }
    virtual void Restore() = 0;
    virtual void SetClipHole(Rect clipHole)
    {
        clipHole_ = clipHole;
    }

    void SetNeedRestoreHole(bool restore)
    {
        needRestoreHole_ = restore;
    }

    bool GetNeedRestoreHole() const
    {
        return needRestoreHole_;
    }

protected:
    RenderContext() = default;
    Rect clipHole_;
    bool needRestoreHole_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_CONTEXT_H
