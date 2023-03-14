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

#include "core/image/test/unittest/image_cache_test.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

class ImageCacheTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp()
    {
        imageCache->SetCapacity(80);
    }
    void TearDown() {}

    RefPtr<ImageCache> imageCache = ImageCache::Create();
};

/**
 * @tc.name: MemoryCache001
 * @tc.desc: new image success insert into cache with LRU.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, MemoryCache001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. cache images one by one.
     * @tc.expected: new item should at begin of cacheList_ and imagCache has right iters.
     */
    for (size_t i = 0; i < CACHE_FILES.size(); i++) {
        imageCache->CacheImage(FILE_KEYS[i], std::make_shared<CachedImage>(flutter::CanvasImage::Create()));
        std::string frontKey = (imageCache->cacheList_).front().cacheKey;
        ASSERT_EQ(frontKey, FILE_KEYS[i]);
        ASSERT_EQ(frontKey, imageCache->imageCache_[FILE_KEYS[i]]->cacheKey);
    }

    /**
     * @tc.steps: step2. cache a image already in cache for example FILE_KEYS[3] e.t. "key4".
     * @tc.expected: the cached item should at begin of cacheList_ and imagCache has right iters.
     */
    imageCache->CacheImage(FILE_KEYS[3], std::make_shared<CachedImage>(flutter::CanvasImage::Create()));
    ASSERT_EQ(imageCache->cacheList_.front().cacheKey, FILE_KEYS[3]);
    ASSERT_EQ(imageCache->imageCache_[FILE_KEYS[3]]->cacheKey, FILE_KEYS[3]);
}

/**
 * @tc.name: MemoryCache002
 * @tc.desc: get image success in cache with LRU.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, MemoryCache002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. cache images one by one.
     */
    for (size_t i = 0; i < CACHE_FILES.size(); i++) {
        imageCache->CacheImage(FILE_KEYS[i], std::make_shared<CachedImage>(flutter::CanvasImage::Create()));
        std::string frontKey = (imageCache->cacheList_).front().cacheKey;
    }
    /**
     * @tc.steps: step2. find a image already in cache for example FILE_KEYS[2] e.t. "key3".
     * @tc.expected: the cached iterator should not at end() of cacheList_ and imagCache.
     *              after GetImageCache(), the item should at begin() of cacheList_.
     */
    auto iter = (imageCache->imageCache_).find(FILE_KEYS[2]);
    ASSERT_NE(iter, imageCache->imageCache_.end());
    imageCache->GetCacheImage(FILE_KEYS[2]);
    ASSERT_EQ(imageCache->cacheList_.front().cacheKey, FILE_KEYS[2]);
    ASSERT_EQ(imageCache->imageCache_[FILE_KEYS[2]]->cacheKey, FILE_KEYS[2]);

    /**
     * @tc.steps: step3. find a image not in cache for example "key8".
     * @tc.expected: return null.
     */
    auto image = imageCache->GetCacheImage("key8");
    ASSERT_EQ(image, nullptr);
}

/**
 * @tc.name: MemoryCache003
 * @tc.desc: Set memory cache capacity success.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, MemoryCache003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call set capacity.
     * @tc.expected: capacity set to 1000.
     */
    imageCache->SetCapacity(1000);
    ASSERT_EQ(static_cast<int32_t>(imageCache->capacity_), 1000);
}

