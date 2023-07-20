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
#ifndef __CSI_HOST_H__
#define __CSI_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*CSI_HOST_IRQ_NOTIFY_T)(uint32_t error);

int csi_host_open(void);

int csi_host_start(void);

int csi_host_stop(void);

int csi_host_close(void);

int csi_host_calib_start(void);

bool csi_host_status_get(void);

void csi_host_status_set(bool status);

int csi_host_cmd_set_page(uint32_t page);

int csi_host_cmd_access_mem(int wr, uint32_t arg0, uint32_t arg1);

int csi_host_cmd_poll_reg(int on, uint32_t addr);

int csi_host_cmd_dump_data(uint32_t bytes, uint32_t bits);

int csi_host_cmd_trace_quiet(bool quiet);

int csi_host_cmd_test_patt_enable(bool enable);

int csi_key_event_process(uint32_t key_code, uint8_t key_event);

void csi_host_state_notify(int state);

void csi_debug_cfg(void);

void csi_host_setup_csi_irq_notify(CSI_HOST_IRQ_NOTIFY_T notify);

void csi_host_setup_dsi_irq_notify(CSI_HOST_IRQ_NOTIFY_T notify);

#ifdef __cplusplus
}
#endif

#endif
