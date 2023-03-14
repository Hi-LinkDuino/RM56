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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PICTURE_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PICTURE_ANIMATION_H

#include <list>

#include "core/animation/animation.h"
#include "core/animation/curve.h"

namespace OHOS::Ace {

template<typename T>
class PictureFrame : public AceType {
public:
    PictureFrame() = default;
    PictureFrame(float duration, const T& pictureInfo) : duration_(duration), pictureInfo_(pictureInfo) {}

    ~PictureFrame() override = default;

    float GetDuration() const
    {
        return duration_;
    }
    const T& GetPictureInfo() const
    {
        return pictureInfo_;
    }

    void UpdateDurationWithScale(float scale)
    {
        if (scale <= 0.0f) {
            LOGE("update picture with scale failed. scale is invalid.");
            return;
        }

        duration_ /= scale;
        LOGD("update with scale. duration: %{public}f, scale: %{public}f", duration_, scale);
    }

private:
    float duration_ { 0.0f };
    const T pictureInfo_ {};
};

template<typename T>
class PictureAnimation : public Animation<T> {
public:
    PictureAnimation() = default;

    ~PictureAnimation() override = default;

    // add picture. duration in normalized time.
    bool AddPicture(float duration, const T& pictureInfo)
    {
        if (duration <= NORMALIZED_DURATION_MIN || duration > NORMALIZED_DURATION_MAX) {
            LOGE("duration time check failed. duration: %{public}f", duration);
            return false;
        }
        if (pictures_.empty()) {
            this->duration_ = 0.0f;
        }
        LOGD("picture animation: add picture ref, duration: %{public}f", duration);
        pictures_.emplace_back(AceType::MakeRefPtr<Ace::PictureFrame<T>>(duration, pictureInfo));
        this->duration_ += duration;
        return true;
    }

    void ClearPictures()
    {
        this->duration_ = 0.0f;
        pictures_.clear();
    }

    const T& GetValue() const override
    {
        return currentPicture_->GetPictureInfo();
    }

    // if total normalized duration of all pictures not equals 1.0, scale it to 1.0
    // usually call it after all pictures had been added.
    void AutoScale()
    {
        if (NearZero(this->duration_)) {
            return;
        }
        if (pictures_.empty()) {
            return;
        }

        // already equals to 1.0
        if (NearEqual(this->duration_, NORMALIZED_DURATION_MAX)) {
            return;
        }

        for (const auto& picture : pictures_) {
            picture->UpdateDurationWithScale(this->duration_);
        }

        LOGD("auto scale. scale: %{public}f", this->duration_);
        this->duration_ = NORMALIZED_DURATION_MAX;
    }

private:
    void UpdateAndNotifyPicture(const RefPtr<PictureFrame<T>>& picture)
    {
        if (!picture) {
            LOGE("update picture is null.");
            return;
        }
        if (currentPicture_ != picture) {
            ValueListenable<T>::NotifyListener(picture->GetPictureInfo());
            currentPicture_ = picture;
        }
    }

    void OnNormalizedTimestampChanged(float normalized, bool revserse) override
    {
        if (normalized < NORMALIZED_DURATION_MIN || normalized > NORMALIZED_DURATION_MAX) {
            LOGE("normalized time check failed. normalized: %{public}f", normalized);
            return;
        }
        if (pictures_.empty()) {
            return;
        }

        float elapsedPictureTime = 0.0f;
        for (const auto& picture : pictures_) {
            elapsedPictureTime += picture->GetDuration();
            if (normalized < elapsedPictureTime) {
                UpdateAndNotifyPicture(picture);
                return;
            }
        }
        // timestampInMillisecond larger than total picture duration
        UpdateAndNotifyPicture(pictures_.back());
    }

    RefPtr<PictureFrame<T>> currentPicture_ = AceType::MakeRefPtr<PictureFrame<T>>();
    std::list<RefPtr<PictureFrame<T>>> pictures_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PICTURE_ANIMATION_H
