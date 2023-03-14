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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_COMPONENT_H

#include <string>

#include "base/image/pixel_map.h"
#include "base/resource/internal_resource.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/measurable.h"
#include "core/pipeline/base/render_component.h"
#include "core/components/common/properties/decoration.h"

namespace OHOS::Ace {
// A component can show image.
class ACE_EXPORT ImageComponent : public RenderComponent, public Measurable {
    DECLARE_ACE_TYPE(ImageComponent, RenderComponent, Measurable);

public:
    explicit ImageComponent(const std::string& src = std::string());
    explicit ImageComponent(InternalResource::ResourceId resourceId) : resourceId_(resourceId) {}
    ~ImageComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void SetSrc(const std::string& src);
    void SetAlt(const std::string& alt);
    void SetAlignment(const Alignment& alignment);
    void SetColor(const std::optional<Color>& color);
    void SetLoadSuccessEvent(const EventMarker& loadSuccessEvent);
    void SetLoadFailEvent(const EventMarker& loadFailEvent);
    void SetSvgAnimatorFinishEvent(const EventMarker& svgAnimatorFinishEvent);
    void SetResourceId(InternalResource::ResourceId resourceId);
    void SetBorder(const Border& border);
    void SetFitMaxSize(bool fitMaxSize);
    void SetMatchTextDirection(bool matchTextDirection);
    void SetImageFill(const std::optional<Color>& color);
    void SetImageFit(ImageFit imageFit);
    void SetImageInterpolation(ImageInterpolation imageInterpolation);
    void SetImageRenderMode(ImageRenderMode imageRenderMode);
    void SetImageRepeat(ImageRepeat imageRepeat);
    void SetImageSourceSize(const std::pair<Dimension, Dimension>& sourceSize);
    void SetUseSkiaSvg(bool useSkiaSvg);
    void SetPixmap(const RefPtr<PixelMap>& pixmap);
    void SetAutoResize(bool autoResize);
    void SetSyncMode(bool syncMode);
    bool GetSyncMode();

    const std::string& GetAlt() const;
    const Alignment& GetAlignment() const;
    const std::string& GetSrc() const;
    const std::optional<Color>& GetColor() const;
    const Border& GetBorder() const;
    const EventMarker& GetLoadSuccessEvent() const;
    const EventMarker& GetLoadFailEvent() const;
    const EventMarker& GetSvgAnimatorFinishEvent() const;
    InternalResource::ResourceId GetResourceId() const;
    bool GetFitMaxSize() const;
    bool IsMatchTextDirection() const;
    bool IsSrcSvgImage() const;
    ImageFit GetImageFit() const;
    ImageInterpolation GetImageInterpolation() const;
    ImageRenderMode GetImageRenderMode() const;
    ImageRepeat GetImageRepeat() const;
    const std::pair<Dimension, Dimension>& GetImageSourceSize() const;
    bool GetUseSkiaSvg() const;
    bool GetAutoResize() const;
    static bool IsSvgSuffix(const std::string& src);
    const RefPtr<PixelMap>& GetPixmap() const;
    void SetHasObjectPosition(bool hasObjectPosition);
    bool GetHasObjectPosition() const;
    void SetImageObjectPosition(const ImageObjectPosition& imageObjectPosition);
    const ImageObjectPosition& GetImageObjectPosition() const;
    const std::optional<Color>& GetImageFill() const;

    static RefPtr<ImageComponent> MakeFromOtherWithoutSourceAndEvent(const RefPtr<ImageComponent>& other);

private:
    std::string src_;
    std::string alt_;
    Alignment alignment_ = Alignment::CENTER;
    ImageObjectPosition imageObjectPosition_;

    std::optional<Color> color_;
    std::optional<Color> fillColor_; // used for paint svg path.

    EventMarker loadSuccessEvent_;
    EventMarker loadFailEvent_;
    EventMarker svgAnimatorFinishEvent_;
    InternalResource::ResourceId resourceId_ = InternalResource::ResourceId::NO_ID;
    Border border_;
    bool fitMaxSize_ = true;
    bool hasObjectPosition_ = false;
    bool matchTextDirection_ = false;
    bool useSkiaSvg_ = true;
    bool autoResize_ = true;

    ImageFit imageFit_ = ImageFit::COVER;
    // set default value to [ImageInterpolation::LOW] to keep consistent for the old frontend
    ImageInterpolation imageInterpolation_ = ImageInterpolation::LOW;
    ImageRenderMode imageRenderMode_ = ImageRenderMode::ORIGINAL;
    ImageRepeat imageRepeat_ = ImageRepeat::NOREPEAT;
    std::pair<Dimension, Dimension> imageSourceSize_ = std::pair<Dimension, Dimension>(Dimension(-1), Dimension(-1));
    RefPtr<PixelMap> pixmap_ = nullptr;
    bool syncMode_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_IMAGE_IMAGE_COMPONENT_H
