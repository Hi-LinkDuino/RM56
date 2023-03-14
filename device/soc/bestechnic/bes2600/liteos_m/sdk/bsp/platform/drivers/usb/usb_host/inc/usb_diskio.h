/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __USB_DISKIO__
#define __USB_DISKIO__

#ifdef __cplusplus
extern "C" {
#endif

#include "diskio.h"

DSTATUS usb_disk_status();
DSTATUS usb_disk_initialize();
DRESULT usb_disk_write(const BYTE* buffer, LBA_t block_number, UINT count);
DRESULT usb_disk_read(BYTE* buffer, LBA_t block_number, UINT count);
int     usb_disk_sync();
LBA_t   usb_disk_get_sectors();
DRESULT usb_disk_ioctl(u8 ctrl, void *buff);

#ifdef __cplusplus
}
#endif

#endif
