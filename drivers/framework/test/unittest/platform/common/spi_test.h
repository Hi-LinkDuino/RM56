/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SPI_TEST_H
#define SPI_TEST_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum SpiTestCmd {
    SPI_TRANSFER_TEST = 0,
    SPI_MULTI_TRANSFER_TEST,
    SPI_DMA_TRANSFER_TEST,
    SPI_INT_TRANSFER_TEST,
    SPI_RELIABILITY_TEST,
    SPI_PERFORMANCE_TEST,
    SPI_TEST_ALL,
    SPI_TEST_CMD_MAX,
};

struct SpiTestConfig {
    uint32_t bus;
    uint32_t cs;
    uint32_t len;
    uint8_t *wbuf;
    uint8_t *rbuf;
    uint32_t testDma;
};

struct SpiTester {
    struct SpiTestConfig config;
    DevHandle handle;
    uint16_t total;
    uint16_t fails;
};

int32_t SpiTestExecute(int cmd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SPI_TEST_H */
