/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "devsvc_manager_clnt.h"
#include "hdf_bt_transport.h"
#include "hdf_chip.h"
#include "hdf_chip_config.h"
#include "hdf_device_desc.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal/osal_io.h"
#include "osal/osal_mem.h"
#include "osal/osal_spinlock.h"
#include "platform/gpio_if.h"

#define HDF_LOG_TAG HDF_BT

#define MAX_BT_DEVICE_COUNT 2

#define MAX_NODE_NAME_SIZE 32

struct HdfBtVirtualDevice {
    struct HdfVirtualDevice *device;
    struct HdfBtTransport *transport;
};

enum HdfBtTransportType
{
    BT_TRANSPORT_TYPE_RAW = 0,
    BT_TRANSPORT_TYPE_CUSTOM
};

struct HdfBtTransportConfig {
    uint8_t type;
    union {
        const char *devName;
        const char *serviceName;
    };
};

struct HdfBtRawTransport {
    struct HdfBtTransport base;
    const char *devName;
};

typedef int32_t (*DeviceOperator)(struct HdfVirtualDevice *);
typedef int32_t (*BtDeviceOperator)(struct HdfBtVirtualDevice *);

enum HDF_BT_CMD
{
    HDF_BT_CMD_GET_DEVICE_COUNT = 0,
    HDF_BT_CMD_INIT_DEVICE,
    HDF_BT_CMD_DEINIT_DEVICE
};

// registed device count
uint8_t g_deviceCount = 0;
// registed device tab
struct HdfBtVirtualDevice g_btDevices[MAX_BT_DEVICE_COUNT];
// lock of the device tab
OSAL_DECLARE_SPINLOCK(g_devicesLock);

