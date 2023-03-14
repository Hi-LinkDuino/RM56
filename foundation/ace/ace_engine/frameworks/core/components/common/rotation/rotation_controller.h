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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_ROTATION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_ROTATION_CONTROLLER_H

#include <functional>

#include "base/memory/ace_type.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class RenderNode;

using RequestRotationImpl = std::function<void(bool)>;

class RotationController : public virtual AceType {
    DECLARE_ACE_TYPE(RotationController, AceType);

public:
    RotationController() = default;
    ~RotationController() override = default;

    void SetRequestRotationImpl(const WeakPtr<RenderNode>& weak, const WeakPtr<PipelineContext>& context)
    {
        requestRotationImpl_ = [weak, context](bool rotation) {
            auto pipeContext = context.Upgrade();
            if (pipeContext) {
                rotation ? pipeContext->SetRequestedRotationNode(weak)
                         : pipeContext->RemoveRequestedRotationNode(weak);
            }
        };
    }

    void RequestRotation(bool rotation)
    {
        if (requestRotationImpl_) {
            requestRotationImpl_(rotation);
        }
    }

private:
    RequestRotationImpl requestRotationImpl_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_ROTATION_CONTROLLER_H
