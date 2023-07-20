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
#ifndef __CSI_HOST_APP_H__
#define __CSI_HOST_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////// CSI UART CMD /////////////////////
//CSI TEST OFF      : AT+CSI=0
//CSI TEST ON       : AT+CSI=1
//CSI TEST I2C READ : AT+CSI=2,0xf1,1
//CSI TEST I2C WRITE: AT+CSI=3,0xfe,0x00
//CSI TEST REBOOT   : AT+CSI=4
//CSI TEST TEST ON  : AT+CSI=5
//CSI TEST DUMP DATA: AT+CSI=6, [bytes]
//CSI TEST SET PAGE : AT+CSI=7, [bytes]
//CSI TEST TRACE_ON : AT+CSI=30
//CSI TEST TRACE OFF: AT+CSI=31
///////////////////////////////////////////////////////////

enum CSI_TEST_CMD_T {
    // csi host app commands
    CSI_TEST_CMD_OFF       = 0,
    CSI_TEST_CMD_ON        = 1,
    CSI_TEST_CMD_I2C_READ  = 2,
    CSI_TEST_CMD_I2C_WRITE = 3,
    CSI_TEST_CMD_REBOOT    = 4,
    CSI_TEST_CMD_TEST_PATT = 5,
    CSI_TEST_CMD_DUMP_DATA = 6,
    CSI_TEST_CMD_SET_PAGE  = 7,
    CSI_TEST_CMD_GET_CHIP_ID = 8,

    //csi dsi loop commands
    CSI_TEST_CMD_LOOP_APP_ON   = 11, //enable csi_dsi_loop_app
    CSI_TEST_CMD_LOOP_APP_OFF  = 12, //disable csi_dsdi_loop_app

    CSI_TEST_CMD_TRACE_ON  = 30,
    CSI_TEST_CMD_TRACE_OFF = 31,
    CSI_TEST_CMD_I2C_SLAVE = 32,
    CSI_TEST_CMD_RD_MEM = 33,
    CSI_TEST_CMD_WR_MEM = 34,
    CSI_TEST_CMD_POLL_REG_ON = 35,
    CSI_TEST_CMD_POLL_REG_OFF = 36,
};

void csi_host_app_init(void);

void csi_host_app_on(bool on);

void csi_host_app_restart(void);

int  csi_host_app_cmd_handler(int cmd, int argc, int arg[]);

void csi_host_app_cmd_help(void);

bool csi_host_app_is_calibed(void);

#ifdef __cplusplus
}
#endif

#endif
