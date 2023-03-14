/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pixel_map_ohos.h"

#include "base/log/log_wrapper.h"

namespace OHOS::Ace {

PixelFormat PixelMapOhos::PixelFormatConverter(Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::RGB_565:
            return PixelFormat::RGB_565;
        case Media::PixelFormat::RGBA_8888:
            return PixelFormat::RGBA_8888;
        case Media::PixelFormat::BGRA_8888:
            return PixelFormat::BGRA_8888;
        case Media::PixelFormat::ALPHA_8:
            return PixelFormat::ALPHA_8;
        case Media::PixelFormat::RGBA_F16:
            return PixelFormat::RGBA_F16;
        case Media::PixelFormat::UNKNOWN:
            return PixelFormat::UNKNOWN;
        case Media::PixelFormat::ARGB_8888:
            return PixelFormat::ARGB_8888;
        case Media::PixelFormat::RGB_888:
            return PixelFormat::RGB_888;
        case Media::PixelFormat::NV21:
            return PixelFormat::NV21;
        case Media::PixelFormat::NV12:
            return PixelFormat::NV12;
        case Media::PixelFormat::CMYK:
            return PixelFormat::CMYK;
        default:
            return PixelFormat::UNKNOWN;
    }
}

AlphaType PixelMapOhos::AlphaTypeConverter(Media::AlphaType alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
        default:
            return AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    }
}

RefPtr<PixelMap> PixelMap::CreatePixelMap(void* rawPtr)
{
    std::shared_ptr<Media::PixelMap>* pixmapPtr = reinterpret_cast<std::shared_ptr<Media::PixelMap>*>(rawPtr);
    if (*pixmapPtr == nullptr) {
        LOGE("pixmap pointer is nullptr.");
    }
    return AceType::MakeRefPtr<PixelMapOhos>(*pixmapPtr);
}

int32_t PixelMapOhos::GetWidth() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, return default width.");
        return 0;
    }
    return pixmap_->GetWidth();
}

int32_t PixelMapOhos::GetHeight() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, return default height.");
        return 0;
    }
    return pixmap_->GetHeight();
}

const uint8_t* PixelMapOhos::GetPixels() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, return default address of pixels.");
        return nullptr;
    }
    return pixmap_->GetPixels();
}

PixelFormat PixelMapOhos::GetPixelFormat() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, return default pixel format.");
        return PixelFormat::UNKNOWN;
    }
    return PixelFormatConverter(pixmap_->GetPixelFormat());
}

AlphaType PixelMapOhos::GetAlphaType() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, return default alpha type.");
        return AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    }
    return AlphaTypeConverter(pixmap_->GetAlphaType());
}

int32_t PixelMapOhos::GetRowBytes() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, return default row bytes.");
        return 0;
    }
    return pixmap_->GetRowBytes();
}

int32_t PixelMapOhos::GetByteCount() const
{
    if (!pixmap_) {
        return 0;
    }
    return pixmap_->GetByteCount();
}

void* PixelMapOhos::GetPixelManager() const
{
    Media::InitializationOptions opts;
    if (!pixmap_) {
        return nullptr;
    }
    auto newPixelMap = Media::PixelMap::Create(*pixmap_, opts);
    return reinterpret_cast<void*>(new Media::PixelMapManager(newPixelMap.release()));
}

void* PixelMapOhos::GetRawPixelMapPtr() const
{
    if (!pixmap_) {
        LOGE("pixmap is nullptr, raw pixel map pointer is nullptr.");
        return nullptr;
    }
    return pixmap_.get();
}

std::string PixelMapOhos::GetId()
{
    return std::string();
}

std::string PixelMapOhos::GetModifyId()
{
    return std::string();
}

} // namespace OHOS::Ace