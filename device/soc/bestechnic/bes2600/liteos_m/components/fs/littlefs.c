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
#include "littlefs.h"
#include "flash.h"

int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                            lfs_off_t off, void *dst, lfs_size_t size)
{
    uint32_t addr = c->block_size * block + off;
    return hal_flash_read((hal_partition_t)c->context, addr, dst, size);
}

int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                             lfs_off_t off, const void *dst, lfs_size_t size)
{
    uint32_t addr = c->block_size * block + off;
    return hal_flash_write((hal_partition_t)c->context, addr, dst, size);
}

int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t addr = c->block_size * block;
    return hal_flash_erase((hal_partition_t)c->context, addr, c->block_size);
}

int32_t littlefs_block_sync(const struct lfs_config *c)
{
    return 0;
}
