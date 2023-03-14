/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef UART_TEST_H
#define UART_TEST_H

#include "uart_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t UartTestExecute(int cmd);

enum UartTestCmd {
    UART_TEST_CMD_WRITE = 0,
    UART_TEST_CMD_READ = 1,
    UART_TEST_CMD_SET_BAUD = 2,
    UART_TEST_CMD_GET_BAUD = 3,
    UART_TEST_CMD_SET_ATTRIBUTE = 4,
    UART_TEST_CMD_GET_ATTRIBUTE = 5,
    UART_TEST_CMD_SET_TRANSMODE = 6,
    UART_TEST_CMD_RELIABILITY = 7,
    UART_TEST_CMD_PERFORMANCE = 8,
    UART_TEST_CMD_MAX = 9,
};

struct UartTestConfig {
    uint32_t port;
    uint32_t len;
    uint8_t *wbuf;
    uint8_t *rbuf;
};

struct UartTester {
    struct UartTestConfig config;
    DevHandle handle;
    uint16_t total;
    uint16_t fails;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UART_TEST_H */
