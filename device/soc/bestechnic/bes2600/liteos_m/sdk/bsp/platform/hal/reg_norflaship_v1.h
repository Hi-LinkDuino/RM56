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
#ifndef __REG_NORFLASHIP_V1_H__
#define __REG_NORFLASHIP_V1_H__

#include "plat_types.h"

/* ip register */
/* 0x0 */
#define TX_CONFIG1_BASE 0x0
#define TX_ADDR_SHIFT (8)
#define TX_ADDR_MASK ((0xfffffff)<TX_ADDR_SHIFT)
#define TX_CMD_SHIFT (0)
#define TX_CMD_MASK ((0xFF)<TX_CMD_SHIFT)
#define TX_ADDR_RW_SHIFT (16)
#define TX_ADDR_RW_MASK ((0x1)<<TX_ADDR_RW_SHIFT)

#define EXT_CMD_FLAG (0<<0)
#define EXT_CMD_SHIFT (8)
#define EXT_CMD_MASK (0xFF<<EXT_CMD_SHIFT)
#define EXT_CMD_RX_MASK (1<<16)
#define EXT_CMD_QUAD_MASK (1<<17)

/* 0x4 */
#define TX_CONFIG2_BASE 0x4
#define TX_CONMOD_SHIFT (24)
#define TX_CONMOD_MASK ((0x1)<<TX_CONMOD_SHIFT)
#define TX_BLKSIZE_SHIFT (8)
#define TX_BLKSIZE_MASK ((0x1ff)<<TX_BLKSIZE_SHIFT)
#define TX_MODBIT_SHIFT (0)
#define TX_MODBIT_MASK ((0xff)<<TX_MODBIT_SHIFT)

/* 0x8 */
#define TXDATA_BASE 0x8
#define TXDATA_SHIFT (0)
#define TXDATA_MASK ((0xff)<<TXDATA_SHIFT)

/* 0xc */
#define INT_STATUS_BASE 0xc
#define RXFIFOCOUNT_SHIFT (4)
#define RXFIFOCOUNT_MASK ((0xf)<<RXFIFOCOUNT_SHIFT)
#define RXFIFOEMPTY_SHIFT (3)
#define RXFIFOEMPTY_MASK ((0x1)<<RXFIFOEMPTY_SHIFT)
#define TXFIFOFULL_SHIFT (2)
#define TXFIFOFULL_MASK ((0x1)<<TXFIFOFULL_SHIFT)
#define TXFIFOEMPTY_SHIFT (1)
#define TXFIFOEMPTY_MASK ((0x1)<<TXFIFOEMPTY_SHIFT)
#define BUSY_SHIFT (0)
#define BUSY_MASK ((0x1)<<BUSY_SHIFT)

/* 0x10 */
#define RXDATA_BASE 0x10
#define RXDATA_SHIFT (0)
#define RXDATA_MASK ((0xffffffff)<<RXDATA_SHIFT)

/* 0x14 */
#define MODE1_CONFIG_BASE 0x14
#define ADDRBYTE4_SHIFT (29)
#define ADDRBYTE4_MASK ((0x1)<<ADDRBYTE4_SHIFT)
#define RXCANCELEN_SHIFT (28)
#define RXCANCELEN_MASK (0x1<<RXCANCELEN_SHIFT)
#define WRAPTYPE_SHIFT (26)
#define WRAPTYPE_MASK (0x3<<WRAPTYPE_SHIFT)
#define WRAPEN_SHIFT (25)
#define WRAPEN_MASK (0x1<<WRAPEN_SHIFT)
#define FETCHEN_SHIFT (24)
#define FETCHEN_MASK (0x1<<FETCHEN_SHIFT)
#define DUMMYCLC_SHIFT (20)
#define DUMMYCLC_MASK ((0xf)<<DUMMYCLC_SHIFT)
#define DUMMYCLCEN_SHIFT (19)
#define DUMMYCLCEN_MASK ((0x1)<<DUMMYCLCEN_SHIFT)
#define NEG_PHASE_SHIFT (18)
#define NEG_PHASE_MASK ((0x1)<<NEG_PHASE_SHIFT)
#define POS_NEG_SHIFT (17)
#define POS_NEG_MASK ((0x1)<<POS_NEG_SHIFT)
#define CMDQUAD_SHIFT (16)
#define CMDQUAD_MASK ((0x1)<<CMDQUAD_SHIFT)
#define CLKDIV_SHIFT (8)
#define CLKDIV_MASK ((0xff)<<CLKDIV_SHIFT)
#define SAMDLY_SHIFT (4)
#define SAMDLY_MASK ((0x7)<<SAMDLY_SHIFT)
#define DUALMODE_SHIFT (3)
#define DUALMODE_MASK ((0x1)<<DUALMODE_SHIFT)
#define HOLDPIN_SHIFT (2)
#define HOLDPIN_MASK ((0x1)<<HOLDPIN_SHIFT)
#define WPRPIN_SHIFT (1)
#define WPRPIN_MASK ((0x1)<<WPRPIN_SHIFT)
#define QUADMODE_SHIFT (0)
#define QUADMODE_MASK ((0x1)<<QUADMODE_SHIFT)

/* 0x18 */
#define FIFO_CONFIG_BASE 0x18
#define TXFIFOCLR_SHIFT (1)
#define TXFIFOCLR_MASK ((0x1)<<TXFIFOCLR_SHIFT)
#define RXFIFOCLR_SHIFT (0)
#define RXFIFOCLR_MASK ((0x1)<<RXFIFOCLR_SHIFT)

/* 0x20 */
#define MODE2_CONFIG_BASE 0x20
#define DUALIOCMD_SHIFT (24)
#define DUALIOCMD_MASK ((0xff)<<DUALIOCMD_SHIFT)
#define RDCMD_SHIFT (16)
#define RDCMD_MASK ((0xff)<<RDCMD_SHIFT)
#define FRDCMD_SHIFT (8)
#define FRDCMD_MASK ((0xff)<<FRDCMD_SHIFT)
#define QRDCMDBIT_SHIFT (0)
#define QRDCMDBIT_MASK ((0xff)<<QRDCMDBIT_SHIFT)

/* 0x34 */
#define PULL_UP_DOWN_CONFIG_BASE 0x34
#define SPIRUEN_SHIFT (4)
#define SPIRUEN_MASK ((0xf)<<SPIRUEN_SHIFT)
#define SPIRDEN_SHIFT (0)
#define SPIRDEN_MASK ((0xf)<<SPIRDEN_SHIFT)

#endif

