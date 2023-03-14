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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_TEXTURE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_TEXTURE_COMPONENT_H

#include <string>

#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/render_component.h"
#include "frameworks/core/components/common/properties/decoration.h"

namespace OHOS::Ace {

inline constexpr int64_t INVALID_TEXTURE = -1;
inline constexpr double IMAGE_POSITION_DEFAULT_SIZE = 50.0;

// A component can show Texture.
class TextureComponent : public RenderComponent {
    DECLARE_ACE_TYPE(TextureComponent, RenderComponent);

public:
    TextureComponent()
    {
        imagePosition_.SetSizeValueX(IMAGE_POSITION_DEFAULT_SIZE);
        imagePosition_.SetSizeValueY(IMAGE_POSITION_DEFAULT_SIZE);
        imagePosition_.SetSizeTypeX(BackgroundImagePositionType::PERCENT);
        imagePosition_.SetSizeTypeY(BackgroundImagePositionType::PERCENT);
    }
    ~TextureComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    int64_t GetTextureId() const
    {
        return textureId_;
    }

    void SetTextureId(int64_t textureId)
    {
        textureId_ = textureId;
    }

    uint32_t GetSrcWidth() const
    {
        return srcWidth_;
    }

    uint32_t GetSrcHeight() const
    {
        return srcHeight_;
    }

    void SetSrcWidth(uint32_t width)
    {
        srcWidth_ = width;
    }

    void SetSrcHeight(uint32_t height)
    {
        srcHeight_ = height;
    }

    void SetFit(ImageFit fit)
    {
        imageFit_ = fit;
    }

    ImageFit GetFit() const
    {
        return imageFit_;
    }

    RefPtr<Component> GetChild() const
    {
        return child_;
    }

    void SetChild(const RefPtr<Component>& child)
    {
        child_ = child;
    }

    void SetImagePosition(ImageObjectPosition imagePosition)
    {
        imagePosition_ = imagePosition;
    }

    ImageObjectPosition GetImagePosition() const
    {
        return imagePosition_;
    }

private:
    int64_t textureId_ = INVALID_TEXTURE;
    uint32_t srcWidth_ = 0;
    uint32_t srcHeight_ = 0;
    ImageFit imageFit_ { ImageFit::CONTAIN };
    ImageObjectPosition imagePosition_;

    RefPtr<Component> child_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_TEXTURE_COMPONENT_H
