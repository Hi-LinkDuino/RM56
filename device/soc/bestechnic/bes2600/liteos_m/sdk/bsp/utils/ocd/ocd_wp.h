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

#ifndef __BES_WATCH_POINT__
#define __BES_WATCH_POINT__

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

/*armv8m*/
#define DWT_CTRL       0xE0001000  /*DWT Control Register*/
#define DWT_CYCCNT     0xE0001004  /*DWT Cycle Count Register*/
#define DWT_COMP0      0xE0001020  /*DWT Comparator Register n*/
#define DWT_FUNCTION0  0xE0001028  /*DWT Comparator Function Register n*/
#define DWT_VMASK0     0xE000102C  /*DWT Comparator Value Mask Register n*/
#define DCB_DEMCR      0xE000EDFC  /*Debug Exception and Monitor Control Register*/
#define DWT_DEVARCH    0xE0001FBC  /*DWT Device Architecture Register*/


/* DCB_DEMCR bit and field definitions */
#define TRCENA          BIT(24)
#define MON_EN          BIT(16)
#define VC_HARDERR      BIT(10)
#define VC_INTERR       BIT(9)
#define VC_BUSERR       BIT(8)
#define VC_STATERR      BIT(7)
#define VC_CHKERR       BIT(6)
#define VC_NOCPERR      BIT(5)
#define VC_MMERR        BIT(4)
#define VC_CORERESET    BIT(0)

#define DWT_OK               (0)
#define ERROR_FAIL           (-2)
#define DWT_LEN_ERR          (-307) 
#define DWT_ADDR_UNALIGNED   (-308)
#define DWT_PARA_ERR         (-309)

#ifdef __cplusplus
extern "C" {
#endif

enum watchpoint_rw {
    DWT_READ,     /*trigger dwt exception when read*/
    DWT_WRITE,    /*trigger dwt exception when write*/
    DWT_ACCESS,   /*trigger dwt exception when read or write*/
    DWT_MODE_OTHER
};

struct watchpoint {
    uint32_t address;
    uint32_t length;
    enum watchpoint_rw rw;
};

void ocd_watchpoint_setup(void);
int ocd_watchpoint_show(void);
int ocd_add_watch_point(uint32_t address, enum watchpoint_rw rw);
int ocd_remove_watch_point(uint32_t  address, enum watchpoint_rw rw);

#ifdef __cplusplus
}
#endif

#endif
