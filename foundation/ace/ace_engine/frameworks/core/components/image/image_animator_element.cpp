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

#include "core/components/image/image_animator_element.h"

#include "base/utils/string_utils.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

ImageAnimatorElement::~ImageAnimatorElement()
{
    auto pageElement = pageElement_.Upgrade();
    if (pageElement && callbackId_ >= 0) {
        pageElement->CancelHiddenCallback(callbackId_);
    }
}

void Ace::ImageAnimatorElement::Update()
{
    const auto imageAnimatorComponent = AceType::DynamicCast<ImageAnimatorComponent>(component_);
    if (!imageAnimatorComponent) {
        LOGE("ImageAnimator element update failed. imageAnimatorComponent is null.");
        return;
    }
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>();
    }
    UpdateCallbackAndFunc(imageAnimatorComponent);
    if (!animator_->HasScheduler()) {
        animator_->AttachScheduler(context_);
    }
    animator_->SetFillMode(imageAnimatorComponent->GetFillMode());
    animator_->SetIteration(imageAnimatorComponent->GetIteration());
    status_ = imageAnimatorComponent->GetStatus();
    preDecode_ = imageAnimatorComponent->GetPreDecode();
    duration_ = imageAnimatorComponent->GetDuration();
    isReverse_ = imageAnimatorComponent->GetIsReverse();
    images_ = imageAnimatorComponent->GetImageProperties();
    isFixedSize_ = imageAnimatorComponent->GetIsFixedSize();
    border_ = imageAnimatorComponent->GetBorder();
    fillMode_ = imageAnimatorComponent->GetFillMode();
    iteration_ = imageAnimatorComponent->GetIteration();
    UpdateFilterImages();

    if (!pageElement_.Invalid()) {
        return;
    }

    auto pageElement = GetPageElement();
    if (!pageElement) {
        return;
    }
    pageElement_ = pageElement;
    callbackId_ = pageElement->RegisterHiddenCallback([weak = AceType::WeakClaim(this)](bool hidden) {
        auto element = weak.Upgrade();
        if (!element || !element->animator_) {
            return;
        }

        if (hidden) {
            if (element->animator_->GetStatus() == Animator::Status::RUNNING) {
                element->animator_->Pause();
                element->isPaused_ = true;
            }
        } else {
            if (element->isPaused_) {
                if (element->isReverse_) {
                    element->animator_->Backward();
                } else {
                    element->animator_->Forward();
                }
                element->isPaused_ = false;
            }
        }
    });
}

void ImageAnimatorElement::PerformBuild()
{
    int32_t size = static_cast<int32_t>(images_.size());
    if (size <= 0) {
        LOGE("image size is less than 0.");
        return;
    }
    if (children_.empty()) {
        // first time to set image child.
        childComponent_ = BuildChild();
        ComposedElement::PerformBuild();
    }
    if (preDecode_ > 1) {
        auto boxComponent = DynamicCast<BoxComponent>(childComponent_);
        UpdatePreLoadImages(boxComponent);
    }
    CreatePictureAnimation(size);
    if (!animator_) {
        LOGE("animator is null, need to get animator first.");
        return;
    }
    // update duration after a loop of animation.
    if (!isSetDuration_) {
        durationTotal_ > 0 ? animator_->SetDuration(durationTotal_) : animator_->SetDuration(duration_);
        isSetDuration_ = true;
    }
    animator_->ClearInterpolators();
    animator_->AddInterpolator(pictureAnimation_);
    animator_->RemoveRepeatListener(repeatCallbackId_);
    repeatCallbackId_ = static_cast<int64_t>(animator_->AddRepeatListener([weak = WeakClaim(this)]() {
        auto imageAnimator = weak.Upgrade();
        if (!imageAnimator) {
            return;
        }
        if (imageAnimator->durationTotal_ > 0) {
            imageAnimator->animator_->SetDuration(imageAnimator->durationTotal_);
        } else {
            imageAnimator->animator_->SetDuration(imageAnimator->duration_);
        }
    }));
    animator_->RemoveStopListener(stopCallbackId_);
    stopCallbackId_ = static_cast<int64_t>(animator_->AddStopListener([weak = WeakClaim(this)]() {
        auto imageAnimator = weak.Upgrade();
        if (imageAnimator) {
            imageAnimator->isSetDuration_ = false;
        }
    }));

    // for declarative frontend.
    if (context_.Upgrade() && context_.Upgrade()->GetIsDeclarative()) {
        if (status_ == Animator::Status::IDLE) {
            CallAnimatorMethod(CANCEL);
        } else if (status_ == Animator::Status::PAUSED) {
            CallAnimatorMethod(PAUSE);
        } else if (status_ == Animator::Status::STOPPED) {
            CallAnimatorMethod(STOP);
        } else {
            CallAnimatorMethod(START);
        }
        return;
    }
    isReverse_ ? animator_->Backward() : animator_->Forward();
}

