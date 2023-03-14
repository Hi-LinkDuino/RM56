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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_TEST_UNITTEST_IMAGE_PROVIDER_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_TEST_UNITTEST_IMAGE_PROVIDER_TEST_UTILS_H

#include "core/components/test/unittest/image/image_test_utils.h"
#define protected public
#include "core/components/display/render_display.h"
#include "core/components/stack/render_stack.h"
#define private public
#include "core/image/image_loader.h"
#include "core/image/image_provider.h"

namespace OHOS::Ace {

const std::string ASSET_IMAGE = "gifs/unbroken.gif";

const std::string FILE_GIF = "file:///data/test/resource/imageprovider/images/unbroken.gif";
const std::string FILE_GIF_BROKEN = "file:///data/test/resource/imageprovider/images/broken.gif";

const std::string FILE_JPG = "file:///data/test/resource/imageprovider/images/unbroken.jpg";
const std::string FILE_JPG_BROKEN = "file:///data/test/resource/imageprovider/images/broken.jpg";

const std::string FILE_PNG = "file:///data/test/resource/imageprovider/images/unbroken.png";
const std::string FILE_PNG_BROKEN = "file:///data/test/resource/imageprovider/images/broken.png";

const std::string FILE_WEBP = "file:///data/test/resource/imageprovider/images/unbroken.webp";
const std::string FILE_WEBP_BROKEN = "file:///data/test/resource/imageprovider/images/broken.webp";

const std::string FILE_BMP = "file:///data/test/resource/imageprovider/images/unbroken.bmp";
const std::string FILE_BMP_BROKEN = "file:///data/test/resource/imageprovider/images/broken.bmp";

const std::string NO_FILE = "file:///nofile";

const std::string NETWORK_HTTP_IMAGE = "http://data/images/webps/image.xxx";
const std::string NETWORK_HTTPS_IMAGE = "https://data/images/webps/image.xxx";

const std::string WRONG_URI = "files:///wrong uri";

size_t GetFileSize(FILE* file)
{
    fseek(file, 0, SEEK_END);
    size_t readLen = ftell(file);
    fseek(file, 0, SEEK_SET);
    return readLen;
}

std::vector<uint8_t> ReadFromFile(std::unique_ptr<FILE, decltype(&fclose)> file)
{
    std::vector<uint8_t> result;
    if (file == nullptr) {
        return result;
    }
    size_t fileSize = GetFileSize(file.get());
    if (fileSize != 0) {
        result.resize(fileSize);
        size_t num = fread(&result[0], 1, fileSize, file.get());
        if (num > fileSize) {
            GTEST_LOG_(INFO) << "file is becoming bigger!";
            return result;
        }
        if (num != fileSize) {
            GTEST_LOG_(INFO) << "file is becoming smaller!";
            result.resize(num);
        }
    }
    return result;
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_TEST_UNITTEST_IMAGE_PROVIDER_TEST_UTILS_H
