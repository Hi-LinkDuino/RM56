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

#include "display_gfx_test.h"
#include <chrono>
#include <dlfcn.h>
#include <securec.h>
#include "display_gfx.h"
#include "display_gralloc.h"
#include "display_test.h"
#include "soft_blit.h"

#define LIB_HDI_GFX_NAME "libdisplay_gfx.z.so"
#define LIB_GFX_FUNC_INIT "GfxInitialize"
#define LIB_GFX_FUNC_DEINIT "GfxUninitialize"

namespace {
const uint32_t DEFAULT_COLOR = 0x11225588;
const uint32_t CHECK_COLOR = 0x22336699;
const uint32_t ROTATE_DEFULT_X = 200;
const uint32_t ROTATE_DEFULT_Y = 100;

static void SourceSurfaceInit(ISurface &surface, const BufferHandle &buffer)
{
    surface.width = buffer.width;
    surface.height = buffer.height;
    surface.phyAddr = buffer.phyAddr;
    surface.enColorFmt = (PixelFormat)buffer.format;
    surface.stride = buffer.stride;
    surface.bAlphaExt1555 = true;
    surface.bAlphaMax255 = true;
    surface.alpha0 = 0XFF;
    surface.alpha1 = 0XFF;
}

static int32_t GetPixelFormatBpp(PixelFormat format)
{
    const int32_t bppRgba8888 = 32;
    switch (format) {
        case PIXEL_FMT_RGBA_8888:
            return bppRgba8888;
        case PIXEL_FMT_BGRA_8888:
            return bppRgba8888;
        default:
            return -1;
    }
}

static uint32_t GetPixelValue(const BufferHandle &handle, int x, int y)
{
    const int32_t pixelBytes = 4;
    int32_t bpp = GetPixelFormatBpp((PixelFormat)handle.format);
    DISPLAY_TEST_CHK_RETURN((bpp <= 0), 0, DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN((handle.virAddr == nullptr), 0,
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN((x < 0 || x >= handle.width), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN((y < 0 || y >= handle.height), 0,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    return *pixel;
}

static uint32_t CheckPixel(const BufferHandle &handle, int x, int y, uint32_t color)
{
    const int32_t pixelBytes = 4;
    int32_t bpp = GetPixelFormatBpp((PixelFormat)handle.format);
    DISPLAY_TEST_CHK_RETURN((bpp <= 0), 0, DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN((handle.virAddr == nullptr), DISPLAY_FAILURE,
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN((x < 0 || x >= handle.width), DISPLAY_FAILURE,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN((y < 0 || y >= handle.height), DISPLAY_FAILURE,
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    if (*pixel != color) {
        DISPLAY_TEST_LOGD("the pixel color not match vAddr:%p position:%d pixel:%x color:%x", handle.virAddr, position,
            *pixel, color);
        DISPLAY_TEST_LOGD("x:%d y:%d width:%d", x, y, handle.width);
    }
    return *pixel;
}

static void SetPixel(const BufferHandle &handle, int x, int y, uint32_t color)
{
    const int32_t pixelBytes = 4;
    int32_t bpp = GetPixelFormatBpp((PixelFormat)handle.format);
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((bpp <= 0),
        DISPLAY_TEST_LOGE("CheckPixel do not support format %d", handle.format));
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((handle.virAddr == nullptr),
        DISPLAY_TEST_LOGE("CheckPixel viraddr is null must map it"));
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((x < 0 || x >= handle.width),
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter x:%d width:%d", x, handle.width));
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((y < 0 || y >= handle.height),
        DISPLAY_TEST_LOGE("CheckPixel invalid parameter y:%d height:%d", y, handle.height));

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        DISPLAY_TEST_LOGE("the pixel postion outside\n");
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    *pixel = color;
}

static void CalculRotateSize(TransformType angle, int &width, int &height)
{
    switch (angle) {
        case ROTATE_NONE:
            break;
        case ROTATE_180:
            break;
        case ROTATE_90:
        case ROTATE_270:
            width = width ^ height;
            height = width ^ height;
            width = width ^ height;
            break;
        default:
            break;
    }
}

static BufferHandle *AllocateTestMemory(const GrallocFuncs &grallocFuncs, const AllocInfo &info, uint32_t color)
{
    int32_t ret;
    BufferHandle *buffer = nullptr;
    ret = grallocFuncs.AllocMem(&info, &buffer);
    if (ret != DISPLAY_SUCCESS) {
        return nullptr;
    }

    void *vAddr = grallocFuncs.Mmap(buffer);
    if (vAddr == nullptr) {
        return nullptr;
    }

    DISPLAY_TEST_LOGD(" vAddr %p buffer->size %d", vAddr, buffer->size);
    for (int x = 0; x < buffer->width; x++) {
        for (int y = 0; y < buffer->height; y++) {
            SetPixel(*buffer, x, y, color);
        }
    }

    ret = grallocFuncs.FlushCache(buffer);
    if (ret != DISPLAY_SUCCESS) {
        return nullptr;
    }
    return buffer;
}

static BufferHandle *AllocateRotateTestMemory(const GrallocFuncs &grallocFuncs, const AllocInfo &info, int position_x,
    int position_y)
{
    BufferHandle *buffer = nullptr;
    int32_t ret = grallocFuncs.AllocMem(&info, &buffer);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_TEST_LOGE("RotateTest allocMem failed!");
        return nullptr;
    }

    void *vAddr = grallocFuncs.Mmap(buffer);
    if (vAddr == nullptr) {
        DISPLAY_TEST_LOGE("RotateTest mmap failed!");
        grallocFuncs.FreeMem(buffer);
        return nullptr;
    }

    DISPLAY_TEST_LOGD(" vAddr %p buffer->size %d", vAddr, buffer->size);
    for (int x = 0; x < buffer->width; x++) {
        for (int y = 0; y < buffer->height; y++) {
            if (position_x == x && position_y == y) {
                SetPixel(*buffer, x, y, CHECK_COLOR);
            } else {
                SetPixel(*buffer, x, y, DEFAULT_COLOR);
            }
        }
    }
    ret = grallocFuncs.FlushCache(buffer);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_TEST_LOGE("RotateTest FlushCache failed!");
        grallocFuncs.Unmap(buffer);
        grallocFuncs.FreeMem(buffer);
        return nullptr;
    }
    return buffer;
}

static void ReleaseTestMemory(const GrallocFuncs &grallocFuncs, BufferHandle &bufferHdl)
{
    grallocFuncs.Unmap((&bufferHdl));
    grallocFuncs.FreeMem(&bufferHdl);
}

void GfxTestBase::TestSetUp()
{
    int32_t ret = GrallocInitialize(&mGrallocFuncs);
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS), DISPLAY_TEST_LOGE("gralloc initialize failure");
        ASSERT_TRUE(0));
    ret = GfxTestModuleInit();
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS) || (mGfxFuncs == nullptr),
        DISPLAY_TEST_LOGE("gfx initialize failure"); ASSERT_TRUE(0));
    ret = mGfxFuncs->InitGfx();
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS), DISPLAY_TEST_LOGE("gfx intgfx failure");
        ASSERT_TRUE(0));
    InitTestBuffer();
}

void GfxTestBase::TestTearDown()
{
    int32_t ret;
    DeInitTestBuffer();
    if (mGfxFuncs == nullptr) {
        DISPLAY_TEST_LOGE("mGfxFuncs is null");
        return;
    }
    ret = mGfxFuncs->DeinitGfx();
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS), DISPLAY_TEST_LOGE("gfx intgfx failure");
        ASSERT_TRUE(0));
    ret = GrallocUninitialize(mGrallocFuncs);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_TEST_LOGE("gralloc uninitialize failure");
        ASSERT_TRUE(0);
    }
    ret = GfxTestModuleDeinit();
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_TEST_LOGE("gfx uninitialize failure");
        ASSERT_TRUE(0);
    }
}

