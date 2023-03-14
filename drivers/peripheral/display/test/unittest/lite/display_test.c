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

#include "display_test.h"
#include <stdio.h>
#include "securec.h"
#include "hdf_log.h"
#include "loadbmp_test.h"

DisplayTest g_displayTest;
static BufferHandle* g_buffer;

static int32_t LoadBmp(const int8_t *fileName, BufferHandle **buffer)
{
    int32_t ret;
    uint8_t *pBuf = NULL;
    OsdSurface surface;
    OsdBitMapFileHeader bmpFileHeader = {0};
    OsdBitMapInfo bmpInfo = {0};

    if (fileName == NULL) {
        HDF_LOGE("%s: fileName is null", __func__);
        return DISPLAY_FAILURE;
    }
    if (GetBmpInfo(fileName, &bmpFileHeader, &bmpInfo) < 0) {
        HDF_LOGE("%s: GetBmpInfo err", __func__);
        return DISPLAY_FAILURE;
    }
    AllocInfo info = {
        .width = bmpInfo.header.width + 1,
        .height = bmpInfo.header.height + 1,
        .format = PIXEL_FMT_RGBA_8888,
        .usage =  HBM_USE_MEM_MMZ
    };
    // alloc picture buffer
    if (g_displayTest.grallocFuncs->AllocMem != NULL) {
        ret = g_displayTest.grallocFuncs->AllocMem(&info, buffer);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: pictureBuf alloc failure", __func__);
            return DISPLAY_FAILURE;
        }
    }
    // load bmp picture
    pBuf = (uint8_t *)(*buffer)->virAddr;
    surface.colorFmt = OSD_COLOR_FMT_RGB1555;
    CreateSurfaceByBitMap(fileName, &surface, pBuf, (*buffer)->size);
    return DISPLAY_SUCCESS;
}

static void GetLayerInfo(LayerInfo *layInfo)
{
    layInfo->width = (uint32_t)g_displayTest.displayInfo.width;
    layInfo->height = (uint32_t)g_displayTest.displayInfo.height;
    layInfo->bpp = LAYER_BPP;
    layInfo->pixFormat = PIXEL_FMT_RGBA_5551;
    layInfo->type = LAYER_TYPE_GRAPHIC;
}

static void WriteDataToBuf(int32_t width, int32_t height, uint16_t *pBuf)
{
    int32_t x;
    int32_t y;

    for (y = ((height / LINE_WIDTH) - LINE_WIDTH); y < ((height / LINE_WIDTH) + LINE_WIDTH); y++) {
        for (x = 0; x < width; x++) {
            *((uint16_t*)pBuf + y * width + x) = HIFB_RED_1555;
        }
    }
    for (y = 0; y < height; y++) {
        for (x = ((width / LINE_WIDTH) - LINE_WIDTH); x < ((width / LINE_WIDTH) + LINE_WIDTH); x++) {
            *((uint16_t*)pBuf + y * width + x) = HIFB_RED_1555;
        }
    }
}

static void PicSourceSurfaceInit(ISurface *surface, uint64_t phyAddr, int32_t bpp)
{
    surface->width = SAMPLE_IMAGE_WIDTH;
    surface->height = SAMPLE_IMAGE_HEIGHT;
    surface->phyAddr = phyAddr;
    surface->enColorFmt = PIXEL_FMT_RGBA_5551;
    surface->stride = SAMPLE_IMAGE_WIDTH * bpp / BITS_PER_BYTE;
    surface->bAlphaExt1555 = true;
    surface->bAlphaMax255  = true;
    surface->alpha0 = 0XFF;
    surface->alpha1 = 0XFF;
}

