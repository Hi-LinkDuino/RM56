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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_LOADER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_LOADER_H

#include <regex>
#include <string>

#include "third_party/skia/include/core/SkImage.h"

#include "base/geometry/size.h"
#include "base/memory/ace_type.h"
#include "base/resource/internal_resource.h"
#include "base/resource/shared_image_manager.h"
#include "core/components/common/layout/constants.h"
#include "core/image/image_source_info.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class ImageLoader : public virtual AceType {
    DECLARE_ACE_TYPE(ImageLoader, AceType);

public:
    virtual sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) = 0;

    static std::string RemovePathHead(const std::string& uri);
    static RefPtr<ImageLoader> CreateImageLoader(const ImageSourceInfo& imageSourceInfo);
    static sk_sp<SkData> LoadDataFromCachedFile(const std::string& uri);
};

// File image provider: read image from file.
class FileImageLoader : public ImageLoader {
public:
    FileImageLoader() = default;
    ~FileImageLoader() override = default;
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;
};

// data provider image loader.
class DataProviderImageLoader : public ImageLoader {
public:
    DataProviderImageLoader() = default;
    ~DataProviderImageLoader() override = default;
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;
};

class AssetImageLoader final : public ImageLoader {
public:
    AssetImageLoader() = default;
    ~AssetImageLoader() override = default;
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;
    std::string LoadJsonData(const std::string& src, const WeakPtr<PipelineContext> context = nullptr);
};

// Network image provider: read image from network.
class NetworkImageLoader final : public ImageLoader {
public:
    NetworkImageLoader() = default;
    ~NetworkImageLoader() override = default;
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;
};

class InternalImageLoader final : public ImageLoader {
public:
    InternalImageLoader() = default;
    ~InternalImageLoader() override = default;
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;
};

class Base64ImageLoader final : public ImageLoader {
public:
    Base64ImageLoader() = default;
    ~Base64ImageLoader() override = default;
    static std::string GetBase64ImageCode(const std::string& uri, size_t& imagSize);
    static size_t GetBase64ImageSize(const std::string& code);
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;
};

class ResourceImageLoader final : public ImageLoader {
public:
    ResourceImageLoader() = default;
    ~ResourceImageLoader() override = default;
    sk_sp<SkData> LoadImageData(
        const ImageSourceInfo& imageSourceInfo, const WeakPtr<PipelineContext> context = nullptr) override;

private:
    bool GetResourceId(const std::string& uri, const RefPtr<ThemeConstants>& themeContants, uint32_t& resId) const;
    bool GetResourceId(const std::string& uri, const RefPtr<ThemeConstants>& themeContants, std::string& path) const;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_LOADER_H