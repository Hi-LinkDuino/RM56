/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ADC_IF_H
#define ADC_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct AdcIoMsg {
    uint32_t number;
    uint32_t channel;
};

DevHandle AdcOpen(uint32_t number);

void AdcClose(DevHandle handle);

int32_t AdcRead(DevHandle handle, uint32_t channel, uint32_t *val);

/**
 * @brief Enumerates ADC I/O commands.
 *
 * @since 1.0
 */
enum AdcIoCmd {
    ADC_IO_READ = 0,    /**< Read the A/D data. */
    ADC_IO_OPEN,        /**< Open the ADC device. */
    ADC_IO_CLOSE,       /**< Close the ADC device. */
};
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* ADC_IF_H */
