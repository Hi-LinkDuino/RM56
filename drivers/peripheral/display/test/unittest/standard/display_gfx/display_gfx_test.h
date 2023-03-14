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

#ifndef DISPLAY_GFX_TEST_H
#define DISPLAY_GFX_TEST_H
#include "gtest/gtest.h"
#include "display_gfx.h"
#include "display_gralloc.h"
#include "display_type.h"
#include "display_test.h"

namespace {
class GfxTestBase {
public:
    virtual ~GfxTestBase() {}

protected:
    void TestSetUp();
    void TestTearDown();
    void DeInitTestBuffer();
    void InitTestBuffer();
    int32_t GfxTestModuleInit(void);
    int32_t GfxTestModuleDeinit(void);
    IRect mSrcRect = { 0 };
    IRect mDstRect = { 0 };
    BufferHandle *mSrcBuffer = nullptr;
    BufferHandle *mDstBuffer = nullptr;
    BufferHandle *mSoftBlendBuffer = nullptr;
    GrallocFuncs *mGrallocFuncs = nullptr;
    GfxFuncs *mGfxFuncs = nullptr;
    void *mGfxTestModule = nullptr;
};

struct RotateParam {
    TransformType transformType;
    BlendType blendType;
    int expectX;
    int expectY;
};

class GfxFillTest : public ::testing::TestWithParam<uint32_t>, public GfxTestBase {
protected:
    void SetUp();
    void TearDown();
    int32_t FillRectTest(uint32_t testColor);
};

class GfxRotateTest : public ::testing::TestWithParam<RotateParam>, public GfxTestBase {
protected:
    void SetUp();
    void TearDown();
    int32_t RotateTest(RotateParam param);
};

class GfxBlendTypeTest : public ::testing::TestWithParam<BlendType>, public GfxTestBase {
protected:
    void SetUp();
    void TearDown();
    int32_t GfxBlitBlendTypeTest(BlendType type);
    int32_t GfxBlitTest(const GfxFuncs &gfxFuncs, const GrallocFuncs &grallocFuncs, GfxOpt &opt);
};
}

#endif // DISPLAY_GFX_TEST_H
