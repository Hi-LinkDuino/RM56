/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Display
 * @{
 *
 * @brief Defines driver functions of the display module.
 *
 * This module provides driver functions for the graphics subsystem, including graphics layer management,
 * device control, graphics hardware acceleration, display memory management, and callbacks.
 *
 * @since 1.0
 * @version 2.0
 */

/**
 * @file display_type.h
 *
 * @brief Declares the data types used by the display driver functions.
 *
 * @since 1.0
 * @version 2.0
 */

#ifndef DISPLAY_TYPE_H
#define DISPLAY_TYPE_H
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "buffer_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates return values of the functions.
 *
 */
typedef enum {
    DISPLAY_SUCCESS = 0,           /**< Success */
    DISPLAY_FAILURE = -1,          /**< Failure */
    DISPLAY_FD_ERR = -2,           /**< File handle (FD) error */
    DISPLAY_PARAM_ERR = -3,        /**< Parameter error */
    DISPLAY_NULL_PTR = -4,         /**< Null pointer */
    DISPLAY_NOT_SUPPORT = -5,      /**< Unsupported feature */
    DISPLAY_NOMEM = -6,            /**< Insufficient memory */
    DISPLAY_SYS_BUSY = -7,         /**< System busy */
    DISPLAY_NOT_PERM = -8          /**< Forbidden operation */
} DispErrCode;

/**
 * @brief Enumerates layer types.
 *
 */
typedef enum {
    LAYER_TYPE_GRAPHIC,         /**< Graphic layer */
    LAYER_TYPE_OVERLAY,         /**< Overlay layer */
    LAYER_TYPE_SDIEBAND,        /**< Sideband layer */
    LAYER_TYPE_CURSOR,          /**< Cursor Layer */
    LAYER_TYPE_BUTT             /**< Empty layer */
} LayerType;

/* *
 * @brief Defines the buffer usage.
 *
 */
enum {
    HBM_USE_CPU_READ = (1 << 0),        /**< CPU will read the memory */
    HBM_USE_CPU_WRITE = (1 << 1),       /**< CPU will write the memory */
    HBM_USE_MEM_MMZ = (1 << 2),         /**< will use mmz to allocate memory */
    HBM_USE_MEM_DMA = (1 << 3),         /**< the allocator should support dma buffer */
    HBM_USE_MEM_SHARE = (1 << 4),       /**< the allocator should support shared memory buffer*/
    HBM_USE_MEM_MMZ_CACHE = (1 << 5),   /**< will use mmz to allocate memory with cache*/
    HBM_USE_MEM_FB = (1 << 6),          /**< the buffer allocate for framebuffer */
    HBM_USE_ASSIGN_SIZE = (1 << 7),     /**< assign memory size from requester */
};

/**
 * @brief Enumerates pixel formats.
 *
 */
typedef enum {
    PIXEL_FMT_CLUT8 = 0,        /**< CLUT8 format */
    PIXEL_FMT_CLUT1,            /**< CLUT1 format */
    PIXEL_FMT_CLUT4,            /**< CLUT4 format */
    PIXEL_FMT_RGB_565,          /**< RGB565 format */
    PIXEL_FMT_RGBA_5658,        /**< RGBA5658 format */
    PIXEL_FMT_RGBX_4444,        /**< RGBX4444 format */
    PIXEL_FMT_RGBA_4444,        /**< RGBA4444 format */
    PIXEL_FMT_RGB_444,          /**< RGB444 format */
    PIXEL_FMT_RGBX_5551,        /**< RGBX5551 format */
    PIXEL_FMT_RGBA_5551,        /**< RGBA5551 format */
    PIXEL_FMT_RGB_555,          /**< RGB555 format */
    PIXEL_FMT_RGBX_8888,        /**< RGBX8888 format */
    PIXEL_FMT_RGBA_8888,        /**< RGBA8888 format */
    PIXEL_FMT_RGB_888,          /**< RGB888 format */
    PIXEL_FMT_BGR_565,          /**< BGR565 format */
    PIXEL_FMT_BGRX_4444,        /**< BGRX4444 format */
    PIXEL_FMT_BGRA_4444,        /**< BGRA4444 format */
    PIXEL_FMT_BGRX_5551,        /**< BGRX5551 format */
    PIXEL_FMT_BGRA_5551,        /**< BGRA5551 format */
    PIXEL_FMT_BGRX_8888,        /**< BGRX8888 format */
    PIXEL_FMT_BGRA_8888,        /**< BGRA8888 format */
    PIXEL_FMT_YUV_422_I,        /**< YUV422 interleaved format */
    PIXEL_FMT_YCBCR_422_SP,     /**< YCBCR422 semi-planar format */
    PIXEL_FMT_YCRCB_422_SP,     /**< YCRCB422 semi-planar format */
    PIXEL_FMT_YCBCR_420_SP,     /**< YCBCR420 semi-planar format */
    PIXEL_FMT_YCRCB_420_SP,     /**< YCRCB420 semi-planar format */
    PIXEL_FMT_YCBCR_422_P,      /**< YCBCR422 planar format */
    PIXEL_FMT_YCRCB_422_P,      /**< YCRCB422 planar format */
    PIXEL_FMT_YCBCR_420_P,      /**< YCBCR420 planar format */
    PIXEL_FMT_YCRCB_420_P,      /**< YCRCB420 planar format */
    PIXEL_FMT_YUYV_422_PKG,     /**< YUYV422 packed format */
    PIXEL_FMT_UYVY_422_PKG,     /**< UYVY422 packed format */
    PIXEL_FMT_YVYU_422_PKG,     /**< YVYU422 packed format */
    PIXEL_FMT_VYUY_422_PKG,     /**< VYUY422 packed format */
    PIXEL_FMT_BUTT              /**< Invalid pixel format */
} PixelFormat;

/**
 * @brief Enumerates transform types of images.
 *
 */
typedef enum {
    ROTATE_NONE = 0,        /**< No rotation */
    ROTATE_90,              /**< Rotation by 90 degrees */
    ROTATE_180,             /**< Rotation by 180 degrees */
    ROTATE_270,             /**< Rotation by 270 degrees */
    ROTATE_BUTT             /**< Invalid operation */
} TransformType;

/**
 * @brief Enumerates image blending types.
 *
 * The system combines images based on a specified blending type during hardware acceleration.
 *
 */
typedef enum {
    BLEND_NONE = 0,         /**< No blending */
    BLEND_CLEAR,            /**< CLEAR blending */
    BLEND_SRC,              /**< SRC blending */
    BLEND_SRCOVER,          /**< SRC_OVER blending */
    BLEND_DSTOVER,          /**< DST_OVER blending */
    BLEND_SRCIN,            /**< SRC_IN blending */
    BLEND_DSTIN,            /**< DST_IN blending */
    BLEND_SRCOUT,           /**< SRC_OUT blending */
    BLEND_DSTOUT,           /**< DST_OUT blending */
    BLEND_SRCATOP,          /**< SRC_ATOP blending */
    BLEND_DSTATOP,          /**< DST_ATOP blending */
    BLEND_ADD,              /**< ADD blending */
    BLEND_XOR,              /**< XOR blending */
    BLEND_DST,              /**< DST blending */
    BLEND_AKS,              /**< AKS blending */
    BLEND_AKD,              /**< AKD blending */
    BLEND_BUTT              /**< Null operation */
} BlendType;

/**
 * @brief Enumerates ROP types supported by hardware acceleration.
 *
 * ROP performs bitwise Boolean operations (including bitwise AND and bitwise OR) on the RGB color and
 * alpha values of the foreground bitmap with those of the background bitmap, and then outputs the result.
 *
 */
typedef enum {
    ROP_BLACK = 0,          /**< Blackness */
    ROP_NOTMERGEPEN,        /**< ~(S2+S1) */
    ROP_MASKNOTPEN,         /**< ~S2&S1 */
    ROP_NOTCOPYPEN,         /**< ~S2 */
    ROP_MASKPENNOT,         /**< S2&~S1 */
    ROP_NOT,                /**< ~S1 */
    ROP_XORPEN,             /**< S2^S1 */
    ROP_NOTMASKPEN,         /**< ~(S2&S1) */
    ROP_MASKPEN,            /**< S2&S1 */
    ROP_NOTXORPEN,          /**< ~(S2^S1) */
    ROP_NOP,                /**< S1 */
    ROP_MERGENOTPEN,        /**< ~S2+S1 */
    ROP_COPYPE,             /**< S2 */
    ROP_MERGEPENNOT,        /**< S2+~S1 */
    ROP_MERGEPEN,           /**< S2+S1 */
    ROP_WHITE,              /**< Whiteness */
    ROP_BUTT                /**< Invalid ROP type */
} RopType;

/**
 * @brief Enumerates color key types supported by hardware acceleration.
 *
 */
typedef enum {
    CKEY_NONE = 0,      /**< No color key */
    CKEY_SRC,           /**< Source color key */
    CKEY_DST,           /**< Destination color key */
    CKEY_BUTT           /**< Null operation */
} ColorKey;

/**
 * @brief Enumerates mirror types supported by hardware acceleration.
 *
 */
typedef enum {
    MIRROR_NONE = 0,      /**< No mirror */
    MIRROR_LR,            /**< Left and right mirrors */
    MIRROR_TB,            /**< Top and bottom mirrors */
    MIRROR_BUTT           /**< Null operation */
} MirrorType;

/**
 * @brief Enumerates connection types of hot plugging.
 *
 */
typedef enum {
    CON_INVALID = 0,            /**< Invalid connection */
    CONNECTED,                  /**< Connected */
    DISCONNECTED                /**< Disconnected */
} Connection;

/**
 * @brief Defines display information.
 *
 */
typedef struct {
    uint32_t width;              /**< Display width */
    uint32_t height;             /**< Display height */
    int32_t rotAngle;            /**< Rotation angle of the display */
} DisplayInfo;

/**
 * @brief Defines layer information.
 *
 * <b>LayerInfo</b> must be passed to the {@link OpenLayer} function, which creates a layer based on the layer
 * information.
 *
 */
typedef struct {
    int32_t width;              /**< Layer width */
    int32_t height;             /**< Layer height */
    LayerType type;             /**< Layer type, which can be a graphic layer, overlay layer, or sideband layer */
    int32_t bpp;                /**< Number of bits occupied by each pixel */
    PixelFormat pixFormat;      /**< Pixel format of the layer */
} LayerInfo;

/**
 * @brief Defines alpha information about a layer.
 *
 */
typedef struct {
    bool enGlobalAlpha;   /**< Global alpha enable bit */
    bool enPixelAlpha;    /**< Pixel alpha enable bit */
    uint8_t alpha0;       /**< Alpha0 value, ranging from 0 to 255 */
    uint8_t alpha1;       /**< Alpha1 value, ranging from 0 to 255 */
    uint8_t gAlpha;       /**< Global alpha value, ranging from 0 to 255 */
} LayerAlpha;


/**
 * @brief Defines buffer data of a layer, including the virtual and physical memory addresses.
 *
 */
typedef struct {
    uint64_t phyAddr;     /**< Physical memory address */
    void *virAddr;        /**< Virtual memory address */
} BufferData;

/**
 * @brief Defines the buffer, which is used to store layer data.
 *
 */
typedef struct {
    int32_t fenceId;          /**< Fence ID of the buffer */
    int32_t width;            /**< Buffer width */
    int32_t height;           /**< Buffer height */
    int32_t pitch;            /**< Number of bytes from one row of pixels in memory to the next */
    PixelFormat pixFormat;    /**< Pixel format of the buffer */
    BufferData data;          /**< Layer buffer data */
    BufferHandle* hdl;        /**< Layer buffer handle */
} LayerBuffer;

/**
 * @brief Defines the information about a rectangle.
 *
 */
typedef struct {
    int32_t x;      /**< Start X coordinate of the rectangle */
    int32_t y;      /**< Start Y coordinate of the rectangle */
    int32_t w;      /**< Width of the rectangle */
    int32_t h;      /**< Height of the rectangle */
} IRect;

/**
 * @brief Stores surface information for hardware acceleration, such as draw image and bit blit.
 *
 */
typedef struct {
    uint64_t phyAddr;         /**< Start physical address of an image */
    int32_t height;           /**< Image height */
    int32_t width;            /**< Image width */
    int32_t stride;           /**< Image stride */
    PixelFormat enColorFmt;   /**< Image format */
    bool bYCbCrClut;          /**< Whether the color lookup table (CLUT) is in the YCbCr space */
    bool bAlphaMax255;        /**< Maximum alpha value of an image (255 or 128) */
    bool bAlphaExt1555;       /**< ARGB1555 alpha extension enable bit */
    uint8_t alpha0;           /**< Value of alpha0, ranging from 0 to 255 */
    uint8_t alpha1;           /**< Value of alpha1, ranging from 0 to 255 */
    uint64_t cbcrPhyAddr;     /**< CbCr physical address */
    int32_t cbcrStride;       /**< CbCr stride */
    uint64_t clutPhyAddr;     /**< Start physical address of the CLUT, used for color extension or correction */
} ISurface;

/**
 * @brief Describes a line to help draw lines in hardware acceleration.
 *
 */
typedef struct {
    int32_t x0;                 /**< X coordinate of the start point of a line */
    int32_t y0;                 /**< Y coordinate of the start point of a line */
    int32_t x1;                 /**< X coordinate of the end point of a line */
    int32_t y1;                 /**< Y coordinate of the end point of a line */
    uint32_t color;             /**< Line color */
} ILine;

/**
 * @brief Describes a circle to help draw circles in hardware acceleration.
 *
 */
typedef struct {
    int32_t x;                  /**< X coordinate of a circle center */
    int32_t y;                  /**< Y coordinate of a circle center */
    int32_t r;                  /**< Radius of a circle */
    uint32_t color;             /**< Circle color */
} ICircle;

/**
 * @brief Describes a rectangle to help draw rectangles in hardware acceleration.
 *
 */
typedef struct {
    IRect rect;                 /**< Bounds of a rectangle */
    uint32_t color;             /**< Rectangle color */
} Rectangle;

/**
 * @brief Defines hardware acceleration options.
 *
 */
typedef struct {
    bool enGlobalAlpha;         /**< Golbal alpha enable bit */
    uint32_t globalAlpha;       /**< Global alpha value */
    bool enPixelAlpha;          /**< Pixel alpha enable bit */
    BlendType blendType;        /**< Blending type */
    ColorKey colorKeyFrom;      /**< Color key mode */
    bool enableRop;             /**< Raster operations pipeline (ROP) enable bit */
    RopType colorRopType;       /**< Color ROP type */
    RopType alphaRopType;       /**< Alpha ROP type */
    bool enableScale;           /**< Scaling enable bit */
    TransformType rotateType;   /**< Rotation type */
    MirrorType mirrorType;      /**< Mirror type */
} GfxOpt;

#define PROPERTY_NAME_LEN  50

/**
 * @brief Defines property object which contains name, property id and value.
 *
 */
typedef struct {
    char name[PROPERTY_NAME_LEN]; /**< Name of the property */
    uint32_t propId;     /**< Property id which was decided in the DRM internal */
    uint64_t value;      /**< the value of property  */
} PropertyObject;

/**
 * @brief Enumerates interface types.
 *
 */
typedef enum {
    DISP_INTF_HDMI = 0,       /**< HDMI interface */
    DISP_INTF_LCD,            /**< LCD interface */
    DISP_INTF_BT1120,         /**< BT1120 interface */
    DISP_INTF_BT656,          /**< BT656 interface */
    DISP_INTF_YPBPR,          /**< YPBPR interface */
    DISP_INTF_RGB,            /**< RGB interface */
    DISP_INTF_CVBS,           /**< CVBS interface */
    DISP_INTF_SVIDEO,         /**< SVIDEO interface */
    DISP_INTF_VGA,            /**< VGA interface */
    DISP_INTF_MIPI,           /**< MIPI interface */
    DISP_INTF_PANEL,          /**< PANEL interface */
    DISP_INTF_BUTT,
} InterfaceType;

/**
 * @brief Defines the capability of the output.
 */
typedef struct {
    char name[PROPERTY_NAME_LEN];       /**< name of output */
    InterfaceType type;                 /**< interface type of output */
    uint32_t phyWidth;                  /**< Physical width */
    uint32_t phyHeight;                 /**< Physical width */
    uint32_t supportLayers;             /**< BitMask of LayerType */
    uint32_t virtualDispCount;          /**< the count of virtual displays supported*/
    bool supportWriteBack;              /**< wether support writeback*/
    uint32_t propertyCount;             /**< Count of properties */
    PropertyObject* props;              /**< Array of property objects */
} DisplayCapability;

/**
 * @brief Defines output mode info
 */
typedef struct {
    int32_t width;      /**< width in pixel */
    int32_t height;     /**< height in pixel */
    uint32_t freshRate; /**< fresh rate in one second */
    int32_t id;         /**< the id of the mode */
} DisplayModeInfo;

/**
 * @brief Defines information for allocate memory
 *
 */
typedef struct {
    uint32_t width;               /**< The width of the request allocation */
    uint32_t height;              /**< The height of the request allocation */
    uint64_t usage;               /**< The usage of the request allocation */
    PixelFormat format;           /**< The format of the request allocation */
    uint32_t expectedSize;        /**< The size assigned by memory requester */
} AllocInfo;
/**
 * @brief Enumerates power status.
 */

typedef enum {
    POWER_STATUS_ON,              /**< The power status is on */
    POWER_STATUS_STANDBY,         /**< The power status is standby */
    POWER_STATUS_SUSPEND,         /**< The power status is suspend */
    POWER_STATUS_OFF,             /**< The power status is off */
    POWER_STATUS_BUTT
} DispPowerStatus;

/**
 * @brief Enumerates composition type for special layer
 */
typedef enum {
    COMPOSITION_CLIENT,       /**< client composistion type, the composer should been cpu or gpu */
    COMPOSITION_DEVICE,       /**< device composistion type, the composer should been a hardware */
    COMPOSITION_CURSOR,       /**< cursor composistion type. it should been used for cursor */
    COMPOSITION_VIDEO,        /**< cursor composistion type. it should been used for video */
    COMPOSITION_BUTT
} CompositionType;

/**
 * @brief Enumerates the color gamuts.
 *
 */
typedef enum {
    COLOR_GAMUT_INVALID = -1,            /**< Invalid */
    COLOR_GAMUT_NATIVE = 0,              /**< Native or default */
    COLOR_GAMUT_SATNDARD_BT601 = 1,      /**< Standard BT601 */
    COLOR_GAMUT_STANDARD_BT709 = 2,      /**< Standard BT709 */
    COLOR_GAMUT_DCI_P3 = 3,              /**< DCI P3 */
    COLOR_GAMUT_SRGB = 4,                /**< SRGB */
    COLOR_GAMUT_ADOBE_RGB = 5,           /**< Adobe RGB */
    COLOR_GAMUT_DISPLAY_P3 = 6,          /**< display P3 */
    COLOR_GAMUT_BT2020 = 7,              /**< BT2020 */
    COLOR_GAMUT_BT2100_PQ = 8,           /**< BT2100 PQ */
    COLOR_GAMUT_BT2100_HLG = 9,          /**< BT2100 HLG */
    COLOR_GAMUT_DISPLAY_BT2020 = 10,     /**< Display BT2020 */
} ColorGamut;

/**
 * @brief Enumerates the color gamut maps.
 *
 */
typedef enum {
    GAMUT_MAP_CONSTANT = 0,
    GAMUT_MAP_EXPANSION = 1,
    GAMUT_MAP_HDR_CONSTANT = 2,
    GAMUT_MAP_HDR_EXPANSION = 3,
} GamutMap;

/**
 * @brief Enumerates the color data spaces.
 *
 */

typedef enum {
    COLOR_DATA_SPACE_UNKNOWN = 0,
    GAMUT_BT601 = 0x00000001,
    GAMUT_BT709 = 0x00000002,
    GAMUT_DCI_P3 = 0x00000003,
    GAMUT_SRGB = 0x00000004,
    GAMUT_ADOBE_RGB = 0x00000005,
    GAMUT_DISPLAY_P3 = 0x00000006,
    GAMUT_BT2020 = 0x00000007,
    GAMUT_BT2100_PQ = 0x00000008,
    GAMUT_BT2100_HLG = 0x00000009,
    GAMUT_DISPLAY_BT2020 = 0x0000000a,
    TRANSFORM_FUNC_UNSPECIFIED = 0x00000100,
    TRANSFORM_FUNC_LINEAR = 0x00000200,
    TRANSFORM_FUNC_SRGB = 0x00000300,
    TRANSFORM_FUNC_SMPTE_170M = 0x00000400,
    TRANSFORM_FUNC_GM2_2 = 0x00000500,
    TRANSFORM_FUNC_GM2_6 = 0x00000600,
    TRANSFORM_FUNC_GM2_8 = 0x00000700,
    TRANSFORM_FUNC_ST2084 = 0x00000800,
    TRANSFORM_FUNC_HLG = 0x00000900,
    PRECISION_UNSPECIFIED = 0x00010000,
    PRECISION_FULL = 0x00020000,
    PRESION_LIMITED = 0x00030000,
    PRESION_EXTENDED = 0x00040000,
    BT601_SMPTE170M_FULL = GAMUT_BT601 | TRANSFORM_FUNC_SMPTE_170M | PRECISION_FULL,
    BT601_SMPTE170M_LIMITED = GAMUT_BT601 | TRANSFORM_FUNC_SMPTE_170M | PRESION_LIMITED,
    BT709_LINEAR_FULL = GAMUT_BT709 | TRANSFORM_FUNC_LINEAR | PRECISION_FULL,
    BT709_LINEAR_EXTENDED = GAMUT_BT709 | TRANSFORM_FUNC_LINEAR | PRESION_EXTENDED,
    BT709_SRGB_FULL = GAMUT_BT709 | TRANSFORM_FUNC_SRGB | PRECISION_FULL,
    BT709_SRGB_EXTENDED = GAMUT_BT709 | TRANSFORM_FUNC_SRGB | PRESION_EXTENDED,
    BT709_SMPTE170M_LIMITED = GAMUT_BT709 | TRANSFORM_FUNC_SMPTE_170M | PRESION_LIMITED,
    DCI_P3_LINEAR_FULL = GAMUT_DCI_P3 | TRANSFORM_FUNC_LINEAR | PRECISION_FULL,
    DCI_P3_GAMMA26_FULL = GAMUT_DCI_P3 | TRANSFORM_FUNC_GM2_6 | PRECISION_FULL,
    DISPLAY_P3_LINEAR_FULL = GAMUT_DISPLAY_P3 | TRANSFORM_FUNC_LINEAR | PRECISION_FULL,
    DCI_P3_SRGB_FULL = GAMUT_DCI_P3 | TRANSFORM_FUNC_SRGB | PRECISION_FULL,
    ADOBE_RGB_GAMMA22_FULL = GAMUT_ADOBE_RGB | TRANSFORM_FUNC_GM2_2 | PRECISION_FULL,
    BT2020_LINEAR_FULL = GAMUT_BT2020 | TRANSFORM_FUNC_LINEAR | PRECISION_FULL,
    BT2020_SRGB_FULL = GAMUT_BT2020 | TRANSFORM_FUNC_SRGB | PRECISION_FULL,
    BT2020_SMPTE170M_FULL = GAMUT_BT2020 | TRANSFORM_FUNC_SMPTE_170M | PRECISION_FULL,
    BT2020_ST2084_FULL = GAMUT_BT2020 | TRANSFORM_FUNC_ST2084 | PRECISION_FULL,
    BT2020_HLG_FULL = GAMUT_BT2020 | TRANSFORM_FUNC_HLG | PRECISION_FULL,
    BT2020_ST2084_LIMITED = GAMUT_BT2020 | TRANSFORM_FUNC_ST2084 | PRESION_LIMITED,
} ColorDataSpace;

/**
 * @brief Enumerates the HDR formats.
 *
 */
typedef enum {
    NOT_SUPPORT_HDR = 0,
    DOLBY_VISION = 1,
    HDR10 = 2,
    HLG = 3,
    HDR10_PLUS = 4,
    HDR_VIVID = 5,
} HDRFormat;

/**
 * @brief Defines the HDR capability.
 *
 */
typedef struct {
    uint32_t formatCount;
    HDRFormat* formats;
    float maxLum;
    float maxAverageLum;
    float minLum;
} HDRCapability;

/**
 * @brief Enumerates the HDR metadata keys.
 *
 */
typedef enum {
    MATAKEY_RED_PRIMARY_X = 0,
    MATAKEY_RED_PRIMARY_Y = 1,
    MATAKEY_GREEN_PRIMARY_X = 2,
    MATAKEY_GREEN_PRIMARY_Y = 3,
    MATAKEY_BLUE_PRIMARY_X = 4,
    MATAKEY_BLUE_PRIMARY_Y = 5,
    MATAKEY_WHITE_PRIMARY_X = 6,
    MATAKEY_WHITE_PRIMARY_Y = 7,
    MATAKEY_MAX_LUMINANCE = 8,
    MATAKEY_MIN_LUMINANCE = 9,
    MATAKEY_MAX_CONTENT_LIGHT_LEVEL = 10,
    MATAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL = 11,
    MATAKEY_HDR10_PLUS = 12,
    MATAKEY_HDR_VIVID = 13,
} HDRMetadataKey;

/**
 * @brief Defines the HDR metadata.
 *
 */
typedef struct {
    HDRMetadataKey key;
    float value;
} HDRMetaData;

#ifdef __cplusplus
}
#endif
#endif
/* @} */
