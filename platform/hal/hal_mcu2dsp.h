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
#ifndef __HAL_MCU2DSP_H__
#define __HAL_MCU2DSP_H__

#ifdef __cplusplus
extern "C" {
#endif

enum HAL_MCU2DSP_ID_T {
    HAL_MCU2DSP_ID_0,
    HAL_MCU2DSP_ID_1,

    HAL_MCU2DSP_ID_QTY
};

typedef unsigned int (*HAL_MCU2DSP_RX_IRQ_HANDLER)(const void *data, unsigned int len);

typedef void (*HAL_MCU2DSP_TX_IRQ_HANDLER)(const void *data, unsigned int len);

int hal_mcu2dsp_open(enum HAL_MCU2DSP_ID_T id, HAL_MCU2DSP_RX_IRQ_HANDLER rxhandler, HAL_MCU2DSP_TX_IRQ_HANDLER txhandler, int rx_flowctrl);

int hal_mcu2dsp_close(enum HAL_MCU2DSP_ID_T id);

int hal_mcu2dsp_start_recv(enum HAL_MCU2DSP_ID_T id);

int hal_mcu2dsp_stop_recv(enum HAL_MCU2DSP_ID_T id);

int hal_mcu2dsp_send_seq(enum HAL_MCU2DSP_ID_T id, const void *data, unsigned int len, unsigned int *seq);

int hal_mcu2dsp_send(enum HAL_MCU2DSP_ID_T id, const void *data, unsigned int len);

void hal_mcu2dsp_rx_done(enum HAL_MCU2DSP_ID_T id);

void hal_mcu2dsp_tx_irq_run(enum HAL_MCU2DSP_ID_T id);

int hal_mcu2dsp_tx_active(enum HAL_MCU2DSP_ID_T id, unsigned int seq);

int hal_mcu2dsp_opened(enum HAL_MCU2DSP_ID_T id);

int hal_mcu2dsp_rx_irq_pending(enum HAL_MCU2DSP_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

