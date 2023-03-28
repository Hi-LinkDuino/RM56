/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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

#ifndef USB_MASS_STORAGE_H
#define USB_MASS_STORAGE_H

#include "plat_types.h"

#define USB_MASS_STORAGE_BLOCK_SIZE  512

bool get_usb_disk_init_status();

int32_t usb_mass_storage_init(int32_t device);

int32_t usb_mass_storage_get_capacity(int32_t device,
                                      uint32_t* block_count,
                                      uint32_t* blockSize);

int32_t usb_mass_storage_read(int32_t device,
                              uint32_t block_addr,
                              uint32_t block_count,
                              uint8_t* dst,
                              u32 blockSize);

int32_t usb_mass_storage_write(int32_t device,
                               uint32_t block_addr,
                               uint32_t block_count,
                               uint8_t* dst,
                               u32 blockSize);

void usb_mass_storage_set_read_ep(uint8_t ep);

void usb_mass_storage_set_write_ep(uint8_t ep);

void usb_mass_storage_set_itf_num(uint8_t num);

#endif // USB_MASS_STORAGE_H
