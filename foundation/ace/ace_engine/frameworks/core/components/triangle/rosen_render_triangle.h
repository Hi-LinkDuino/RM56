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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_ROSEN_RENDER_TRIANGLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_ROSEN_RENDER_TRIANGLE_H

#include "core/components/triangle/render_triangle.h"

namespace OHOS::Ace {

class RosenRenderTriangle final : public RenderTriangle {
    DECLARE_ACE_TYPE(RosenRenderTriangle, RenderTriangle);

public:
    RosenRenderTriangle() = default;
    ~RosenRenderTriangle() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_ROSEN_RENDER_TRIANGLE_H
