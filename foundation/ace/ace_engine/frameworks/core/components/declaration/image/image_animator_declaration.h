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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_IMAGE_ANIMATOR_IMAGE_ANIMATOR_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_IMAGE_ANIMATOR_IMAGE_ANIMATOR_DECLARATION_H

#include "core/components/common/properties/border.h"
#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/core/animation/animator.h"

namespace OHOS::Ace {

struct ImageProperties {
    std::string src;
    Dimension width;
    Dimension height;
    Dimension top;
    Dimension left;
    int32_t duration = 0;
};

struct ImageAnimatorAttribute : Attribute {
    FillMode fillMode = FillMode::FORWARDS;
    Animator::Status status = Animator::Status::IDLE;
    int32_t iteration = -1;
    int32_t duration = 0;
    int32_t preDecode = 0;
    bool isReverse = false;
    bool isFixedSize = true;
    std::vector<ImageProperties> images;
};

class ImageAnimatorDeclaration : public Declaration {
    DECLARE_ACE_TYPE(ImageAnimatorDeclaration, Declaration);

public:
    ImageAnimatorDeclaration() = default;
    ~ImageAnimatorDeclaration() override = default;

    void SetFillMode(FillMode fillMode)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.fillMode = fillMode;
    }

    void SetStatus(Animator::Status status)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.status = status;
    }

    void SetIteration(int32_t iteration)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.iteration = iteration;
    }

    void SetDuration(int32_t duration)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.duration = duration;
    }

    void SetIsReverse(bool isReverse)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isReverse = isReverse;
    }

    void SetIsFixedSize(bool isFixedSize)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isFixedSize = isFixedSize;
    }

    void SetBorder(const Border& border)
    {
        border_ = border;
    }

    void SetImageProperties(const std::vector<ImageProperties>& images)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.images = images;
    }

    void SetPreDecode(int32_t preDecode)
    {
        auto& attribute = MaybeResetAttribute<ImageAnimatorAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.preDecode = preDecode;
    }

    FillMode GetFillMode() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.fillMode;
    }

    Animator::Status GetStatus() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.status;
    }

    int32_t GetIteration() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.iteration;
    }

    int32_t GetDuration() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.duration;
    }

    int32_t GetPreDecode() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.preDecode;
    }

    bool GetIsReverse() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isReverse;
    }

    bool GetIsFixedSize() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isFixedSize;
    }

    const Border& GetBorder() const
    {
        return border_;
    }

    const std::vector<ImageProperties>& GetImageProperties() const
    {
        auto& attribute = static_cast<ImageAnimatorAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.images;
    }

protected:
    void InitSpecialized() override;

private:
    Border border_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_IMAGE_ANIMATOR_IMAGE_ANIMATOR_DECLARATION_H
