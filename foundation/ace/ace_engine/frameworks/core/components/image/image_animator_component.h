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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_ANIMATOR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_ANIMATOR_COMPONENT_H

#include "core/components/declaration/image/image_animator_declaration.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

using AnimatorFunc = std::function<void(const std::string& method)>;
using AnimatorGetStatusFunc = std::function<Animator::Status()>;

class ImageAnimatorController : public virtual AceType {
    DECLARE_ACE_TYPE(ImageAnimatorController, AceType);

public:
    void CallAnimationFunc(const std::string& method)
    {
        if (animatorFunc_) {
            animatorFunc_(method);
        }
    }

    void SetAnimationFunc(const AnimatorFunc& animatorFunc)
    {
        animatorFunc_ = animatorFunc;
    }

    void SetAnimatorGetStatusFunc(const AnimatorGetStatusFunc& animatorGetStatusFunc)
    {
        animatorGetStatusFunc_ = animatorGetStatusFunc;
    }

    Animator::Status CallAnimatorGetStatusFunc()
    {
        if (animatorGetStatusFunc_) {
            return animatorGetStatusFunc_();
        }
        return Animator::Status::IDLE;
    }

    void SetStartEvent(const EventMarker& startEvent)
    {
        startEvent_ = startEvent;
    }

    const EventMarker& GetStartEvent() const
    {
        return startEvent_;
    }

    void SetStopEvent(const EventMarker& stopEvent)
    {
        stopEvent_ = stopEvent;
    }

    const EventMarker& GetStopEvent() const
    {
        return stopEvent_;
    }

    void SetPauseEvent(const EventMarker& pauseEvent)
    {
        pauseEvent_ = pauseEvent;
    }

    const EventMarker& GetPauseEvent() const
    {
        return pauseEvent_;
    }

    void SetResumeEvent(const EventMarker& resumeEvent)
    {
        resumeEvent_ = resumeEvent;
    }

    const EventMarker& GetResumeEvent() const
    {
        return resumeEvent_;
    }

    void SetRepeatEvent(const EventMarker& repeatEvent)
    {
        repeatEvent_ = repeatEvent;
    }

    const EventMarker& GetRepeatEvent() const
    {
        return repeatEvent_;
    }

    void SetCancelEvent(const EventMarker& cancelEvent)
    {
        cancelEvent_ = cancelEvent;
    }

    const EventMarker& GetCancelEvent() const
    {
        return cancelEvent_;
    }

private:
    AnimatorFunc animatorFunc_;
    AnimatorGetStatusFunc animatorGetStatusFunc_;

    EventMarker startEvent_;
    EventMarker stopEvent_;
    EventMarker pauseEvent_;
    EventMarker resumeEvent_;
    EventMarker repeatEvent_;
    EventMarker cancelEvent_;
};

class ACE_EXPORT ImageAnimatorComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(ImageAnimatorComponent, ComposedComponent);

public:
    explicit ImageAnimatorComponent(const std::string& name);
    ~ImageAnimatorComponent() override = default;

    RefPtr<Element> CreateElement() override;

    static ComposeId GenerateComponentId();

    void SetFillMode(FillMode fillMode);
    void SetIteration(int32_t iteration);
    void SetDuration(int32_t duration);
    void SetIsReverse(bool isReverse);
    void SetIsFixedSize(bool isFixedSize);
    void SetBorder(const Border& border);
    void SetImageProperties(const std::vector<ImageProperties>& images);
    void SetPreDecode(int32_t preDecode);
    void SetStatus(Animator::Status status);

    FillMode GetFillMode() const;
    int32_t GetIteration() const;
    int32_t GetDuration() const;
    int32_t GetPreDecode() const;
    bool GetIsReverse() const;
    bool GetIsFixedSize() const;
    const Border& GetBorder() const;
    const std::vector<ImageProperties>& GetImageProperties() const;
    Animator::Status GetStatus() const;

    const RefPtr<ImageAnimatorController>& GetImageAnimatorController() const
    {
        return imageAnimatorController_;
    }

private:
    RefPtr<ImageAnimatorDeclaration> declaration_;
    RefPtr<ImageAnimatorController> imageAnimatorController_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_ANIMATOR_COMPONENT_H
