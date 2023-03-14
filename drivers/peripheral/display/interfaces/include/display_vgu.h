/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* *
 * @addtogroup Display
 * @{
 *
 * @brief Defines driver functions of the display module.
 *
 * This module provides driver functions for the graphics subsystem, including graphics layer management,
 * device control, graphics hardware acceleration, display memory management, and callbacks.
 *
 * @since 3.0
 */

/* *
 * @file display_vgu.h
 *
 * @brief Declares the driver functions for implementing 2D vector hardware acceleration.
 *
 * @since 3.0
 */

#ifndef DISPLAY_VGU_H
#define DISPLAY_VGU_H
#include "display_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef HDI_VGU_SCALAR_IS_FLOAT
#define HDI_VGU_SCALAR_IS_FLOAT 1

#ifdef HDI_VGU_SCALAR_IS_FLOAT
typedef float VGUScalar;
#else
typedef int32_t VGUScalar;
#endif

typedef PixelFormat VGUPixelFormat;     /* < Pixel formats */
typedef BlendType VGUBlendType;         /* < Blend types supported by hardware acceleration */

/* *
 * @brief Enumerates data types of paths.
 *
 */
typedef enum {
    VGU_DATA_TYPE_S16 = 0,  /* < Integer (2 bytes) */
    VGU_DATA_TYPE_S32,      /* < Integer (4 bytes) */
    VGU_DATA_TYPE_F32       /* < Floating point number (4 bytes) */
} VGUPathDataType;

/* *
 * @brief Enumerates supported hardware acceleration capabilities.
 *
 */
typedef enum {
    VGU_CAP_BLIT = (1 << 0),        /* < Bit blit */
    VGU_CAP_BLIT_NUM = (1 << 1),    /* < Maximum number of images that can be combined during bit blit */
    VGU_CAP_PATH = (1 << 2),        /* < Path filling and stroking */
    VGU_CAP_FILTER_BLUR = (1 << 3), /* < Blur filter */
} VGUCapability;

/* *
 * @brief Enumerates result codes that may return.
 *
 */
typedef enum {
    VGU_SUCCESS = 0,            /* < The operation is successful. */
    VGU_NO_SUPPORT = -1,        /* < This feature is not supported. */
    VGU_OPERATION_FAILED = -2,   /* < The operation failed. */
    VGU_OUT_OF_MEMORY = -3,     /* < The operation ran out of memory. */
    VGU_TIMEOUT = -4,           /* < The operation times out. */
    VGU_INVALID_PARAMETER = -5, /* < One or more parameters are invalid. */
    VGU_BUSY = -6,              /* < The device is busy. */
    VGU_NO_CONTEXT = -7,        /* < There is no context specified. */
} VGUResult;

/* *
 * @brief Enumerates styles for the endpoints of a stroked line.
 *
 */
typedef enum {
    VGU_LINECAP_BUTT = 0,   /* < A line with a squared-off end (default value) */
    VGU_LINECAP_ROUND,      /* < A line with a rounded end */
    VGU_LINECAP_SQUARE      /* < A line with a squared-off end */
} VGULineCap;

/* *
 * @brief Enumerates join types for stroked lines.
 *
 */
typedef enum {
    VGU_LINE_JOIN_MITER = 0,    /* < A join with a sharp corner (default value) */
    VGU_LINE_JOIN_ROUND,        /* < A join with a rounded end */
    VGU_LINE_JOIN_BEVEL,        /* < A join with a squared-off end */
    VGU_LINE_JOIN_BUTT          /* < Invalid definition */
} VGUJointType;

/* *
 * @brief Defines the coordinates of a point.
 *
 */
typedef struct {
    VGUScalar x;    /* < Horizontal coordinate of the point */
    VGUScalar y;    /* < Vertical coordinate of the point */
} VGUPoint;

/* *
 * @brief Defines a rectangle.
 *
 */
typedef struct {
    VGUScalar x; /* < Horizontal coordinate of the start point of the rectangle */
    VGUScalar y; /* < Vertical coordinate of the start point of the rectangle */
    VGUScalar w; /* < Width of the rectangle */
    VGUScalar h; /* < Height of the rectangle */
} VGURect;

/* *
 * @brief Enumerates filter types for rendering an image.
 *
 */
typedef enum {
    VGU_FILTER_BILINEAR = 0,    /* < Bilinear interpolation filter (default value) */
    VGU_FILTER_NEAREST,         /* < No interpolation filter */
    VGU_FILTER_LINEAR,          /* < Linear interpolation filter */
    VGU_FILTER_BUTT             /* < Invalid definition */
} VGUFilter;

/* *
 * @brief Enumerates fill rules for graphics.
 *
 */
typedef enum {
    VGU_RULE_WINDING = 0,   /* < Non-zero winding rule (default value) */
    VGU_RULE_EVEN_ODD,      /* < Even-odd rule */
    VGU_RULE_BUTT           /* < Invalid definition */
} VGUFillRule;

/* *
 * @brief Enumerates fill types of the outside of the gradient area.
 *
 */
typedef enum {
    VGU_SPREAD_PAD = 0,     /* < The area is filled with the closest gradient stop color. (Default value) */
    VGU_SPREAD_REFLECT,     /* < The gradient is reflected outside the area. */
    VGU_SPREAD_REPEAT,      /* < The gradient is repeated outside the area. */
    VGU_SPREAD_BUTT         /* < Invalid definition */
} VGUFillSpread;

/* *
 * @brief Enumerates wrap types of a pattern.
 *
 */
typedef enum {
    VGU_WRAP_REFLECT = 0,   /* < The pattern is reflected. */
    VGU_WRAP_REPEAT,        /* < The pattern is repeated. */
    VGU_WRAP_BUTT           /* < Invalid definition */
} VGUWrapType;

/* *
 * @brief Enumerates commands for drawing a path.
 *
 */
typedef enum {
    VGU_PATH_CMD_CLOSE = 0,     /* < Close the current subpath (coordinates: none). */
    VGU_PATH_CMD_MOVE,          /* < Move to the specified point (coordinates: x0, y0). */
    VGU_PATH_CMD_LINE,          /* < Draw a line (coordinates: x0, y0). */
    VGU_PATH_CMD_HLINE,         /* < Draw a horizontal line (coordinates: x0). */
    VGU_PATH_CMD_VLINE,         /* < Draw a vertical line (coordinates: y0). */
    VGU_PATH_CMD_QUAD,          /* < Draw a quadratic Bezier curve (coordinates: x0, y0, x1, y1). */
    VGU_PATH_CMD_CUBIC,         /* < Draw a cubic Bezier curve (coordinates: x0, y0, x1, y1, x2, y2). */
    VGU_PATH_CMD_SQUAD,         /* < Draw a smooth quadratic Bezier curve (coordinates: x1, y1). */
    VGU_PATH_CMD_SCUBIC,        /* < Draw a smooth cubic Bezier curve (coordinates: x1, y1, x2, y2). */
    VGU_PATH_CMD_BUTT,          /* < Invalid definition */
} VGUPathCmd;

/* *
 * @brief Defines a path object, which stores path-related commands and coordinates.
 *
 */
typedef struct {
    uint8_t *segment;       /* < Pointer to the path command data */
    int32_t numSegments;    /* < Total number of path commands */
    uint8_t *data;          /* < Pointer to the coordinates used in the path commands */
    VGUPathDataType type;   /* < Data type of the path */
    bool enAlias;           /* < Whether to enable anti-aliasing */
    VGURect boundBox;       /* < Bounding box of the path */
} VGUPath;

/* *
 * @brief Enumerates transform types.
 *
 */
typedef enum {
    VGU_TRANSFORM_TRANSLATE = (1 << 0),     /* < Translate */
    VGU_TRANSFORM_SCALE = (1 << 1),         /* < Scale */
    VGU_TRANSFORM_ROTATE_90 = (1 << 2),     /* < Rotate by 90 degrees */
    VGU_TRANSFORM_ROTATE_180 = (1 << 3),    /* < Rotate by 180 degrees */
    VGU_TRANSFORM_ROTATE_270 = (1 << 4),    /* < Rotate by 270 degrees */
    VGU_TRANSFORM_OTHER = (1 << 16)         /* < Other transform type */
} VGUTransformType;

/* *
 * @brief Defines a transformation matrix.
 *
 */
typedef struct {
    float m[3][3];    /* < 3x3 transformation matrix */
    uint32_t type;    /* < Transform type, which can be scale, translate, or rotate by 90 x <i>N</i> degrees */
} VGUMatrix3;

/* *
 * @brief Stores bitmap information for hardware acceleration.
 *
 */
typedef struct {
    VGUPixelFormat pixelFormat;  /* < Pixel format */
    uint32_t width;             /* < Bitmap width */
    uint32_t height;            /* < Bitmap height */
    uint32_t stride;            /* < Bitmap stride */
    void *virAddr;              /* < Virtual address of the requested memory */
    uint64_t phyAddr;           /* < Physical memory address */
} VGUBuffer;

/* *
 * @brief Enumerates clip types of a surface.
 *
 */
typedef enum {
    VGU_CLIP_RECT = 0,  /* < Rectangle clip (default value) */
    VGU_CLIP_PATH,      /* < Path clip */
    VGU_CLIP_BUTT       /* < Invalid definition */
} VGUClipType;

/* *
 * @brief Defines a mask layer.
 *
 */
typedef struct {
    VGUBuffer *buffer;  /* < Pointer to the buffer for the mask */
    VGURect *rect;      /* < Pointer to the rectangle for the mask */
} VGUMaskLayer;

/* *
 * @brief Stores surface information for 2D hardware acceleration.
 *
 */
typedef struct {
    VGUBuffer *buffer;      /* < Bitmap buffer */
    union {
        VGURect *clipRect;  /* < Pointer to the clip rectangle. If it is null, the entire surface will be rendered. */
        VGUPath *clipPath;  /* < Pointer to the clip path. If it is null, the entire surface will be rendered. */
    };
    VGUClipType clipType;   /* < Clip type of the surface */
    VGUMaskLayer *mask;     /* < Mask layer, which can be null */
    VGUBlendType blend;     /* < Blend type, specifying how a new image is drawn onto an existing surface */
    VGUFilter filter;       /* < Filter type */
} VGUSurface;

/* *
 * @brief Defines how the colors are distributed along the gradient.
 *
 */
typedef struct {
    float stop;     /* < Stop position. The value ranges from 0.0 to 1.0. */
    uint32_t color; /* < Color of the stop */
} VGUColorStop;

/* *
 * @brief Defines a linear gradient.
 *
 */
typedef struct {
    VGUScalar x1; /* < Horizontal coordinate of the start point of the linear gradient */
    VGUScalar y1; /* < Vertical coordinate of the start point of the linear gradient */
    VGUScalar x2; /* < Horizontal coordinate of the end point of the linear gradient */
    VGUScalar y2; /* < Vertical coordinate of the end point of the linear gradient */
} VGULinear;

/* *
 * @brief Defines a radial gradient.
 *
 */
typedef struct {
    VGUScalar x0; /* < Horizontal coordinate of the center of the inner circle */
    VGUScalar y0; /* < Vertical coordinate of the center of the inner circle */
    VGUScalar r0; /* < Radius of the inner circle */
    VGUScalar x1; /* < Horizontal coordinate of the center of the outer circle */
    VGUScalar y1; /* < Vertical coordinate of the center of the outer circle */
    VGUScalar r1; /* < Radius of the outer circle */
} VGURadial;

/* *
 * @brief Defines a conic gradient.
 *
 */
typedef struct {
    VGUScalar cx; /* < Horizontal coordinate of the center of the circle */
    VGUScalar cy; /* < Vertical coordinate of the center of the circle */
} VGUConic;

/* *
 * @brief Defines an image.
 *
 */
typedef struct {
    VGUBuffer *buffer;  /* < Image buffer */
    VGUMatrix3 *matrix; /* < Pointer to the transformation matrix. If it is null, the identity matrix is used. */
    VGURect *rect;      /* < Pointer to the rectangle of the image. If it is null, the entire buffer data is used. */
    uint8_t opacity;    /* < Opacity. The value ranges from 0 to 255. */
} VGUImage;

/* *
 * @brief Defines an image pattern.
 *
 */
typedef struct {
    VGUImage *image;    /* < Pointer to the image object */
    VGUWrapType wrapx;  /* < Wrap the image horizontally. */
    VGUWrapType wrapy;  /* < Wrap the image vertically. */
} VGUPattern;

/* *
 * @brief Enumerates gradient types.
 *
 */
typedef enum {
    VGU_GRADIENT_LINEAR = 0,    /* < Linear gradient */
    VGU_GRADIENT_RADIAL,        /* < Radial gradient */
    VGU_GRADIENT_CONIC,         /* < Conic gradient */
    VGU_GRADIENT_BUTT           /* < Invalid definition */
} VGUGradientType;

/* *
 * @brief Defines a gradient object.
 *
 */
typedef struct {
    VGUMatrix3 *matrix;         /* < Pointer to the transformation matrix of the gradient object */
    VGUColorStop *colorStops;   /* < Pointer to the gradient stop color array */
    uint16_t stopCount;         /* < Number of stop colors */
    union {
        VGULinear linear;       /* < Linear gradient object */
        VGURadial radial;       /* < Radial gradient object */
        VGUConic conic;         /* < Conic gradient object */
    };
    VGUGradientType type;       /* < Gradient type */
    VGUFillSpread spread;       /* < Gradient spread mode */
    uint8_t opacity;            /* < Opacity. The value ranges from 0 to 255. */
} VGUGradient;

/* *
 * @brief Defines a solid color.
 *
 */
typedef struct {
    uint32_t color;     /* < Solid color */
    uint8_t opacity;    /* < Opacity. The value ranges from 0 to 255. */
} VGUSolid;

/* *
 * @brief Enumerates paint types.
 *
 */
typedef enum {
    VGU_PAINT_SOLID = 0,    /* < Paint a solid color. */
    VGU_PAINT_GRADIENT,     /* < Paint a gradient object. */
    VGU_PAINT_PATTERN,      /* < Paint a pattern. */
    VGU_PAINT_BUTT          /* < Invalid operation */
} VGUPaintType;

/* *
 * @brief Defines the paint style when filling or stroking a path.
 *
 */
typedef struct {
    union {
        VGUGradient *gradient;  /* < Pointer to the gradient object */
        VGUPattern *pattern;    /* < Pointer to the pattern object */
        VGUSolid *solid;        /* < Pointer to the solid color object */
    };
    VGUPaintType type;          /* < Paint type */
} VGUPaintStyle;

/* *
 * @brief Defines path filling attributes.
 *
 */
typedef struct {
    VGUFillRule rule; /* < Fill rule */
} VGUFillAttr;

/* *
 * @brief Defines path stroking attributes.
 *
 */
typedef struct {
    VGULineCap cap;     /* < Line cap style */
    VGUJointType join;  /* < Join type */
    float miterLimit;   /* < Miter limit */
    float width;        /* < Line width */
} VGUStrokeAttr;

/* *
 * @brief Defines driver functions for 2D hardware acceleration.
 */
typedef struct {
    /* *
     * @brief Initializes hardware acceleration.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @see DeinitVgu
     * @since 3.0
     */
    VGUResult (*InitVgu)(void);

    /* *
     * @brief Deinitializes hardware acceleration.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @see InitVgu
     * @since 3.0
     */
    VGUResult (*DeinitVgu)(void);

    /* *
     * @brief Queries hardware acceleration capabilities.
     *
     * @param cap Indicates the capabilities to query, which are defined by <b>VGUCapability</b>.
     *
     * @return Returns a value greater than or equal to 0 if the operation is successful; returns an error code defined
     * in {@link VGUResult} otherwise.
     * @since 3.0
     */
    int32_t (*QueryCapability)(uint32_t cap);

    /* *
     * @brief Fills the given path with a specified paint style.
     *
     * @param target Indicates the pointer to the target surface.
     * @param path Indicates the pointer to the path object.
     * @param matrix Indicates the pointer to the transformation matrix object. If this parameter is null,
     * the identity matrix is used by default.
     * @param attr Indicates the pointer to the path filling attributes.
     * @param style Indicates the pointer to the paint style to use.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderFill)(VGUSurface *target, const VGUPath *path, const VGUMatrix3 *matrix, const VGUFillAttr *attr,
        const VGUPaintStyle *style);

    /* *
     * @brief Strokes the given path with a specified paint style.
     *
     * @param target Indicates the pointer to the target surface.
     * @param path Indicates the pointer to the path object.
     * @param matrix Indicates the pointer to the transformation matrix object. If this parameter is null,
     * the identity matrix is used by default.
     * @param attr Indicates the pointer to the path stroking attributes.
     * @param style Indicates the pointer to the paint style to use.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderStroke)(VGUSurface *target, const VGUPath *path, const VGUMatrix3 *matrix,
        const VGUStrokeAttr *attr, const VGUPaintStyle *style);

    /* *
     * @brief Blurs a specified surface.
     *
     * @param target Indicates the pointer to the target surface.
     * @param blur Indicates the blur radius.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderBlur)(VGUSurface *target, uint16_t blur);

    /* *
     * @brief Blits an image to the target surface.
     *
     * During bit blit, color space conversion (CSC) and transformation can be implemented.
     *
     * @param target Indicates the pointer to the target surface.
     * @param src Indicates the pointer to the source image.
     * @param color Indicates the color for blending. If this parameter is <b>0</b>, color blending is not performed.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderBlit)(VGUSurface *target, const VGUImage *src, uint32_t color);

    /* *
     * @brief Blits multiple images to the target surface.
     *
     * During bit blit, color space conversion (CSC) and transformation can be implemented. You can use this 
     * function to combine multiple source images to the target surface.
     * To query the maximum number of source images allowed, call the <b>QueryCapability<b/> function.
     *
     * @param target Indicates the pointer to the target surface.
     * @param src Indicates the pointer to the array of source images.
     * @param count Indicates the number of source images.
     * @param color Indicates the color for blending. If this parameter is <b>0</b>, color blending is not performed.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderBlitN)(VGUSurface *target, const VGUImage *src, uint16_t count, uint32_t color);

    /* *
     * @brief Clears a rectangle with a given color on the target surface.
     *
     * @param target Indicates the pointer to the target surface.
     * @param rect Indicates the pointer to the rectangle to clear. If this parameter is null, the entire surface
     * will be cleared.
     * @param color Indicates the color to fill.
     * @param opacity Indicates the opacity to set.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderClearRect)(VGUSurface *target, const VGURect *rect, uint32_t color, uint8_t opacity);

    /* *
     * @brief Disables hardware acceleration for rendering.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderCancel)();

    /* *
     * @brief Synchronizes hardware acceleration when it is used to draw and blit bitmaps.
     *
     * This function blocks the process until hardware acceleration is complete.
     *
     * @param timeOut Indicates the timeout duration for hardware acceleration synchronization.
     * The value <b>0</b> indicates no timeout, so the process keeps waiting until hardware acceleration is complete.
     *
     * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
     * {@link VGUResult} otherwise.
     * @since 3.0
     */
    VGUResult (*RenderSync)(int32_t timeOut);
} VGUFuncs;

/* *
 * @brief Initializes a path object.
 *
 * @param path Indicates the pointer to the path object.
 * @param type Indicates the data type of the path.
 * @param segments Indicates the pointer to the path commands.
 * @param numSegments Indicates the total number of path commands.
 * @param data Indicates the pointer to the coordinate data used in the path commands.
 * @param enAlias Specifies whether to enable anti-aliasing.
 * @param boundBox Indicates the bounding box of the path.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUPathInit(VGUPath *path, VGUPathDataType type, const uint8_t* segments, int numSegments,
    const uint8_t *data, bool enAlias, VGURect boundBox);

/* *
 * @brief Adds a subpath to a specified path.
 *
 * @param path Indicates the pointer to the path object.
 * @param subpath Indicates the pointer to the subpath object.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUPathAppend(VGUPath *path, const VGUPath *subpath);

/* *
 * @brief Clears the memory of a specified path object.
 *
 * @param path Indicates the pointer to the path object.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUPathClear(VGUPath *path);

/* *
 * @brief Loads an identity matrix into a specified matrix object.
 *
 * @param matrix Indicates the pointer to the transformation matrix object.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUMatrixIdentity(VGUMatrix3 *matrix);

/* *
 * @brief Scales a specified transformation matrix.
 *
 * @param matrix Indicates the pointer to the transformation matrix object.
 * @param xScale Indicates how much you want to scale the horizontal coordinate by.
 * @param yScale Indicates how much you want to scale the vertical coordinate by.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUMatrixScale(VGUMatrix3 *matrix, float xScale, float yScale);

/* *
 * @brief Rotates a specified transformation matrix.
 *
 * @param matrix Indicates the pointer to the transformation matrix object.
 * @param degree Indicates the number of degrees to rotate.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUMatrixRotate(VGUMatrix3 *matrix, float degree);

/* *
 * @brief Translates a specified transformation matrix.
 *
 * @param matrix Indicates the pointer to the transformation matrix object.
 * @param x Indicates how much you want to translate the horizontal coordinate by.
 * @param y Indicates how much you want to translate the vertical coordinate by.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUMatrixTranslate(VGUMatrix3 *matrix, float x, float y);

/* *
 * @brief Adds color stops to a specified gradient.
 *
 * @param gradient Indicates the pointer to the gradient object.
 * @param colorStop Indicates the pointer to the color stop array.
 * @param count Indicates the total number of color stops.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUGradientColorStop(VGUGradient *gradient, const VGUColorStop *colorStop, uint32_t count);

/* *
 * @brief Clears color stops of a specified gradient.
 *
 * @param gradient Indicates the pointer to the gradient object.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUGradientClearStop(VGUGradient *gradient);

/* *
 * @brief Sets a transformation matrix for a specified gradient.
 *
 * @param gradient Indicates the pointer to the gradient object.
 * @param matrix Indicates the pointer to the transformation matrix object to set.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUGradientMatrix(VGUGradient *gradient, const VGUMatrix3 *matrix);

/* *
 * @brief Creates a linear gradient object.
 *
 * @param gradient Indicates the pointer to the gradient object.
 * @param p1 Indicates the pointer to the coordinates of the start point.
 * @param p2 Indicates the pointer to the coordinates of the end point.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUGradientLinear(VGUGradient *gradient, const VGUPoint *p1, const VGUPoint *p2);

/* *
 * @brief Creates a radial gradient object.
 *
 * @param gradient Indicates the pointer to the gradient object.
 * @param p1 Indicates the pointer to the center point coordinates of the inner circle.
 * @param r1 Indicates the radius of the inner circle.
 * @param p2 Indicates the pointer to the center point coordinates of the outer circle.
 * @param r2 Indicates the radius of the outer circle.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUGradientRadial(VGUGradient *gradient, const VGUPoint *p1, VGUScalar r1, const VGUPoint *p2, VGUScalar r2);

/* *
 * @brief Creates a conic gradient object.
 *
 * @param gradient Indicates the pointer to the gradient object.
 * @param cx Indicates the horizontal coordinate of the center point of the gradient.
 * @param cy Indicates the vertical coordinate of the center point of the gradient.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUGradientConic(VGUGradient *gradient, VGUScalar cx, VGUScalar cy);

/* *
 * @brief Initializes the hardware acceleration module to obtain the pointer to functions for
 * hardware acceleration operations.
 *
 * @param funcs Indicates the double pointer to the functions for hardware acceleration operations.
 * Memory is allocated automatically when you initiate the hardware acceleration module, so you can simply use
 * the pointer to gain access to the functions.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 *
 * @since 3.0
 */
VGUResult VGUInitialize(VGUFuncs **funcs);

/* *
 * @brief Deinitializes the hardware acceleration module to release the pointer to functions
 * for hardware acceleration operations.
 *
 * @param funcs Indicates the pointer to the functions for hardware acceleration operations.
 *
 * @return Returns <b>VGU_SUCCESS</b> if the operation is successful; returns an error code defined in
 * {@link VGUResult} otherwise.
 * @since 3.0
 */
VGUResult VGUUninitialize(VGUFuncs *funcs);

#ifdef __cplusplus
}
#endif
#endif
