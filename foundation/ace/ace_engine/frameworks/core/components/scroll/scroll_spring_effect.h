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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_SPRING_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_SPRING_EFFECT_H

#include "base/utils/macros.h"
#include "core/components/scroll/scroll_edge_effect.h"

namespace OHOS::Ace {

class ACE_EXPORT ScrollSpringEffect final : public ScrollEdgeEffect {
    DECLARE_ACE_TYPE(ScrollSpringEffect, ScrollEdgeEffect);

public:
    ScrollSpringEffect() : ScrollEdgeEffect(EdgeEffect::SPRING) {}
    ~ScrollSpringEffect() override = default;

    void RegisterSpringCallback() override;
    void InitialEdgeEffect() override;
    void ProcessScrollOver(double velocity) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_SPRING_EFFECT_H
