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

#include "core/components/image/image_component.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/image/image_element.h"
#include "core/components/image/render_image.h"

namespace OHOS::Ace {

ImageComponent::ImageComponent(const std::string& src) : src_(src) {}

RefPtr<RenderNode> ImageComponent::CreateRenderNode()
{
    return RenderImage::Create();
}

RefPtr<Element> ImageComponent::CreateElement()
{
    return AceType::MakeRefPtr<ImageElement>();
}

void ImageComponent::SetSrc(const std::string& src)
{
    src_ = src;
}

void ImageComponent::SetImageFill(const std::optional<Color>& color)
{
    fillColor_ = color;
}

void ImageComponent::SetAlignment(const Alignment& alignment)
{
    alignment_ = alignment;
}

const Alignment& ImageComponent::GetAlignment() const
{
    return alignment_;
}

const Border& ImageComponent::GetBorder() const
{
    return border_;
}

const std::string& ImageComponent::GetSrc() const
{
    return src_;
}

void ImageComponent::SetAlt(const std::string& alt)
{
    alt_ = alt;
}

const std::string& ImageComponent::GetAlt() const
{
    return alt_;
}

void ImageComponent::SetColor(const std::optional<Color>& color)
{
    color_ = color;
}

bool ImageComponent::IsMatchTextDirection() const
{
    return matchTextDirection_;
}

bool ImageComponent::IsSrcSvgImage() const
{
    return IsSvgSuffix(src_);
}

const std::optional<Color>& ImageComponent::GetColor() const
{
    return color_;
}

void ImageComponent::SetLoadSuccessEvent(const EventMarker& loadSuccessEvent)
{
    loadSuccessEvent_ = loadSuccessEvent;
}

const EventMarker& ImageComponent::GetLoadSuccessEvent() const
{
    return loadSuccessEvent_;
}

void ImageComponent::SetLoadFailEvent(const EventMarker& loadFailEvent)
{
    loadFailEvent_ = loadFailEvent;
}

const EventMarker& ImageComponent::GetLoadFailEvent() const
{
    return loadFailEvent_;
}

void ImageComponent::SetSvgAnimatorFinishEvent(const EventMarker& svgAnimatorFinishEvent)
{
    svgAnimatorFinishEvent_ = svgAnimatorFinishEvent;
}

const EventMarker& ImageComponent::GetSvgAnimatorFinishEvent() const
{
    return svgAnimatorFinishEvent_;
}

InternalResource::ResourceId ImageComponent::GetResourceId() const
{
    return resourceId_;
}

void ImageComponent::SetResourceId(InternalResource::ResourceId resourceId)
{
    resourceId_ = resourceId;
}

void ImageComponent::SetBorder(const Border& border)
{
    border_ = border;
}

void ImageComponent::SetMatchTextDirection(bool matchTextDirection)
{
    matchTextDirection_ = matchTextDirection;
}

void ImageComponent::SetFitMaxSize(bool fitMaxSize)
{
    fitMaxSize_ = fitMaxSize;
}

void ImageComponent::SetImageFit(ImageFit imageFit)
{
    imageFit_ = imageFit;
}

void ImageComponent::SetImageInterpolation(ImageInterpolation imageInterpolation)
{
    imageInterpolation_ = imageInterpolation;
}

void ImageComponent::SetImageRenderMode(ImageRenderMode imageRenderMode)
{
    imageRenderMode_ = imageRenderMode;
}

void ImageComponent::SetImageRepeat(ImageRepeat imageRepeat)
{
    imageRepeat_ = imageRepeat;
}

void ImageComponent::SetImageSourceSize(const std::pair<Dimension, Dimension>& sourceSize)
{
    imageSourceSize_ = sourceSize;
}

void ImageComponent::SetUseSkiaSvg(bool useSkiaSvg)
{
    useSkiaSvg_ = useSkiaSvg;
}

void ImageComponent::SetAutoResize(bool autoResize)
{
    autoResize_ = autoResize;
}

void ImageComponent::SetSyncMode(bool syncMode)
{
    syncMode_ = syncMode;
}

bool ImageComponent::GetSyncMode()
{
    return syncMode_;
}

RefPtr<ImageComponent> ImageComponent::MakeFromOtherWithoutSourceAndEvent(const RefPtr<ImageComponent>& other)
{
    RefPtr<ImageComponent> imageComponent = AceType::MakeRefPtr<ImageComponent>("");
    imageComponent->SetAlignment(other->GetAlignment());
    imageComponent->SetImageFill(other->GetImageFill());
    imageComponent->SetColor(other->GetColor());
    imageComponent->SetBorder(other->GetBorder());
    imageComponent->SetFitMaxSize(other->GetFitMaxSize());
    imageComponent->SetMatchTextDirection(other->IsMatchTextDirection());
    imageComponent->SetImageFit(other->GetImageFit());
    imageComponent->SetImageInterpolation(other->GetImageInterpolation());
    imageComponent->SetImageRenderMode(other->GetImageRenderMode());
    imageComponent->SetImageRepeat(other->GetImageRepeat());
    imageComponent->SetImageSourceSize(other->GetImageSourceSize());
    imageComponent->SetUseSkiaSvg(other->GetUseSkiaSvg());
    imageComponent->SetAutoResize(other->GetAutoResize());
    return imageComponent;
}

const std::optional<Color>& ImageComponent::GetImageFill() const
{
    return fillColor_;
}

bool ImageComponent::GetFitMaxSize() const
{
    return fitMaxSize_;
}

ImageFit ImageComponent::GetImageFit() const
{
    return imageFit_;
}

ImageInterpolation ImageComponent::GetImageInterpolation() const
{
    return imageInterpolation_;
}

ImageRenderMode ImageComponent::GetImageRenderMode() const
{
    return imageRenderMode_;
}

ImageRepeat ImageComponent::GetImageRepeat() const
{
    return imageRepeat_;
}

const std::pair<Dimension, Dimension>& ImageComponent::GetImageSourceSize() const
{
    return imageSourceSize_;
}

bool ImageComponent::GetUseSkiaSvg() const
{
    return useSkiaSvg_;
}

bool ImageComponent::GetAutoResize() const
{
    return autoResize_;
}

bool ImageComponent::IsSvgSuffix(const std::string& src)
{
    // 4 is the length of ".svg".
    return (src.size() > 4 && src.substr(src.size() - 4) == ".svg");
}

void ImageComponent::SetPixmap(const RefPtr<PixelMap>& pixmap)
{
    pixmap_ = pixmap;
}

const RefPtr<PixelMap>& ImageComponent::GetPixmap() const
{
    return pixmap_;
}

void ImageComponent::SetHasObjectPosition(bool hasObjectPosition)
{
    hasObjectPosition_ = hasObjectPosition;
}

bool ImageComponent::GetHasObjectPosition() const
{
    return hasObjectPosition_;
}

void ImageComponent::SetImageObjectPosition(const ImageObjectPosition& imageObjectPosition)
{
    imageObjectPosition_ = imageObjectPosition;
    SetHasObjectPosition(true);
}

const ImageObjectPosition& ImageComponent::GetImageObjectPosition() const
{
    return imageObjectPosition_;
}

} // namespace OHOS::Ace
