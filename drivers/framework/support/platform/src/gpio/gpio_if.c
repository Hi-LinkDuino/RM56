/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_if.h"
#include "devsvc_manager_clnt.h"
#include "gpio/gpio_core.h"

#include "hdf_base.h"

#define HDF_LOG_TAG gpio_if

int32_t GpioRead(uint16_t gpio, uint16_t *val)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrRead(cntlr, GpioCntlrGetLocal(cntlr, gpio), val);

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioWrite(uint16_t gpio, uint16_t val)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrWrite(cntlr, GpioCntlrGetLocal(cntlr, gpio), val);

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioSetDir(uint16_t gpio, uint16_t dir)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrSetDir(cntlr, GpioCntlrGetLocal(cntlr, gpio), dir);

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioGetDir(uint16_t gpio, uint16_t *dir)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrGetDir(cntlr, GpioCntlrGetLocal(cntlr, gpio), dir);

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioSetIrq(uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrSetIrq(cntlr, GpioCntlrGetLocal(cntlr, gpio), mode, func, arg);

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioUnsetIrq(uint16_t gpio, void *arg)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrUnsetIrq(cntlr, GpioCntlrGetLocal(cntlr, gpio), arg);

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioEnableIrq(uint16_t gpio)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrEnableIrq(cntlr, GpioCntlrGetLocal(cntlr, gpio));

    GpioCntlrPut(cntlr);
    return ret;
}

int32_t GpioDisableIrq(uint16_t gpio)
{
    int32_t ret;
    struct GpioCntlr *cntlr = GpioCntlrGetByGpio(gpio);

    ret = GpioCntlrDisableIrq(cntlr, GpioCntlrGetLocal(cntlr, gpio));

    GpioCntlrPut(cntlr);
    return ret;
}
