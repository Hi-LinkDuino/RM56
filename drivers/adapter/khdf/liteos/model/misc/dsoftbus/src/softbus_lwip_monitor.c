/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
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

#include "softbus_lwip_monitor.h"

#include <lwip/netif.h>
#include <securec.h>
#include "hdf_dsoftbus_driver.h"
#include "module_manager.h"

#define HDF_LOG_TAG "softbus_lwip_monitor"
#define NETIF_STATUS_UP 1
#define NETIF_STATUS_DOWN 0
#define NETIF_NAME_LENGTH 16

typedef enum {
    EVENT_IFACE_STATUS_CHANGED = 0,
    EVENT_IFACE_ADDR_CHANGED,
    EVENT_MAX_INDEX
} Event;

typedef struct {
    uint32_t event;
    char ifName[NETIF_NAME_LENGTH];
    union ExtInfo {
        int32_t status;
    } extInfo;
} LwipMonitorReportInfo;

static void ReportEvent(const LwipMonitorReportInfo *reportInfo)
{
    struct HdfSBuf *data = HdfSbufObtainDefaultSize();

    if (data == NULL) {
        dprintf("get sbuf fail\n");
        return;
    }
    if (!HdfSbufWriteBuffer(data, (const void *)reportInfo, sizeof(LwipMonitorReportInfo))) {
        dprintf("sbuf write report value fail\n");
        HdfSbufRecycle(data);
        return;
    }
    HdfSoftbusBroadcastEvent(SOFTBUS_MODULE_LWIP_MONITOR, data);
    HdfSbufRecycle(data);
}

static void NetifStatusCallback(struct netif *netif, netif_nsc_reason_t reason,
    const netif_ext_callback_args_t *args)
{
    LwipMonitorReportInfo reportInfo;
    bool needReport = true;

    if (netif == NULL || args == NULL) {
        dprintf("NetifStatusCallback: input args is NULL\n");
        return;
    }
    dprintf("NetifStatusCallback(%s): nsc event: 0x%x\n", netif->full_name, (uint32_t)reason);
    if (strncpy_s(reportInfo.ifName, NETIF_NAME_LENGTH, netif->full_name, strlen(netif->full_name)) != EOK) {
        dprintf("NetifStatusCallback(%s): copy netif full name fail: %s\n", netif->full_name);
        return;
    }
    switch (reason) {
        case LWIP_NSC_STATUS_CHANGED:
            reportInfo.event = EVENT_IFACE_STATUS_CHANGED;
            reportInfo.extInfo.status = args->status_changed.state;
            dprintf("NetifStatusCallback(%s): nsc status changed: %d\n", netif->full_name, args->status_changed.state);
            break;
        case LWIP_NSC_IPV4_ADDRESS_CHANGED:
            reportInfo.event = EVENT_IFACE_ADDR_CHANGED;
            dprintf("NetifStatusCallback(%s): nsc ipv4 addr changed\n", netif->full_name);
            break;
        case LWIP_NSC_NETIF_ADDED:
            dprintf("NetifStatusCallback(%s): nsc netif added: %d\n", netif->full_name, netif_is_up(netif));
            if (netif_is_up(netif)) {
                reportInfo.event = EVENT_IFACE_STATUS_CHANGED;
                reportInfo.extInfo.status = NETIF_STATUS_UP;
            }
            break;
        case LWIP_NSC_NETIF_REMOVED:
            dprintf("NetifStatusCallback(%s): nsc netif removed\n", netif->full_name);
            reportInfo.event = EVENT_IFACE_STATUS_CHANGED;
            reportInfo.extInfo.status = NETIF_STATUS_DOWN;
            break;
        default:
            needReport = false;
            break;
    }
    if (needReport) {
        ReportEvent(&reportInfo);
    }
}

int32_t SoftbusLwipMonitorInit(struct HdfDeviceObject *device)
{
    (void)device;
    NETIF_DECLARE_EXT_CALLBACK(NetifCallback);
    netif_add_ext_callback(&NetifCallback, NetifStatusCallback);

    dprintf("SoftbusLwipMonitorInit init success...\n");
    return HDF_SUCCESS;
}
