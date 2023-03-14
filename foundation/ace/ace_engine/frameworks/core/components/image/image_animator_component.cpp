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

#include "core/components/image/image_animator_component.h"

#include "core/animation/animation_pub.h"
#include "core/components/image/image_animator_element.h"

namespace OHOS::Ace {
namespace {

const char* PREFIX = "ImageAnimator";
uint32_t g_componentId = 0;

} // namespace

ImageAnimatorComponent::ImageAnimatorComponent(const std::string& name) : ComposedComponent(GenerateComponentId(), name)
{
    if (!imageAnimatorController_) {
        imageAnimatorController_ = AceType::MakeRefPtr<ImageAnimatorController>();
    }
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<ImageAnimatorDeclaration>();
        declaration_->Init();
    }
}

RefPtr<Element> ImageAnimatorComponent::CreateElement()
{
    return AceType::MakeRefPtr<ImageAnimatorElement>(GetId());
}

void ImageAnimatorComponent::SetFillMode(FillMode fillMode)
{
    declaration_->SetFillMode(fillMode);
}

FillMode ImageAnimatorComponent::GetFillMode() const
{
    return declaration_->GetFillMode();
}

void ImageAnimatorComponent::SetStatus(Animator::Status status)
{
    declaration_->SetStatus(status);
}

Animator::Status ImageAnimatorComponent::GetStatus() const
{
    return declaration_->GetStatus();
}

void ImageAnimatorComponent::SetIteration(int32_t iteration)
{
    declaration_->SetIteration(iteration);
}

int32_t ImageAnimatorComponent::GetIteration() const
{
    return declaration_->GetIteration();
}

void ImageAnimatorComponent::SetPreDecode(int32_t preDecode)
{
    declaration_->SetPreDecode(preDecode);
}

int32_t ImageAnimatorComponent::GetPreDecode() const
{
    return declaration_->GetPreDecode();
}

void ImageAnimatorComponent::SetDuration(int32_t duration)
{
    if (duration <= 0) {
        declaration_->SetDuration(0);
        return;
    }
    declaration_->SetDuration(duration);
}

int32_t ImageAnimatorComponent::GetDuration() const
{
    return declaration_->GetDuration();
}

void ImageAnimatorComponent::SetBorder(const Border& border)
{
    declaration_->SetBorder(border);
}

const Border& ImageAnimatorComponent::GetBorder() const
{
    return declaration_->GetBorder();
}

void ImageAnimatorComponent::SetIsReverse(bool isReverse)
{
    declaration_->SetIsReverse(isReverse);
}

bool ImageAnimatorComponent::GetIsReverse() const
{
    return declaration_->GetIsReverse();
}

void ImageAnimatorComponent::SetIsFixedSize(bool isFixedSize)
{
    declaration_->SetIsFixedSize(isFixedSize);
}

bool ImageAnimatorComponent::GetIsFixedSize() const
{
    return declaration_->GetIsFixedSize();
}

void ImageAnimatorComponent::SetImageProperties(const std::vector<ImageProperties>& images)
{
    if (images.empty()) {
        LOGW("Images are empty.");
        return;
    }
    declaration_->SetImageProperties(images);
}

const std::vector<ImageProperties>& ImageAnimatorComponent::GetImageProperties() const
{
    return declaration_->GetImageProperties();
}

ComposeId ImageAnimatorComponent::GenerateComponentId()
{
    return PREFIX + (std::to_string(g_componentId++));
}

} // namespace OHOS::Ace
