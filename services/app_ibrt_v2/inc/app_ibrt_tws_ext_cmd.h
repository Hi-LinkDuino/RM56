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
#ifndef __APP_IBRT_TWS_EXT_CMD_HANDLER_H__
#define __APP_IBRT_TWS_EXT_CMD_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

void app_ibrt_tws_ext_cmd_init(void);

void app_ibrt_send_ext_cmd_a2dp_playing_device(uint8_t curr_playing_device, bool is_response);

#ifdef __cplusplus
}
#endif

#endif /* __APP_IBRT_TWS_EXT_CMD_HANDLER_H__ */

