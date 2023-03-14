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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_PROGRESS_PROGRESS_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_PROGRESS_PROGRESS_TEST_UTILS_H

#include "core/components/progress/render_loading_progress.h"

namespace OHOS::Ace {
namespace {

class MockRenderLoadingProgress final : public RenderLoadingProgress {
    DECLARE_ACE_TYPE(MockRenderLoadingProgress, RenderLoadingProgress);

public:
    MockRenderLoadingProgress() = default;
    ~MockRenderLoadingProgress() override = default;

    double GetCometRadius() const
    {
        return cometRadius_;
    }

    bool CheckLoadingMode(int32_t value)
    {
        return value == loadingMode_;
    }

    bool CheckDiameter(double value)
    {
        return NearEqual(value, diameter_);
    }

    bool CheckCenter(Offset value)
    {
        return value == center_;
    }

    bool CheckScale(double value)
    {
        return NearEqual(value, scale_);
    }

    bool CheckMoveRatio(double value)
    {
        return NearEqual(value, moveRatio_);
    }

    bool CheckRingWidth(double value)
    {
        return NearEqual(value, ringWidth_);
    }

    bool CheckRingRadius(double value)
    {
        return NearEqual(value, ringRadius_);
    }

    bool CheckOrbitRadius(double value)
    {
        return NearEqual(value, orbitRadius_);
    }

    bool CheckCometRadius(double value)
    {
        return NearEqual(value, cometRadius_);
    }

    bool CheckCometTailLen(double value)
    {
        return NearEqual(value, cometTailLen_);
    }

    bool CheckProgressColor(Color value)
    {
        return value == progressColor_;
    }

    bool CheckRingBgWidth(double value)
    {
        return NearEqual(value, ringBgWidth_);
    }

    bool CheckRingBlurRadius(double value)
    {
        return NearEqual(value, ringBlurRadius_);
    }

    bool CheckRingBgBlurRadius(double value)
    {
        return NearEqual(value, ringBgBlurRadius_);
    }

};

} // namespace

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_PROGRESS_PROGRESS_TEST_UTILS_H
