/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "usbd_port.h"
#include <fcntl.h>
#include <hdf_sbuf.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "hdf_log.h"
#include "osal_time.h"
#include "securec.h"
#include "usbd_function.h"
#include "usbd_publisher.h"
#include "default_config.h"

#define DEFAULT_PORT_ID 1

#define PORT_MODE_HOST_STR "host"
#define PORT_MODE_DEVICE_STR "device"

#define POWER_ROLE_NONE 0
#define POWER_ROLE_SOURCE 1
#define POWER_ROLE_SINK 2
#define POWER_ROLE_MAX 3

#define DATA_ROLE_NONE 0
#define DATA_ROLE_HOST 1
#define DATA_ROLE_DEVICE 2
#define DATA_ROLE_MAX 3

#define PORT_MODE_NONE 0
#define PORT_MODE_DEVICE 1
#define PORT_MODE_HOST 2

static int32_t currentPortId = DEFAULT_PORT_ID;
static int32_t currentPowerRole = POWER_ROLE_SINK;
static int32_t currentDataRole = DATA_ROLE_DEVICE;
static int32_t currentMode = PORT_MODE_DEVICE;

int32_t SetPortInit(int32_t portId, int32_t powerRole, int32_t dataRole);

static int32_t IfCanSwitch(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    if (portId != DEFAULT_PORT_ID) {
        HDF_LOGE("%{public}s: portId error", __func__);
        return HDF_FAILURE;
    }
    if (powerRole <= POWER_ROLE_NONE || powerRole >= POWER_ROLE_MAX) {
        HDF_LOGE("%{public}s: powerRole error", __func__);
        return HDF_FAILURE;
    }
    if (dataRole <= DATA_ROLE_NONE || dataRole >= DATA_ROLE_MAX) {
        HDF_LOGE("%{public}s: dataRole error", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t WritePortFile(int32_t powerRole, int32_t dataRole, int32_t mode)
{
    char *fn = PORT_FILE_PATH;
    char *modeStr = NULL;

    if (mode == PORT_MODE_HOST || mode == PORT_MODE_DEVICE) {
        switch (mode) {
            case PORT_MODE_HOST:
                modeStr = PORT_MODE_HOST_STR;
                UsbdSetFunction(USB_FUNCTION_NONE);
                break;
            case PORT_MODE_DEVICE:
                modeStr = PORT_MODE_DEVICE_STR;
                break;
            default:
                modeStr = PORT_MODE_NONE;
                break;
        }
    }
    if (modeStr == NULL) {
        HDF_LOGE("%{public}s: modeStr error", __func__);
        return HDF_FAILURE;
    }
    int32_t len = strlen(modeStr);
    int32_t fd = open(fn, O_WRONLY | O_TRUNC);
    if (fd < 0) {
        HDF_LOGE("%{public}s: file open error fd= %{public}d", __func__, fd);
        return HDF_FAILURE;
    }
    int32_t ret = write(fd, modeStr, len);
    close(fd);
    if (ret == len) {
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t SetPortInit(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    int32_t mode = PORT_MODE_DEVICE;
    if (IfCanSwitch(portId, powerRole, dataRole)) {
        return HDF_FAILURE;
    }
    if (powerRole == POWER_ROLE_SOURCE && dataRole == DATA_ROLE_HOST) {
        mode = PORT_MODE_HOST;
    }
    if (powerRole == POWER_ROLE_SINK && dataRole == DATA_ROLE_DEVICE) {
        mode = PORT_MODE_DEVICE;
    }
    if (WritePortFile(powerRole, dataRole, mode)) {
        return HDF_FAILURE;
    }
    currentPortId = portId;
    currentPowerRole = powerRole;
    currentDataRole = dataRole;
    currentMode = mode;
    if (currentMode == PORT_MODE_DEVICE) {
        UsbdSetFunction(USB_FUNCTION_HDC);
    }
    return HDF_SUCCESS;
}

int32_t SetPort(int32_t portId, int32_t powerRole, int32_t dataRole, const struct UsbdService *service)
{
    int32_t ret = SetPortInit(portId, powerRole, dataRole);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SetPortInit fail! ret:%{public}d", __func__, ret);
        return ret;
    }

    NotifyUsbPortSubscriber(service->subscriber, currentPortId, currentPowerRole, currentDataRole, currentMode);
    return HDF_SUCCESS;
}

int32_t QueryPort(int32_t *portId, int32_t *powerRole, int32_t *dataRole, int32_t *mode, struct UsbdService *service)
{
    if (portId == NULL || powerRole == NULL || dataRole == NULL || mode == NULL || service ==NULL) {
        HDF_LOGE("%{public}s: Pointer is NULL", __func__);
        return HDF_FAILURE;
    }
    *portId = currentPortId;
    *powerRole = currentPowerRole;
    *dataRole = currentDataRole;
    *mode = currentMode;
    return HDF_SUCCESS;
}
