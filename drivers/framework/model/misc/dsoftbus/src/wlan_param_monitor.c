/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "wlan_param_monitor.h"

#include "hdf_dsoftbus_driver.h"
#include "hdf_log.h"
#include "module_manager.h"
#include "osal_time.h"

#define HDF_LOG_TAG "wlan_param_monitor"

#define WLAN_PARAM_REPORT_INTERVAL 1000

typedef enum {
    CMD_REQUEST_PARAM = 0,
    CMD_MAX_INDEX
} Command;

typedef enum {
    EVENT_WLAN_PARAM = 0,
    EVENT_MAX_INDEX
} Event;

typedef struct {
    uint32_t event;
    uint32_t value;
} ReportInfo;

static void ProcessRequestParam(void)
{
    ReportInfo info;
    struct HdfSBuf *data = NULL;

    info.event = EVENT_WLAN_PARAM;
    info.value = (uint32_t)OsalGetSysTimeMs();
    data = HdfSbufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("get sbuf fail");
        return;
    }
    if (!HdfSbufWriteBuffer(data, (const void *)&info, sizeof(info))) {
        HDF_LOGE("sbuf write report value fail");
        HdfSbufRecycle(data);
        return;
    }
    HdfSoftbusBroadcastEvent(SOFTBUS_MODULE_WLAN_PARAM_MONITOR, data);
    HdfSbufRecycle(data);
}

int32_t SoftbusWlanParamMonitorInit(struct HdfDeviceObject *device)
{
    (void)device;
    HDF_LOGI("SoftbusWlanParamMonitorInit init");
    return HDF_SUCCESS;
}

void SoftbusWlanParamMonitorProcess(const struct HdfSBuf *reqData, struct HdfSBuf *rspData)
{
    uint32_t cmd;
    const void *data = NULL;
    uint32_t dataSize;

    (void)rspData;
    if (reqData == NULL) {
        HDF_LOGE("reqData is null");
        return;
    }
    if (!HdfSbufReadBuffer((struct HdfSBuf *)reqData, &data, &dataSize)) {
        HDF_LOGE("read command fail");
        return;
    }
    cmd = *((uint32_t *)data);
    HDF_LOGI("process command: %u", cmd);
    if (cmd == CMD_REQUEST_PARAM) {
        ProcessRequestParam();
    }
}