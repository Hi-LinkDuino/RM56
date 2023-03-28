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
#include "a7_cmd.h"
#include "hal_trace.h"

static void a7_cmd_receive(char *data, int len)
{
    TRACE(0, "m33 get %s",data);
}

int bes_a7_cmd_test( void )
{
#ifdef __A7_DSP_ENABLE__
    set_a7_cmd_callback_handler(a7_cmd_receive);
#endif
    return 0;
}

int bes_m33_cmd_test ( void )
{
#ifdef __A7_DSP_ENABLE__
    A7_CMD_T a7_cmd;
    a7_cmd.type = A7_CMD_TYPE_CUST_CTRL_PLANE;
    a7_cmd.p1 = 1234;
    a7_cmd.p2 = 5678;
    mcu_cmd_send(&a7_cmd);
#endif
    return 0;
}