void GfxTestBase::InitTestBuffer()
{
    const int testWidth = 1080;
    const int testHeight = 1920;
    const uint32_t initSrcRgba = 0x555555ee; // RGBA
    const uint32_t initDstRgba = 0xaaaaaa22; // RGBA

    mSrcRect = { 0, 0, testWidth, testHeight };
    mDstRect = { 0, 0, testWidth, testHeight };
    // allocate a 1080p memory.
    AllocInfo allocInfo = {
        .width = testWidth,   // 1080p width memory
        .height = testHeight, // 1080p height memory
        .usage = (uint64_t)(HBM_USE_CPU_WRITE | HBM_USE_CPU_READ | HBM_USE_MEM_DMA),
        .format = PIXEL_FMT_RGBA_8888
    };

    mSrcBuffer = AllocateTestMemory(*mGrallocFuncs, allocInfo, initSrcRgba);
    mDstBuffer = AllocateTestMemory(*mGrallocFuncs, allocInfo, initDstRgba);
    mSoftBlendBuffer = AllocateTestMemory(*mGrallocFuncs, allocInfo, initDstRgba);
    if ((mSrcBuffer == nullptr) || (mDstBuffer == nullptr) || (mSoftBlendBuffer == nullptr)) {
        DISPLAY_TEST_LOGE("init memory failed");
        ASSERT_TRUE(0);
    }
}

