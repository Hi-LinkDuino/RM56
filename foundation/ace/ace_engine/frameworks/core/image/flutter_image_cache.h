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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_FLUTTER_IMAGE_CACHE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_FLUTTER_IMAGE_CACHE_H

#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/painting/image.h"

#include "core/image/image_cache.h"
#include "core/image/image_object.h"

namespace OHOS::Ace {

struct CachedImage {
    explicit CachedImage(const fml::RefPtr<flutter::CanvasImage>& image) : imagePtr(image) {}
    fml::RefPtr<flutter::CanvasImage> imagePtr;
};

struct SkiaCachedImageData : public CachedImageData {
    DECLARE_ACE_TYPE(SkiaCachedImageData, CachedImageData);
public:
    explicit SkiaCachedImageData(const sk_sp<SkData>& data) : imageData(data) {}
    ~SkiaCachedImageData() override = default;

    size_t GetSize() override
    {
        return imageData ? imageData->size() : 0;
    }

    const uint8_t* GetData() override
    {
        return imageData ? imageData->bytes() : nullptr;
    }

    sk_sp<SkData> imageData;
};

class FlutterImageCache : public ImageCache {
    DECLARE_ACE_TYPE(FlutterImageCache, ImageCache);

public:
    FlutterImageCache() = default;
    ~FlutterImageCache() override = default;
    void Clear() override;
    RefPtr<CachedImageData> GetDataFromCacheFile(const std::string& filePath) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_FLUTTER_IMAGE_CACHE_H
