/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Bluetooth Basic tool library, This file is part of BTStack.
 *        Data Struct buffer declarations.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Buffer Buffer;

/**
 * @brief Malloc new fixed size Buffer.
 *
 * @param size Buffer size.
 * @return Buffer pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *BufferMalloc(uint32_t size);

/**
 * @brief Copy malloc new Buffer by Existing Buffer. Do not copy Data.
 *
 * @param buf Buffer pointer.
 * @return Buffer pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *BufferRefMalloc(const Buffer *buf);

/**
 * @brief
 *        Slice from Existing Buffer's offset, slice length is size.
 *
 * @param[in] buf Buffer pointer.
 * @param[in] offset
 * @param[in] size Slice Buffer size.
 * @return Slice Buffer pointer.
 */

/**
 * @brief Slice new Buffer from Existing Buffer. Do not copy Data.
 *
 * @param buf Buffer pointer.
 * @param offset Offset relative to old buffer.
 * @param size Target buffer size.
 * @return Buffer pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *BufferSliceMalloc(const Buffer *buf, uint32_t offset, uint32_t size);

/**
 * @brief Resize Buffer.
 *
 * @param buf Buffer pointer.
 * @param offset Offset buffer offset
 * @param size Target size.
 * @return Return resized buffer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *BufferResize(Buffer *buf, uint32_t offset, uint32_t size);

/**
 * @brief Release Buffer.
 *
 * @param buf Buffer pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void BufferFree(Buffer *buf);

/**
 * @brief Get Buffer data pointer.
 *
 * @param buf Buffer pointer.
 * @return Buffer data pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void *BufferPtr(const Buffer *buf);

/**
 * @brief Get Buffer size.
 *
 * @param buf Buffer pointer.
 * @return Buffer size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t BufferGetSize(const Buffer *buf);

#ifdef __cplusplus
}
#endif

#endif  // BUFFER_H