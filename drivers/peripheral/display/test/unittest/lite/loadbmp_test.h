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

#ifndef LOAD_BMP_H
#define LOAD_BMP_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EIGHT_BITS_PER_PIXEL 8
#define INVALID_BITS 2
#define ONE_OFFSET 1
#define TWO_OFFSET 2

#define TWO_BITS_PER_PIXEL 2
#define THREE_BITS_PER_PIXEL 3
#define FOUR_BITS_PER_PIXEL 4

#define BITMAP_FILE 0x4d42

/* the color format OSD supported */
typedef enum {
    OSD_COLOR_FMT_RGB444    = 0,
    OSD_COLOR_FMT_RGB4444   = 1,
    OSD_COLOR_FMT_RGB555    = 2,
    OSD_COLOR_FMT_RGB565    = 3,
    OSD_COLOR_FMT_RGB1555   = 4,
    OSD_COLOR_FMT_RGB888    = 6,
    OSD_COLOR_FMT_RGB8888   = 7,
    OSD_COLOR_FMT_BUTT
} OsdColorFmt;

typedef struct {
    OsdColorFmt       colorFmt;      /* color format */
    uint16_t          height;        /* operation height */
    uint16_t          width;         /* operation width */
    uint16_t          stride;        /* surface stride */
    uint16_t          reserved;
} OsdSurface;

typedef struct {
    uint32_t    width;        /* out */
    uint32_t    height;       /* out */
    uint32_t    stride;       /* in */
    uint32_t    len;          /* picBuffer length */
    uint16_t    bpp;          /* bpp */
    uint8_t     *picBuffer;   /* in/out */
} OsdLogo;

typedef struct {
    uint16_t      size;
    uint32_t      width;
    uint32_t      height;
    uint16_t      planes;
    uint16_t      bitCnt;
    uint32_t      compress;
    uint32_t      sizeImage;
    uint32_t      xPelsPerMeter;
    uint32_t      yPelsPerMeter;
    uint32_t      clrUsed;
    uint32_t      clrImp;
} OsdBitMapInfoHeader;

typedef struct {
    uint32_t    size;
    uint16_t    reserved1;
    uint16_t    reserved2;
    uint32_t    offBits;
} OsdBitMapFileHeader;

typedef struct {
    uint8_t    blue;
    uint8_t    green;
    uint8_t    red;
    uint8_t    reserved;
} OsdRgbQuad;

typedef struct {
    OsdBitMapInfoHeader    header;
    OsdRgbQuad             colors[1];
} OsdBitMapInfo;

typedef struct {
    uint8_t aLen;
    uint8_t rLen;
    uint8_t gLen;
    uint8_t bLen;
} OsdCompInfo;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} OsdColor;

int32_t GetBmpInfo(const int8_t *fileName, OsdBitMapFileHeader *bmpFileHeader, OsdBitMapInfo *bmpInfo);
int32_t CreateSurfaceByBitMap(const int8_t *fileName, OsdSurface *pstSurface, uint8_t *virAddr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef LOAD_BMP_H */
