/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef __UART_BES_H__
#define __UART_BES_H__

#include "uart_if.h"
#include "uart_core.h"
#include "hal_uart.h"
#include "kfifo.h"
#include "osal_sem.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UART_DEV_SERVICE_NAME_PREFIX "HDF_PLATFORM_UART%d"
#define MAX_DEV_NAME_SIZE 32

struct UartResource {
    uint32_t num;      /* UART port num */
    uint32_t baudRate; /* Default baudrate */
    uint32_t wLen;     /* Default word length */
    uint32_t parity;   /* Default parity */
    uint32_t stopBit;  /* Default stop bits */
    bool txDMA;
    bool rxDMA;
};

enum UartDeviceState {
    UART_DEVICE_UNINITIALIZED = 0x0u,
    UART_DEVICE_INITIALIZED = 0x1u,
};

struct UART_CTX_OBJ {
    uint8_t *buffer;
    bool txDMA;
    bool rxDMA;
    bool isBlock;
    struct kfifo fifo;
    struct OsalSem rxSem;
    struct OsalSem txSem;
    void (*UartDmaRxHandler)(uint32_t xferSize, int dmaError, union HAL_UART_IRQ_T status);
    void (*UartDmaTxHandler)(uint32_t xferSize, int dmaError);
};

struct UartDevice {
    struct IDeviceIoService ioService;
    struct UartResource resource;
    struct HAL_UART_CFG_T config;
    uint32_t uartId;
    bool initFlag;
    uint32_t transMode;
};

enum {
    UART_READ = 0,
    UART_WRITE
};

int32_t UartDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);

#ifdef __cplusplus
}
#endif

#endif