void GfxTestBase::DeInitTestBuffer()
{
    if (mSrcBuffer != nullptr) {
        ReleaseTestMemory(*mGrallocFuncs, *mSrcBuffer);
    }
    if (mDstBuffer != nullptr) {
        ReleaseTestMemory(*mGrallocFuncs, *mDstBuffer);
    }
    if (mSoftBlendBuffer != nullptr) {
        ReleaseTestMemory(*mGrallocFuncs, *mSoftBlendBuffer);
    }
}

int32_t GfxTestBase::GfxTestModuleInit(void)
{
    mGfxTestModule = dlopen(LIB_HDI_GFX_NAME, RTLD_NOW | RTLD_NOLOAD);
    if (mGfxTestModule != nullptr) {
        DISPLAY_TEST_LOGD("Module %s already loaded", LIB_HDI_GFX_NAME);
    } else {
        DISPLAY_TEST_LOGD("Loading module %s", LIB_HDI_GFX_NAME);
        mGfxTestModule = dlopen(LIB_HDI_GFX_NAME, RTLD_NOW);
        if (mGfxTestModule == nullptr) {
            DISPLAY_TEST_LOGE("Failed to load module: %s", dlerror());
            return DISPLAY_FAILURE;
        }
    }

    using InitFunc = int32_t (*)(GfxFuncs **funcs);
    InitFunc func = reinterpret_cast<InitFunc>(dlsym(mGfxTestModule, LIB_GFX_FUNC_INIT));
    if (func == nullptr) {
        DISPLAY_TEST_LOGE("Failed to lookup %s function: %s", LIB_GFX_FUNC_INIT, dlerror());
        dlclose(mGfxTestModule);
        return DISPLAY_FAILURE;
    }
    return func(&mGfxFuncs);
}

int32_t GfxTestBase::GfxTestModuleDeinit(void)
{
    int32_t ret = DISPLAY_SUCCESS;
    if (mGfxTestModule == nullptr) {
        using DeinitFunc = int32_t (*)(GfxFuncs *funcs);
        DeinitFunc func = reinterpret_cast<DeinitFunc>(dlsym(mGfxTestModule, LIB_GFX_FUNC_DEINIT));
        if (func == nullptr) {
            DISPLAY_TEST_LOGE("Failed to lookup %s function: %s", LIB_GFX_FUNC_DEINIT, dlerror());
        } else {
            ret = func(mGfxFuncs);
        }
        dlclose(mGfxTestModule);
    }
    return ret;
}

void GfxBlendTypeTest::SetUp()
{
    TestSetUp();
}

void GfxBlendTypeTest::TearDown()
{
    TestTearDown();
}

int32_t GfxBlendTypeTest::GfxBlitBlendTypeTest(BlendType type)
{
    int ret;
    GfxOpt opt = { 0 };
    opt.blendType = type;
    opt.enPixelAlpha = true;
    if (mGfxFuncs == nullptr) {
        DISPLAY_TEST_LOGE("mGfxFuncs is null");
        return DISPLAY_FAILURE;
    }
    ret = GfxBlitTest(*mGfxFuncs, *mGrallocFuncs, opt);
    return ret;
}

int32_t GfxBlendTypeTest::GfxBlitTest(const GfxFuncs &gfxFuncs, const GrallocFuncs &grallocFuncs, GfxOpt &opt)
{
    int32_t ret;
    ISurface dstSurface = { 0 };
    ISurface srcSurface = { 0 };

    SourceSurfaceInit(srcSurface, *mSrcBuffer);
    SourceSurfaceInit(dstSurface, *mDstBuffer);
    SoftBlit softBlit(*mSrcBuffer, mSrcRect, *mSoftBlendBuffer, mDstRect, opt.blendType);

    auto blitBegin = std::chrono::steady_clock::now();
    ret = gfxFuncs.Blit(&srcSurface, &mSrcRect, &dstSurface, &mDstRect, &opt);
    auto blitEnd = std::chrono::steady_clock::now();
    double durationBlit = std::chrono::duration<double, std::milli>(blitEnd - blitBegin).count();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("Blit failed ret:%d", ret));
    auto syncBegin = std::chrono::steady_clock::now();
    ret = grallocFuncs.InvalidateCache(mDstBuffer);
    auto syncEnd = std::chrono::steady_clock::now();
    double durationSync = std::chrono::duration<double, std::milli>(syncEnd - syncBegin).count();
    DISPLAY_TEST_LOGD("gfx blit cost %lf ms, sync cost %lf ms", durationBlit, durationSync);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret,
        DISPLAY_TEST_LOGE("gralloc buffer flush cache failed ret:%d", ret));
    if (!softBlit.RunAndCheck(*mDstBuffer)) {
        ret = DISPLAY_FAILURE;
    }
    return ret;
}

