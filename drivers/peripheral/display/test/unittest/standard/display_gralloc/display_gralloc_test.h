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

#ifndef DISPLAY_GRALLOC_TEST_H
#define DISPLAY_GRALLOC_TEST_H
#include "gtest/gtest.h"
#include "display_gralloc.h"

namespace {
struct AllocTestPrms {
    AllocInfo allocInfo;
    int32_t expectStride;
    int32_t expectSize;
};

class GrallocAllocTest : public ::testing::TestWithParam<AllocTestPrms> {
protected:
    virtual void SetUp();
    virtual void TearDown();
    int32_t AllocMemTest(AllocTestPrms &info);
    GrallocFuncs *mGrallocFuncs = nullptr;
};
}
#endif // DISPLAY_GRALLOC_TEST_H