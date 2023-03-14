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

#include "loadbmp_test.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include "securec.h"
#include "hdf_log.h"
#include "display_type.h"

#define EOK 0

OsdCompInfo g_osdCompInfo[OSD_COLOR_FMT_BUTT] = {
    {0, 4, 4, 4},   /* RGB444 */
    {4, 4, 4, 4},   /* ARGB4444 */
    {0, 5, 5, 5},   /* RGB555 */
    {0, 5, 6, 5},   /* RGB565 */
    {1, 5, 5, 5},   /* ARGB1555 */
    {0, 0, 0, 0},   /* RESERVED */
    {0, 8, 8, 8},   /* RGB888 */
    {8, 8, 8, 8}    /* ARGB8888 */
};

uint16_t OsdMakeColorU16(uint8_t r, uint8_t g, uint8_t b, OsdCompInfo compinfo)
{
    uint8_t r1;
    uint8_t g1;
    uint8_t b1;
    uint16_t pixel = 0;
    uint32_t tmp = 15; // 16bit color

    r1 = g1 = b1 = 0;
    r1 = r >> (EIGHT_BITS_PER_PIXEL - compinfo.rLen);
    g1 = g >> (EIGHT_BITS_PER_PIXEL - compinfo.gLen);
    b1 = b >> (EIGHT_BITS_PER_PIXEL - compinfo.bLen);
    while (compinfo.aLen) {
        pixel |= (1 << tmp);
        tmp--;
        compinfo.aLen--;
    }

    pixel |= (r1 | (g1 << compinfo.bLen) | (b1 << (compinfo.bLen + compinfo.gLen)));
    return pixel;
}

int32_t GetBmpInfo(const int8_t *fileName, OsdBitMapFileHeader *bmpFileHeader, OsdBitMapInfo *bmpInfo)
{
    FILE *file = NULL;
    uint16_t bfType = 0;
    char realPath[PATH_MAX] = {0};

    if (realpath((char*)fileName, realPath) == NULL) {
        printf("%s: file %s does not exist\n", __func__, fileName);
        return DISPLAY_FAILURE;
    }
    if ((file = fopen((const char*)realPath, "rb")) == NULL) {
        HDF_LOGE("%s: Open file failure: %s", __func__, fileName);
        return DISPLAY_FAILURE;
    }

    (void)fread(&bfType, 1, sizeof(bfType), file);
    if (bfType != BITMAP_FILE) {
        HDF_LOGE("%s: not bitmap file", __func__);
        fclose(file);
        return DISPLAY_FAILURE;
    }

    (void)fread(bmpFileHeader, 1, sizeof(OsdBitMapFileHeader), file);
    (void)fread(bmpInfo, 1, sizeof(OsdBitMapInfo), file);
    fclose(file);

    return DISPLAY_SUCCESS;
}

