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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_CHAIN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_CHAIN_H

#include "base/geometry/dimension.h"
#include "core/animation/param_transfer.h"
#include "core/animation/simple_spring_adapter.h"

namespace OHOS::Ace {

class SpringChainProperty {
public:
    void SetControlStiffness(double stiffness)
    {
        controlStiffness_ = stiffness;
    }

    double ControlStiffness() const
    {
        return controlStiffness_;
    }

    void SetControlDamping(double damping)
    {
        controlDamping_ = damping;
    }

    double ControlDamping() const
    {
        return controlDamping_;
    }

    static const RefPtr<SpringProperty>& GetDefaultOverSpringProperty();

    static double GetDefaultFriction();

    void SetStiffnessTransfer(bool useCurve)
    {
        stiffnessTransfer_ = useCurve;
    }

    bool StiffnessTransfer() const
    {
        return stiffnessTransfer_;
    }

    void SetDampingTransfer(bool useCurve)
    {
        dampingTransfer_ = useCurve;
    }

    bool DampingTransfer() const
    {
        return dampingTransfer_;
    }

    void SetStiffnessCoefficient(double stiffnessCoefficient)
    {
        stiffnessCoefficient_ = stiffnessCoefficient;
    }

    double StiffnessCoefficient() const
    {
        return stiffnessCoefficient_;
    }

    void SetDampingCoefficient(double dampingCoefficient)
    {
        dampingCoefficient_ = dampingCoefficient;
    }

    double DampingCoefficient() const
    {
        return dampingCoefficient_;
    }

    void SetInterval(const Dimension& interval)
    {
        interval_ = interval;
    }

    const Dimension& Interval() const
    {
        return interval_;
    }

    void SetMinInterval(const Dimension& minInterval)
    {
        minInterval_ = minInterval;
    }

    const Dimension& MinInterval() const
    {
        return minInterval_;
    }

    void SetMaxInterval(const Dimension& maxInterval)
    {
        maxInterval_ = maxInterval;
    }

    const Dimension& MaxInterval() const
    {
        return maxInterval_;
    }

    void SetFrameDelay(int32_t frameDelay)
    {
        frameDelay_ = frameDelay;
    }

    int32_t FrameDelay() const
    {
        return frameDelay_;
    }

private:
    static constexpr double CHAIN_CONTROL_DAMPING = 30.0;
    static constexpr double CHAIN_CONTROL_STIFFNESS = 228.0;
    static constexpr Dimension CHAIN_INTERVAL_DEFAULT = 20.0_vp;
    static constexpr Dimension CHAIN_MIN_INTERVAL_DEFAULT = 10.0_vp;
    static constexpr Dimension CHAIN_MAX_INTERVAL_DEFAULT = 40.0_vp;

    double controlStiffness_ { CHAIN_CONTROL_STIFFNESS };
    double controlDamping_ { CHAIN_CONTROL_DAMPING };
    bool stiffnessTransfer_ { true }; // true for curve
    bool dampingTransfer_ { true };
    double dampingCoefficient_ { 0.0 };
    double stiffnessCoefficient_ { 1.0 };
    Dimension interval_ { CHAIN_INTERVAL_DEFAULT };
    Dimension minInterval_ { CHAIN_MIN_INTERVAL_DEFAULT };
    Dimension maxInterval_ { CHAIN_MAX_INTERVAL_DEFAULT };
    int32_t frameDelay_ { 1 };
};

class SimpleSpringChain : public SpringAdapter::AdapterObserve {
    DECLARE_ACE_TYPE(SimpleSpringChain, SpringAdapter::AdapterObserve);

public:
    explicit SimpleSpringChain(const RefPtr<SpringAdapter>& springAdapter);
    ~SimpleSpringChain() override = default;

    void OnControlNodeChange() override
    {
        TransferParamsInternal();
    }

    void OnNodeAdd(RefPtr<SpringNode>& node) override;

    void OnNodeDelete(RefPtr<SpringNode>& node) override;

    void SetDeltaValue(double delta);

    void SetValue(double value);

    void FlushAnimation();

    void EndToPosition(double value, double velocity);

    void Cancel();

    RefPtr<SpringNode> GetControlNode() const
    {
        return springAdapter_->GetControlNode();
    }

    int32_t GetSize() const
    {
        return springAdapter_->GetSize();
    }

    double GetControlStiffness() const
    {
        return controlStiffness_;
    }

    void SetControlStiffness(double controlStiffness)
    {
        if (controlStiffness > 0.0) {
            controlStiffness_ = controlStiffness;
        }
    }

    double GetControlDamping() const
    {
        return controlDamping_;
    }

    void SetControlDamping(double controlDamping)
    {
        if (controlDamping > 0.0) {
            controlDamping_ = controlDamping;
        }
    }

    const RefPtr<SpringAdapter>& GetSpringAdapter() const
    {
        return springAdapter_;
    }

    void SetSpringAdapter(const RefPtr<SpringAdapter>& springAdapter)
    {
        if (springAdapter) {
            springAdapter_ = springAdapter;
        }
    }

    int32_t GetFrameDelta() const
    {
        return frameDelta_;
    }

    void SetFrameDelta(int32_t frameDelta)
    {
        frameDelta_ = frameDelta;
    }

    void SetDampingTransfer(const RefPtr<ParamTransfer>& dampingTransfer)
    {
        if (dampingTransfer) {
            dampingTransfer_ = dampingTransfer;
        }
    }

    void SetStiffnessTransfer(const RefPtr<ParamTransfer>& stiffnessTransfer)
    {
        if (stiffnessTransfer) {
            stiffnessTransfer_ = stiffnessTransfer;
        }
    }

    void SetDecoration(double decoration)
    {
        decoration_ = decoration;
    }

    void SetMinDecoration(double minDecoration)
    {
        minDecoration_ = minDecoration;
    }

    void SetMaxDecoration(double maxDecoration)
    {
        maxDecoration_ = maxDecoration;
    }

protected:
    RefPtr<SpringAdapter> springAdapter_;

private:
    void TransferParamsInternal();
    void SetParams(RefPtr<SpringNode>& node);

    double controlStiffness_ = 0.0;
    double controlDamping_ = 0.0;
    double decoration_ = 0.0;
    double minDecoration_ = 0.0;
    double maxDecoration_ = 0.0;
    int32_t frameDelta_ = 0;
    RefPtr<ParamTransfer> stiffnessTransfer_;
    RefPtr<ParamTransfer> dampingTransfer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SIMPLE_SPRING_CHAIN_H
