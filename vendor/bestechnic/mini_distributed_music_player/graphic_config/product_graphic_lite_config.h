/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#ifndef PRODUCT_GRAPHIC_LITE_CONFIG_H
#define PRODUCT_GRAPHIC_LITE_CONFIG_H

/**
 * @brief Font shaping, which is disabled by default on Huawei LiteOS.
 *        Cannot take effect without ENABLE_ICU.
 */
#define ENABLE_SHAPING 0
/**
 * @brief Advanced algorithm for line breaks, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_ICU 0
/**
 * @brief Multi-Font type, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_MULTI_FONT 0
/**
 * @brief Multi-window, which does not take effect on Huawei LiteOS.
 */
#define ENABLE_WINDOW 0
/**
 * @brief Display buffer for rendering data refresh, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_FRAME_BUFFER 1
/**
 * @brief Vector type font, which is enabled by default on Huawei LiteOS.
 */
#define ENABLE_VECTOR_FONT 1
/**
 * @brief Bitmap type font, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_BITMAP_FONT 0
/**
 * @brief Static Bitmap type font, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_STATIC_FONT 0

#define ENABLE_JPEG_AND_PNG 1

#define ENABLE_GIF 1

#define FULLY_RENDER 1

/**
 * @brief Graphics rendering hardware acceleration, which is enabled by default on Huawei LiteOS.
 */
#define ENABLE_HARDWARE_ACCELERATION 0
/**
 * @brief Graphics rendering hardware acceleration for text, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_HARDWARE_ACCELERATION_FOR_TEXT 0
/**
 * @brief Graphics rendering accelerated by gfx_engines, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_GFX_ENGINES 0
/**
 * @brief ARM NEON ability, which is disabled by default on Huawei LiteOS.
 */
#define ENABLE_ARM_NEON 0

#define ENABLE_ROTATE_INPUT 1

#define ENABLE_VIBRATOR 0

#define ENABLE_FOCUS_MANAGER 1

/**
 * @brief Graphics local rendering, which is disabled by default.
 */
#define LOCAL_RENDER 0

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
#define ENABLE_BUFFER_RGBA 1
/**
 * @brief Debug mode, which is disabled by default.
 */
#define ENABLE_DEBUG 1
/**
 * @brief Memory hook, which is enabled by default. The system memory allocation is taken over after it is enabled.
 */
#define ENABLE_MEMORY_HOOKS 0
/**
 * @brief Function for monitoring the image refresh frame rate, which is disabled by default.
 */
#define ENABLE_FPS_SUPPORT 1
/**
 * @brief Anti-aliasing, which is enabled by default.
 */
#define ENABLE_ANTIALIAS 1
/**
 * @brief Rectangle anti-aliasing, which is disabled by default.
 */
#define ENABLE_RECT_ANTIALIAS 0
/**
 * @brief Font color mode, which is disabled by default.
 * After it is enabled, the font color mode is set to <b>4</b> to accelerate font rendering.
 */
#define ENABLE_SPEC_FONT 0
/**
 * @brief Log function of a graphics subsystem, which is disabled by default
 */
#define ENABLE_GRAPHIC_LOG 1
/**
 * @brief Performance tracking for debugging, which is disabled by default.
 */
#define ENABLE_DEBUG_PERFORMANCE_TRACE 0
/**
 * @brief Function for receiving input events in screen-off mode, which is disabled by default.
 */
#define ENABLE_AOD 0

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
#define GRAPHIC_LOG_LEVEL 5
/**
 * @brief Defines the color depth of graphics rendering. The default value is <b>32</b> bits.
 * The value can be <b>16</b> or <b>32</b>.
 */
#define COLOR_DEPTH 32

/**
 * @brief Represents the code number of the layer pixel format.
 * 0:     LAYER_PF_ARGB1555
 * 1:     LAYER_PF_ARGB8888
 * Other: LAYER_PF_ARGB8888
 */
#define LAYER_PF_CODE 1

/**
 * @brief Defines the file name of default vector font.
 */
#define DEFAULT_VECTOR_FONT_FILENAME "font.ttf"

#ifdef CONFIG_DISPLAY_RM69330
#define HORIZONTAL_RESOLUTION 454
#define VERTICAL_RESOLUTION 454
#elif defined(CONFIG_DISPLAY_ICNA3311)
#define HORIZONTAL_RESOLUTION 280
#define VERTICAL_RESOLUTION 456
#elif defined(CONFIG_DISPLAY_A064)
#define HORIZONTAL_RESOLUTION 480
#define VERTICAL_RESOLUTION 480
#endif

/* Default file path for font */
#define VECTOR_FONT_DIR "/data/" // It must end with '/'.

#define IMG_CACHE_SIZE 8

#endif // PRODUCT_GRAPHIC_LITE_CONFIG_H
