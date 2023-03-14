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

#include "core/components/scroll/scroll_spring_effect.h"

namespace OHOS::Ace {

void ScrollSpringEffect::InitialEdgeEffect()
{
    RegisterSpringCallback();
}

void ScrollSpringEffect::RegisterSpringCallback()
{
    if (scrollable_) {
        scrollable_->SetScrollOverCallBack([weakEffect = AceType::WeakClaim(this)](double velocity) {
            auto effect = weakEffect.Upgrade();
            if (effect) {
                effect->ProcessScrollOver(velocity);
            }
        });
    }
}

void ScrollSpringEffect::ProcessScrollOver(double velocity)
{
    if (currentPositionCallback_ && leadingCallback_ && trailingCallback_) {
        double position = currentPositionCallback_();
        double minExtent = leadingCallback_();
        double maxExtent = trailingCallback_();
        double initMinExtent = initLeadingCallback_();
        double initMaxExtent = initTrailingCallback_();
        if (scrollable_) {
            if (!scrollable_->Available() && minExtent < 0.0) {
                minExtent = 0.0;
            }
            scrollable_->StartSpringMotion(
                position, velocity, ExtentPair(minExtent, maxExtent), ExtentPair(initMinExtent, initMaxExtent));
        }
    }
}

} // namespace OHOS::Ace