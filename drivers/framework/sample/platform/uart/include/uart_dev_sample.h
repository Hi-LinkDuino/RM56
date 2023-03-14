/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef UART_DEV_SAMPLE_H
#define UART_DEV_SAMPLE_H

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

void AddUartDevice(struct UartHost *host);
void RemoveUartDevice(struct UartHost *host);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* UART_DEV_SAMPLE_H */