void GfxFillTest ::SetUp()
{
    TestSetUp();
}

void GfxFillTest::TearDown()
{
    TestTearDown();
}

int32_t GfxFillTest::FillRectTest(uint32_t testColor)
{
    int32_t ret;
    ISurface dstSurface = { 0 };
    GfxOpt opt = { 0 };
    if (mGfxFuncs == nullptr) {
        DISPLAY_TEST_LOGE("mGfxFuncs is null");
        return DISPLAY_FAILURE;
    }
    SourceSurfaceInit(dstSurface, *mDstBuffer);
    ret = mGfxFuncs->InitGfx();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("InitGfx failed ret:%d", ret));

    ret = mGfxFuncs->FillRect(&dstSurface, &mDstRect, testColor, &opt);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("FillRect failed ret:%d", ret));

    ret = mGfxFuncs->DeinitGfx();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("DeinitGfx failed ret:%d", ret));

    ret = mGrallocFuncs->InvalidateCache(mDstBuffer);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret,
        DISPLAY_TEST_LOGE("gralloc buffer flush cache failed ret:%d", ret));
    // check the buffer
    for (int x = 0; x < mDstBuffer->width; x++) {
        for (int y = 0; y < mDstBuffer->height; y++) {
            uint32_t pixel = CheckPixel(*mDstBuffer, x, y, testColor);
            if (pixel != testColor) {
                DISPLAY_TEST_LOGE("the color check failed pixel x: %d y:%d color: 0x%x", x, y, pixel);
                return DISPLAY_FAILURE;
            }
        }
    }
    return ret;
}

void GfxRotateTest::SetUp()
{
    int32_t ret = GrallocInitialize(&mGrallocFuncs);
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS), DISPLAY_TEST_LOGE("gralloc initialize failure");
        ASSERT_TRUE(0));
    ret = GfxTestModuleInit();
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS) || (mGfxFuncs == nullptr),
        DISPLAY_TEST_LOGE("gfx initialize failure"); ASSERT_TRUE(0));
    ret = mGfxFuncs->InitGfx();
    DISPLAY_TEST_CHK_RETURN_NOT_VALUE((ret != DISPLAY_SUCCESS), DISPLAY_TEST_LOGE("gfx intgfx failure");
        ASSERT_TRUE(0));
    const int width = 1920;
    const int height = 1080;

    mSrcRect = { 0, 0, width, height };
    AllocInfo srcAllocInfo = {
        .width = width,
        .height = height,
        .usage = (uint64_t)(HBM_USE_CPU_WRITE | HBM_USE_CPU_READ | HBM_USE_MEM_DMA),
        .format = PIXEL_FMT_BGRA_8888
    };

    mSrcBuffer = AllocateRotateTestMemory(*mGrallocFuncs, srcAllocInfo, ROTATE_DEFULT_X, ROTATE_DEFULT_Y);
}

void GfxRotateTest::TearDown()
{
    TestTearDown();
}

