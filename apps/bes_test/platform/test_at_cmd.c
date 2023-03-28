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
#include "console.h"
#include "hal_trace.h"
#include "test_i2c_case.h"
#include "stdio.h"
#include "stdlib.h"
#include "bes_test_cases.h"
/* input AT+CUST=B","E","S" to verify */

static int do_cust_cmd( cmd_tbl_t *cmd, int argc, char *argv[])
{

    //TRACE(0, "%s maxargs:%d cmd:%p usage:%s",cmd->name,cmd->maxargs,cmd->cmd,cmd->usage);
    int casedo = atoi(argv[1]);
    
    printf("Will run case %d \r\n", casedo);
    test_case_event_handle(0, casedo);
    printf("RESP OK\n");
    return 0;

}

static void add_cust_cmd()
{
    int i, j;
    cmd_tbl_t cmd_list[] = {
        /*Demo CMD  */
        {
            "AT+CUST",          4,   do_cust_cmd,
            "AT+CUST            - exec custom at cmd demo\n"
        },
        {
            "AT+I2CTEST",       12,   do_I2C_cmd,
            "AT+I2CTEST         - exec custom I2C at cmd demo\n"
        },
        /* Add your CMDS  */


    };
#ifndef USE_EXT_CONSOLE
    j = sizeof(cmd_list) / sizeof(cmd_tbl_t);
    for (i = 0; i < j; i ++) {
        if (0 != console_cmd_add(&cmd_list[i])) {
            printf("Add cmd failed\r\n");
        }
    }
#endif
}

int bes_at_cmd_test()
{
    add_cust_cmd();
    return 0;
}