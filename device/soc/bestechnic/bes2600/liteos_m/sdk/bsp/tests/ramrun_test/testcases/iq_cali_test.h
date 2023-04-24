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
#ifndef __iq_cali_test_H__
#define __iq_cali_test_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_trace.h"
#include "hal_uart.h"
#include <stdint.h>

#ifdef MTEST_NO_TRACE
#define MT_TRACE(n, str, ...)   TRACE_DUMMY(n, str, ##__VA_ARGS__)
#else
#define MT_TRACE(n, str, ...)   hal_uart_printf(str, ##__VA_ARGS__)
#endif

#if (IQ_CALI_TEST > 0)

void efuse_write_2300ZA(uint8_t page, uint16_t val);
int iq_cali_test(void);

#endif

#ifdef __cplusplus
}
#endif

#endif

