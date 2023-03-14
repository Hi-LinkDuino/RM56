/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hdf_log.h"
#include "device_resource_if.h"
#include "hdf_usb.h"

#define USB_CONTROLLER_LIST_NAME  "controller_list"
#define USB_NODE_MATCH_ATTR       "hisilicon,hi35xx-xhci"
#define USB_MAX_CFG_LEN           8
#define USB_CONTROL_NAME_LEN      32
#define USB_CFG_UNIT              2

struct UsbConfig {
    uint32_t type;
    uint32_t unit;
    uint32_t start;
    uint32_t end;
    uint32_t count;
};

struct UsbInfo {
    struct UsbConfig cfg[USB_MAX_CFG_LEN];
    uint32_t len;
};

static const struct DeviceResourceNode *GetHdfUsbNode(const struct DeviceResourceIface *instance)
{
    const struct DeviceResourceNode *root = instance->GetRootNode();

    if (root == NULL) {
        HDF_LOGE("%s get root node is null\n", __func__);
        return NULL;
    }

    return instance->GetNodeByMatchAttr(root, USB_NODE_MATCH_ATTR);
}

static int GetUsbControllerParam(const struct DeviceResourceIface *instance, const struct DeviceResourceNode *node,
    struct UsbConfig *cfg)
{
    int ret;

    ret = instance->GetUint32(node, "res_type", &cfg->type, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read res_type fail", __func__);
        return HDF_FAILURE;
    }
    ret = instance->GetUint32(node, "res_unit", &cfg->unit, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read res unit fail", __func__);
        return HDF_FAILURE;
    }
    ret = instance->GetUint32(node, "res_start", &cfg->start, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read res type fail", __func__);
        return HDF_FAILURE;
    }
    ret = instance->GetUint32(node, "res_end", &cfg->end, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read res end fail", __func__);
        return HDF_FAILURE;
    }
    ret = instance->GetUint32(node, "res_count", &cfg->count, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read res count fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int ProcControllorParam(const struct DeviceResourceIface *instance, const struct DeviceResourceNode *node,
    const char *name, add_res_callback_t callback, struct UsbInfo *info)
{
    int32_t ret;
    const struct DeviceResourceNode *subNode = NULL;
    const char *ctrlName = NULL;
    uint32_t index = 0;
    struct UsbConfig *cfg = NULL;

    subNode = node->child;
    if (subNode == NULL) {
        return HDF_FAILURE;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, subNode) {
        if (index >= info->len) {
            break;
        }

        cfg = &(info->cfg[index]);
        ret = instance->GetString(subNode, "controller", &ctrlName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read res_type fail", __func__);
            return HDF_FAILURE;
        }

        if (strcmp(name, ctrlName) != 0) {
            HDF_LOGI("%s: no match name", __func__);
            continue;
        }

        ret = GetUsbControllerParam(instance, subNode, cfg);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: read controller param fail", __func__);
            return HDF_FAILURE;
        }
        callback(name, cfg->type, cfg->unit, cfg->start, cfg->end, cfg->count);
        index++;
    }

    return HDF_SUCCESS;
}

static void HdfUsbInitParam(device_t dev, add_res_callback_t callback)
{
    const struct DeviceResourceIface *devResInstance = NULL;
    const struct DeviceResourceNode *hdfUsbNode = NULL;
    const char *ctrlName = NULL;
    int32_t ret;
    int32_t count;
    int32_t i;
    struct UsbInfo info = {0};

    devResInstance = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (devResInstance == NULL) {
        HDF_LOGE("%s get devResInstance node is null", __func__);
        return;
    }
    hdfUsbNode = GetHdfUsbNode(devResInstance);
    if (hdfUsbNode == NULL) {
        HDF_LOGE("%s get hdf usb node is null", __func__);
        return;
    }

    count = devResInstance->GetElemNum(hdfUsbNode, USB_CONTROLLER_LIST_NAME);
    if ((count <= 0) || (count > USB_MAX_CFG_LEN / USB_CFG_UNIT)) {
        HDF_LOGE("%s get elemnum failed", __func__);
        return;
    }

    info.len = USB_MAX_CFG_LEN;
    for (i = 0; i < count; i++) {
        ret = devResInstance->GetStringArrayElem(hdfUsbNode, USB_CONTROLLER_LIST_NAME, i, &ctrlName, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s get hdf usb list name fail", __func__);
            return;
        }

        device_add_child(dev, ctrlName, 0);
        ret = ProcControllorParam(devResInstance, hdfUsbNode, ctrlName, callback, &info);
        if (ret != HDF_SUCCESS) {
            HDF_LOGI("%s proc usb controllor not match, goto next", __func__);
        }
    }
}

void UsbResourceInit(device_t dev, add_res_callback_t callback)
{
    if (callback == NULL) {
        HDF_LOGE("usb resource init failed");
        return;
    }

    HdfUsbInitParam(dev, callback);
}
