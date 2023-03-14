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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_BILATERAL_SPRING_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_BILATERAL_SPRING_NODE_H

#include "base/memory/ace_type.h"
#include "core/animation/bilateral_spring_adapter.h"
#include "core/animation/simple_spring_node.h"

namespace OHOS::Ace {

class BilateralSpringNode : public SimpleSpringNode {
    DECLARE_ACE_TYPE(BilateralSpringNode, SimpleSpringNode);

public:
    BilateralSpringNode(const WeakPtr<PipelineContext>& context, int32_t index, double value)
        : SimpleSpringNode(context, index, value) {}
    ~BilateralSpringNode() override = default;

    void NotifyNext(double endValue, double initVelocity) override;

    void SetDeltaValue(double delta) override;

protected:
    void OnAnimation() override;

private:
    void DoCollision();

    void FixValue(double springValue);

    void SimpleFixedValue(double springValue);

    bool switchCollision_ = true;
    bool fixMode_ = false;
    double lastTheoryValue_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_BILATERAL_SPRING_NODE_H
