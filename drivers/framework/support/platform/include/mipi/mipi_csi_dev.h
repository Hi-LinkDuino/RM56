/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MIPI_CSI_DEV_H
#define MIPI_CSI_DEV_H

#include "hdf_base.h"
#include "mipi_csi_if.h"

typedef enum {
    WORK_MODE_LVDS = 0x0,
    WORK_MODE_MIPI = 0x1,
    WORK_MODE_CMOS = 0x2,
    WORK_MODE_BT1120 = 0x3,
    WORK_MODE_SLVS = 0x4,
    WORK_MODE_BUTT
} WorkMode;

typedef struct {
    uint8_t devno;
    PhyCmvMode cmvMode;
} PhyCmv;

#define CONFIG_HI_PROC_SHOW_SUPPORT

#define HI_MIPI_IOC_MAGIC            'm'

/* init data lane, input mode, data type */
#define HI_MIPI_SET_DEV_ATTR         _IOW(HI_MIPI_IOC_MAGIC, 0x01, ComboDevAttr)

/* set phy common mode voltage mode */
#define HI_MIPI_SET_PHY_CMVMODE      _IOW(HI_MIPI_IOC_MAGIC, 0x04, PhyCmv)

/* reset sensor */
#define HI_MIPI_RESET_SENSOR         _IOW(HI_MIPI_IOC_MAGIC, 0x05, uint8_t)

/* unreset sensor */
#define HI_MIPI_UNRESET_SENSOR       _IOW(HI_MIPI_IOC_MAGIC, 0x06, uint8_t)

/* reset mipi */
#define HI_MIPI_RESET_MIPI           _IOW(HI_MIPI_IOC_MAGIC, 0x07, uint8_t)

/* unreset mipi */
#define HI_MIPI_UNRESET_MIPI         _IOW(HI_MIPI_IOC_MAGIC, 0x08, uint8_t)

/* reset slvs */
#define HI_MIPI_RESET_SLVS           _IOW(HI_MIPI_IOC_MAGIC, 0x09, uint8_t)

/* unreset slvs */
#define HI_MIPI_UNRESET_SLVS         _IOW(HI_MIPI_IOC_MAGIC, 0x0a, uint8_t)

/* set mipi hs_mode */
#define HI_MIPI_SET_HS_MODE          _IOW(HI_MIPI_IOC_MAGIC, 0x0b, LaneDivideMode)

/* enable mipi clock */
#define HI_MIPI_ENABLE_MIPI_CLOCK    _IOW(HI_MIPI_IOC_MAGIC, 0x0c, uint8_t)

/* disable mipi clock */
#define HI_MIPI_DISABLE_MIPI_CLOCK   _IOW(HI_MIPI_IOC_MAGIC, 0x0d, uint8_t)

/* enable slvs clock */
#define HI_MIPI_ENABLE_SLVS_CLOCK    _IOW(HI_MIPI_IOC_MAGIC, 0x0e, uint8_t)

/* disable slvs clock */
#define HI_MIPI_DISABLE_SLVS_CLOCK   _IOW(HI_MIPI_IOC_MAGIC, 0x0f, uint8_t)

/* enable sensor clock */
#define HI_MIPI_ENABLE_SENSOR_CLOCK  _IOW(HI_MIPI_IOC_MAGIC, 0x10, uint8_t)

/* disable sensor clock */
#define HI_MIPI_DISABLE_SENSOR_CLOCK _IOW(HI_MIPI_IOC_MAGIC, 0x11, uint8_t)

#define HI_MIPI_SET_EXT_DATA_TYPE    _IOW(HI_MIPI_IOC_MAGIC, 0x12, ExtDataType)

int MipiCsiDevModuleInit(uint8_t id);
void MipiCsiDevModuleExit(uint8_t id);

#endif /* MIPI_CSI_DEV_H */
