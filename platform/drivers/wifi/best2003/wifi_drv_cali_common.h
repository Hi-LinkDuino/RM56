/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#ifndef __WIFI_DRV_COMMON_H__
#define __WIFI_DRV_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "plat_addr_map.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_analogif.h"

#include "patch.h"
#include "wifi_rf_test_def.h"


/*********************** macro **********************/

//cali_flag_one
#define  WIFI_CALI_DONE_2G                   (1 << 0)
#define  WIFI_CALI_DONE_5G                   (1 << 1)
#define  WIFI_CALI_RC_DONE                   (1 << 2)
// #define  WIFI_REPEAT_CALI_DISABLE            (1 << 3)
#define  WIFI_USE_DC_CALI_FLASH_DATA         (1 << 4)

/*********************** enum **********************/
enum ENUM_WIFI_TRANSMIT_DIR
{
    WIFI_TRANSMIT_RAM2FLASH = 0, //trasmission direction: ram to flash
    WIFI_TRANSMIT_FLASH2RAM,     //trasmission direction: flash to ram
    WIFI_TRANSMIT_MAX
};


/*********************** struct **********************/

typedef struct WIFI_CALI_DATA_S
{
    uint32 cali_flag_one;
    uint32 cali_flag_two;//reserve

    uint16 dac_i_way_result[11][2];//11:reg num; 2(0:reg; 1:reg value)
    uint16 dac_q_way_result[11][2];

}WIFI_CALI_DATA;



/********************* interface ***********************/


void wifi_set_repeat_cali_enable(bool enable);
bool wifi_get_repeat_cali_enable_status(void);

void dac_cali_write_result(void);
void wifi_rcosc_transmt_res(void);

int wifi_reset_skip_cali(void);
void wifi_cali(uint8 band);



#ifdef __cplusplus
}
#endif

#endif

