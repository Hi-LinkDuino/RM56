/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UART_DEV_H
#define UART_DEV_H

#include "console.h"
#include "sys/ioctl.h"
#include "uart_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define UART_IOC_MAGIC   'u'

/* baudrate config */
#define UART_CFG_BAUDRATE _IO(UART_IOC_MAGIC, 1)

/* DMA CONFIG: receive */
#define UART_DMA_RX_EN    1
#define UART_DMA_RX_DIS   0
#define UART_CFG_DMA_RX   _IO(UART_IOC_MAGIC, 2)

/* DMA CONFIG: send */
#define UART_DMA_TX_EN    1
#define UART_DMA_TX_DIS   0
#define UART_CFG_DMA_TX   _IO(UART_IOC_MAGIC, 3)

/* Read Block: */
#define UART_RD_BLOCK     1
#define UART_RD_NONBLOCK  0
#define UART_CFG_RD_BLOCK CONSOLE_CMD_RD_BLOCK_SERIAL

/* attribute CONFIG */
#define UART_CFG_ATTR     _IOW(UART_IOC_MAGIC, 5, int32_t)

/* Private CONFIG */
#define UART_CFG_PRIVATE  _IOW(UART_IOC_MAGIC, 6, int32_t)

void UartAddDev(struct UartHost *host);
void UartRemoveDev(struct UartHost *host);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* UART_DEV_H */
