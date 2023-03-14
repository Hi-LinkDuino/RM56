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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_PATH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_PATH_H

#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"

#include "frameworks/core/components/svg/render_svg_path.h"

namespace OHOS::Ace {

class RosenRenderSvgPath : public RenderSvgPath {
    DECLARE_ACE_TYPE(RosenRenderSvgPath, RenderSvgPath);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

    void PaintDirectly(RenderContext& context, const Offset& offset) override;

    bool HasEffectiveTransform() const override
    {
        return NeedTransform();
    }

    void UpdateMotion(const std::string& path, const std::string& rotate, double percent) override;

    Rect GetPaintBounds(const Offset& offset) override;

protected:
    void GetPath(SkPath& out);

private:
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_PATH_H