/**
 * @tc.name: MemoryCache004
 * @tc.desc: memory cache of image data.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, MemoryCache004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. set data limit to 10 bytes, cache some data.check result
     * @tc.expected: result is right.
     */
    imageCache->dataSizeLimit_ = 10;

    // create 3 bytes data, cache it, current size is 3
    const uint8_t data1[] = {'a', 'b', 'c' };
    sk_sp<SkData> skData1 = SkData::MakeWithCopy(data1, 3);
    auto cachedData1 = AceType::MakeRefPtr<SkiaCachedImageData>(skData1);
    imageCache->CacheImageData(KEY_1, cachedData1);
    ASSERT_EQ(imageCache->curDataSize_, 3u);

    // create 2 bytes data, cache it, current size is 5. {abc} {de}
    const uint8_t data2[] = {'d', 'e' };
    sk_sp<SkData> skData2 = SkData::MakeWithCopy(data2, 2);
    auto cachedData2 = AceType::MakeRefPtr<SkiaCachedImageData>(skData2);
    imageCache->CacheImageData(KEY_2, cachedData2);
    ASSERT_EQ(imageCache->curDataSize_, 5u);

    // create 7 bytes data, cache it, current size is 5. new data not cached.
    const uint8_t data3[] = { 'f', 'g', 'h', 'i', 'j', 'k', 'l' };
    sk_sp<SkData> skData3 = SkData::MakeWithCopy(data3, 7);
    auto cachedData3 = AceType::MakeRefPtr<SkiaCachedImageData>(skData3);
    imageCache->CacheImageData(KEY_3, cachedData3);
    ASSERT_EQ(imageCache->curDataSize_, 5u);
    auto data = imageCache->GetCacheImageData(KEY_3);
    ASSERT_EQ(data, nullptr);

    // create 5 bytes data, cache it, current size is 10 {abc} {de} {mnopq}
    const uint8_t data4[] = { 'm', 'n', 'o', 'p', 'q' };
    sk_sp<SkData> skData4 = SkData::MakeWithCopy(data4, 5);
    auto cachedData4 = AceType::MakeRefPtr<SkiaCachedImageData>(skData4);
    imageCache->CacheImageData(KEY_4, cachedData4);
    ASSERT_EQ(imageCache->curDataSize_, 10u);

    // create 2 bytes data, cache it, current size is 9 {de}{mnopq}{rs}
    const uint8_t data5[] = { 'r', 's' };
    sk_sp<SkData> skData5 = SkData::MakeWithCopy(data5, 2);
    auto cachedData5 = AceType::MakeRefPtr<SkiaCachedImageData>(skData5);
    imageCache->CacheImageData(KEY_5, cachedData5);
    ASSERT_EQ(imageCache->curDataSize_, 9u);

    // create 5 bytes, cache it, current size is 7 {rs}{tuvwx}
    const uint8_t data6[] = { 't', 'u', 'v', 'w', 'x' };
    sk_sp<SkData> skData6 = SkData::MakeWithCopy(data6, 5);
    auto cachedData6 = AceType::MakeRefPtr<SkiaCachedImageData>(skData6);
    imageCache->CacheImageData(KEY_6, cachedData6);
    ASSERT_EQ(imageCache->curDataSize_, 7u);

    // cache data witch is already cached. {rs}{y}
    const uint8_t data7[] = { 'y' };
    sk_sp<SkData> skData7 = SkData::MakeWithCopy(data7, 1);
    auto cachedData7 = AceType::MakeRefPtr<SkiaCachedImageData>(skData7);
    imageCache->CacheImageData(KEY_6, cachedData7);
    ASSERT_EQ(imageCache->curDataSize_, 3u);

    // cache data witch is already cached. {y}{fg}
    const uint8_t data8[] = { 'f', 'g' };
    sk_sp<SkData> skData8 = SkData::MakeWithCopy(data8, 2);
    auto cachedData8 = AceType::MakeRefPtr<SkiaCachedImageData>(skData8);
    imageCache->CacheImageData(KEY_5, cachedData8);
    ASSERT_EQ(imageCache->curDataSize_, 3u);
    auto dataFront = imageCache->dataCacheList_.front().imageDataPtr->GetData();
    for (int i = 0; i < 2; ++i) {
        ASSERT_EQ(dataFront[i], data8[i]);
    }
    auto dataKey5 = imageCache->GetCacheImageData(KEY_5);
    auto dataRaw5 = dataKey5->GetData();
    ASSERT_EQ(dataFront, dataRaw5);

    // Get key6
    auto dataKey6 = imageCache->GetCacheImageData(KEY_6);
    auto dataRaw6 = dataKey6->GetData();
    ASSERT_EQ(dataRaw6[0], 'y');
    dataFront = imageCache->dataCacheList_.front().imageDataPtr->GetData();
    ASSERT_EQ(dataFront, dataRaw6);
}

