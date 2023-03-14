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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_ANIMATOR_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_ANIMATOR_ELEMENT_H

#include "core/animation/animator.h"
#include "core/animation/picture_animation.h"
#include "core/components/box/box_component.h"
#include "core/components/image/image_animator_component.h"
#include "core/components/image/image_component.h"
#include "core/components/page/page_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

class ImageAnimatorElement : public ComposedElement {
    DECLARE_ACE_TYPE(ImageAnimatorElement, ComposedElement);

public:
    explicit ImageAnimatorElement(const ComposeId& id) : ComposedElement(id)
    {
        animator_ = AceType::MakeRefPtr<Animator>();
    }
    ~ImageAnimatorElement() override;

    void Update() override;
    void PerformBuild() override;

    Animator::Status GetStatus() const
    {
        return status_;
    }

    int32_t GetDuration() const
    {
        return duration_;
    }

    bool GetReverse() const
    {
        return isReverse_;
    }

    bool GetFixedSize() const
    {
        return isFixedSize_;
    }

    int32_t GetPreDecode() const
    {
        return preDecode_;
    }

    FillMode GetFillMode() const
    {
        return fillMode_;
    }

    int32_t GetIteration() const
    {
        return iteration_;
    }

    std::vector<ImageProperties> GetImages() const
    {
        return images_;
    }

protected:
    RefPtr<Component> BuildChild() override;

private:
    void CreatePictureAnimation(int32_t size);
    void PlayImageAnimator(int32_t index);
    void UpdateFilterImages();
    void UpdateImageSize(ImageProperties& imageProperties, const RefPtr<ImageComponent>& image);
    void UpdatePreLoadImages(const RefPtr<BoxComponent>& box);
    void CallAnimatorMethod(const std::string& method);
    void UpdateCallbackAndFunc(const RefPtr<ImageAnimatorComponent>& imageAnimatorComponent);
    Animator::Status GetAnimatorStatus() const;

    std::vector<ImageProperties> images_;
    std::vector<ImageProperties> filterImages_;
    RefPtr<Animator> animator_;
    RefPtr<Component> childComponent_;
    RefPtr<PictureAnimation<int32_t>> pictureAnimation_;
    WeakPtr<PageElement> pageElement_;
    Border border_;
    Animator::Status status_ = Animator::Status::IDLE;
    int32_t durationTotal_ = 0;
    int32_t preDecode_ = 1;
    int32_t callbackId_ = -1;
    int32_t duration_ = 0; // Duration in millisecond.
    int64_t stopCallbackId_ = -1;
    int64_t repeatCallbackId_ = -1;
    FillMode fillMode_ = FillMode::FORWARDS;
    int32_t iteration_ = -1;
    bool isPaused_ = false;
    bool isReverse_ = false;
    bool isFixedSize_ = true;
    bool isSetDuration_ = false;
    bool isResetBox_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_ANIMATOR_ELEMENT_H
