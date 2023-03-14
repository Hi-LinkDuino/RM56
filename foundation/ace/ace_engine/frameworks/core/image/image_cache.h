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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_CACHE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_CACHE_H

#include <algorithm>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "base/utils/macros.h"

namespace OHOS::Ace {

struct CachedImage;
class ImageObject;
template<typename T>
struct CacheNode {
    CacheNode(const std::string& key, const T& obj)
        : cacheKey(key), cacheObj(obj)
    {}
    std::string cacheKey;
    T cacheObj;
};

struct CachedImageData : public AceType {
    DECLARE_ACE_TYPE(CachedImageData, AceType);
public:
    CachedImageData() = default;
    virtual ~CachedImageData() = default;
    virtual size_t GetSize() = 0;
    virtual const uint8_t* GetData() = 0;
};

struct CacheImageDataNode {
    CacheImageDataNode(const std::string& key, const RefPtr<CachedImageData>& imageData)
        : imageDataKey(key), imageDataPtr(imageData)
    {}
    std::string imageDataKey;
    RefPtr<CachedImageData> imageDataPtr;
};

struct FileInfo {
    FileInfo(const std::string& path, size_t size, time_t time)
        : filePath(path), fileSize(size), accessTime(time)
    {}

    // file information will be sort by access time.
    bool operator<(const FileInfo& otherFile) const
    {
        return accessTime < otherFile.accessTime;
    }
    std::string filePath;
    size_t fileSize;
    time_t accessTime;
};

class ACE_EXPORT ImageCache : public AceType {
    DECLARE_ACE_TYPE(ImageCache, AceType);

public:
    static RefPtr<ImageCache> Create();
    ImageCache() = default;
    virtual ~ImageCache() = default;
    void CacheImage(const std::string& key, const std::shared_ptr<CachedImage>& image);
    std::shared_ptr<CachedImage> GetCacheImage(const std::string& key);

    void CacheImageData(const std::string& key, const RefPtr<CachedImageData>& imageData);
    RefPtr<CachedImageData> GetCacheImageData(const std::string& key);

    void CacheImgObj(const std::string& key, const RefPtr<ImageObject>& imgObj);
    RefPtr<ImageObject> GetCacheImgObj(const std::string& key);

    static void SetCacheFileInfo();
    static void WriteCacheFile(const std::string& url, const void * const data, const size_t size);

    void SetCapacity(size_t capacity)
    {
        LOGI("Set Capacity : %{public}d", static_cast<int32_t>(capacity));
        capacity_ = capacity;
    }

    void SetDataCacheLimit(size_t sizeLimit)
    {
        LOGI("Set data size cache limit : %{public}d", static_cast<int32_t>(sizeLimit));
        dataSizeLimit_ = sizeLimit;
    }

    size_t GetCapacity() const
    {
        return capacity_;
    }

    size_t GetCachedImageCount() const
    {
        std::lock_guard<std::mutex> lock(cacheListMutex_);
        return cacheList_.size();
    }

    static void SetImageCacheFilePath(const std::string& cacheFilePath)
    {
        std::unique_lock<std::shared_mutex> lock(cacheFilePathMutex_);
        if (cacheFilePath_.empty()) {
            cacheFilePath_ = cacheFilePath;
        }
    }

    static std::string GetImageCacheFilePath()
    {
        std::shared_lock<std::shared_mutex> lock(cacheFilePathMutex_);
        return cacheFilePath_;
    }

    static std::string GetImageCacheFilePath(const std::string& url)
    {
        std::shared_lock<std::shared_mutex> lock(cacheFilePathMutex_);
#if !defined(WINDOWS_PLATFORM) && !defined(MAC_PLATFORM)
        return cacheFilePath_ + "/" + std::to_string(std::hash<std::string> {}(url));
#elif defined(MAC_PLATFORM)
        return "/tmp/" + std::to_string(std::hash<std::string> {}(url));
#elif defined(WINDOWS_PLATFORM)
        char *pathvar;
        pathvar = getenv("TEMP");
        if (!pathvar) {
            return std::string("C:\\Windows\\Temp") + "\\" + std::to_string(std::hash<std::string> {}(url));
        }
        return std::string(pathvar) + "\\" + std::to_string(std::hash<std::string> {}(url));
#endif
    }

    static void SetCacheFileLimit(size_t cacheFileLimit)
    {
        LOGI("Set file cache limit size : %{public}d", static_cast<int32_t>(cacheFileLimit));
        cacheFileLimit_ = cacheFileLimit;
    }

    static void SetClearCacheFileRatio(float clearRatio)
    {
        // clearRatio must in (0, 1].
        if (clearRatio < 0) {
            clearRatio = 0.1f;
        } else if (clearRatio > 1) {
            clearRatio = 1.0f;
        }
        clearCacheFileRatio_ = clearRatio;
    }

    static bool GetFromCacheFile(const std::string& filePath);

    virtual void Clear() = 0;

    virtual RefPtr<CachedImageData> GetDataFromCacheFile(const std::string& filePath) = 0;

    static void Purge();

protected:
    static void ClearCacheFile(const std::vector<std::string>& removeFiles);

    template<typename T>
    static void CacheWithCountLimitLRU(
        const std::string& key,
        const T& cacheObj,
        std::list<CacheNode<T>>& cacheList,
        std::unordered_map<std::string, typename std::list<CacheNode<T>>::iterator>& cache,
        const std::atomic<size_t>& capacity);

    template<typename T>
    static T GetCacheObjWithCountLimitLRU(
        const std::string& key,
        std::list<CacheNode<T>>& cacheList,
        std::unordered_map<std::string, typename std::list<CacheNode<T>>::iterator>& cache);

    static bool GetFromCacheFileInner(const std::string& filePath);

    bool processImageDataCacheInner(size_t dataSize);

    mutable std::mutex cacheListMutex_;
    std::list<CacheNode<std::shared_ptr<CachedImage>>> cacheList_;

    std::mutex imageCacheMutex_;
    std::unordered_map<std::string, std::list<CacheNode<std::shared_ptr<CachedImage>>>::iterator> imageCache_;

    std::atomic<size_t> capacity_ = 0; // by default memory cache can store 0 images.

    mutable std::mutex dataCacheListMutex_;
    std::list<CacheImageDataNode> dataCacheList_;

    std::mutex imageDataCacheMutex_;
    std::unordered_map<std::string, std::list<CacheImageDataNode>::iterator> imageDataCache_;

    std::atomic<size_t> dataSizeLimit_ = 0; // by default, image data before decoded cache is 0 MB.;
    std::atomic<size_t> curDataSize_ = 0;

    std::mutex cacheImgObjListMutex_;
    std::list<CacheNode<RefPtr<ImageObject>>> cacheImgObjList_;
    std::mutex imgObjCacheMutex_;
    std::unordered_map<std::string, std::list<CacheNode<RefPtr<ImageObject>>>::iterator> imgObjCache_;
    std::atomic<size_t> imgObjCapacity_ = 2000; // imgObj is cached after clear image data.

    static std::shared_mutex cacheFilePathMutex_;
    static std::string cacheFilePath_;

    static std::atomic<size_t> cacheFileLimit_;

    static std::atomic<float> clearCacheFileRatio_;

    static std::mutex cacheFileSizeMutex_;
    static int32_t cacheFileSize_;

    static std::mutex cacheFileInfoMutex_;
    static std::list<FileInfo> cacheFileInfo_;
    static bool hasSetCacheFileInfo_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_IMAGE_CACHE_H
