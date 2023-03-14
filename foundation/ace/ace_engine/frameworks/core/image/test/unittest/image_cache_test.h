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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_TEST_UNITTEST_IMAGE_CACHE_TEST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_TEST_UNITTEST_IMAGE_CACHE_TEST_H

#define private public
#define protected public
#include "core/image/flutter_image_cache.h"

namespace OHOS::Ace {

const int32_t FILE_SIZE = 15;
const std::string CACHE_FILE_PATH = "/data/test/resource/imagecache/images";
const std::string CACHE_IMAGE_FILE_1 = "/data/test/resource/imagecache/images/748621363886323660";
const std::string CACHE_IMAGE_FILE_2 = "/data/test/resource/imagecache/images/8819493328252140263";
const std::string CACHE_IMAGE_FILE_3 = "/data/test/resource/imagecache/images/1008157312073340586";
const std::string CACHE_IMAGE_FILE_4 = "/data/test/resource/imagecache/images/13610839755484614436";
const std::string CACHE_IMAGE_FILE_5 = "/data/test/resource/imagecache/images/5841967474238710136";
const std::vector<std::string> CACHE_FILES = { CACHE_IMAGE_FILE_1, CACHE_IMAGE_FILE_2, CACHE_IMAGE_FILE_3,
    CACHE_IMAGE_FILE_4, CACHE_IMAGE_FILE_5 };
const size_t TEST_COUNT = CACHE_FILES.size();

const std::string KEY_1 = "key1";
const std::string KEY_2 = "key2";
const std::string KEY_3 = "key3";
const std::string KEY_4 = "key4";
const std::string KEY_5 = "key5";
const std::string KEY_6 = "key6";
const std::vector<std::string> FILE_KEYS = { KEY_1, KEY_2, KEY_3, KEY_4, KEY_5 };

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_IMAGE_TEST_UNITTEST_IMAGE_CACHE_TEST_H