static int32_t ParseTransportConfig(const struct DeviceResourceNode *node, struct HdfBtTransportConfig *config) {
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret = HDF_SUCCESS;
    if (node == NULL || config == NULL) {
        HDF_LOGE("%s: one of the input para is NULL!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL || drsOps->GetString == NULL) {
        HDF_LOGE("%s: bad DeviceResourceIface!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "type", &config->type, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read type fail!", __func__);
        return HDF_FAILURE;
    }
    switch (config->type) {
    case BT_TRANSPORT_TYPE_RAW: {
        if (drsOps->GetString(node, "devName", &config->devName, "") != HDF_SUCCESS) {
            HDF_LOGE("%s: devName is required!", __func__);
            ret = HDF_FAILURE;
        }
        break;
    }
    case BT_TRANSPORT_TYPE_CUSTOM: {
        if (drsOps->GetString(node, "serviceName", &config->serviceName, "") != HDF_SUCCESS) {
            HDF_LOGE("%s: serviceName is required!", __func__);
            ret = HDF_FAILURE;
        }
        break;
    }
    default:
        HDF_LOGE("%s: unexpected transport type %d!", __func__, config->type);
        ret = HDF_FAILURE;
        break;
    }
    return ret;
}

static int32_t InitDeivceList() {
    (void)memset_s(g_btDevices, sizeof(g_btDevices), 0, sizeof(g_btDevices));
    g_deviceCount = 0;
    return OsalSpinInit(&g_devicesLock);
}

static int32_t RegistBtDevice(struct HdfVirtualDevice *device, struct HdfBtTransport *transport) {
    int ret;
    if (device == NULL || transport == NULL) {
        HDF_LOGE("%s:nullptr!", __func__);
        return HDF_FAILURE;
    }
    ret = OsalSpinLockIrq(&g_devicesLock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:lock failed!ret=%d", __func__, ret);
        return ret;
    }
    if (g_deviceCount < MAX_BT_DEVICE_COUNT) {
        g_btDevices[g_deviceCount].device = device;
        g_btDevices[g_deviceCount].transport = transport;
        ++g_deviceCount;
    } else {
        HDF_LOGE("%s:deviceList is full!", __func__);
        ret = HDF_FAILURE;
    }
    (void)OsalSpinUnlockIrq(&g_devicesLock);
    return ret;
}

inline static int32_t OperateDevice(uint8_t id, DeviceOperator operator) {
    int32_t ret = OsalSpinLockIrq(&g_devicesLock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:lock failed!ret=%d", __func__, ret);
        return ret;
    }
    HDF_LOGW("%s:operator %d", __func__, id);

    do {
        if (id >= g_deviceCount) {
            HDF_LOGE("%s:no such device", __func__);
            ret = HDF_FAILURE;
            break;
        }
        ret = operator(g_btDevices[id].device);
    } while (false);

    (void)OsalSpinUnlockIrq(&g_devicesLock);
    return ret;
}

#define OperateBtDevice(RET, ID, Operator, ARGS...)                                                                    \
    do {                                                                                                               \
        RET = OsalSpinLockIrq(&g_devicesLock);                                                                         \
        if (RET != HDF_SUCCESS) {                                                                                      \
            HDF_LOGE("%s:lock failed!ret=%d", __func__, RET);                                                          \
            break;                                                                                                     \
        }                                                                                                              \
        do {                                                                                                           \
            if (id >= g_deviceCount) {                                                                                 \
                HDF_LOGE("%s:no such device", __func__);                                                               \
                RET = HDF_FAILURE;                                                                                     \
                break;                                                                                                 \
            }                                                                                                          \
            RET = Operator(&g_btDevices[ID], ##ARGS);                                                                  \
        } while (false);                                                                                               \
        (void)OsalSpinUnlockIrq(&g_devicesLock);                                                                       \
    } while (false)

inline int32_t PowerOnDevice(uint8_t id) {
    return OperateDevice(id, HdfPowerOnVirtualDevice);
}

inline int32_t PowerOffDevice(uint8_t id) {
    return OperateDevice(id, HdfPowerOffVirtualDevice);
}

static int32_t InitTransportOperation(struct HdfBtVirtualDevice *device) {
    if (device == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    if (device->transport != NULL && device->transport->ops != NULL && device->transport->ops->Init != NULL) {
        return device->transport->ops->Init(device->transport);
    }
    return HDF_SUCCESS;
}

inline int32_t InitTransport(uint8_t id) {
    int ret = HDF_SUCCESS;
    OperateBtDevice(ret, id, InitTransportOperation);
    return ret;
}

static int32_t DeinitTransportOperation(struct HdfBtVirtualDevice *device) {
    if (device == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return HDF_FAILURE;
    }
    if (device->transport != NULL && device->transport->ops != NULL && device->transport->ops->Deinit != NULL) {
        device->transport->ops->Deinit(device->transport);
    }
    return HDF_SUCCESS;
}

inline int32_t DeinitTransport(uint8_t id) {
    int ret = HDF_SUCCESS;
    OperateBtDevice(ret, id, DeinitTransportOperation);
    return ret;
}

static int32_t GetDevNodeNameOperation(struct HdfBtVirtualDevice *device, char *buf, uint32_t size) {
    if (device == NULL || device->transport == NULL || device->transport->ops == NULL ||
        device->transport->ops->GetVfsDevName == NULL) {
        HDF_LOGE("%s:bad transport.", __func__);
        return HDF_FAILURE;
    }
    return device->transport->ops->GetVfsDevName(device->transport, buf, size);
}

inline int32_t GetDevNodeName(uint8_t id, char *buf, uint32_t size) {
    int ret = HDF_SUCCESS;
    OperateBtDevice(ret, id, GetDevNodeNameOperation, buf, size);
    return ret;
}

static int32_t BtMessageDispatcher(struct HdfDeviceIoClient *client, int id, struct HdfSBuf *reqData,
                                   struct HdfSBuf *rspData) {
    int ret = HDF_FAILURE;
    HDF_LOGV("%s: enter", __func__);
    (void)client;
    if (reqData == NULL) {
        HDF_LOGE("%s:nullptr", __func__);
        return ret;
    }
    switch (id) {
    case HDF_BT_CMD_GET_DEVICE_COUNT: {
        if (HdfSbufWriteUint8(rspData, g_deviceCount)) {
            ret = HDF_SUCCESS;
        } else {
            HDF_LOGE("%s:reponse device count failed!", __func__);
        }
        break;
    }
    case HDF_BT_CMD_INIT_DEVICE: {
        uint8_t id = 0;
        char buff[MAX_NODE_NAME_SIZE];
        if (!HdfSbufReadUint8(reqData, &id)) {
            HDF_LOGE("%s:read deviceID failed!", __func__);
            break;
        }
        HDF_LOGI("%s:power on. devID=%d", __func__, id);
        ret = PowerOnDevice(id);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:power on failed.devideID=%d", __func__, id);
            break;
        }
        ret = InitTransport(id);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:init transport failed.devideID=%d,ret=%d", __func__, id, ret);
            break;
        }
        ret = GetDevNodeName(id, buff, MAX_NODE_NAME_SIZE);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:get dev node failed!id=%d", __func__, id);
            break;
        }
        if (rspData != NULL) {
            if (!HdfSbufWriteString(rspData, buff)) {
                HDF_LOGE("%s:respose dev node failed!id=%d", __func__, id);
                break;
            }
        }
        ret = HDF_SUCCESS;
        break;
    }
    case HDF_BT_CMD_DEINIT_DEVICE: {
        uint8_t id = 0;
        if (!HdfSbufReadUint8(reqData, &id)) {
            HDF_LOGE("%s:read deviceID failed!", __func__);
            break;
        }
        ret = DeinitTransport(id);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Deinit transport failed.devideID=%d", __func__, id);
        }
        HDF_LOGI("%s:power on %d", __func__, id);
        ret = PowerOffDevice(id);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:power off failed.devideID=%d", __func__, id);
            break;
        }

        break;
    }
    default:
        HDF_LOGE("%s:unexpected cmd %d!", __func__, id);
        break;
    };
    return ret;
}

