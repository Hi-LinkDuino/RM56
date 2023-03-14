/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *  
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MTD_LEGACY_LITE_H
#define MTD_LEGACY_LITE_H

#include "hdf_base.h"
#include "mtd/mtd_core.h"
#include "fs/driver.h"

typedef struct erase_info {
    int scrub;
    struct erase_info *next;
    unsigned char state;
    unsigned long priv;
    void (*callback)(struct erase_info *self);
    unsigned int cell;
    unsigned int dev;
    unsigned long retries;
    unsigned long time;
    uint64_t fail_addr;
    uint64_t len;
    uint64_t addr;
    struct mtd_info *mtd;
} EraseInfoLegacy;

typedef struct mtd_info {
    void *priv;
    uint8_t type;
    const char *name;
    uint32_t flags;
    uint64_t size;
    uint32_t erasesize;
    uint32_t writesize;
    uint32_t oobsize;
    int (*erase)(struct mtd_info *mtd, struct erase_info *instr);
    int (*read)(struct mtd_info *mtd, off_t from, size_t len, size_t *retlen, char *buf);
    int (*write)(struct mtd_info *mtd, off_t to, size_t len, size_t *retlen, const char *buf);
    int (*read_oob)(struct mtd_info *mtd, off_t from, size_t len, size_t *retlen, char *buf);
    int (*write_oob)(struct mtd_info *mtd, off_t to, size_t len, size_t *retlen, const char *buf);
    int (*block_isbad)(struct mtd_info *mtd, off_t ofs);
    int (*block_markbad)(struct mtd_info *mtd, off_t ofs);
} MtdInfoLegacy;

extern MtdInfoLegacy *nand_mtd;

const struct file_operations_vfs *GetMtdCharFops(void);
const struct block_operations *GetDevSpinorOps(void);
const struct block_operations *GetDevNandOps(void);
void MtdDeviceLegacyFillMtdInfo(struct MtdDevice *mtdDevice);

#define MTD_NORFLASH                3
#define MTD_NANDFLASH               4

#define MTD_WRITEABLE               0x400
#define MTD_CAP_NANDFLASH           (MTD_WRITEABLE)

#define MTD_FAIL_ADDR_UNKNOWN       (-1LL)

#define GOOD_BLOCK                  0


#endif /* MTD_LEGACY_LITE_H */
