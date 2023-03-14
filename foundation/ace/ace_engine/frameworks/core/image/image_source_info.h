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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_ACE_IMAGE_SOURCE_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_ACE_IMAGE_SOURCE_INFO_H

#include <optional>

#include "base/geometry/dimension.h"
#include "base/geometry/size.h"
#include "base/image/pixel_map.h"
#include "base/resource/internal_resource.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace {
class ImageSourceInfo {
public:
    static bool IsSVGSource(const std::string& imageSrc, InternalResource::ResourceId resourceId);
    static SrcType ResolveURIType(const std::string& uri);
    static bool IsValidBase64Head(const std::string& uri, const std::string& pattern);

    explicit ImageSourceInfo(
        const std::string& imageSrc,
        Dimension width = Dimension(-1),
        Dimension height = Dimension(-1),
        InternalResource::ResourceId resourceId = InternalResource::ResourceId::NO_ID,
        const RefPtr<PixelMap>& pixmap = nullptr);

    ImageSourceInfo() = default;
    ~ImageSourceInfo() = default;

    bool operator==(const ImageSourceInfo& info) const
    {
        return ((!pixmap_ && !info.pixmap_) || (pixmap_ && info.pixmap_ && pixmap_ == info.pixmap_)) &&
                // TODO: Use GetModifyId to distinguish two PixelMap objects after Media provides it
               src_ == info.src_ &&
               resourceId_ == info.resourceId_ &&
               sourceWidth_ == info.sourceWidth_ &&
               sourceHeight_ == info.sourceHeight_ &&
               fillColor_ == info.fillColor_;
    }

    bool operator!=(const ImageSourceInfo& info) const
    {
        return (!pixmap_ && info.pixmap_) ||
               (pixmap_ && !info.pixmap_) ||
               (pixmap_ && info.pixmap_ && pixmap_ != info.pixmap_) ||
                // TODO: Use GetModifyId to distinguish two PixelMap objects after Media provides it
               src_ != info.src_ ||
               resourceId_ != info.resourceId_ ||
               sourceWidth_ != info.sourceWidth_ ||
               sourceHeight_ != info.sourceHeight_ ||
               fillColor_ != info.fillColor_;
    }

    void SetSrc(const std::string& src, std::optional<Color> fillColor = std::nullopt)
    {
        src_ = src;
        resourceId_ = InternalResource::ResourceId::NO_ID;
        isSvg_ = IsSVGSource(src_, resourceId_);
        fillColor_ = fillColor;
        pixmap_ = nullptr;
    }

    const std::string& GetSrc() const
    {
        return src_;
    }

    void SetResourceId(InternalResource::ResourceId id, std::optional<Color> fillColor = std::nullopt)
    {
        resourceId_ = id;
        src_.clear();
        isSvg_ = IsSVGSource(src_, resourceId_);
        fillColor_ = fillColor;
        pixmap_ = nullptr;
    }

    InternalResource::ResourceId GetResourceId() const
    {
        return resourceId_;
    }

    bool IsInternalResource() const
    {
        return src_.empty() && resourceId_ != InternalResource::ResourceId::NO_ID && !pixmap_;
    }

    bool IsValid() const
    {
        return (src_.empty() && resourceId_ != InternalResource::ResourceId::NO_ID) ||
               (!src_.empty() && resourceId_ == InternalResource::ResourceId::NO_ID) ||
               pixmap_;
    }

    bool IsSvg() const
    {
        return isSvg_;
    }

    bool IsPixmap() const
    {
        return pixmap_ != nullptr;
    }

    SrcType GetSrcType() const
    {
        return srcType_;
    }

    std::string ToString() const
    {
        if (!src_.empty()) {
            return src_ + std::string("w") + std::to_string(sourceWidth_.Value()) +
                std::string("h") + std::to_string(sourceHeight_.Value());
        } else if (resourceId_ != InternalResource::ResourceId::NO_ID) {
            return std::string("internal resource id: ") + std::to_string(static_cast<int32_t>(resourceId_));
        } else if (pixmap_) {
            return std::string("pixmapID: ") + pixmap_->GetId() +
                std::string(" -> modifyID: ") + pixmap_->GetModifyId();
        } else {
            return std::string("empty source");
        }
    }

    void SetDimension(Dimension width, Dimension Height)
    {
        sourceWidth_ = width;
        sourceHeight_ = Height;
    }

    bool IsSourceDimensionValid() const
    {
        return sourceWidth_.IsValid() && sourceHeight_.IsValid();
    }

    Size GetSourceSize() const
    {
        return Size(sourceWidth_.Value(), sourceHeight_.Value());
    }

    void Reset()
    {
        src_.clear();
        sourceWidth_ = Dimension(-1);
        sourceHeight_ = Dimension(-1);
        resourceId_ = InternalResource::ResourceId::NO_ID;
        isSvg_ = false;
        fillColor_.reset();
        pixmap_ = nullptr;
        cacheKey_.clear();
    }

    void SetFillColor(const Color& color);

    std::optional<Color> GetFillColor() const
    {
        return fillColor_;
    }

    const RefPtr<PixelMap>& GetPixmap() const
    {
        return pixmap_;
    }

    const std::string& GetCacheKey() const
    {
        return cacheKey_;
    }

private:
    SrcType ResolveSrcType() const;

    std::string src_;
    std::string cacheKey_;
    Dimension sourceWidth_ = Dimension(-1);
    Dimension sourceHeight_ = Dimension(-1);
    InternalResource::ResourceId resourceId_ = InternalResource::ResourceId::NO_ID;
    RefPtr<PixelMap> pixmap_;
    bool isSvg_ = false;

    // only Svg will set it.
    std::optional<Color> fillColor_;

    // image source type for example:FILE, ASSET, NETWORK, MEMORY, BASE64, INTERNAL, RESOURCE or DATA_ABILITY,
    SrcType srcType_ = SrcType::UNSUPPORTED;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_ACE_IMAGE_SOURCE_INFO_H