RefPtr<Component> ImageAnimatorElement::BuildChild()
{
    uint32_t size = images_.size();
    if (size == 0) {
        LOGE("image size is 0.");
        return nullptr;
    }
    auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetFlex(BoxFlex::FLEX_XY);
    boxComponent->SetAlignment(Alignment::TOP_LEFT);
    ImageProperties childImage;
    if (durationTotal_ > 0) {
        childImage = (isReverse_ ? filterImages_.back() : filterImages_.front());
    } else {
        childImage = (isReverse_ ? images_.back() : images_.front());
    }
    auto imageComponent = AceType::MakeRefPtr<ImageComponent>(childImage.src);
    if (!isFixedSize_) {
        UpdateImageSize(childImage, imageComponent);
    }
    if (!childImage.src.empty()) {
        imageComponent->SetBorder(border_);
        imageComponent->SetFitMaxSize(true);
    }
    boxComponent->SetChild(imageComponent);
    return boxComponent;
}

void ImageAnimatorElement::UpdatePreLoadImages(const RefPtr<BoxComponent>& box)
{
    if (!box) {
        LOGE("boxComponent is null.");
        return;
    }
    int32_t size = static_cast<int32_t>(images_.size());
    for (int32_t idx = 0; (idx < preDecode_) && (idx < size); idx++) {
        auto imageComponent = DynamicCast<ImageComponent>(box->GetChild());
        if (!imageComponent) {
            LOGE("imageComponent is null.");
            return;
        }
        ImageProperties childImage = images_[idx];
        if (!childImage.src.empty()) {
            imageComponent->SetSrc(childImage.src);
        }
        UpdateChild(GetFirstChild(), box);
    }
}

void ImageAnimatorElement::CreatePictureAnimation(int32_t size)
{
    if (!pictureAnimation_) {
        pictureAnimation_ = MakeRefPtr<PictureAnimation<int32_t>>();
    }

    pictureAnimation_->ClearListeners();
    pictureAnimation_->ClearPictures();
    if (durationTotal_ > 0) {
        int32_t filterImagesSize = static_cast<int32_t>(filterImages_.size());
        for (int32_t index = 0; index < filterImagesSize; ++index) {
            int32_t imageDuration = filterImages_[index].duration;
            pictureAnimation_->AddPicture((float)imageDuration / durationTotal_, index);
        }
    } else {
        for (int32_t index = 0; index < size; ++index) {
            pictureAnimation_->AddPicture(NORMALIZED_DURATION_MAX / size, index);
        }
    }
    pictureAnimation_->AddListener([weak = WeakClaim(this)](const int32_t index) {
        auto imageAnimator = weak.Upgrade();
        if (imageAnimator) {
            imageAnimator->PlayImageAnimator(index);
        }
    });
}

void ImageAnimatorElement::UpdateFilterImages()
{
    filterImages_.clear();
    durationTotal_ = 0;
    for (auto& childImage : images_) {
        if (!childImage.src.empty() && childImage.duration > 0) {
            durationTotal_ += childImage.duration;
            filterImages_.emplace_back(childImage);
        }
    }
}

void ImageAnimatorElement::PlayImageAnimator(int32_t index)
{
    auto boxComponent = DynamicCast<BoxComponent>(childComponent_);
    if (!boxComponent) {
        LOGE("child boxComponent is null.");
        return;
    }
    auto imageComponent = DynamicCast<ImageComponent>(boxComponent->GetChild());
    if (!imageComponent) {
        LOGE("imageComponent is null.");
        return;
    }
    ImageProperties childImage;
    if (durationTotal_ > 0) {
        childImage = filterImages_[index];
    } else {
        childImage = images_[index];
    }
    if (!isFixedSize_) {
        UpdateImageSize(childImage, imageComponent);
        isResetBox_ = false;
    } else {
        if (!isResetBox_) {
            imageComponent->SetLeft(Dimension());
            imageComponent->SetTop(Dimension());
            // Follows the size of the parent component
            imageComponent->SetWidth(-1.0);
            imageComponent->SetHeight(-1.0);
            isResetBox_ = true;
        }
    }
    if (!childImage.src.empty()) {
        imageComponent->SetSrc(childImage.src);
    }
    UpdateChild(GetFirstChild(), boxComponent);
}

