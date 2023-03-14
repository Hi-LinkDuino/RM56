/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "device_resource_if.h"
#include "hcs_tree_if.h"
#include "hdf_log.h"

static void HcsIfaceConstruct(struct DeviceResourceIface *instance)
{
    instance->GetRootNode = HcsGetRootNode;
    instance->GetBool = HcsGetBool;
    instance->GetUint8 = HcsGetUint8;
    instance->GetUint8ArrayElem = HcsGetUint8ArrayElem;
    instance->GetUint8Array = HcsGetUint8Array;
    instance->GetUint16 = HcsGetUint16;
    instance->GetUint16ArrayElem = HcsGetUint16ArrayElem;
    instance->GetUint16Array = HcsGetUint16Array;
    instance->GetUint32 = HcsGetUint32;
    instance->GetUint32ArrayElem = HcsGetUint32ArrayElem;
    instance->GetUint32Array = HcsGetUint32Array;
    instance->GetUint64 = HcsGetUint64;
    instance->GetUint64ArrayElem = HcsGetUint64ArrayElem;
    instance->GetUint64Array = HcsGetUint64Array;
    instance->GetString = HcsGetString;
    instance->GetStringArrayElem = HcsGetStringArrayElem;
    instance->GetElemNum = HcsGetElemNum;
    instance->GetNodeByMatchAttr = HcsGetNodeByMatchAttr;
    instance->GetChildNode = HcsGetChildNode;
    instance->GetNodeByRefAttr = HcsGetNodeByRefAttr;
}

static bool DeviceResourceIfaceConstruct(struct DeviceResourceIface *instance, DeviceResourceType type)
{
    switch (type) {
        case HDF_CONFIG_SOURCE:
            HcsIfaceConstruct(instance);
            break;
        default:
            HDF_LOGE("%s: Currently, this configuration type is not supported, the type is %d", __func__, type);
            return false;
    }
    return true;
}

struct DeviceResourceIface *DeviceResourceGetIfaceInstance(DeviceResourceType type)
{
    static struct DeviceResourceIface *instance = NULL;
    if (instance == NULL) {
        static struct DeviceResourceIface singletonInstance;
        if (!DeviceResourceIfaceConstruct(&singletonInstance, type)) {
            return NULL;
        }
        instance = &singletonInstance;
    }
    return instance;
}
