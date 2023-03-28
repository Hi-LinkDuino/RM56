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

#ifndef __WIFI_DEF_BEST2003_H__
#define __WIFI_DEF_BEST2003_H__

#ifdef __cplusplus
extern "C" {
#endif

//add 2021-4-2 for use tx power calib data
#define USE_CALIB_DATA              1

#define USE_TX_DIG_GAIN_CFG         0

#define FORMULA_POWERLEVEL_OFFSET   1

#if USE_TX_DIG_GAIN_CFG
#define RATE_POWER_MAP_2G      (0<<0 | 8<<4 | 8<<8 | 8<<12 | 8<<16 | 4<<20 | 4<<24 | 0<<28);
#define RATE_POWER_MAP_5G      (0<<0 | 8<<4 | 8<<8 | 8<<12 | 8<<16 | 4<<20 | 4<<24 | 0<<28);
#endif


#ifdef __cplusplus
}
#endif

#endif /* WIFI_DEF_BEST2003_H */
