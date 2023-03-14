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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_BILATERAL_SPRING_ADAPTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_BILATERAL_SPRING_ADAPTER_H

#include "base/memory/ace_type.h"
#include "core/animation/simple_spring_adapter.h"

namespace OHOS::Ace {

class BilateralSpringAdapter : public SimpleSpringAdapter {
    DECLARE_ACE_TYPE(BilateralSpringAdapter, SimpleSpringAdapter);

public:
    int32_t GetControlIndex() const override
    {
        return nodes_.size() / 2;
    }

    double ResetControl(int32_t delta);

    void FlushAnimation() override;

    void SetDeltaValue(double delta) override;

private:
    void MoveNode(const RefPtr<SpringNode>& srcNode, int32_t dstIndex);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_BILATERAL_SPRING_ADAPTER_H
