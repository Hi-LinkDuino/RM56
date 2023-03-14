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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_ANIMATION_MOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_ANIMATION_MOCK_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/animation/flush_event.h"
#include "core/animation/keyframe_animation.h"
#include "core/animation/picture_animation.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/tween_option.h"

namespace OHOS::Ace {

class AnimationMock : public FlushEvent {
    DECLARE_ACE_TYPE(AnimationMock, FlushEvent);

public:
    AnimationMock() = default;
    ~AnimationMock() override = default;

    void OnPostFlush() override;

    void InitController(const WeakPtr<PipelineContext>& context)
    {
        animator_ = AceType::MakeRefPtr<Animator>(context);
    }

    double GetPositionResult() const;
    const RefPtr<Animator>& GetAnimator() const;

    void SetPositionResult(double positionResult);
    bool animationStartStatus_ { false };
    bool animationStopStatus_ { false };
    bool animationIdleStatus_ { false };
    bool animationPauseStatus_ { false };
    AnimationOperation operation_ { AnimationOperation::PLAY };

    bool setRepeatSucc_ { false };
    int32_t postFlushCallTimes_ { 0 };
    int32_t iteration_ { 1 };
    int32_t repeatDoneTimes_ { 0 };
    int32_t animationIntValue_ { 0 };
    int32_t animationIntStopValue_ { 0 };

    int32_t pictureIntValue_ { 0 };
    int32_t animationDuration_ { 0 };
    int32_t startDelay_ { 0 };
    float animationFloatValue_ { 0.0f };
    Color animationColorValue_ { Color::WHITE };

    float keyframeAnimationValue_ { 0.0f };
    std::string pictureStringValue_;
    RefPtr<CurveAnimation<int32_t>> animationInt_;
    RefPtr<CurveAnimation<Color>> animationColor_;
    RefPtr<CurveAnimation<float>> animationFloat_;
    RefPtr<PictureAnimation<int32_t>> pictureInt_;
    RefPtr<PictureAnimation<std::string>> pictureString_;
    RefPtr<KeyframeAnimation<float>> keyframeAnimation_;

private:
    void CreatePictureInterpolators();
    void CreateInterpolators();
    void AddListeners();
    void ExecuteOperation();
    RefPtr<Animator> animator_;
    double positionResult_ { 0.0 };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_ANIMATION_MOCK_H
