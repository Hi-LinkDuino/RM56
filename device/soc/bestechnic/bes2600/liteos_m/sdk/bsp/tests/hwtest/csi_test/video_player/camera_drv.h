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
#ifndef __CAMERA_DRV_H__
#define __CAMERA_DRV_H__

#ifdef __cpulsplus
extern "C" {
#endif

#include "video_player.h"

//////////////////////// CAMERA DEVICE DRIVER ////////////////////
struct VP_DEVICE_OPS_T *camera_device_setup(enum VP_DEVICE_T dev, struct VP_DEVICE_CONFIG_T *cfg);

void camera_device_power_on(void);

void camera_device_power_off(void);

int camera_device_probe(void);

int camera_device_open(void);

int camera_device_start(void);

int camera_device_stop(void);

int camera_device_close(void);

int camera_device_reset(void);

int camera_device_bus_write(uint32_t reg_addr, uint32_t val);

int camera_device_bus_read(uint32_t reg_addr, uint32_t *val);

void camera_device_test_mode(bool enable, uint8_t value);

void camera_device_test_mode_set_value(uint8_t value);

//////////////////////// CAMERA DATA DRIVER ////////////////////
void camera_data_dump_trap(void);

int camera_data_check_error(uint8_t *buf, uint32_t len);

void camera_data_dump(uint8_t *buf, uint32_t len, uint32_t pix_bits);

void camera_data_dump2(uint8_t *buf, uint32_t len, uint32_t pix_bits);

int camera_data_raw10_to_yuv422(uint32_t dst, uint32_t src, uint32_t size);

int camera_data_memcpy(uint8_t *dst, uint8_t *src, uint32_t size);

int camera_data_memcpy_dma(uint8_t *dst, uint8_t *src, uint32_t size);

int camera_data_clean(uint8_t *src, uint32_t size);

#ifdef __cpulsplus
}
#endif

#endif
