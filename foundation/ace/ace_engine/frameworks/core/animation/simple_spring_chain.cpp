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

#include "core/animation/simple_spring_chain.h"

#include "base/utils/utils.h"

namespace OHOS::Ace {
namespace {

constexpr double DEFAULT_CHAIN_STIFFNESS = 228.0;
constexpr double DEFAULT_CHAIN_DAMPING = 30.0;
const RefPtr<SpringProperty> DEFAULT_OVER_SPRING_PROPERTY =
    AceType::MakeRefPtr<SpringProperty>(1.0, DEFAULT_CHAIN_STIFFNESS, DEFAULT_CHAIN_DAMPING);
constexpr double DEFAULT_FRICTION = 1.0;
constexpr int32_t DEFAULT_CHAIN_FRAME_DELTA = 1;

} // namespace

SimpleSpringChain::SimpleSpringChain(const RefPtr<SpringAdapter>& springAdapter)
{
    if (springAdapter) {
        springAdapter_ = springAdapter;
    } else {
        springAdapter_ = AceType::MakeRefPtr<SimpleSpringAdapter>();
    }
    springAdapter_->SetObserve(AceType::WeakClaim(this));
    stiffnessTransfer_ = AceType::MakeRefPtr<ExpParamTransfer>(1.0);
    dampingTransfer_ = AceType::MakeRefPtr<ExpParamTransfer>(0.0);
    controlStiffness_ = DEFAULT_CHAIN_STIFFNESS;
    controlDamping_ = DEFAULT_CHAIN_DAMPING;
    frameDelta_ = DEFAULT_CHAIN_FRAME_DELTA;
}

void SimpleSpringChain::OnNodeAdd(RefPtr<SpringNode>& node)
{
    if (!node) {
        return;
    }
    SetParams(node);
}

void SimpleSpringChain::OnNodeDelete(RefPtr<SpringNode>& node)
{
    if (!node) {
        return;
    }
    auto next = springAdapter_->GetNext(node);
    while (next) {
        next->SetIndex(next->GetIndex() - 1);
        SetParams(next);
        next = springAdapter_->GetNext(next);
    }
}

void SimpleSpringChain::SetDeltaValue(double delta)
{
    for (int32_t index = 0; index < springAdapter_->GetSize(); index++) {
        auto node = springAdapter_->GetNode(index);
        if (node) {
            node->SetDeltaValue(delta);
        }
    }
}

void SimpleSpringChain::SetValue(double value)
{
    auto node = springAdapter_->GetControlNode();
    if (node) {
        node->SetValue(value);
    }
}

void SimpleSpringChain::FlushAnimation()
{
    // flush from control to edge.
    springAdapter_->FlushAnimation();
}

void SimpleSpringChain::EndToPosition(double value, double velocity)
{
    auto node = springAdapter_->GetControlNode();
    if (node) {
        node->EndToValue(value, velocity);
    }
}

void SimpleSpringChain::Cancel()
{
    auto currentNode = springAdapter_->GetControlNode();
    for (int32_t idx = 0; idx < springAdapter_->GetSize(); idx++) {
        if (currentNode) {
            currentNode->Cancel();
            currentNode = springAdapter_->GetNext(currentNode);
        } else {
            LOGW("Node size mismatch. size: %{public}d, current size: %{public}d", springAdapter_->GetSize(), idx + 1);
            break;
        }
    }
}

void SimpleSpringChain::TransferParamsInternal()
{
    for (int32_t idx = 0; idx < springAdapter_->GetSize(); idx++) {
        auto currentNode = springAdapter_->GetNode(idx);
        SetParams(currentNode);
    }
}

void SimpleSpringChain::SetParams(RefPtr<SpringNode>& node)
{
    int32_t index = node->GetIndex();
    auto controlNode = springAdapter_->GetControlNode();
    if (!controlNode) {
        controlNode = node;
    }
    int32_t delta = std::abs(index - controlNode->GetIndex());
    double transferStiffness = stiffnessTransfer_->Transfer(controlStiffness_, delta);
    double transferDamping = dampingTransfer_->Transfer(controlDamping_, delta);
    node->TransferParams(transferStiffness, transferDamping);
    node->SetFrameDelta(frameDelta_);
    node->SetDecoration(decoration_);
    // make sure minDecoration <= decoration <= maxDecoration
    node->SetMinDecoration(std::min(minDecoration_, decoration_));
    node->SetMaxDecoration(std::max(maxDecoration_, decoration_));
    if (!node->GetAdapter()) {
        node->SetAdapter(springAdapter_);
    }
}

const RefPtr<SpringProperty>& SpringChainProperty::GetDefaultOverSpringProperty()
{
    return DEFAULT_OVER_SPRING_PROPERTY;
}

double SpringChainProperty::GetDefaultFriction()
{
    return DEFAULT_FRICTION;
}

} // namespace OHOS::Ace