static void DestSurfaceInit(ISurface *surface, uint64_t phyAddr, int32_t bpp)
{
    surface->width = (uint32_t)g_displayTest.displayInfo.width;
    surface->height = (uint32_t)g_displayTest.displayInfo.height;
    surface->phyAddr = phyAddr;
    surface->enColorFmt = PIXEL_FMT_RGBA_5551;
    surface->stride = g_displayTest.displayInfo.width * bpp / BITS_PER_BYTE;
    surface->bAlphaExt1555 = true;
    surface->bAlphaMax255  = true;
    surface->alpha0 = 0XFF;
    surface->alpha1 = 0XFF;
    surface->bYCbCrClut = 0;
}

int32_t DisplayInit(void)
{
    int32_t ret;

    ret = LayerInitialize(&g_displayTest.layerFuncs);
    if (ret != DISPLAY_SUCCESS || g_displayTest.layerFuncs == NULL) {
        HDF_LOGE("%s: layer initialize failure", __func__);
        return DISPLAY_FAILURE;
    }
    ret = GrallocInitialize(&g_displayTest.grallocFuncs);
    if (ret != DISPLAY_SUCCESS || g_displayTest.layerFuncs == NULL) {
        HDF_LOGE("%s: gralloc initialize failure", __func__);
        return DISPLAY_FAILURE;
    }
    ret = GfxInitialize(&g_displayTest.gfxFuncs);
    if (ret != DISPLAY_SUCCESS || g_displayTest.gfxFuncs == NULL) {
        HDF_LOGE("%s: gfx initialize failure", __func__);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

int32_t DisplayUninit(void)
{
    LayerUninitialize(g_displayTest.layerFuncs);
    GrallocUninitialize(g_displayTest.grallocFuncs);
    GfxUninitialize(g_displayTest.gfxFuncs);
    return DISPLAY_SUCCESS;
}

int32_t InitDisplayTest(void)
{
    int32_t ret;

    g_displayTest.devId = 0;
    // init display
    if (g_displayTest.layerFuncs->InitDisplay != NULL) {
        ret = g_displayTest.layerFuncs->InitDisplay(g_displayTest.devId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: MPI VO can't be opened repeatedly, so here bypass ret check", __func__);
            return DISPLAY_SUCCESS;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t DeinitDisplayTest(void)
{
    int32_t ret = DISPLAY_SUCCESS;

    g_displayTest.devId = 0;
    // deinit display
    if (g_displayTest.layerFuncs->DeinitDisplay != NULL) {
        HDF_LOGE("%s: Here bypass call DeinitDisplay func for keep test environment continuable", __func__);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: deinit display failed", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t GetDisplayInfoTest(void)
{
    int32_t ret;

    g_displayTest.devId = 0;
    // get display info
    if (g_displayTest.layerFuncs->GetDisplayInfo != NULL) {
        ret = g_displayTest.layerFuncs->GetDisplayInfo(g_displayTest.devId, &g_displayTest.displayInfo);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: get disp info failed", __func__);
            return DISPLAY_FAILURE;
        }
    }
    HDF_LOGI("%s: dispInfo width = %d", __func__, g_displayTest.displayInfo.width);
    HDF_LOGI("%s: dispInfo height = %d", __func__, g_displayTest.displayInfo.height);
    HDF_LOGI("%s: dispInfo rotAngle = %d", __func__, g_displayTest.displayInfo.rotAngle);
    return DISPLAY_SUCCESS;
}

int32_t CreateLayerTest(void)
{
    int32_t ret;
    LayerInfo layInfo;

    g_displayTest.devId = 0;
    // create layer
    GetLayerInfo(&layInfo);
    if (g_displayTest.layerFuncs->CreateLayer != NULL) {
        ret = g_displayTest.layerFuncs->CreateLayer(g_displayTest.devId, &layInfo, &g_displayTest.layerId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: CreateLayer fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t CloseLayerTest(void)
{
    int32_t ret;

    if (g_displayTest.layerFuncs->CloseLayer != NULL) {
        ret = g_displayTest.layerFuncs->CloseLayer(g_displayTest.devId, g_displayTest.layerId);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: CloseLayer fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t SetLayerVisibleTest(void)
{
    int32_t ret;

    // set layer visible
    if (g_displayTest.layerFuncs->SetLayerVisible != NULL) {
        ret = g_displayTest.layerFuncs->SetLayerVisible(g_displayTest.devId, g_displayTest.layerId, true);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: SetLayerVisible fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t SetLayerDirtyRegionTest(void)
{
    int32_t ret;
    IRect rect = {0, 0, g_displayTest.displayInfo.width, g_displayTest.displayInfo.height};

    // set refresh region
    if (g_displayTest.layerFuncs->SetLayerDirtyRegion != NULL) {
        ret = g_displayTest.layerFuncs->SetLayerDirtyRegion(g_displayTest.devId, g_displayTest.layerId, &rect);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: SetLayerDirtyRegion fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t GetLayerBufferTest(void)
{
    int32_t ret;

    if (g_displayTest.layerFuncs->GetLayerBuffer != NULL) {
        ret = g_displayTest.layerFuncs->GetLayerBuffer(g_displayTest.devId, g_displayTest.layerId,
                                                       &g_displayTest.buffer);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: GetLayerBuffer fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    // write buffer data
    uint16_t *pBuf = (uint16_t *)g_displayTest.buffer.data.virAddr;
    WriteDataToBuf(g_displayTest.displayInfo.width, g_displayTest.displayInfo.height, pBuf);
    return DISPLAY_SUCCESS;
}

int32_t FlushTest(void)
{
    int32_t ret;

    // refresh layer to display
    if (g_displayTest.layerFuncs->Flush != NULL) {
        ret = g_displayTest.layerFuncs->Flush(g_displayTest.devId, g_displayTest.layerId, &g_displayTest.buffer);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: flush fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t InitGfxTest(void)
{
    int32_t ret;
    // init gfx
    if (g_displayTest.gfxFuncs->InitGfx != NULL) {
        ret = g_displayTest.gfxFuncs->InitGfx();
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: init gfx fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t DeinitGfxTest(void)
{
    int32_t ret;
    // deinit gfx
    if (g_displayTest.gfxFuncs->DeinitGfx != NULL) {
        ret = g_displayTest.gfxFuncs->DeinitGfx();
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: deinit gfx fail", __func__);
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t BlitTest(void)
{
    int32_t ret;
    ISurface srcSurface = {0};
    ISurface dstSurface = {0};
    BufferHandle* pictureBuf = NULL;
    uint32_t layerBufSize = g_displayTest.displayInfo.width * g_displayTest.displayInfo.height * PIXEL_BYTE;

    // clean the layer buffer
    (void)memset_s(g_displayTest.buffer.data.virAddr, layerBufSize, 0, layerBufSize);
    // load bmp test picture
    ret = LoadBmp((const int8_t *)PIC_RES_PATH, &pictureBuf);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: LoadBmp fail", __func__);
        return DISPLAY_FAILURE;
    }
    // use picture buffer to create source surface
    IRect srcRect = {0, 0, SAMPLE_IMAGE_WIDTH, SAMPLE_IMAGE_HEIGHT};
    PicSourceSurfaceInit(&srcSurface, pictureBuf->phyAddr, LAYER_BPP);
    // use layer buffer to create dest surface
    IRect dstRect = srcRect;
    DestSurfaceInit(&dstSurface, g_displayTest.buffer.data.phyAddr, LAYER_BPP);
    // TDE: copy bmp picture buffer to layer buffer
    if (g_displayTest.gfxFuncs->Blit != NULL) {
        ret = g_displayTest.gfxFuncs->Blit(&srcSurface, &srcRect, &dstSurface, &dstRect, NULL);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: Blit fail", __func__);
            goto EXIT;
        }
    }
    return DISPLAY_SUCCESS;

EXIT:
    /* free picture buffer */
    if (g_displayTest.grallocFuncs->FreeMem != NULL) {
        g_displayTest.grallocFuncs->FreeMem(pictureBuf);
    }
    return ret;
}

int32_t FillRectTest(void)
{
    int32_t ret;
    ISurface dstSurface = {0};
    GfxOpt opt = {0};
    IRect rect = {0, 0, SAMPLE_RECT_WIDTH, SAMPLE_RECT_HEIGHT};
    uint32_t layerBufSize = g_displayTest.displayInfo.width * g_displayTest.displayInfo.height * PIXEL_BYTE;

    opt.enGlobalAlpha = true;
    opt.globalAlpha = MAX_GLOBLE_ALPHA;
    // clean the layer buffer
    (void)memset_s(g_displayTest.buffer.data.virAddr, layerBufSize, 0, layerBufSize);
    DestSurfaceInit(&dstSurface, g_displayTest.buffer.data.phyAddr, LAYER_BPP);
    // TDE: copy bmp picture buffer to layer buffer
    if (g_displayTest.gfxFuncs->FillRect != NULL) {
        ret = g_displayTest.gfxFuncs->FillRect(&dstSurface, &rect, HIFB_RED_1555, &opt);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: FillRect fail", __func__);
            return ret;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t AllocMemTest1(void)
{
    int32_t ret = DISPLAY_FAILURE;

    AllocInfo info = {
        .width = SAMPLE_IMAGE_WIDTH,
        .height = SAMPLE_IMAGE_HEIGHT,
        .format = PIXEL_FMT_RGBA_8888,
        .usage =  HBM_USE_MEM_MMZ
    };
    if (g_displayTest.grallocFuncs->AllocMem != NULL) {
        ret = g_displayTest.grallocFuncs->AllocMem(&info, &g_buffer);
    }
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: normal memory allocMem failed", __func__);
        return ret;
    }
    return DISPLAY_SUCCESS;
}

int32_t MmapCacheTest(void)
{
    int32_t ret = DISPLAY_FAILURE;
    void *mapCacheAddr = NULL;
    AllocInfo info = {
        .width = SAMPLE_RECT_WIDTH,
        .height = SAMPLE_RECT_HEIGHT,
        .format = PIXEL_FMT_RGBA_8888,
        .usage =  HBM_USE_MEM_MMZ_CACHE
    };

    if (g_displayTest.grallocFuncs->AllocMem != NULL) {
        ret = g_displayTest.grallocFuncs->AllocMem(&info, &g_buffer);
    }
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: normal memory allocMem failed", __func__);
        return ret;
    }

    if (g_displayTest.grallocFuncs->MmapCache != NULL) {
        mapCacheAddr = g_displayTest.grallocFuncs->MmapCache(g_buffer);
        if (mapCacheAddr == NULL) {
            return DISPLAY_FAILURE;
        }
    }
    return DISPLAY_SUCCESS;
}

int32_t FreeMemTest(void)
{
    if (g_displayTest.grallocFuncs->FreeMem != NULL) {
        g_displayTest.grallocFuncs->FreeMem(g_buffer);
    }
    return DISPLAY_SUCCESS;
}

int32_t AllocMemTest2(void)
{
    int32_t ret = DISPLAY_FAILURE;
    AllocInfo info = {
        .width = SAMPLE_IMAGE_WIDTH,
        .height = SAMPLE_IMAGE_HEIGHT,
        .format = PIXEL_FMT_RGBA_8888,
        .usage =  HBM_USE_MEM_MMZ
    };

    if (g_displayTest.grallocFuncs->AllocMem != NULL) {
        ret = g_displayTest.grallocFuncs->AllocMem(&info, &g_buffer);
    }
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: cache memory allocMem failed", __func__);
        return ret;
    }

    return DISPLAY_SUCCESS;
}

int32_t FlushMCacheTest(void)
{
    int32_t ret;

    if (g_displayTest.grallocFuncs->FlushMCache != NULL) {
        ret = g_displayTest.grallocFuncs->FlushMCache(g_buffer);
        if (ret != DISPLAY_SUCCESS) {
            return ret;
        }
    }
    return DISPLAY_SUCCESS;
}
