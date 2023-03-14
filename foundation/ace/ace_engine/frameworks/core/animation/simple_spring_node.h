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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_NODE_H

#include "core/animation/scheduler.h"
#include "core/animation/spring_motion.h"
#include "core/animation/spring_node.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class SimpleSpringNode : public SpringNode {
    DECLARE_ACE_TYPE(SimpleSpringNode, SpringNode);

public:
    SimpleSpringNode(const WeakPtr<PipelineContext>& context, int32_t index, double value);
    ~SimpleSpringNode() override = default;

    void EndToValue(double endValue, double velocity) override;

    void TransferParams(double stiffness, double damping) override;

    double GetValue() const override
    {
        return value_;
    }

    void SetValue(double value) override;

    void SetDeltaValue(double delta) override
    {
        value_ += delta;
    }

    void ResetNode(double value, double velocity) override;

    double GetVelocity() const override
    {
        return velocity_;
    }

    void SetVelocity(double velocity)
    {
        velocity_ = velocity;
    }

    const RefPtr<SpringProperty> GetTransferParams() const
    {
        return springProperty_;
    }

    void OnAnimation() override;

protected:
    static constexpr double DEFAULT_CHAIN_VALUE_ACCURACY = 1.0;
    virtual void NotifyNext(double endValue, double initVelocity);
    RefPtr<SpringMotion> spring_;
    RefPtr<SpringProperty> springProperty_;
    RefPtr<Scheduler> scheduler_;
    const WeakPtr<PipelineContext> context_;
    double value_ = 0.0;
    double velocity_ = 0.0;
    double valueAccuracy_ = DEFAULT_CHAIN_VALUE_ACCURACY;

    void Cancel() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_NODE_H
