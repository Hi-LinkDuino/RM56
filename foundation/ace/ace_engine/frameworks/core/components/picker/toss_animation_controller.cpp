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

#include "toss_animation_controller.h"

#include <sys/time.h>

#include "core/components/picker/render_picker_column.h"

namespace OHOS::Ace {
namespace {

constexpr double MIN_TIME = 1.0;
constexpr int MIN_DURATION = 250;
constexpr double DRAG = 0.995;
constexpr double ZERO_SPEED = 0.5;

} // namespace


void TossAnimationController::SetStart(double y)
{
    if (toss_) {
        toss_->Stop();
    }

    yStart_ = y;
    timeStart_ = GetCurrentTime();
}

void TossAnimationController::SetEnd(double y)
{
    if (toss_) {
        toss_->Stop();
    }

    yEnd_ = y;
    timeEnd_ = GetCurrentTime();
}

bool TossAnimationController::Play()
{
    auto timeDiff = timeEnd_ - timeStart_;
    if (timeDiff < MIN_TIME) {
        LOGW("toss time[%{public}lf] too small.", timeDiff);
        return false;
    }

    double speed = (yEnd_ - yStart_) / timeDiff;
    if (NearZero(speed)) {
        LOGW("toss speed is zero");
        return false;
    }

    double zeroSpeed = (speed > 0 ? ZERO_SPEED : 0.0 - ZERO_SPEED);
    double time = zeroSpeed / speed;
    time = std::log(time) / std::log(DRAG);
    if (time < MIN_DURATION) {
        LOGW("toss time[%{public}lf] to small.", time);
        return false;
    }

    LOGD("toss play speed: %{public}lf, time: %{public}lf", speed, time);
    speed_ = speed;
    int nTime = static_cast<int>(time);
    auto weak = AceType::WeakClaim(this);
    toss_ = AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, time, 0, nTime, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            LOGE("toss ref is null.");
            return;
        }
        auto column = AceType::DynamicCast<RenderPickerColumn>(ref->column_.Upgrade());
        if (!column) {
            LOGE("toss column is null.");
            return;
        }
        double distance = std::pow(DRAG, value);
        distance = (distance - 1.0) * ref->speed_ / std::log(DRAG);
        column->UpdateToss(ref->yEnd_ + distance);
    });
    toss_->AddStopCallback([weak] {
        auto ref = weak.Upgrade();
        if (!ref) {
            LOGE("toss ref is null when stop.");
            return;
        }
        auto column = AceType::DynamicCast<RenderPickerColumn>(ref->column_.Upgrade());
        if (!column) {
            LOGE("column is null when stop.");
            return;
        }
        column->TossStoped();
    });
    toss_->Play();
    return true;
}

double TossAnimationController::GetCurrentTime() const
{
    struct timeval tv = { 0 };
    int result = gettimeofday(&tv, nullptr);
    if (result != 0) {
        LOGE("toss gettimeofday failed.");
        return 0.0;
    }

    double sec = static_cast<double>(tv.tv_sec);
    double msec = static_cast<double>(tv.tv_usec / 1000.0); // usec / 1000 is msec
    return (sec * 1000 + msec); // sec * 1000 is msec
}

} // namespace OHOS::Ace

