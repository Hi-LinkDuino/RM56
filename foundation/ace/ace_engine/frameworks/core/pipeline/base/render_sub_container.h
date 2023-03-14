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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_SUB_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_SUB_CONTAINER_H

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderSubContainer : public RenderNode {
    DECLARE_ACE_TYPE(RenderSubContainer, RenderNode);

public:
    RenderSubContainer() : RenderNode(false) {}

    virtual RefPtr<PipelineContext> GetSubPipelineContext() = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_RENDER_SUB_CONTAINER_H
