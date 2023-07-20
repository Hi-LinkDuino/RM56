/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __VPLAYER_APP_COMMON_H__
#define __VPLAYER_APP_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

enum VPLAYER_APP_MODE_T {
    VPLAYER_APP_MODE_NONE,
    VPLAYER_APP_MODE_CSI,
    VPLAYER_APP_MODE_DSI,
    VPLAYER_APP_MODE_CSI_DSI_LOOP,
};

#define REMAP_C_ADDR(addr)   (((uint32_t)addr & (~0xFF000000)) | PSRAMUHS_BASE)
#define REMAP_NC_ADDR(addr)  (((uint32_t)addr & (~0xFF000000)) | PSRAMUHS_NC_BASE)
#define REMAP_DMA_ADDR(addr) REMAP_NC_ADDR(addr)

#ifndef ALIGNED4
#define ALIGNED4 ALIGNED(4)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VPLAYER_APP_COMMON_H__ */
