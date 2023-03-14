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
#ifndef __HAL_CSI_CONFIG_H__
#define __HAL_CSI_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GC2145MIPI_2LANE

#define CSI_PHY_CK_RANGE_VALUE (CSI_PHY_CLK_RANGE_200MHZ)
#define CSI_PHY_CK_DELAY_VALUE (3)
#define CSI_PHY_D0_DELAY_VALUE (0)
#define CSI_PHY_D1_DELAY_VALUE (0)

//#define CSI_PHY_NO_CALIB

#define CSI_PHY_CALIB_SKIP_CNT (1)

#define CSI_PHY_D0_CALIB_START (0x00)
#define CSI_PHY_D0_CALIB_END   (0x0F)
#define CSI_PHY_D0_CALIB_COEF  (0x4)
//#define CSI_PHY_D0_CALIB_USE_FIRST_VALUE
#define CSI_PHY_D0_CALIB_CHECK_ERR (CSI_R_INVALID_WC | CSI_R_DTYPE_ERR | CSI_R_MULTI_ECC_ERR)

#ifdef NOAPP
#define CSI_PRPR_TIME 0x08
#else
#define CSI_PRPR_TIME 0x15
//#define CSI_PRPR_TIME 0x28
#endif
#define CSI_PWR_INI_DLY  0xC0
#define CSI_R_HDLY_0     0x1A0
#define CSI_R_HDLY_1     0x30F

#define CSI_PIX_CLK_DIV 0x3

#ifdef __cplusplus
}
#endif

#endif
