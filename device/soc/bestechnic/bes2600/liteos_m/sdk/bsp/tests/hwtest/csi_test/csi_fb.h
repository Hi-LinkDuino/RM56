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
#ifndef __CSI_FB_H__
#define __CSI_FB_H__

#ifdef __cplusplus
extern "C" {
#endif

enum DSI_FB_CH_T {
    DSI_FB_CH0,
    DSI_FB_CH1,
};

struct CSI_DSI_FB_CNTL_T {
    uint32_t enable : 1;
    uint32_t flush  : 1;
    uint32_t lock   : 1;
    uint32_t updata : 1;
    uint32_t upaddr : 1;
    uint32_t rsvd   : 27;

    uint32_t addr;
    uint32_t len;
    uint32_t err;
    uint32_t cnt;
};

// DSI FB driver
void dsi_fb_init(enum DSI_FB_CH_T chan);

void dsi_fb_enable(enum DSI_FB_CH_T chan);

void dsi_fb_disable(enum DSI_FB_CH_T chan);

int dsi_fb_callback(uint8_t layer, uint8_t chan, uint32_t addr);

struct CSI_DSI_FB_CNTL_T *dsi_fb_get(enum DSI_FB_CH_T chan);

// CSI FB driver
void csi_fb_init(void);

void csi_fb_enable(void);

void csi_fb_disable(void);

int csi_fb_callback(uint8_t *buf, uint32_t len, uint32_t errcode);

struct CSI_DSI_FB_CNTL_T *csi_fb_get(void);

uint32_t csi_fb_get_addr(uint32_t *plen);

int csi_fb_flush_status(void);

#ifdef __cplusplus
}
#endif

#endif