static int32_t CheckBmpInfo(const OsdBitMapInfo *bmpInfo)
{
    uint16_t bpp;

    bpp = bmpInfo->header.bitCnt / EIGHT_BITS_PER_PIXEL;
    if (bpp < INVALID_BITS) {
        /* only support 1555.8888  888 bitmap */
        HDF_LOGE("%s: bitmap format not supported", __func__);
        return DISPLAY_FAILURE;
    }

    if (bmpInfo->header.compress != 0) {
        HDF_LOGE("%s: not support compressed bitmap file", __func__);
        return DISPLAY_FAILURE;
    }
    if (bmpInfo->header.height == 0) {
        HDF_LOGE("%s: bmpInfo.header.height is 0", __func__);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

static int32_t LoadPicToBuffer(const int8_t *fileName, OsdLogo *videoLogo, OsdColorFmt enFmt,
                               uint8_t **outBuf, uint32_t *stride)
{
    FILE *file = NULL;
    OsdBitMapFileHeader bmpFileHeader;
    OsdBitMapInfo       bmpInfo;
    uint32_t h;
    uint64_t byteNum;
    char realPath[PATH_MAX] = {0};

    if (GetBmpInfo(fileName, &bmpFileHeader, &bmpInfo) < 0) {
        return DISPLAY_FAILURE;
    }
    if (CheckBmpInfo(&bmpInfo) != DISPLAY_SUCCESS) {
        /* only support 1555.8888  888 bitmap */
        HDF_LOGE("%s: bitmap format not supported", __func__);
        return DISPLAY_FAILURE;
    }
    videoLogo->bpp = bmpInfo.header.bitCnt / EIGHT_BITS_PER_PIXEL;
    if (realpath((char*)fileName, realPath) == NULL) {
        printf("%s: file %s does not exist\n", __func__, fileName);
        return DISPLAY_FAILURE;
    }
    if ((file = fopen((const char*)realPath, "rb")) == NULL) {
        HDF_LOGE("%s: Open file failure: %s", __func__, fileName);
        return DISPLAY_FAILURE;
    }
    videoLogo->width = bmpInfo.header.width;
    videoLogo->height = ((bmpInfo.header.height > 0) ? bmpInfo.header.height :
                        (-bmpInfo.header.height));
    *stride = videoLogo->width * videoLogo->bpp;
    h = videoLogo->height;
    if ((*stride % FOUR_BITS_PER_PIXEL) != 0) {
        *stride = (*stride & 0xfffc) + FOUR_BITS_PER_PIXEL;
    }
    /* RGB8888 or RGB1555 */
    *outBuf = (uint8_t*)malloc(videoLogo->height * (*stride));
    if (*outBuf == NULL) {
        HDF_LOGE("%s: not enough memory to malloc", __func__);
        fclose(file);
        return DISPLAY_FAILURE;
    }
    fseek(file, bmpFileHeader.offBits, 0);
    byteNum = h * (*stride);
    if (byteNum > UINT32_MAX) {
        HDF_LOGE("%s: buffer size is beyond param's limit", __func__);
        fclose(file);
        free(*outBuf);
        *outBuf = NULL;
        return DISPLAY_FAILURE;
    }
    if (fread((*outBuf), 1, byteNum, file) != byteNum) {
        HDF_LOGE("%s: fread %u*%u error", __func__, h, *stride);
        fclose(file);
        free(*outBuf);
        *outBuf = NULL;
        return DISPLAY_FAILURE;
    }
    if (enFmt >= OSD_COLOR_FMT_RGB888) {
        videoLogo->stride = videoLogo->width * FOUR_BITS_PER_PIXEL;
    } else {
        videoLogo->stride = videoLogo->width * TWO_BITS_PER_PIXEL;
    }
    fclose(file);
    return DISPLAY_SUCCESS;
}

static void LoadRgbData(OsdLogo *videoLogo, OsdColorFmt enFmt, uint32_t stride, uint8_t *origBuf)
{
    uint16_t i;
    uint16_t j;
    uint8_t *start = NULL;
    uint16_t *dst = NULL;
    uint32_t h;
    OsdColor c;

    h = videoLogo->height;
    for (i = 0; i < videoLogo->height; i++) {
        for (j = 0; j < videoLogo->width; j++) {
            /* start color convert */
            start = origBuf + ((h - 1) - i) * stride + j * videoLogo->bpp;
            dst = (uint16_t*)(videoLogo->picBuffer + i * videoLogo->stride + j * TWO_OFFSET);
            c.r = *(start);
            c.g = *(start + ONE_OFFSET);
            c.b = *(start + TWO_OFFSET);
            *dst = OsdMakeColorU16(c.r, c.g, c.b, g_osdCompInfo[enFmt]);
        }
    }
}

static int32_t LoadData(OsdLogo *videoLogo, OsdColorFmt enFmt, uint32_t stride, uint8_t *origBuf)
{
    int32_t ret;
    uint16_t i;
    uint16_t j;
    uint32_t h;

    (void)enFmt;
    h = videoLogo->height;
    for (i = 0; i < videoLogo->height; i++) {
        for (j = 0; j < videoLogo->width; j++) {
            ret = memcpy_s((videoLogo->picBuffer + i * videoLogo->stride + j * FOUR_BITS_PER_PIXEL), videoLogo->len,
                (origBuf + ((h - 1) - i) * stride + j * videoLogo->bpp), videoLogo->bpp);
            if (ret != EOK) {
                HDF_LOGE("%s: file: %s, line: %d, memcpy_s failure", __func__, __FILE__, __LINE__);
                return DISPLAY_FAILURE;
            }
            *(videoLogo->picBuffer + i * videoLogo->stride + j * FOUR_BITS_PER_PIXEL + THREE_BITS_PER_PIXEL) = 0xff;
        }
    }
    return DISPLAY_SUCCESS;
}

static int32_t Copy3BitsByFmt(OsdLogo *videoLogo, OsdColorFmt enFmt, uint32_t stride, uint8_t *origBuf)
{
    int32_t ret = DISPLAY_SUCCESS;

    switch (enFmt) {
        case OSD_COLOR_FMT_RGB444:
        case OSD_COLOR_FMT_RGB555:
        case OSD_COLOR_FMT_RGB565:
        case OSD_COLOR_FMT_RGB1555:
        case OSD_COLOR_FMT_RGB4444:
            /* start color convert */
            LoadRgbData(videoLogo, enFmt, stride, origBuf);
            break;
        case OSD_COLOR_FMT_RGB888:
        case OSD_COLOR_FMT_RGB8888:
            ret = LoadData(videoLogo, enFmt, stride, origBuf);
            break;
        default:
            HDF_LOGE("%s: file: %s, line: %d, no such format", __func__, __FILE__, __LINE__);
            return DISPLAY_FAILURE;
    }
    return ret;
}

static int32_t Copy2BitsAnd4Bits(OsdLogo *videoLogo, uint32_t stride, uint8_t *origBuf)
{
    int32_t ret;
    uint16_t i;
    uint16_t j;
    uint8_t *pRGBBuf;

    pRGBBuf = videoLogo->picBuffer;
    for (i = 0; i < videoLogo->height; i++) {
        for (j = 0; j < videoLogo->width; j++) {
            ret = memcpy_s((pRGBBuf + i * videoLogo->stride + j * videoLogo->bpp), videoLogo->len,
                (origBuf + ((videoLogo->height - 1) - i)*stride + j * videoLogo->bpp), videoLogo->bpp);
            if (ret != EOK) {
                HDF_LOGE("%s: file: %s, line: %d, memcpy_s failure", __func__, __FILE__, __LINE__);
                return DISPLAY_FAILURE;
            }
        }
    }
    return DISPLAY_SUCCESS;
}

static int32_t LoadBMPEx(const int8_t *fileName, OsdLogo *videoLogo, OsdColorFmt enFmt)
{
    int32_t ret;
    uint32_t stride;
    uint8_t *origBmpBuf = NULL;

    ret = LoadPicToBuffer(fileName, videoLogo, enFmt, &origBmpBuf, &stride);
    if (ret != DISPLAY_SUCCESS) {
        HDF_LOGE("%s: LoadPicToBuffer failure", __func__);
        return DISPLAY_FAILURE;
    }
    if (videoLogo->bpp == THREE_BITS_PER_PIXEL) {
        ret = Copy3BitsByFmt(videoLogo, enFmt, stride, origBmpBuf);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: Copy3BitsByFmt failure", __func__);
        }
    } else if ((videoLogo->bpp == TWO_BITS_PER_PIXEL) || (videoLogo->bpp == FOUR_BITS_PER_PIXEL)) {
        ret = Copy2BitsAnd4Bits(videoLogo, stride, origBmpBuf);
        if (ret != DISPLAY_SUCCESS) {
            HDF_LOGE("%s: Copy2BitsAnd4Bits failure", __func__);
            free(origBmpBuf);
            return ret;
        }
    }
    free(origBmpBuf);
    return DISPLAY_SUCCESS;
}

static int32_t LoadImageEx(const int8_t *fileName, OsdLogo *videoLogo, OsdColorFmt enFmt)
{
    char *ext = strrchr((const char *)fileName, '.');

    if (ext == NULL) {
        HDF_LOGE("%s: LoadImageEx error", __func__);
        return DISPLAY_FAILURE;
    }

    ext = ext + 1;
    if (strcmp(ext, "bmp") == 0) {
        if (LoadBMPEx(fileName, videoLogo, enFmt) != 0) {
            HDF_LOGE("%s: LoadBMPEx error", __func__);
            return DISPLAY_FAILURE;
        }
    } else {
        HDF_LOGE("%s: not supported image file", __func__);
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

int32_t CreateSurfaceByBitMap(const int8_t *fileName, OsdSurface *pstSurface, uint8_t *virAddr, uint32_t len)
{
    OsdLogo stLogo;

    if (fileName == NULL) {
        HDF_LOGE("%s: fileName is null", __func__);
        return DISPLAY_FAILURE;
    }
    if (pstSurface == NULL) {
        HDF_LOGE("%s: pstSurface is null", __func__);
        return DISPLAY_FAILURE;
    }
    if (virAddr == NULL) {
        HDF_LOGE("%s: virAddr is null", __func__);
        return DISPLAY_FAILURE;
    }
    (void)memset_s(&stLogo, sizeof(OsdLogo), 0, sizeof(OsdLogo));
    stLogo.picBuffer = virAddr;
    stLogo.len = len;
    if (LoadImageEx(fileName, &stLogo, pstSurface->colorFmt) < 0) {
        HDF_LOGE("%s: load bmp error", __func__);
        return DISPLAY_FAILURE;
    }
    pstSurface->height = stLogo.height;
    pstSurface->width = stLogo.width;
    pstSurface->stride = stLogo.stride;
    return DISPLAY_SUCCESS;
}
