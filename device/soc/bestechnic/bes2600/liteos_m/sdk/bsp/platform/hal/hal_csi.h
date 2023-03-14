/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __HAL_CSI_H__
#define __HAL_CSI_H__

#ifdef __cplusplus
extern "C" {
#endif

enum CSI_PIX_FMT_T {
    CSI_PIX_FMT_RGB565      = 0,
    CSI_PIX_FMT_RGB888      = 1,
    CSI_PIX_FMT_YUV422      = 2,
    CSI_PIX_FMT_RGBA888     = 3,
};

enum CSI_DATA_LANE_NUM_T {
    CSI_DATA_LANE_NUM_1 = 0,
    CSI_DATA_LANE_NUM_2,
    CSI_DATA_LANE_NUM_3,
    CSI_DATA_LANE_NUM_4,
};

typedef uint32_t (*HAL_CSI_STREAM_HANDLER_T)(uint8_t *buf, uint32_t len, uint32_t errcode);

typedef void (*HAL_CSI_CALIB_WAIT_CALLBACK_T)(void);

struct HAL_CSI_CONFIG_T {
    enum CSI_PIX_FMT_T fmt;
    enum CSI_DATA_LANE_NUM_T data_lane_num;
    uint32_t width;
    uint32_t height;
    uint32_t pp_mode: 1; //ping pong buffer mode;

    uint8_t *data_ptr;
    uint32_t data_size;
    HAL_CSI_STREAM_HANDLER_T handler;
};

#define CSI_CALIB_CFG_FLAG_DONE   (1<<0)

struct HAL_CSI_CALIB_CONFIG_T {
    uint8_t flag;
    uint8_t clock_range;
    uint16_t clock_delay;
    uint16_t data_delay[2];
    uint16_t pix_clk_div;
    uint16_t pwr_ini_dly;
    uint16_t prpr_time;
    uint16_t r_hdly[2];
};

void hal_csi_open(struct HAL_CSI_CONFIG_T *cfg);

void hal_csi_close(void);

void hal_csi_start(void);

void hal_csi_stop(void);

void hal_csi_restart(void);

int hal_csi_start_calib(bool force_calib, bool gpio_trigger);

bool hal_csi_calib_status(void);

int hal_csi_trigger_calib_process(int param);

int hal_csi_set_calib_value(struct HAL_CSI_CALIB_CONFIG_T *cfg);

int hal_csi_get_calib_value(struct HAL_CSI_CALIB_CONFIG_T *cfg);

void hal_csi_setup_calib_wait_callback(HAL_CSI_CALIB_WAIT_CALLBACK_T callback);

#ifdef __cplusplus
}
#endif

#endif

