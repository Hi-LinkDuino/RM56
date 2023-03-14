/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "regulator_if.h"
#include "hdf_log.h"
#include "regulator_core.h"

DevHandle RegulatorOpen(const char *name)
{
    if (name == NULL) {
        HDF_LOGE("%s: name is null", __func__);
        return NULL;
    }

    return (DevHandle)RegulatorNodeOpen(name);
}

void RegulatorClose(DevHandle handle)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return;
    }

    if (RegulatorNodeClose(node) != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeClose fail", __func__);
        return;
    }
}

int32_t RegulatorEnable(DevHandle handle)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeEnable(node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeEnable fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorDisable(DevHandle handle)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeDisable(node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeDisable fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorForceDisable(DevHandle handle)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeForceDisable(node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeForceDisable fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorSetVoltage(DevHandle handle, uint32_t minUv, uint32_t maxUv)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }

    if (minUv > maxUv) {
        HDF_LOGE("RegulatorSetVoltage: %s Uv [%d, %d] invalid!",
            node->regulatorInfo.name, minUv, maxUv);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeSetVoltage(node, minUv, maxUv);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorSetVoltage fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorGetVoltage(DevHandle handle, uint32_t *voltage)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL || voltage == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeGetVoltage(node, voltage);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeGetVoltage fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorSetCurrent(DevHandle handle, uint32_t minUa, uint32_t maxUa)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL) {
        HDF_LOGE("%s: node is null", __func__);
        return HDF_FAILURE;
    }

    if (minUa > maxUa) {
        HDF_LOGE("RegulatorSetCurrent: %s Ua [%d, %d] invalid!",
            node->regulatorInfo.name, minUa, maxUa);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeSetCurrent(node, minUa, maxUa);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeSetCurrent fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorGetCurrent(DevHandle handle, uint32_t *regCurrent)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL || regCurrent == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeGetCurrent(node, regCurrent);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeGetCurrent fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t RegulatorGetStatus(DevHandle handle, uint32_t *status)
{
    struct RegulatorNode *node = (struct RegulatorNode *)handle;
    if (node == NULL || status == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    int ret = RegulatorNodeGetStatus(node, status);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: RegulatorNodeGetStatus fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