static int HdfBtChipDriverBind(struct HdfDeviceObject *dev) {
    static struct IDeviceIoService btService = {
        .object.objectId = 1,
        .Dispatch = BtMessageDispatcher,
    };
    HDF_LOGV("%s: enter", __func__);

    dev->service = &btService;
    return 0;
}

static int32_t HdfBtInitRawTransport(struct HdfBtTransport *transport) {
    return HDF_SUCCESS;
}

static void HdfBtDeinitRawTransport(struct HdfBtTransport *transport) {
    return;
}

static void HdfBtDestoryRawTransport(struct HdfBtTransport *transport) {
    if (transport != NULL) {
        OsalMemFree(transport);
    }
}

static int32_t HdfBtGetRawTransportDeviceName(struct HdfBtTransport *transport, char *buf, uint32_t size) {
    struct HdfBtRawTransport *rawTransport = (struct HdfBtRawTransport *)transport;
    if (rawTransport == NULL) {
        return HDF_FAILURE;
    }
    if (strncpy_s(buf, size, rawTransport->devName, strlen(rawTransport->devName)) != EOK) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static const struct HdfBtTransportOps g_rawTransportOps = {.Init = HdfBtInitRawTransport,
                                                     .Deinit = HdfBtDeinitRawTransport,
                                                     .GetVfsDevName = HdfBtGetRawTransportDeviceName,
                                                     .Destory = HdfBtDestoryRawTransport};

static struct HdfBtRawTransport *CreateRawTransport(const char *devName) {
    struct HdfBtRawTransport *transport = NULL;
    if (devName == NULL) {
        return NULL;
    }
    transport = (struct HdfBtRawTransport *)OsalMemCalloc(sizeof(struct HdfBtRawTransport));
    if (transport == NULL) {
        return NULL;
    }
    transport->base.ops = &g_rawTransportOps;
    transport->devName = devName;
    return transport;
}

static struct HdfBtTransport *CreateCustomTransport(const char *serviceName, const struct DeviceResourceNode *node) {
    struct SubscriberCallback callback = {NULL};
    struct HdfDeviceObject *object = NULL;
    struct HdfBtTransportService *service = NULL;

    int ret = DevSvcManagerClntSubscribeService(serviceName, callback);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:load service failed!serviceName=%s", __func__, serviceName);
        return NULL;
    }
    object = DevSvcManagerClntGetDeviceObject(serviceName);
    if (object == NULL || object->service == NULL) {
        HDF_LOGE("%s:bad service %s", __func__, serviceName);
        return NULL;
    }
    service = (struct HdfBtTransportService *)object->service;
    if (service->CreateTransport == NULL) {
        HDF_LOGE("%s:service %s has no CreateTransport method", __func__, serviceName);
        return NULL;
    }
    return service->CreateTransport(node);
}

