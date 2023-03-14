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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_RENDER_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_RENDER_FACTORY_H

#include "core/pipeline/base/factories/render_node_factory.h"

namespace OHOS::Ace {

class RenderFactory : public virtual AceType {
    DECLARE_ACE_TYPE(RenderFactory, AceType);

public:
    RenderFactory() = default;
    ~RenderFactory() override = default;

    virtual RefPtr<RenderNodeFactory>& GetRenderTextFactory() = 0;

    virtual RefPtr<RenderNodeFactory>& GetRenderImageFactory() = 0;

    virtual RefPtr<RenderNodeFactory>& GetRenderBoxFactory() = 0;

    virtual RefPtr<RenderNodeFactory>& GetRenderFlexFactory() = 0;

    virtual RefPtr<RenderNodeFactory>& GetRenderFlexItemFactory() = 0;

    virtual RefPtr<RenderNodeFactory>& GetRenderWrapFactory() = 0;

private:
    ACE_DISALLOW_COPY_AND_MOVE(RenderFactory);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_FACTORIES_RENDER_FACTORY_H
