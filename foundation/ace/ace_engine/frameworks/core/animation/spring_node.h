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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_NODE_H

#include "base/memory/ace_type.h"
#include "core/animation/spring_adapter.h"
#include "core/animation/spring_node_listenable.h"

namespace OHOS::Ace {

class SpringNode : public AceType, public SpringNodeListenable {
    DECLARE_ACE_TYPE(SpringNode, AceType);

public:
    explicit SpringNode(int32_t index)
    {
        index_ = index;
    }

    ~SpringNode() override = default;

    static constexpr int32_t UNSET_DISTANCE = -1;

    int32_t GetIndex() const
    {
        return index_;
    }

    void SetIndex(int32_t index)
    {
        index_ = index;
    }

    int32_t GetFrameDelta() const
    {
        return frameDelta_;
    }

    void SetFrameDelta(int32_t frameDelta)
    {
        frameDelta_ = frameDelta;
    }

    void SetAdapter(const RefPtr<SpringAdapter>& adapter)
    {
        if (adapter) {
            adapter_ = adapter;
        }
    }

    void SetMinDecoration(double minDecoration)
    {
        if (GreatOrEqual(minDecoration, 0.0)) {
            minDecoration_ = minDecoration;
        }
    }

    void SetMaxDecoration(double maxDecoration)
    {
        if (GreatOrEqual(maxDecoration, 0.0)) {
            maxDecoration_ = maxDecoration;
        }
    }

    void SetDecoration(double decoration)
    {
        if (GreatOrEqual(decoration, 0.0)) {
            decoration_ = decoration;
        }
    }

    const RefPtr<SpringAdapter>& GetAdapter() const
    {
        return adapter_;
    }

    // Set control node's value
    virtual void SetValue(double value)
    {
        isAnimateToEnd_ = false;
    }

    virtual void EndToValue(double endValue, double velocity)
    {
        isAnimateToEnd_ = true;
    }

    virtual void Cancel() = 0;

    virtual void TransferParams(double stiffness, double damping) = 0;

    virtual double GetValue() const = 0;
    virtual double GetVelocity() const = 0;

    virtual void OnAnimation() = 0;

    void OnUpdate(double value, double velocity) {}

    void OnEnd(double value) {}

    virtual void SetDeltaValue(double delta) {}

    virtual void ResetNode(double value, double velocity) {}

    bool IsRunning() const
    {
        return isRunning_;
    }

    bool IsAnimateToEnd() const
    {
        return isAnimateToEnd_;
    }

    int32_t GetDistance() const
    {
        return distance_;
    }

    void SetDistance(int32_t distance)
    {
        distance_ = distance;
    }

protected:
    uint64_t startTime_ = 0;     // The start time when the node starts to move
    bool isRunning_ = false; // Whether the node is moving
    RefPtr<SpringAdapter> adapter_;
    int32_t index_ = 0; // Node's index
    double decoration_ = 0.0;
    double minDecoration_ = 0.0;
    double maxDecoration_ = 0.0;

private:
    int32_t frameDelta_ = 1;
    bool isAnimateToEnd_ = false;
    int32_t distance_ = UNSET_DISTANCE; // Distance to control point
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SPRING_NODE_H
