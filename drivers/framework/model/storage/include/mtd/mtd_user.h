/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MTD_USER_H
#define MTD_USER_H

#include <sys/ioctl.h>

enum MtdFileMode {
    MTD_FILE_MODE_NORMAL,
    MTD_FILE_MODE_RAW,
    MTD_FILE_MODE_OOB,
    MTD_FILE_MODE_MAX,
};

struct MtdInfo {
    uint8_t type;
    uint32_t size;        /* Total size of the MTD */
    uint32_t erasesize;
    uint32_t writesize;
    uint32_t oobsize;     /* Amount of OOB data per block (e.g. 16) */
    uint64_t padding;     /* Old obsolete field; do not use */
};

struct EraseInfo {
    uint32_t start;
    uint32_t length;
};

struct EraseInfo64 {
    uint64_t start;
    uint64_t length;
};

struct MtdOobBuf {
    uint32_t start;
    uint32_t length;
    unsigned char *ptr;
};

#define MTD_IOC_GETINFO          _IOR('M', 1, struct MtdInfo)
#define MTD_IOC_ERASE            _IOW('M', 2, struct EraseInfo)
#define MTD_IOC_WRITEOOB         _IOWR('M', 3, struct MtdOobBuf)
#define MTD_IOC_READOOB          _IOWR('M', 4, struct MtdOobBuf)
#define MTD_IOC_GETBADBLOCK      _IOW('M', 11, loff_t)
#define MTD_IOC_SETBADBLOCK      _IOW('M', 12, loff_t)
#define MTD_IOC_SETFILEMODE      _IO('M', 19)
#define MTD_IOC_ERASE64          _IOW('M', 20, struct EraseInfo64)

#endif /* MTD_USER_H */