/**
 * @tc.name: FileCache001
 * @tc.desc: init cacheFilePath and cacheFileInfo success.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, FileCache001, TestSize.Level1)
{
    /**
     * @tc.steps: step1.call SetImageCacheFilePath().
     * @tc.expected: cache file size init right and cache file Info init right.
     */
    ImageCache::SetImageCacheFilePath(CACHE_FILE_PATH);
    ASSERT_EQ(ImageCache::cacheFilePath_, CACHE_FILE_PATH);

    /**
     * @tc.steps: step2. call SetCacheFileInfo().
     * @tc.expected: file info init right.
     */
    ImageCache::SetCacheFileInfo();
    ASSERT_EQ(ImageCache::cacheFileSize_, FILE_SIZE);
    ASSERT_EQ(static_cast<int32_t>(ImageCache::cacheFileInfo_.size()), 5);
    size_t i = 0;
    auto iter = ImageCache::cacheFileInfo_.begin();
    while (i < TEST_COUNT - 1) {
        ASSERT_LE(iter->accessTime, (++iter)->accessTime);
        i++;
    }
}

/**
 * @tc.name: FileCache002
 * @tc.desc: write data into cacheFilePath success.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, FileCache002, TestSize.Level1)
{
    /**
     * @tc.steps: step1.construct a data.
     */
    std::vector<uint8_t> imageData = { 1, 2, 3, 4, 5, 6 };
    std::string url = "http:/testfilecache002/image";

    /**
     * @tc.steps: step2. call WriteCacheFile().
     * @tc.expected: file write into filePath and file info update right.
     */
    ImageCache::WriteCacheFile(url, imageData.data(), imageData.size());
    ASSERT_EQ(ImageCache::cacheFileSize_, static_cast<int32_t>(FILE_SIZE + imageData.size()));
    ASSERT_EQ(ImageCache::cacheFileInfo_.size(), TEST_COUNT + 1);
    auto iter = ImageCache::cacheFileInfo_.rbegin();

    ASSERT_EQ(iter->filePath, ImageCache::GetImageCacheFilePath(url));
}

/**
 * @tc.name: FileCache003
 * @tc.desc: Get data from cacheFilePath success with right url. but null with wrong url.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, FileCache003, TestSize.Level1)
{
    /**
     * @tc.steps: step1.set cacheFileLimit_ to 0.
     */
    std::string wrongFilePath = "/data/wrong_data";
    /**
     * @tc.steps: step2. call GetFromCacheFile().
     * @tc.expected:data != nullptr.
     */
    auto data = ImageCache::GetFromCacheFile(CACHE_IMAGE_FILE_2);
    ASSERT_TRUE(data);
    auto nullData = ImageCache::GetFromCacheFile(wrongFilePath);
    ASSERT_TRUE(!nullData);
}

/**
 * @tc.name: FileCache004
 * @tc.desc: clear files from cacheFilePath success while write file exceed limit.
 * @tc.type: FUNC
 */
HWTEST_F(ImageCacheTest, FileCache004, TestSize.Level1)
{
    /**
     * @tc.steps: step1.set cacheFileLimit_ to 0.
     */
    ImageCache::SetCacheFileLimit(0);
    ASSERT_EQ(static_cast<int32_t>(ImageCache::cacheFileLimit_), 0);

    /**
     * @tc.steps: step2. call WriteCacheFile().
     * @tc.expected: file write into filePath and file info update right.
     */
    std::vector<uint8_t> imageData = { 1, 2, 3 };
    std::string url = "http:/testfilecache003/image";
    ImageCache::WriteCacheFile(url, imageData.data(), imageData.size());
    float ratio = ImageCache::clearCacheFileRatio_;
    ASSERT_EQ(ImageCache::cacheFileInfo_.size(), static_cast<size_t>((TEST_COUNT + 2) * ratio + 1));
    ASSERT_LE(ImageCache::cacheFileSize_, FILE_SIZE);
}

} // namespace OHOS::Ace