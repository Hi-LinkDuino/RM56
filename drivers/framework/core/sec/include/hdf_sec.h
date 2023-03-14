/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SEC_H
#define HDF_SEC_H

#if defined(__KERNEL__)
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DEVICE_HDF_SECURE_NAME "hdf_secure"
#define HDF_SECURE_MAGIC 'S'

/* event commands */
#define HDF_SECURE_SET_INFO _IO(HDF_SECURE_MAGIC, 0x1)
#define HDF_SECURE_SET_CURRENT_ID _IO(HDF_SECURE_MAGIC, 0x2)
#define HDF_SECURE_DELETE_INFO _IO(HDF_SECURE_MAGIC, 0x3)

#define HDF_DECLARE_BITMAP(name, bits) \
    uint64_t name[HDF_BITS_TO_LONGS(bits)]
#define HDF_BITS_TO_LONGS(nr) HDF_DIV_ROUND_UP(nr, HDF_BITS_PER_BYTE * sizeof(uint64_t))
#define HDF_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define HDF_BITS_PER_BYTE 8

#define SECMAP_MAX_SIZE 64
#define ID_MAX_SIZE 65

struct SecInfo {
    char secId[ID_MAX_SIZE];
    HDF_DECLARE_BITMAP(secMap, SECMAP_MAX_SIZE);
};
uint32_t hdf_permission_check(uint8_t type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDF_SEC_H */
