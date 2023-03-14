/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_CHIP_CONFIG_H
#define HDF_CHIP_CONFIG_H

#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal/osal_mem.h"
#include "securec.h"

#define HDF_CHIP_MAX_POWER_SUPPORTED 2

#define BUS_FUNC_MAX 1

enum PowerType
{
    POWER_TYPE_ALWAYS_ON = 0,
    POWER_TYPE_GPIO
};

struct HdfConfigGpioBasedSwitch {
    uint8_t gpioId;
    uint8_t activeLevel;
};

struct HdfPowerConfig {
    uint8_t powerSeqDelay;
    uint8_t type;
    union {
        struct HdfConfigGpioBasedSwitch gpio;
    };
};

struct HdfPowersConfig {
    uint8_t powerCount;
    struct HdfPowerConfig power[0];
};

enum ResetType
{
    RESET_TYPE_NOT_MANAGEABLE = 0,
    RESET_TYPE_GPIO
};

struct HdfResetConfig {
    union {
        struct HdfConfigGpioBasedSwitch gpio;
    };
    uint8_t resetType;
    uint8_t resetHoldTime;
};

struct HdfChipConfig {
    const char *name;
    struct HdfPowersConfig *powers;
    struct HdfResetConfig reset;
    uint8_t bootUpTimeOut;
};

static inline int ParsePowerConfig(const struct DeviceResourceNode *node, struct HdfPowerConfig *config) {
    struct DeviceResourceIface *drsOps = NULL;
    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: one of the input para is NULL!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "powerSeqDelay", &config->powerSeqDelay, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: powersSeqDelay fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "powerType", &config->type, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: type fail!", __func__);
        return HDF_FAILURE;
    }

    if (config->type == POWER_TYPE_GPIO) {
        if (drsOps->GetUint8(node, "gpioId", &config->gpio.gpioId, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: gpioId fail!", __func__);
            return HDF_FAILURE;
        }
        if (drsOps->GetUint8(node, "activeLevel", &config->gpio.activeLevel, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: activeLevel fail!", __func__);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static inline struct HdfPowersConfig *ParsePowersConfig(const struct DeviceResourceNode *node) {
    struct DeviceResourceIface *drsOps = NULL;
    struct DeviceResourceNode *childNode = NULL;
    struct HdfPowersConfig *config = NULL;
    uint8_t nodeCount = 0;
    int32_t ret;
    if (node == NULL) {
        HDF_LOGE("%s: input para is NULL!", __func__);
        return NULL;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return NULL;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode) { ++nodeCount; }
    if (nodeCount > HDF_CHIP_MAX_POWER_SUPPORTED) {
        return NULL;
    }
    config = OsalMemCalloc(sizeof(struct HdfPowersConfig) + nodeCount * sizeof(struct HdfPowerConfig));
    if (config == NULL) {
        return NULL;
    }
    config->powerCount = nodeCount;
    for (uint8_t i = 0; i < nodeCount; i++) {
        char buff[32] = {0};
        ret = snprintf_s(buff, 32, 32, "power%d", i);
        if (ret < 0) {
            HDF_LOGE("%s:snprintf_s failed!ret=%d, i=%d", __func__, ret, i);
            break;
        }
        const struct DeviceResourceNode *powerNode = drsOps->GetChildNode(node, buff);
        if (powerNode == NULL) {
            HDF_LOGE("%s:Can not get node %s", __func__, buff);
            ret = HDF_FAILURE;
            break;
        }
        ret = ParsePowerConfig(powerNode, config->power + i);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:parse node %s failed!ret=%d", __func__, buff, ret);
            break;
        }
    }

    if (ret != HDF_SUCCESS) {
        OsalMemFree(config);
        config = NULL;
    }
    return config;
}

static inline int ParseResetConfig(const struct DeviceResourceNode *node, struct HdfResetConfig *reset) {
    struct DeviceResourceIface *drsOps = NULL;
    if (node == NULL || reset == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "resetType", &reset->resetType, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: powersSeqDelay fail!", __func__);
        return HDF_FAILURE;
    }
    if (reset->resetType == RESET_TYPE_GPIO) {
        if (drsOps->GetUint8(node, "gpioId", &reset->gpio.gpioId, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: gpioId fail!", __func__);
            return HDF_FAILURE;
        }

        if (drsOps->GetUint8(node, "activeLevel", &reset->gpio.activeLevel, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: read activeLevel fail!", __func__);
            return HDF_FAILURE;
        }

        if (drsOps->GetUint8(node, "resetHoldTime", &reset->resetHoldTime, 0) != HDF_SUCCESS) {
            HDF_LOGE("%s: read resetHoldTime fail!", __func__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static inline void ClearChipConfig(struct HdfChipConfig *config) {

    if (config->powers != NULL) {
        OsalMemFree(config->powers);
        config->powers = NULL;
    }
}

static inline int32_t ParseChipConfig(const struct DeviceResourceNode *node, struct HdfChipConfig *config) {
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *devPowerNode = NULL;
    const struct DeviceResourceNode *resetNode = NULL;
    int32_t ret = HDF_SUCCESS;
    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: invalid node or devLstConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    config->name = node->name;

    if (drsOps->GetUint8(node, "bootUpTimeOut", &config->bootUpTimeOut, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: bootUpTimeOut fail!", __func__);
        return HDF_FAILURE;
    }

    resetNode = drsOps->GetChildNode(node, "reset");
    if (resetNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseResetConfig(resetNode, &config->reset) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    do {
        devPowerNode = drsOps->GetChildNode(node, "powers");
        if (devPowerNode == NULL) {
            HDF_LOGE("%s: GetChildNode fail!", __func__);
            ret = HDF_FAILURE;
            break;
        }
        config->powers = ParsePowersConfig(devPowerNode);
        if (config->powers == NULL) {
            ret = HDF_FAILURE;
            break;
        }
    } while (false);

    if (ret != HDF_SUCCESS) {
        ClearChipConfig(config);
    }
    return ret;
}

#endif