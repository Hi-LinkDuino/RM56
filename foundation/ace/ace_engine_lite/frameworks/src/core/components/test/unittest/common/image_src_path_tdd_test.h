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
#ifndef ACELITE_IMAGE_SRC_PATH_TDD_TEST_H
#define ACELITE_IMAGE_SRC_PATH_TDD_TEST_H

#ifdef TDD_ASSERTIONS
#include <climits>
#include <gtest/gtest.h>
#else
#include <typeinfo.h>
#endif

#include "acelite_config.h"
#include "test_common.h"

namespace OHOS {
namespace ACELite {
#ifdef TDD_ASSERTIONS
using namespace std;
using namespace testing::ext;
class ImageSrcPathTddTest : public testing::Test {
#else
class ImageSrcPathTddTest {
#endif
public:
    ImageSrcPathTddTest() = default;
    ~ImageSrcPathTddTest() = default;
    void PrivateDataPathSupport001();
    void PrivateDataPathSupport002();
    void PrivateDataPathSupport003();
    void PrivateDataPathSupport004();
    void PrivateDataPathSupport005();
    void PrivateDataPathSupport006();
    void RunTests();
};
}
}
#endif // ACELITE_IMAGE_SRC_PATH_TDD_TEST_H
