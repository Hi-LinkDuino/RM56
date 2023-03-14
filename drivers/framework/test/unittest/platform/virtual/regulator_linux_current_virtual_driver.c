/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regmap.h>
#include <linux/sysfs.h>
#include "hdf_log.h"
#include "hdf_base.h"

#define HDF_LOG_TAG regulator_virtual_current

#define MINIMUN_CURRENT            1000
#define MAXIMUN_CURRENT            50000

struct VirtualCurrentRegulatorDev {
    struct regmap *regmap;
    struct regulator_dev *dev;
};

// note:linux kernel constraints:len(devName) + len(supplyName) < REG_STR_SIZE(64)
static struct regulator_consumer_supply g_virtualCurrentRegulatorSupplies[] = {
    REGULATOR_SUPPLY("vir-current-reg-hdf-adp", "regulator_adapter_consumer01"),
};

// virtual regulator init info
static struct regulator_init_data g_virtualCurrentRegulatorInitData = {
    .constraints = {
        .name = "virtual_current_regulator",
        .min_uA = MINIMUN_CURRENT,
        .max_uA = MAXIMUN_CURRENT,
        .valid_ops_mask = REGULATOR_CHANGE_CURRENT | REGULATOR_CHANGE_STATUS,
    },
    .num_consumer_supplies = ARRAY_SIZE(g_virtualCurrentRegulatorSupplies),
    .consumer_supplies = g_virtualCurrentRegulatorSupplies,
};

static void VirtualCurrentRegulatorDevRelease(struct device *dev)
{
}

static struct platform_device g_virtualCurrentRegulatorPlatformDevice = {
    .name = "virtual_current_regulator_dev",
    .id = -1,
    .dev = {
        .release = VirtualCurrentRegulatorDevRelease,
    }
};

enum RegulatorStatus {
    VIR_REGULATOR_STATUS_OFF,
    VIR_REGULATOR_STATUS_ON,
};

static int g_virStatus = VIR_REGULATOR_STATUS_OFF;
static int VirtualCurrentRegulatorEnable(struct regulator_dev *rdev)
{
    if (rdev == NULL) {
        HDF_LOGE("%s: rdev NULL", __func__);
        return HDF_FAILURE;
    }

    g_virStatus = VIR_REGULATOR_STATUS_ON;
    return HDF_SUCCESS;
}

static int VirtualCurrentRegulatorDisable(struct regulator_dev *rdev)
{
    if (rdev == NULL) {
        HDF_LOGE("%s: rdev NULL", __func__);
        return HDF_FAILURE;
    }

    g_virStatus = VIR_REGULATOR_STATUS_OFF;
    return HDF_SUCCESS;
}

static int VirtualCurrentRegulatorIsEnabled(struct regulator_dev *rdev)
{
    if (rdev == NULL) {
        HDF_LOGE("%s: rdev NULL", __func__);
        return HDF_FAILURE;
    }

    return g_virStatus;
}

static int VirtualCurrentRegulatorSetCurrent(struct regulator_dev *rdev, int min_uA,
    int max_uA)
{
    if ((rdev == NULL) || (rdev->constraints == NULL)) {
        HDF_LOGE("%s: rdev NULL", __func__);
        return HDF_FAILURE;
    }

    struct regulation_constraints *regu_constraints = rdev->constraints;
    if (regu_constraints->min_uA == min_uA &&
        regu_constraints->max_uA == max_uA) {
        return HDF_SUCCESS;
    }

    return HDF_SUCCESS;
}

#define VIRTUAL_CURRENT_VAL_500 500
static int VirtualCurrentRegulatorGetCurrent(struct regulator_dev *rdev)
{
    if (rdev == NULL) {
        HDF_LOGE("%s: rdev NULL", __func__);
        return HDF_FAILURE;
    }

    return VIRTUAL_CURRENT_VAL_500;
}

static struct regulator_ops g_virtualCurrentRegulatorOps = {
    .enable = VirtualCurrentRegulatorEnable,
    .disable = VirtualCurrentRegulatorDisable,
    .is_enabled = VirtualCurrentRegulatorIsEnabled,
    .set_current_limit = VirtualCurrentRegulatorSetCurrent,
    .get_current_limit = VirtualCurrentRegulatorGetCurrent,
};

static struct regulator_desc g_virtualCurrentRegulatorDesc = {
    .name = "regulator_virtual_current",
    .type = REGULATOR_CURRENT,
    .ops = &g_virtualCurrentRegulatorOps,
    .owner = THIS_MODULE,
};

static int VirtualCurrentRegulatorPlatformProbe(struct platform_device *platform_dev)
{
    if (platform_dev == NULL) {
        HDF_LOGE("%s: platform_dev null!", __func__);
        return HDF_FAILURE;
    }
    struct VirtualCurrentRegulatorDev *data;
    struct regulator_config config;

    memset(&config, 0, sizeof(struct regulator_config));

    data = devm_kzalloc(&platform_dev->dev, sizeof(*data), GFP_KERNEL);
    if (!data) {
        HDF_LOGE("%s: devm_kzalloc error!", __func__);
        return -ENOMEM;
    }
    config.dev = &platform_dev->dev;
    config.init_data = &g_virtualCurrentRegulatorInitData;
    config.driver_data = data;

    data->dev = regulator_register(&g_virtualCurrentRegulatorDesc, &config);
    if (IS_ERR(data->dev)) {
        HDF_LOGE("%s: failed to register regulator %s\n", __func__, g_virtualCurrentRegulatorDesc.name);
        return PTR_ERR(data->dev);
    }

    platform_set_drvdata(platform_dev, data);
    HDF_LOGI("%s: success", __func__);
    return 0;
}

static int VirtualCurrentRegulatorPlatformRemove(struct platform_device *platform_dev)
{
    struct VirtualCurrentRegulatorDev *rdev = platform_get_drvdata(platform_dev);

    regulator_unregister(rdev->dev);

    platform_set_drvdata(platform_dev, NULL);
    HDF_LOGI("%s: success", __func__);
    return 0;
}

static struct platform_driver g_virtualCurrentRegulatorPlatformDriver = {
    .driver = {
        .name = "virtual_current_regulator_dev",
        .owner = THIS_MODULE,
    },
    .probe = VirtualCurrentRegulatorPlatformProbe,
    .remove = VirtualCurrentRegulatorPlatformRemove,
};

int VirtualCurrentRegulatorAdapterInit(void)
{
    int ret = platform_device_register(&g_virtualCurrentRegulatorPlatformDevice);
    if (ret == 0) {
        ret = platform_driver_register(&g_virtualCurrentRegulatorPlatformDriver);
    } else {
        HDF_LOGE("%s:device register fail %d", __func__, ret);
    }
    return ret;
}

static int __init VirtualCurrentRegulatorInit(void)
{
    int ret = platform_device_register(&g_virtualCurrentRegulatorPlatformDevice);
    if (ret == 0) {
        ret = platform_driver_register(&g_virtualCurrentRegulatorPlatformDriver);
    }
    return ret;
}

static void __exit VirtualCurrentRegulatorExit(void)
{
    platform_device_unregister(&g_virtualCurrentRegulatorPlatformDevice);
    platform_driver_unregister(&g_virtualCurrentRegulatorPlatformDriver);
}

MODULE_DESCRIPTION("Virtual current Regulator Controller Platform Device Drivers");
MODULE_LICENSE("GPL");
