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

#ifndef DISPLAY_TEST_H
#define DISPLAY_TEST_H
#include "display_gfx.h"
#include "display_gralloc.h"
#include "display_layer.h"
#include "display_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* layer width */
#define LAYER_WIDTH            480
/* layer height */
#define LAYER_HEIGHT           960
#define PIXEL_BYTE             2
#define MAX_GLOBLE_ALPHA       255
#define IMG_KEEP_TIME          2

#define SAMPLE_IMAGE_WIDTH     300
#define SAMPLE_IMAGE_HEIGHT    150
#define SAMPLE_RECT_WIDTH      600
#define SAMPLE_RECT_HEIGHT     300
#define HIFB_RED_1555          0xFC00
#define COLOR_RGB_BLUE         0x0000FF
#define MEM_POOL_COUNT         64
#define BUFFER_BLOCK_COUNT     20
#define LAYER_BPP              16
#define VO_FRAME_RATE          50
#define LINE_WIDTH             2
#define BITS_PER_BYTE          8
/* porch */
#define PORCH_HBP              30
#define PORCH_HFP              10
#define PORCH_HS               10
#define PORCH_VBP              10
#define PORCH_VFP              4
#define PORCH_VS               2

/* device division */
#define DEVICE_DIV             3
/* premult division */
#define PRE_DIV                1
/* LCD division coefficient */
#define LCD_DIV_COE            0x182ed6

#ifdef __LITEOS__
#define PIC_RES_PATH           "/user/data/gfx_test.bmp"
#else
#define PIC_RES_PATH           "/storage/data/gfx_test.bmp"
#endif

typedef struct {
    LayerFuncs *layerFuncs;
    GrallocFuncs *grallocFuncs;
    GfxFuncs *gfxFuncs;
    DisplayInfo displayInfo;
    uint32_t devId;
    uint32_t layerId;
    LayerBuffer buffer;
} DisplayTest;

int32_t DisplayInit(void);
int32_t DisplayUninit(void);
int32_t InitDisplayTest(void);
int32_t DeinitDisplayTest(void);
int32_t GetDisplayInfoTest(void);
int32_t CreateLayerTest(void);
int32_t CloseLayerTest(void);
int32_t SetLayerVisibleTest(void);
int32_t SetLayerDirtyRegionTest(void);
int32_t GetLayerBufferTest(void);
int32_t FlushTest(void);
int32_t InitGfxTest(void);
int32_t DeinitGfxTest(void);
int32_t BlitTest(void);
int32_t FillRectTest(void);
int32_t AllocMemTest1(void);
int32_t MmapCacheTest(void);
int32_t FlushMCacheTest(void);
int32_t FreeMemTest(void);
int32_t AllocMemTest2(void);

#ifdef __cplusplus
}
#endif
#endif // DISPLAY_TEST_H
