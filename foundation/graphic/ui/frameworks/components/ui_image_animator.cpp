/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_image_animator.h"

namespace OHOS {
UIImageAnimatorView::UIImageAnimatorView()
    : imageSrc_(nullptr),
      imageNum_(0),
      tickOfUpdate_(1),
      timeOfUpdate_(DEFAULT_TASK_PERIOD),
      timeOfPause_(0),
      tickOfPause_(0),
      repeatTimes_(1),
      imageAnimator_(&imageAnimatorCallback_, this, 0, true),
      listener_(nullptr),
      reverse_(false),
      repeat_(true),
      sizeFixed_(false),
      fillMode_(true)
{
}

UIImageAnimatorView::~UIImageAnimatorView() {}

void UIImageAnimatorView::ImageAnimatorCallback::Callback(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    UIImageAnimatorView* imageAnimatorView = static_cast<UIImageAnimatorView*>(view);

    imageSrc_ = imageAnimatorView->GetImageAnimatorSrc();
    imageNum_ = imageAnimatorView->GetImageAnimatorImageNum();
    if ((imageSrc_ == nullptr) || (imageNum_ == 0) || (imageAnimatorView->tickOfUpdate_ == 0)) {
        return;
    }

    if (!imageAnimatorView->IsRepeat() && (repeat_ == imageAnimatorView->GetRepeatTimes())) {
        imageAnimatorView->Stop();
        return;
    }

    tickNum_++;

    if (loop_ != imageNum_) {
        if (tickNum_ < imageAnimatorView->tickOfUpdate_) {
            return;
        }
    } else {
        if (imageAnimatorView->tickOfPause_ != 0) {
            if (tickNum_ < imageAnimatorView->tickOfPause_) {
                return;
            }
        } else {
            if (tickNum_ < imageAnimatorView->tickOfUpdate_) {
                return;
            }
        }
        repeat_++;
        loop_ = 0;
        if (!imageAnimatorView->IsRepeat() && (repeat_ == imageAnimatorView->GetRepeatTimes())) {
            imageAnimatorView->Stop();
            return;
        }
    }
    imageAnimatorView->UpdateImage(drawingImage_, loop_);
    tickNum_ = 0;
}

void UIImageAnimatorView::UpdateImage(uint8_t& drawingImage, uint8_t& loop)
{
    Invalidate();
    drawingImage = reverse_ ? (imageNum_ - loop - 1) : loop;
    if (drawingImage >= imageNum_) {
        return;
    }

    ImageAnimatorInfo* imageAnimatorInfo = &(imageSrc_[drawingImage]);
    if (imageAnimatorInfo->imageType == IMG_SRC_FILE_PATH) {
        SetSrc(imageAnimatorInfo->imagePath);
    } else if (imageAnimatorInfo->imageType == IMG_SRC_IMAGE_INFO) {
        SetSrc(imageAnimatorInfo->imageInfo);
    }
    if (!sizeFixed_) {
        SetPosition(imageAnimatorInfo->pos.x, imageAnimatorInfo->pos.y);
        SetWidth(imageAnimatorInfo->width);
        SetHeight(imageAnimatorInfo->height);
    }
    Invalidate();
    loop++;
}

void UIImageAnimatorView::SetImageAnimatorSrc(const ImageAnimatorInfo imageAnimatorInfoSrc[], uint8_t imageNum)
{
    SetImageAnimatorSrc(imageAnimatorInfoSrc, imageNum, timeOfUpdate_);
}

void UIImageAnimatorView::SetImageAnimatorSrc(const ImageAnimatorInfo imageAnimatorInfoSrc[],
                                              uint8_t imageNum,
                                              uint16_t timeOfUpdate)
{
    imageSrc_ = const_cast<ImageAnimatorInfo*>(imageAnimatorInfoSrc);
    imageNum_ = imageNum;
    timeOfUpdate_ = timeOfUpdate;
    tickOfUpdate_ = GetTickByTime(timeOfUpdate);
    return;
}

const ImageAnimatorInfo* UIImageAnimatorView::GetImageAnimatorSrc() const
{
    return imageSrc_;
}

uint8_t UIImageAnimatorView::GetImageAnimatorImageNum() const
{
    return imageNum_;
}

void UIImageAnimatorView::SetTimeOfUpdate(uint16_t timeOfUpdate)
{
    timeOfUpdate_ = timeOfUpdate;
    tickOfUpdate_ = GetTickByTime(timeOfUpdate);
}

uint16_t UIImageAnimatorView::GetTimeOfUpdate() const
{
    return timeOfUpdate_;
}

void UIImageAnimatorView::SetTimeOfPause(uint16_t timeOfPause)
{
    timeOfPause_ = timeOfPause;
    tickOfPause_ = GetTickByTime(timeOfPause);
}

uint16_t UIImageAnimatorView::GetTimeOfPause() const
{
    return timeOfPause_;
}

void UIImageAnimatorView::Start()
{
    Reset(false);
    imageAnimator_.Start();
}

void UIImageAnimatorView::Reset(bool fillMode)
{
    if ((imageSrc_ == nullptr) || (imageNum_ == 0)) {
        return;
    }

    Invalidate();
    uint8_t drawingImage;
    if (fillMode) {
        drawingImage = reverse_ ? 0 : (imageNum_ - 1);
    } else {
        drawingImage = reverse_ ? (imageNum_ - 1) : 0;
    }
    ImageAnimatorInfo* imageAnimatorInfo = &(imageSrc_[drawingImage]);
    if (imageAnimatorInfo->imageType == IMG_SRC_FILE_PATH) {
        SetSrc(imageAnimatorInfo->imagePath);
    } else if (imageAnimatorInfo->imageType == IMG_SRC_IMAGE_INFO) {
        SetSrc(imageAnimatorInfo->imageInfo);
    }
    if (!sizeFixed_) {
        SetPosition(imageAnimatorInfo->pos.x, imageAnimatorInfo->pos.y);
        SetWidth(imageAnimatorInfo->width);
        SetHeight(imageAnimatorInfo->height);
    }
    Invalidate();
    imageAnimatorCallback_.Reset();
}

void UIImageAnimatorView::Stop()
{
    if (imageAnimator_.GetState() == Animator::STOP) {
        return;
    }

    imageAnimator_.Stop();
    Reset(fillMode_);
    if (listener_ != nullptr) {
        listener_->OnAnimatorStop(*this);
    }
}

void UIImageAnimatorView::Pause()
{
    imageAnimator_.Pause();
}

void UIImageAnimatorView::Resume()
{
    imageAnimator_.Resume();
}

uint8_t UIImageAnimatorView::GetTickByTime(uint16_t time) const
{
    uint8_t tick;
    if ((time > 0) && (time <= DEFAULT_TASK_PERIOD)) {
        tick = 1;
    } else {
        tick = time / DEFAULT_TASK_PERIOD;
    }
    return tick;
}
} // namespace OHOS
