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

#ifndef TEST_RESOURCE_CONFIG_H
#define TEST_RESOURCE_CONFIG_H

#include "graphic_config.h"

namespace OHOS {
#if defined QT_COMPILER
#define IMAGE_DIR "..\\..\\simulator\\config\\images\\"
#define FACE_DIR "..\\..\\simulator\\config\\faces\\default\\"
#elif defined _WIN32
#define IMAGE_DIR "..\\config\\images\\"
#define FACE_DIR "..\\config\\faces\\default\\"
#else
#define IMAGE_DIR RESOURCE_DIR
#define FACE_DIR RESOURCE_DIR
#endif

#define BLUE_RGB888_IMAGE_PATH (IMAGE_DIR "blue_rgb888.bin")
#define BLUE_ARGB8888_IMAGE_PATH (IMAGE_DIR "blue_argb8888.bin")
#define BLUE_RGB565_IMAGE_PATH (IMAGE_DIR "blue_rgb565.bin")
#define BLUE_IMAGE_PATH (IMAGE_DIR "blue.png")
#define RED_IMAGE_PATH (IMAGE_DIR "red.png")
#define GREEN_IMAGE_PATH (IMAGE_DIR "green.png")
#define YELLOW_IMAGE_PATH (IMAGE_DIR "yellow.png")
#define IMAGE_ANIMATOR_0_PATH (IMAGE_DIR "image_animator_0.bin")
#define IMAGE_ANIMATOR_1_PATH (IMAGE_DIR "image_animator_1.bin")
#define IMAGE_ANIMATOR_2_PATH (IMAGE_DIR "image_animator_2.bin")
#define IMAGE_ANIMATOR_3_PATH (IMAGE_DIR "image_animator_3.bin")
#define B024_002_IMAGE_PATH (FACE_DIR "B024_002.bin")
#define B024_009_IMAGE_PATH (FACE_DIR "B024_009.bin")
#define B024_010_IMAGE_PATH (FACE_DIR "B024_010.bin")
#define B024_011_IMAGE_PATH (FACE_DIR "B024_011.bin")
#define SLIDER_BACKGROUND_IMAGE_PATH (IMAGE_DIR "sliderBackground.bin")
#define SLIDER_INDICATOR_IMAGE_PATH (IMAGE_DIR "sliderIndicator.bin")
#define SLIDER_KNOB_IMAGE_PATH (IMAGE_DIR "sliderKnob.bin")
#define JPEG_IMAGE_PATH (IMAGE_DIR "jpeg.jpg")
#define PNG_1_PALETTE_IMAGE_PATH (IMAGE_DIR "png_1_palette.png")
#define PNG_2_PALETTE_IMAGE_PATH (IMAGE_DIR "png_2_palette.png")
#define PNG_4_PALETTE_IMAGE_PATH (IMAGE_DIR "png_4_palette.png")
#define PNG_8_PALETTE_IMAGE_PATH (IMAGE_DIR "png_8_palette.png")
#define PNG_8_GRAY_IMAGE_PATH (IMAGE_DIR "png_8_gray.png")
#define PNG_16_GRAY_IMAGE_PATH (IMAGE_DIR "png_16_gray.png")
#define PNG_24_RGB_IMAGE_PATH (IMAGE_DIR "png_24_rgb.png")
#define PNG_32_RGB_ALPHA_IMAGE_PATH (IMAGE_DIR "png_32_rgba.png")
#define GIF_IMAGE_PATH (IMAGE_DIR "gifImage.gif")
#if (ENABLE_GIF == 1)
#define GIF_IMAGE_PATH1 (IMAGE_DIR "gif_image1.gif")
#define GIF_IMAGE_PATH2 (IMAGE_DIR "gif_image2.gif")
#define GIF_IMAGE_PATH_ERROR (IMAGE_DIR "gif_error.gif")
#define TIF_IMAGE_PATH (IMAGE_DIR "tif_image.tif")
#endif
#define VIDEO_SOURCE_DIRECTORY (IMAGE_DIR "video.mp4")
#define TEST_RIGHT_ARROW (IMAGE_DIR "ic_arrow_right.png")
#define TEST_BACK_LEFT_ARROW (IMAGE_DIR "ic_back.png")
#define TEST_CIRCLE_FORE_IMAGE (FACE_DIR "fore.png")
#define IMAGE_RESIZEMODE_PATH (IMAGE_DIR "blue.png")
} // namespace OHOS
#endif // TEST_RESOURCE_CONFIG_H
