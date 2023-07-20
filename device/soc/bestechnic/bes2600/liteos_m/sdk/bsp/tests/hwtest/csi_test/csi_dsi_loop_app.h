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
#ifndef __CSI_DSI_LOOP_APP_H__
#define __CSI_DSI_LOOP_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

void csi_dsi_loop_app_init(void);

void csi_dsi_loop_app_on(bool on);

void csi_dsi_loop_app_run(void);

void csi_dsi_loop_app_rx_cmd_handler(char *data, uint32_t len);

void csi_dsi_loop_app_tx_cmd_handler(char *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
