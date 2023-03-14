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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include "hdf_log.h"
#include "display_test.h"

using namespace testing::ext;

class HdiDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void HdiDisplayTest::SetUpTestCase()
{
    int32_t ret;
    ret = DisplayInit();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: DisplayInit fail", __func__);
        ASSERT_TRUE(0);
    }
    // sleep 1s to wait hdmi open
    sleep(1);
}

void HdiDisplayTest::TearDownTestCase()
{
    int32_t ret;

    ret = DisplayUninit();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: DisplayUninit fail", __func__);
        ASSERT_TRUE(0);
    }
}

/**
  * @tc.name: InitDisplayTest001
  * @tc.desc: open layer test
  * @tc.type: FUNC
  * @tc.require: AR000F868K
  */
HWTEST_F(HdiDisplayTest, InitDisplayTest001, TestSize.Level1)
{
    int32_t ret;

    ret = InitDisplayTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: InitDisplayTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: GetDisplayInfoTest001
  * @tc.desc: get display info test
  * @tc.type: FUNC
  * @tc.require: AR000F868O
  */

HWTEST_F(HdiDisplayTest, GetDisplayInfoTest001, TestSize.Level1)
{
    int32_t ret;

    ret = GetDisplayInfoTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: GetDisplayInfoTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: CreateLayer001
  * @tc.desc: open layer test
  * @tc.type: FUNC
  * @tc.require: AR000F868K, AR000F868O
  */
HWTEST_F(HdiDisplayTest, CreateLayerTest001, TestSize.Level1)
{
    int32_t ret;

    ret = CreateLayerTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: CreateLayerTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SetLayerVisibleTest001
  * @tc.desc: set layer visible test
  * @tc.type: FUNC
  * @tc.require: AR000F868L
  */
HWTEST_F(HdiDisplayTest, SetLayerVisibleTest001, TestSize.Level1)
{
    int32_t ret;

    ret = SetLayerVisibleTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: SetLayerVisibleTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: SetLayerDirtyRegionTest001
  * @tc.desc: set layer dirty region test
  * @tc.type: FUNC
  * @tc.require: AR000F868L
  */
HWTEST_F(HdiDisplayTest, SetLayerDirtyRegionTest001, TestSize.Level1)
{
    int32_t ret;

    ret = SetLayerDirtyRegionTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: SetLayerDirtyRegionTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: GetLayerBufferTest001
  * @tc.desc: get layer buffer to draw test.
  * @tc.type: FUNC
  * @tc.require: AR000F868N
  */
HWTEST_F(HdiDisplayTest, GetLayerBufferTest001, TestSize.Level1)
{
    int32_t ret;

    ret = GetLayerBufferTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: GetLayerBufferTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FlushCrossLineTest001
  * @tc.desc: flush cross line test
  * @tc.type: FUNC
  * @tc.require: AR000F868M
  */
HWTEST_F(HdiDisplayTest, FlushCrossLineTest001, TestSize.Level1)
{
    int32_t ret;

    ret = FlushTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FlushCrossLineTest fail", __func__);
        ASSERT_TRUE(0);
    }
    // sleep 2 to see the frame
    sleep(IMG_KEEP_TIME);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: InitGfxTest001
  * @tc.desc: init gfx test
  * @tc.type: FUNC
  * @tc.require: AR000F868N
  */
HWTEST_F(HdiDisplayTest, InitGfxTest001, TestSize.Level1)
{
    int32_t ret;
    ret = InitGfxTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: InitGfxTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: BlitTest001
  * @tc.desc: Gfx blit test
  * @tc.type: FUNC
  * @tc.require: AR000F868M
  */
HWTEST_F(HdiDisplayTest, BlitTest001, TestSize.Level1)
{
    int32_t ret;
    ret = BlitTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: BlitTest fail", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FlushPictureTest001
  * @tc.desc: flush bpm picture test
  * @tc.type: FUNC
  * @tc.require: AR000F868M
  */
HWTEST_F(HdiDisplayTest, FlushPictureTest001, TestSize.Level1)
{
    int32_t ret;

    ret = FlushTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FlushPictureTest failed", __func__);
        ASSERT_TRUE(0);
    }
    // sleep 2 to see the frame
    sleep(IMG_KEEP_TIME);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FillRectTest001
  * @tc.desc: fill rect test
  * @tc.type: FUNC
  * @tc.require: AR000F868L
  */
HWTEST_F(HdiDisplayTest, FillRectTest001, TestSize.Level1)
{
    int32_t ret;

    ret = FillRectTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FillRectTest failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FlushRectTest001
  * @tc.desc: flush rect picture test
  * @tc.type: FUNC
  * @tc.require: AR000F868N
  */
HWTEST_F(HdiDisplayTest, FlushRectTest001, TestSize.Level1)
{
    int32_t ret;

    ret = FlushTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FlushRectTest failed", __func__);
        ASSERT_TRUE(0);
    }
    // sleep 2 to see the frame
    sleep(IMG_KEEP_TIME);
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: DeinitGfxTest001
  * @tc.desc: deinit gfx test
  * @tc.type: FUNC
  * @tc.require: AR000F868K
  */
HWTEST_F(HdiDisplayTest, DeinitGfxTest001, TestSize.Level1)
{
    int32_t ret;

    ret = DeinitGfxTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: DeinitGfxTest failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: CloseLayerTest001
  * @tc.desc: close layer test
  * @tc.type: FUNC
  * @tc.require: AR000F868K
  */
HWTEST_F(HdiDisplayTest, CloseLayerTest001, TestSize.Level1)
{
    int32_t ret;

    ret = CloseLayerTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: CloseLayerTest failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: DeinitDisplayTest001
  * @tc.desc: deinit display test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868O
  */
HWTEST_F(HdiDisplayTest, DeinitDisplayTest001, TestSize.Level1)
{
    int32_t ret;

    ret = DeinitDisplayTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: DeinitDisplayTest failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: AllocMemTest001
  * @tc.desc: alloc memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868L
  */
HWTEST_F(HdiDisplayTest, AllocMemTest001, TestSize.Level1)
{
    int32_t ret;
    ret = AllocMemTest1();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: AllocMemTest1 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: MmapCacheTest001
  * @tc.desc: map cache memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868L
  */
HWTEST_F(HdiDisplayTest, MmapCacheTest001, TestSize.Level1)
{
    int32_t ret;

    ret = MmapCacheTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: MmapCacheTest01 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FlushMCacheTest001
  * @tc.desc: flush cache to memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868N
  */
HWTEST_F(HdiDisplayTest, FlushMCacheTest001, TestSize.Level1)
{
    int32_t ret;

    ret = FlushMCacheTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FlushMCacheTest01 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FreeMemTest001
  * @tc.desc: free memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868N
  */
HWTEST_F(HdiDisplayTest, FreeMemTest001, TestSize.Level1)
{
    int32_t ret;

    ret = FreeMemTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FreeMemTest01 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: AllocMemTest002
  * @tc.desc: alloc memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868N
  */
HWTEST_F(HdiDisplayTest, AllocMemTest002, TestSize.Level1)
{
    int32_t ret;

    ret = AllocMemTest2();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: AllocMemTest2 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: MmapCacheTest002
  * @tc.desc: map cache memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868L
  */
HWTEST_F(HdiDisplayTest, MmapCacheTest002, TestSize.Level1)
{
    int32_t ret;

    ret = MmapCacheTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: MmapCacheTest02 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FlushMCacheTest002
  * @tc.desc: flush cache to memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868N
  */
HWTEST_F(HdiDisplayTest, FlushMCacheTest002, TestSize.Level1)
{
    int32_t ret;

    ret = FlushMCacheTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FlushMCacheTest02 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}

/**
  * @tc.name: FreeMemTest002
  * @tc.desc: free memory test
  * @tc.type: FUNC
  * @tc.require: SR000F868J, AR000F868N
  */
HWTEST_F(HdiDisplayTest, FreeMemTest002, TestSize.Level1)
{
    int32_t ret;

    ret = FreeMemTest();
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: FreeMemTest02 failed", __func__);
        ASSERT_TRUE(0);
    }
    EXPECT_EQ(0, ret);
}
