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
#ifndef __APP_IR_TEST_H__
#define __APP_IR_TEST_H__

#include "hal_timer.h"
#include "hal_gpio.h"
#include "app_ir.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IR_CMD_MAXARGS      32


typedef  struct ir_tbl_s{
    char *name;
    uint8_t mode;
    //int (*fun)(uint8_t ,uint8_t ,void *,uint8_t);
    int (*fun)(struct ir_tbl_t *,int ,char **);
    char *usage;
}ir_tbl_t;

typedef  struct ir_cmd_s{
    uint8_t id;
    uint16_t *cmd;
    char *usage;
}ir_cmd_t;


/***************************************************************
 * Function     : app_ir_test
 * Description  : app_ir_test
 * Input        : None
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_nec_test(void);

/***************************************************************
 * Function     : app_ir_send
 * Description  : app_ir_send
 * Input        : None
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_send(int argc,char *argv[]);



/***************************************************************
 * Function     : app_ir_parse
 * Description  : app_ir_parse
 * Input        : None
 * Output       : None
 * Return       : 0:success;others:fail
****************************************************************/
int app_ir_parse(int argc, char *argv[]);


#ifdef __cplusplus
}
#endif

#endif
