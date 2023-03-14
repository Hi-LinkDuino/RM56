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

#include "core/animation/test/unittest/mock/animation_mock.h"

#include "base/log/log.h"
#include "core/components/dialog_modal/dialog_modal_component.h"
#include "core/components/semi_modal/semi_modal_component.h"
#include "core/image/image_loader.h"

namespace OHOS::Ace {

void AnimationMock::OnPostFlush()
{
    LOGD("AnimationMock PostFlush start");
    postFlushCallTimes_++;
    CreateInterpolators();
    AddListeners();
    animator_->SetDuration(animationDuration_);
    setRepeatSucc_ = animator_->SetIteration(iteration_);
    animator_->SetStartDelay(startDelay_);
    ExecuteOperation();
    LOGD("AnimationMock PostFlush end");
}

void AnimationMock::ExecuteOperation()
{
    switch (operation_) {
        case AnimationOperation::PLAY:
            animator_->Play();
            break;
        case AnimationOperation::REVERSE:
            animator_->Reverse();
            break;
        case AnimationOperation::FINISH:
            animator_->Finish();
            break;
        case AnimationOperation::PAUSE:
            animator_->Pause();
            break;
        case AnimationOperation::CANCEL:
            animator_->Cancel();
            break;
        case AnimationOperation::NONE:
        default:
            break;
    }
}

void AnimationMock::AddListeners()
{
    auto weak = AceType::WeakClaim(this);
    animator_->AddStartListener([weak]() {
        auto simulator = weak.Upgrade();
        if (simulator) {
            LOGD("AnimationMock vsync triggered. start listener called.");
            simulator->animationStartStatus_ = true;
        }
    });
    animator_->AddStopListener([weak]() {
        auto simulator = weak.Upgrade();
        if (simulator) {
            LOGD("AnimationMock vsync triggered. stop listener called.");
            simulator->animationStopStatus_ = true;
            simulator->animationIntStopValue_ = simulator->animationIntValue_;
        }
    });
    animator_->AddRepeatListener([weak]() {
        auto simulator = weak.Upgrade();
        if (simulator) {
            LOGD("AnimationMock vsync triggered. repeat listener called.");
            ++(simulator->repeatDoneTimes_);
        }
    });
    animator_->AddIdleListener([weak]() {
        auto simulator = weak.Upgrade();
        if (simulator) {
            LOGD("AnimationMock vsync triggered. idle listener called.");
            simulator->animationIdleStatus_ = true;
        }
    });
    animator_->AddPauseListener([weak]() {
        auto simulator = weak.Upgrade();
        if (simulator) {
            LOGD("AnimationMock vsync triggered. resume listener called.");
            simulator->animationPauseStatus_ = true;
        }
    });
}

void AnimationMock::CreatePictureInterpolators()
{
    auto weak = AceType::WeakClaim(this);
    if (pictureInt_) {
        pictureInt_->AddListener([weak](const int& value) {
            auto simulator = weak.Upgrade();
            if (simulator) {
                LOGD("AnimationMock vsync triggered. pictureIntValue_ int value: %{public}d", value);
                simulator->pictureIntValue_ = value;
            }
        });
        animator_->AddInterpolator(pictureInt_);
    }
    if (pictureString_) {
        pictureString_->AddListener([weak](const std::string& value) {
            auto simulator = weak.Upgrade();
            if (simulator) {
                LOGD("AnimationMock vsync triggered. pictureStringValue_ int value: %{public}s", value.data());
                simulator->pictureStringValue_ = value;
            }
        });
        animator_->AddInterpolator(pictureString_);
    }
}

void AnimationMock::CreateInterpolators()
{
    auto weak = AceType::WeakClaim(this);
    if (animationInt_) {
        animationInt_->AddListener([weak](const int& value) {
            auto simulator = weak.Upgrade();
            if (simulator) {
                LOGD("AnimationMock vsync triggered. int value: %{public}d", value);
                simulator->animationIntValue_ = value;
            }
        });
        LOGD("AnimationMock PostFlush. animationInt_ has been added.");
        animator_->AddInterpolator(animationInt_);
    }
    if (animationFloat_) {
        animationFloat_->AddListener([weak](const float& value) {
            auto simulator = weak.Upgrade();
            if (simulator) {
                LOGD("AnimationMock vsync triggered. float value: %{public}f", value);
                simulator->animationFloatValue_ = value;
            }
        });
        LOGD("AnimationMock PostFlush. animationFloat_ has been add.");
        animator_->AddInterpolator(animationFloat_);
    }
    if (keyframeAnimation_) {
        keyframeAnimation_->AddListener([weak](const float& value) {
            auto simulator = weak.Upgrade();
            if (simulator) {
                LOGD("AnimationMock vsync triggered. keyframeAnimation_ float value: %{public}f", value);
                simulator->keyframeAnimationValue_ = value;
            }
        });
        LOGD("AnimationMock PostFlush. keyframeAnimation_ has been added.");
        animator_->AddInterpolator(keyframeAnimation_);
    }
    if (animationColor_) {
        animationColor_->AddListener([weak](const Color& value) {
            auto simulator = weak.Upgrade();
            if (simulator) {
                simulator->animationColorValue_ = value;
            }
        });
        animator_->AddInterpolator(animationColor_);
    }
    CreatePictureInterpolators();
}

const RefPtr<Animator>& AnimationMock::GetAnimator() const
{
    return animator_;
}

double AnimationMock::GetPositionResult() const
{
    return positionResult_;
}

void AnimationMock::SetPositionResult(double positionResult)
{
    positionResult_ = positionResult;
}

RefPtr<Component> SemiModalComponent::Create(
    RefPtr<Component> child, bool isFullScreen, int32_t modalHeight, uint32_t color)
{
    return nullptr;
}

RefPtr<Component> DialogModalComponent::Create(RefPtr<Component> child)
{
    return nullptr;
}

sk_sp<SkData> FileImageLoader::LoadImageData(const std::string& src, const WeakPtr<PipelineContext> context)
{
    return nullptr;
}

} // namespace OHOS::Ace
