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
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file graphic_buffer.h
 *
 * @brief Defines the LCD buffer info for the graphics system.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_BUFFER_H
#define GRAPHIC_LITE_BUFFER_H

#include "gfx_utils/graphic_types.h"
#include "gfx_utils/rect.h"

namespace OHOS {
/**
 * @brief buffer info for drawing.
 * @since 6.0
 * @version 6.0
 */
struct BufferInfo {
    Rect rect;
    int32_t stride;
    void *phyAddr;
    void *virAddr;
    uint16_t width;
    uint16_t height;
    ColorMode mode;
    uint32_t color;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_TYPES_H