void ImageAnimatorElement::UpdateImageSize(ImageProperties& imageProperties, const RefPtr<ImageComponent>& image)
{
    image->SetLeft(imageProperties.left);
    image->SetTop(imageProperties.top);
    if (imageProperties.width.IsValid()) {
        image->SetWidth(imageProperties.width);
    }
    if (imageProperties.height.IsValid()) {
        image->SetHeight(imageProperties.height);
    }
}

void ImageAnimatorElement::CallAnimatorMethod(const std::string& method)
{
    if (!animator_) {
        LOGE("CallAnimatorMethod failed, animator is null.");
        return;
    }
    if (method == START) {
        isReverse_ ? animator_->Backward() : animator_->Forward();
    } else if (method == PAUSE) {
        animator_->Pause();
    } else if (method == STOP) {
        animator_->Finish();
    } else if (method == RESUME) {
        animator_->Resume();
    } else if (method == CANCEL) {
        animator_->Cancel();
    } else {
        LOGE("Unsupported method name : %s", method.c_str());
    }
}

void ImageAnimatorElement::UpdateCallbackAndFunc(const RefPtr<ImageAnimatorComponent>& imageAnimatorComponent)
{
    const auto& imageAnimatorController = imageAnimatorComponent->GetImageAnimatorController();
    if (!imageAnimatorController) {
        LOGE("UpdateCallbackAndFunc failed, imageAnimatorController is null.");
        return;
    }

    // start / stop / pause / resume method.
    imageAnimatorController->SetAnimationFunc([weak = WeakClaim(this)](const std::string& method) {
        auto element = weak.Upgrade();
        if (element) {
            element->CallAnimatorMethod(method);
        }
    });

    // getStatus method.
    imageAnimatorController->SetAnimatorGetStatusFunc([weak = WeakClaim(this)]() -> Animator::Status {
        auto element = weak.Upgrade();
        if (element) {
            return element->GetAnimatorStatus();
        }
        return Animator::Status::IDLE;
    });

    animator_->ClearAllListeners();
    auto startEvent = imageAnimatorController->GetStartEvent();
    if (!startEvent.IsEmpty()) {
        animator_->AddStartListener(
            [startEvent, weakContext = context_] { AceAsyncEvent<void()>::Create(startEvent, weakContext)(); });
    }
    auto stopEvent = imageAnimatorController->GetStopEvent();
    if (!stopEvent.IsEmpty()) {
        animator_->AddStopListener(
            [stopEvent, weakContext = context_] { AceAsyncEvent<void()>::Create(stopEvent, weakContext)(); });
    }
    auto pauseEvent = imageAnimatorController->GetPauseEvent();
    if (!pauseEvent.IsEmpty()) {
        animator_->AddPauseListener(
            [pauseEvent, weakContext = context_] { AceAsyncEvent<void()>::Create(pauseEvent, weakContext)(); });
    }
    auto resumeEvent = imageAnimatorController->GetResumeEvent();
    if (!resumeEvent.IsEmpty()) {
        animator_->AddResumeListener(
            [resumeEvent, weakContext = context_] { AceAsyncEvent<void()>::Create(resumeEvent, weakContext)(); });
    }
    auto repeatEvent = imageAnimatorController->GetRepeatEvent();
    if (!repeatEvent.IsEmpty()) {
        animator_->AddRepeatListener(
            [repeatEvent, weakContext = context_] { AceAsyncEvent<void()>::Create(repeatEvent, weakContext)(); });
    }
    auto cancelEvent = imageAnimatorController->GetCancelEvent();
    if (!cancelEvent.IsEmpty()) {
        animator_->AddIdleListener(
            [cancelEvent, weakContext = context_] { AceAsyncEvent<void()>::Create(cancelEvent, weakContext)(); });
    }
}

Animator::Status ImageAnimatorElement::GetAnimatorStatus() const
{
    if (animator_) {
        return animator_->GetStatus();
    }
    return Animator::Status::IDLE;
}

} // namespace OHOS::Ace