static struct HdfBtTransport *CreateTransport(const struct DeviceResourceNode *node) {
    struct HdfBtTransport *transport = NULL;
    struct DeviceResourceIface *drsOps = NULL;
    struct HdfBtTransportConfig config = {0};
    const struct DeviceResourceNode *transportNode = NULL;
    int32_t ret;
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: bad DeviceResourceIface!", __func__);
        return NULL;
    }
    transportNode = drsOps->GetChildNode(node, "transport");
    if (transportNode == NULL) {
        HDF_LOGE("%s:node transport in hcs is required", __func__);
        return NULL;
    }

    ret = ParseTransportConfig(transportNode, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: parse transport config failed!ret= %d", __func__, ret);
        return NULL;
    }
    switch (config.type) {
    case BT_TRANSPORT_TYPE_RAW: {
        transport = (struct HdfBtTransport *)CreateRawTransport(config.devName);
        break;
    }
    case BT_TRANSPORT_TYPE_CUSTOM: {
        transport = CreateCustomTransport(config.serviceName, node);
        break;
    }
    default:
        HDF_LOGE("%s:unexpected transport type %d", __func__, config.type);
    }
    return transport;
}

static int32_t InitDevice(const struct DeviceResourceNode *node) {
    struct HdfChipConfig config = {0};
    struct HdfVirtualDevice *device = NULL;
    struct HdfBtTransport *transport = NULL;

    int32_t ret = ParseChipConfig(node, &config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:parse config failed!", __func__);
        return ret;
    }
    do {
        device = CreateVirtualDevice(&config);
        if (device == NULL) {
            HDF_LOGE("%s:Create virtual device failed!", __func__);
            ret = HDF_FAILURE;
            break;
        }
        transport = CreateTransport(node);
        if (transport == NULL) {
            HDF_LOGE("%s:Create transport failed!", __func__);
            ret = HDF_FAILURE;
            break;
        }
        ret = RegistBtDevice(device, transport);
    } while (false);
    if (ret != HDF_SUCCESS) {
        if (device != NULL) {
            ReleaseVirtualDevice(device);
            device = NULL;
        }
        if (transport != NULL && transport->ops != NULL && transport->ops->Destory != NULL) {
            transport->ops->Destory(transport);
        }
        transport = NULL;
    }
    ClearChipConfig(&config);
    return ret;
}

static int32_t InitDevices(struct HdfDeviceObject *device) {
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *devListNode = NULL;
    struct DeviceResourceNode *childNode = NULL;
    int32_t ret = HDF_SUCCESS;
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s:nullptr!", __func__);
        return HDF_FAILURE;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    devListNode = drsOps->GetChildNode(device->property, "deviceList");
    if (devListNode == NULL) {
        HDF_LOGW("%s:no device list defined!", __func__);
        return HDF_SUCCESS;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(devListNode, childNode) {
        ret = InitDevice(childNode);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:init device %s failed!", __func__, childNode->name);
            break;
        }
    }

    return ret;
}

static int32_t HdfBtChipDriverInit(struct HdfDeviceObject *device) {

    int ret;
    HDF_LOGV("%s:driver init...", __func__);

    ret = InitDeivceList();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:InitDeivceList failed!ret=%d", __func__, ret);
        return ret;
    }

    ret = InitDevices(device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:init devices failed!ret=%d", __func__, ret);
        return ret;
    }
    return HDF_SUCCESS;
};

static void HdfBtChipDriverRelease(struct HdfDeviceObject *object) {
    uint8_t i;
    int ret = OsalSpinLockIrq(&g_devicesLock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:lock failed!ret=%d", __func__, ret);
        return;
    }

    for (i = g_deviceCount - 1; i >= 0; --i) {
        g_deviceCount--;
        if (g_btDevices[i].device != NULL) {
            ReleaseVirtualDevice(g_btDevices[i].device);
            g_btDevices[i].device = NULL;
        }

        if (g_btDevices[i].transport != NULL && g_btDevices[i].transport->ops != NULL &&
            g_btDevices[i].transport->ops->Destory != NULL) {
            g_btDevices[i].transport->ops->Destory(g_btDevices[i].transport);
        }
        g_btDevices[i].transport = NULL;
    }

    (void)OsalSpinUnlockIrq(&g_devicesLock);
    return;
}

struct HdfDriverEntry g_hdfBTDriver = {
    .moduleVersion = 1,
    .Bind = HdfBtChipDriverBind,
    .Init = HdfBtChipDriverInit,
    .Release = HdfBtChipDriverRelease,
    .moduleName = "HDF_BT",
};

HDF_INIT(g_hdfBTDriver);