int32_t GfxRotateTest::RotateTest(RotateParam param)
{
    int32_t ret;
    ISurface dstSurface = { 0 };
    ISurface srcSurface = { 0 };
    GfxOpt opt = { 0 };
    opt.rotateType = param.transformType;
    opt.blendType = param.blendType;
    int width = 1920;
    int height = 1080;

    if (mGfxFuncs == nullptr) {
        DISPLAY_TEST_LOGE("mGfxFuncs is null");
        return DISPLAY_FAILURE;
    }
    CalculRotateSize(param.transformType, width, height);
    mDstRect = { 0, 0, width, height };
    AllocInfo dstAllocInfo = {
        .width = width,
        .height = height,
        .usage = HBM_USE_CPU_WRITE | HBM_USE_CPU_READ | HBM_USE_MEM_DMA,
        .format = PIXEL_FMT_BGRA_8888
    };
    mDstBuffer = AllocateRotateTestMemory(*mGrallocFuncs, dstAllocInfo, -1, -1);

    SourceSurfaceInit(srcSurface, *mSrcBuffer);
    SourceSurfaceInit(dstSurface, *mDstBuffer);
    ret = mGfxFuncs->InitGfx();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("InitGfx failed ret:%d", ret));
    ret = mGfxFuncs->Blit(&srcSurface, &mSrcRect, &dstSurface, &mDstRect, &opt);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("RotateTest failed ret:%d", ret));
    ret = mGfxFuncs->DeinitGfx();
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret, DISPLAY_TEST_LOGE("DeinitGfx failed ret:%d", ret));

    ret = mGrallocFuncs->InvalidateCache(mDstBuffer);
    DISPLAY_TEST_CHK_RETURN((ret != DISPLAY_SUCCESS), ret,
        DISPLAY_TEST_LOGE("gralloc buffer flush cache failed ret:%d", ret));
    // check the buffer
    ret = DISPLAY_FAILURE;
    for (int x = 0; x < mDstBuffer->width; x++) {
        for (int y = 0; y < mDstBuffer->height; y++) {
            uint32_t pixel = GetPixelValue(*mDstBuffer, x, y);
            if ((pixel == CHECK_COLOR) && (x == param.expectX) && (y == param.expectY)) {
                DISPLAY_TEST_LOGE("angle = %d, current position = %d,%d", param.transformType, x, y);
                ret = DISPLAY_SUCCESS;
            }
        }
    }
    return ret;
}

TEST_P(GfxBlendTypeTest, BlendType)
{
    BlendType type = GetParam();
    int ret = GfxBlitBlendTypeTest(type);
    ASSERT_TRUE(ret == DISPLAY_SUCCESS);
}
TEST_P(GfxFillTest, FillRect)
{
    uint32_t testColor = GetParam();
    int ret = FillRectTest(testColor);
    ASSERT_TRUE(ret == DISPLAY_SUCCESS);
}
TEST_P(GfxRotateTest, Blit)
{
    RotateParam rotateParams = GetParam();
    int ret = RotateTest(rotateParams);
    ASSERT_TRUE(ret == DISPLAY_SUCCESS);
}
// BLEND_AKD BLEND_AKS has no function to test it
static const BlendType BLEND_TYPES[] = {
    BLEND_NONE,
    BLEND_CLEAR,                /* < CLEAR blending    {0, 0} */
    BLEND_SRC,                  /* < SRC blending      {Sa, Sc} */
    BLEND_SRCOVER,              /* < SRC_OVER blending {Sa + (1 - Sa)*Da, Rc = Sc + (1 - Sa)*Dc} */
    BLEND_DSTOVER,              /* < DST_OVER blending {Sa + (1 - Sa)*Da, Rc = Dc + (1 - Da)*Sc} */
    BLEND_SRCIN,                /* < SRC_IN blending */
    BLEND_DSTIN,                /* < DST_IN blending */
    BLEND_SRCOUT,               /* < SRC_OUT blending */
    BLEND_DSTOUT,               /* < DST_OUT blending */
    BLEND_SRCATOP,              /* < SRC_ATOP blending */
    BLEND_DSTATOP,              /* < DST_ATOP blending */
    BLEND_ADD,                  /* < ADD blending */
    BLEND_XOR,                  /* < XOR blending */
    BLEND_DST,                  /* < DST blending */
};

static const uint32_t FILL_COLORS[] = {
    0,
    0xffffffff,
    0xaaaaaaaa,
};

static const RotateParam TEST_ROTATE_PARAMS[] = {
    {ROTATE_NONE, BLEND_SRC, 200, 100},
    {ROTATE_90, BLEND_SRC, 979, 200},
    {ROTATE_180, BLEND_SRC, 1719, 979},
    {ROTATE_270, BLEND_SRC, 100, 1719},
};

INSTANTIATE_TEST_CASE_P(GfxTest, GfxBlendTypeTest, ::testing::ValuesIn(BLEND_TYPES));
INSTANTIATE_TEST_CASE_P(GfxTest, GfxFillTest, ::testing::ValuesIn(FILL_COLORS));
INSTANTIATE_TEST_CASE_P(GfxTest, GfxRotateTest, ::testing::ValuesIn(TEST_ROTATE_PARAMS));
}
