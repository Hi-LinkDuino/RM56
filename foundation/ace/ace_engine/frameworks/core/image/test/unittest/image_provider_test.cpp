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

#include "gtest/gtest.h"

#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#include "core/image/test/unittest/image_provider_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}

class ImageProviderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: GIFSupport001
 * @tc.desc: if gif uri is right, loadImageData can get and decode the data.
 * @tc.type: FUNC
 * @tc.require: AR000DAQ40 AR000DACKU
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, GIFSupport001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create FileImageProvider.
     */
    auto imageLoader = FileImageLoader();
    /**
     * @tc.steps: step2. Call LoadImageData.
     * @tc.expected: step2. data should not be nullptr.
     */
    auto data = imageLoader.LoadImageData(ImageSourceInfo(FILE_GIF));
    ASSERT_TRUE(data);

    /**
     * @tc.steps: step3. Call GetCodec.
     * @tc.expected: step3. codec_ should not be nullptr.
     */
    auto codec = SkCodec::MakeFromData(data);
    ASSERT_TRUE(codec);
}

/**
 * @tc.name: GIFSupport002
 * @tc.desc: if gif uri is right, but the file is broken, cannot decode the data.
 * @tc.type: FUNC
 * @tc.require: AR000DAQ40 AR000DACKU
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, GIFSupport002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create FileImageProvider.
     */
    auto imageLoader = FileImageLoader();

    /**
     * @tc.steps: step2. Call LoadImageData.
     * @tc.expected: step2. data should not be nullptr.
     */
    auto data = imageLoader.LoadImageData(ImageSourceInfo(FILE_GIF_BROKEN));
    ASSERT_TRUE(data);

    /**
     * @tc.steps: step3. Call GetCodec.
     * @tc.expected: step3. codec_ should be nullptr.
     */
    auto codec = SkCodec::MakeFromData(data);
    ASSERT_TRUE(!codec);
}

/**
 * @tc.name: FileSupport001
 * @tc.desc: if file uri is right, test for loadImage can load and decode the image.
 * @tc.type: FUNC
 * @tc.require: AR000DACKU
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, FileSupport001, TestSize.Level1)
{
    std::vector<std::string> fileImages = { FILE_JPG, FILE_PNG, FILE_WEBP, FILE_BMP };
    for (auto file : fileImages) {
        /**
         * @tc.steps: step1. create FileImageProvider.
         */
        auto imageLoader = FileImageLoader();

        /**
         * @tc.steps: step2. Call LoadImageData.
         * @tc.expected: step2. data should not be nullptr.
         */
        auto data = imageLoader.LoadImageData(ImageSourceInfo(file));
        ASSERT_TRUE(data);

        /**
         * @tc.steps: step3. Call GetCodec..
         * @tc.expected: step3. codec_ should not be nullptr.
         */
        auto codec = SkCodec::MakeFromData(data);
        ASSERT_TRUE(codec);
    }
}

/**
 * @tc.name: FileSupport002
 * @tc.desc: if file uri is wrong, loadImageData can not load the image.
 * @tc.type: FUNC
 * @tc.require: AR000DACKU
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, FileSupport002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create FileImageProvider.
     */
    auto imageLoader = FileImageLoader();

    /**
     * @tc.steps: step2. Call LoadImageData.
     * @tc.expected: step2. data should be nullptr.
     */
    auto data = imageLoader.LoadImageData(ImageSourceInfo(NO_FILE));
    ASSERT_TRUE(!data);
}

/**
 * @tc.name: Create001
 * @tc.desc: given right uri, ImageProvider::Create can create an ImageProvider pointer.
 * @tc.type: FUNC
 * @tc.require: AR000DACKT AR000DACKU AR000DACKV
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, Create001, TestSize.Level1)
{
    std::vector<std::string> uris = { ASSET_IMAGE, FILE_GIF, NETWORK_HTTP_IMAGE, NETWORK_HTTPS_IMAGE };
    for (auto uri : uris) {
        /**
         * @tc.steps: step1. create ImageProvider by calling Create().
         * @tc.expected: step1. imageProvider should not be nullptr.
         */
        auto loader = ImageLoader::CreateImageLoader(ImageSourceInfo(uri));
        ASSERT_TRUE(loader);
    }
}

/**
 * @tc.name: Create002
 * @tc.desc: given wrong uri, ImageProvider::Create cannot create an ImageProvider pointer.
 * @tc.type: FUNC
 * @tc.require: AR000DACKT AR000DACKU AR000DACKV
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, Create002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create ImageProvider by calling Create().
     * @tc.expected: step1. imageProvider should be nullptr.
     */
    auto loader = ImageLoader::CreateImageLoader(ImageSourceInfo(WRONG_URI));
    ASSERT_TRUE(!loader);
}

/**
 * @tc.name: WrongImageData001
 * @tc.desc: given right uri, but broken image. FileImageProvider cannot decode the data.
 * @tc.type: FUNC
 * @tc.require: AR000DACKU
 * @tc.author: lushi
 */
HWTEST_F(ImageProviderTest, WrongImageData001, TestSize.Level1)
{
    std::vector<std::string> brokenFiles = { FILE_JPG_BROKEN, FILE_PNG_BROKEN, FILE_WEBP_BROKEN, FILE_BMP_BROKEN };
    for (auto brokenFile : brokenFiles) {
        /**
         * @tc.steps: step1. create FileImageProvider.
         */
        auto imageLoader = FileImageLoader();

        /**
         * @tc.steps: step2. Call LoadImageData.
         * @tc.expected: step2. data should not be nullptr.
         */
        auto data = imageLoader.LoadImageData(ImageSourceInfo(brokenFile));
        ASSERT_TRUE(data);

        /**
         * @tc.steps: step3. Call GetCodec.
         * @tc.expected: step3. codec_ should be nullptr.
         */
        auto codec = SkCodec::MakeFromData(data);
        ASSERT_TRUE(!codec);
    }
}

} // namespace OHOS::Ace