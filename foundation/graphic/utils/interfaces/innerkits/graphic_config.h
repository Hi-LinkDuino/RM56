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
 * @addtogroup Graphic
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file graphic_config.h
 *
 * @brief Provides configuration items required for graphics.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GRAPHIC_CONFIG_H
#define GRAPHIC_LITE_GRAPHIC_CONFIG_H

#if (ENABLE_OHOS_GRAPHIC_UTILS_PRODUCT_CONFIG == 1)
#include "product_graphic_lite_config.h"
#endif

#include "gfx_utils/graphic_types.h"

namespace OHOS {
/**
 * Defines three graphics library versions: lightweight, standard, and extended versions.
 * The three versions have different requirements on the memory and hardware.
 * The standard version is enabled by default.
 *
 * The macros of the versions are defined as follows:
 * Name                | Version Description
 * ------------------- | ----------
 * VERSION_LITE        | Lightweight version
 * VERSION_STANDARD    | Standard version
 * VERSION_EXTENDED    | Extended version
 */
#ifdef __LITEOS_M__
#define VERSION_LITE
#elif defined _WIN32 || defined __APPLE__
#define VERSION_LITE
#else
#define VERSION_STANDARD
#endif

/* Defines some configurations and their default values on LiteOS and other system platforms. */
#ifndef ENABLE_FIXED_POINT
#define ENABLE_FIXED_POINT                    1
#endif

#ifdef __LITEOS_M__
/**
 * @brief Font shaping, which is disabled by default on LiteOS.
 *        Cannot take effect without ENABLE_ICU.
 */
#ifndef ENABLE_SHAPING
#define ENABLE_SHAPING                    0
#endif
/**
 * @brief Advanced algorithm for line breaks, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_ICU
#define ENABLE_ICU                        0
#endif
/**
 * @brief Multi-Font type, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_MULTI_FONT
#define ENABLE_MULTI_FONT                 0
#endif
/**
 * @brief Multi-window, which does not take effect on LiteOS.
 */
#ifndef ENABLE_WINDOW
#define ENABLE_WINDOW                     0
#endif
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_FRAME_BUFFER
#define ENABLE_FRAME_BUFFER               1
#endif
/**
 * @brief Vector type font, which is enabled by default on LiteOS.
 */
#ifndef ENABLE_VECTOR_FONT
#define ENABLE_VECTOR_FONT                1
#endif
/**
 * @brief Bitmap type font, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_BITMAP_FONT
#define ENABLE_BITMAP_FONT                0
#endif
/**
 * @brief Static Bitmap type font, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_STATIC_FONT
#define ENABLE_STATIC_FONT                0
#endif

/**
 * @brief JPEG and PNG, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_JPEG_AND_PNG
#define ENABLE_JPEG_AND_PNG               0
#endif

/**
 * @brief GIF, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_GIF
#define ENABLE_GIF                        0
#endif

/**
 * @brief Fully rectangle render everytime, which is disabled by default on LiteOS.
 */
#ifndef FULLY_RENDER
#define FULLY_RENDER                     0
#endif
/**
 * @brief Graphics rendering hardware acceleration, which is enabled by default on LiteOS.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION
#define ENABLE_HARDWARE_ACCELERATION      1
#endif
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION_FOR_TEXT
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
#endif
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_GFX_ENGINES
#define ENABLE_GFX_ENGINES                0
#endif
/**
 * @brief ARM NEON ability, which is disabled by default on LiteOS.
 */
#ifndef ENABLE_ARM_NEON
#define ENABLE_ARM_NEON                   0
#endif
#elif defined _WIN32 || defined __APPLE__
/**
 * @brief Multi-window, which is disabled by default on WIN32.
 */
#ifndef ENABLE_WINDOW
#define ENABLE_WINDOW                     0
#endif
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on WIN32.
 */
#ifndef ENABLE_FRAME_BUFFER
#define ENABLE_FRAME_BUFFER               0
#endif

#ifndef ENABLE_JPEG_AND_PNG
#define ENABLE_JPEG_AND_PNG               1
#endif
/**
 * @brief ARM NEON ability, which is disabled by default on WIN32.
 */
#ifndef ENABLE_ARM_NEON
#define ENABLE_ARM_NEON                   0
#endif
/**
 * @brief Graphics rendering hardware acceleration, which is disabled by default on WIN32.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION
#define ENABLE_HARDWARE_ACCELERATION      0
#endif
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on WIN32.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION_FOR_TEXT
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
#endif
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default on WIN32.
 */
#ifndef ENABLE_GFX_ENGINES
#define ENABLE_GFX_ENGINES                0
#endif

#else
/**
 * @brief Multi-window, which is enabled by default on other platforms.
 */
#ifndef ENABLE_WINDOW
#define ENABLE_WINDOW                     1
#endif
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on other platforms.
 */
#ifndef ENABLE_FRAME_BUFFER
#define ENABLE_FRAME_BUFFER               0
#endif

#ifndef ENABLE_JPEG_AND_PNG
#define ENABLE_JPEG_AND_PNG               1
#endif
/**
 * @brief ARM NEON ability, which is enabled by default on other platforms.
 */
#ifndef ENABLE_ARM_NEON
#define ENABLE_ARM_NEON                   1
#endif
/**
 * @brief Graphics rendering hardware acceleration, which is disabled by default on other platforms.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION
#define ENABLE_HARDWARE_ACCELERATION      0
#endif
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on other platforms.
 */
#ifndef ENABLE_HARDWARE_ACCELERATION_FOR_TEXT
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT      0
#endif
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is enabled by default on other platforms.
 */
#ifndef ENABLE_GFX_ENGINES
#define ENABLE_GFX_ENGINES                1
#endif // ENABLE_GFX_ENGINES
#endif

#ifndef ENABLE_ROTATE_INPUT
#define ENABLE_ROTATE_INPUT               1
#endif

#ifndef ENABLE_VIBRATOR
#define ENABLE_VIBRATOR                   1
#endif

#ifndef ENABLE_FOCUS_MANAGER
#define ENABLE_FOCUS_MANAGER              1
#endif

/**
 * @brief Graphics local rendering, which is disabled by default.
 */
#ifndef LOCAL_RENDER
#define LOCAL_RENDER                      0
#endif

/**
 * @brief Actually use ARM NEON optimization.
 *        __ARM_NEON__ and __ARM_NEON are set by the compiler according to the compilation option -mfpu=neon
 */
#if (defined(__ARM_NEON__) || defined(__ARM_NEON)) && ENABLE_ARM_NEON == 1
#define ARM_NEON_OPT
#endif

/**
 * @brief Graphics bottom-layer RGBA, which is enabled by default.
 */
#ifndef ENABLE_BUFFER_RGBA
#define ENABLE_BUFFER_RGBA                1
#endif
/**
 * @brief Debug mode, which is disabled by default.
 */
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG                      1
#endif
/**
 * @brief Memory hook, which is enabled by default. The system memory allocation is taken over after it is enabled.
 */
#ifndef ENABLE_MEMORY_HOOKS
#define ENABLE_MEMORY_HOOKS               0
#endif
/**
 * @brief Function for monitoring the image refresh frame rate, which is disabled by default.
 */
#ifndef ENABLE_FPS_SUPPORT
#define ENABLE_FPS_SUPPORT                0
#endif
/**
 * @brief Anti-aliasing, which is enabled by default.
 */
#ifndef ENABLE_ANTIALIAS
#define ENABLE_ANTIALIAS                  1
#endif
/**
 * @brief Rectangle anti-aliasing, which is disabled by default.
 */
#ifndef ENABLE_RECT_ANTIALIAS
#define ENABLE_RECT_ANTIALIAS             0
#endif
/**
 * @brief Font color mode, which is disabled by default.
 * After it is enabled, the font color mode is set to <b>4</b> to accelerate font rendering.
 */
#ifndef ENABLE_SPEC_FONT
#define ENABLE_SPEC_FONT                  0
#endif
/**
 * @brief Log function of a graphics subsystem, which is disabled by default
 */
#ifndef ENABLE_GRAPHIC_LOG
#define ENABLE_GRAPHIC_LOG                0
#endif
/**
 * @brief Performance tracking for debugging, which is disabled by default.
 */
#ifndef ENABLE_DEBUG_PERFORMANCE_TRACE
#define ENABLE_DEBUG_PERFORMANCE_TRACE    0
#endif
/**
 * @brief Function for receiving input events in screen-off mode, which is disabled by default.
 */
#ifndef ENABLE_AOD
#define ENABLE_AOD                        0
#endif

/**
 * @brief Defines the log level. A smaller value indicates a higher priority.
 * Logs whose priorities are higher than a specified level can be recorded.
 * Log levels:
 * NONE: disabling logs
 * FATAL: fatal level
 * ERROR: error level
 * WARN: warning level
 * INFO: info level
 * DEBUG: debugging level
 */
#ifndef GRAPHIC_LOG_LEVEL
#define GRAPHIC_LOG_LEVEL                 5
#endif
/**
 * @brief Defines the color depth of graphics rendering. The default value is <b>32</b> bits.
 * The value can be <b>16</b> or <b>32</b>.
 */
#ifndef COLOR_DEPTH
#define COLOR_DEPTH 32
#endif

/**
 * @brief Represents the code number of the layer pixel format.
 * 0:     LAYER_PF_ARGB1555
 * 1:     LAYER_PF_ARGB8888
 * Other: LAYER_PF_ARGB8888
 */
#ifndef LAYER_PF_CODE
#define LAYER_PF_CODE                     1
#endif

#if LAYER_PF_CODE == 0
#define LAYER_PF_ARGB1555
#elif LAYER_PF_CODE == 1
#define LAYER_PF_ARGB8888
#else
#define LAYER_PF_ARGB8888
#endif

/**
 * @brief Defines whether the C library math function is supported.
 * 0:    Unsupport
 * 1:    Support
  */
#ifndef ENABLE_CMATH
#define ENABLE_CMATH              1
#endif

/**
 * @brief Defines the file name of default vector font.
 */
#if ENABLE_VECTOR_FONT

#ifndef DEFAULT_VECTOR_FONT_FILENAME
#define DEFAULT_VECTOR_FONT_FILENAME      "SourceHanSansSC-Regular.otf"
#endif // DEFAULT_VECTOR_FONT_FILENAME

#else // ENABLE_VECTOR_FONT

#ifndef DEFAULT_VECTOR_FONT_FILENAME
#define DEFAULT_VECTOR_FONT_FILENAME      "SourceHanSansSC-Regular"
#endif // DEFAULT_VECTOR_FONT_FILENAME

#endif // ENABLE_VECTOR_FONT

/* Default font size. The default value is <b>18</b>. */
static constexpr uint8_t DEFAULT_VECTOR_FONT_SIZE = 18;

/* Defines the file name of default line break rule. */
#ifndef DEFAULT_LINE_BREAK_RULE_FILENAME
#define DEFAULT_LINE_BREAK_RULE_FILENAME   "line_cj.brk"
#endif

/* Defines some configurations and their default values on LiteOS and other system platforms. */
#ifdef __LITEOS_M__

/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
#ifndef HORIZONTAL_RESOLUTION
#define HORIZONTAL_RESOLUTION                           454
#endif

/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
#ifndef VERTICAL_RESOLUTION
#define VERTICAL_RESOLUTION                             454
#endif // VERTICAL_RESOLUTION

#elif defined _WIN32 || defined __APPLE__ // __LITEOS_M__

/* Resolution width of a graphics display screen. The default value is <b>454</b>. */
#ifndef HORIZONTAL_RESOLUTION
#define HORIZONTAL_RESOLUTION                           960
#endif

/* Resolution height of a graphics display screen. The default value is <b>454</b>. */
#ifndef VERTICAL_RESOLUTION
#define VERTICAL_RESOLUTION                             480
#endif

#else

/* Resolution width of a graphics display screen. The default value is <b>960</b>. */
#ifndef HORIZONTAL_RESOLUTION
#define HORIZONTAL_RESOLUTION                           960
#endif

/* Resolution height of a graphics display screen. The default value is <b>480</b>. */
#ifndef VERTICAL_RESOLUTION
#define VERTICAL_RESOLUTION                             480
#endif

#endif

#ifndef VERSION_LITE
static constexpr const char* MEDIA_IMAGE_PLAY_CENTER = "/user/data/videoplayer_play_center.png";
static constexpr const char* MEDIA_IMAGE_PLAY = "/user/data/videoplayer_play.png";
static constexpr const char* MEDIA_IMAGE_PAUSE = "/user/data/videoplayer_pause.png";
static constexpr const char* MEDIA_IMAGE_VOLUME = "/user/data/videoplayer_volume.png";
static constexpr const char* MEDIA_IMAGE_MUTE = "/user/data/videoplayer_mute.png";
#endif

/* Defines some configurations and their default values on LiteOS and other system platforms. */
#if defined QT_COMPILER
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = ".\\dump_dom_tree.json";

/* Default file path for font */
#ifndef VECTOR_FONT_DIR
#define VECTOR_FONT_DIR                                 "..\\..\\simulator\\font\\"
#endif

#elif defined _WIN32  // QT_COMPILER

/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = ".\\dump_dom_tree.json";
/* Default file path for font */
#ifndef VECTOR_FONT_DIR
#define VECTOR_FONT_DIR                                 "..\\..\\tools\\font\\font_tool\\font_tool\\font\\"
#endif

#elif defined __LITEOS_M__  // QT_COMPILER

/* Default file path for screenshots */
static constexpr const char* DEFAULT_SCREENSHOT_PATH = "user/log/screenshot.bin";
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = "user/log/dump_dom_tree.json";
/* Default file path for font */
#ifndef VECTOR_FONT_DIR
#define VECTOR_FONT_DIR                                 "/user/data/"
#endif

#else  // QT_COMPILER

/* Default file path for screenshots */
static constexpr const char* DEFAULT_SCREENSHOT_PATH = "/storage/screenshot.bin";
/* Default file path for DOM tree logs */
static constexpr const char* DEFAULT_DUMP_DOM_TREE_PATH = "/storage/dump_dom_tree.json";
/* Default file path for font */
#ifndef VECTOR_FONT_DIR
#define VECTOR_FONT_DIR                                 "/user/data/"
#endif

#endif // QT_COMPILER
/* Default task execution period. The default value is <b>16</b> ms. */
static constexpr uint8_t DEFAULT_TASK_PERIOD = 16;
/* Window manager execution period. The default value is <b>16</b> ms. */
static constexpr uint8_t WMS_MAIN_TASK_PERIOD = 16;
/* Maximum number of cached images. The default value is <b>5</b>. */
#ifndef IMG_CACHE_SIZE
#define IMG_CACHE_SIZE                                  5
#endif
static constexpr uint8_t INDEV_READ_PERIOD = 10; /* Input event read cycle. The default value is <b>10</b> ms. */
/* Drag distance threshold of a drag event. The default value is <b>10px</b>. */
static constexpr uint8_t INDEV_DRAG_LIMIT = 10;
/* Maximum depth of view nesting. The default value is <b>64</b>. */
static constexpr uint8_t COMPONENT_NESTING_DEPTH = 64;
/* Long-press event threshold. The default value is <b>1000</b> ms. */
static constexpr uint16_t INDEV_LONG_PRESS_TIME = 1000;
/* Delay for reporting a press event on a draggable object. The default value is <b>100</b> ms. */
static constexpr uint16_t INDEV_PRESS_TIME_IN_DRAG = 100;
/* Maximum number of bytes in a text that can be continuously displayed. The default value is <b>4096</b> bytes. */
static constexpr uint16_t MAX_TEXT_LENGTH = 4096;
/* Maximum value of the graphic display range. The default value is <b>16383px</b>. */
static constexpr int32_t COORD_MAX = 16383;
/* Minimum value of the graphic display range. The default value is <b>-16384px</b>. */
static constexpr int32_t COORD_MIN = -16384;
static constexpr uint32_t HARFBUZ_CACHE_LENGTH = 0x19000; // 100K
static constexpr uint16_t MAX_LINE_WIDTH = 128;
/* Maximum length of a QR code string. The default value is <b>2048</b>. */
static constexpr uint32_t QRCODE_VAL_MAX = 2048;

/* Rotate sensitivity factor. The default value is <b>1.0<b> time. */
static constexpr float ROTATE_SENSITIVITY = 1.0;
/* Rotate sensitivity factor of UISwipeView. The default value is <b>2.8<b> time. */
static constexpr float DEFAULT_SWIPE_VIEW_ROTATE_FACTOR = 2.8;
/* Rotate sensitivity factor of UIList. The default value is <b>1.2<b> time. */
static constexpr float DEFAULT_LIST_ROTATE_FACTOR = 1.2;
/* Rotate sensitivity factor of UIPicker. The default value is <b>1.2<b> time. */
static constexpr float DEFAULT_PICKER_ROTATE_FACTOR = 1.2;
/* Rotate sensitivity factor of UIScrollView. The default value is <b>2.0<b> time. */
static constexpr float DEFAULT_SCROLL_VIEW_ROTATE_FACTOR = 2.0;
#ifdef _WIN32
/* Rotate sensitivity factor of UISlider. The default value is <b>-1<b> time. */
static constexpr float DEFAULT_SLIDER_ROTATE_FACTOR = -1;
#else
/* Rotate sensitivity factor of UISlider. The default value is <b>-0.05<b> time. */
static constexpr float DEFAULT_SLIDER_ROTATE_FACTOR = -0.05;
#endif
/* When rotating the crown, a vibration is triggered every 30 pixels. */
static constexpr uint8_t DEFAULT_SCROLL_VIEW_VIBRATION_LEN = 30;
/* calculate drag throw distance when rotating, last rotate distance * ROTATE_DISTANCE_COEFFICIENT */
static constexpr uint8_t ABSTRACT_ROTATE_DISTANCE_COEFF = 20;
static constexpr uint8_t SCROLLVIEW_ROTATE_DISTANCE_COEFF = 20;
static constexpr uint8_t PICKERVIEW_ROTATE_DISTANCE_COEFF = 20;
static constexpr uint8_t LIST_ROTATE_DISTANCE_COEFF = 20;
/* calculate rotate throw threshold when rotating the crown, width or height / DEFAULT_ROTATE_THROW_THRESHOLD */
static constexpr uint8_t ABSTRACT_ROTATE_THROW_THRESHOLD = 4;
static constexpr uint8_t SCROLLVIEW_ROTATE_THROW_THRESHOLD = 4;
static constexpr uint8_t PICKERVIEW_ROTATE_THROW_THRESHOLD = 4;
static constexpr uint8_t LIST_ROTATE_THROW_THRESHOLD = 4;
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_CONFIG_H
