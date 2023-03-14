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

#include "lnn_event_monitor.h"

#include <pthread.h>
#include <securec.h>

#include "bus_center_event.h"
#include "lnn_event_monitor_impl.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_adapter_mem.h"

typedef enum {
    MONITOR_IMPL_NETLINK_TYPE = 0,
    MONITOR_IMPL_PRODUCT_TYPE,
    MONITOR_IMPL_LWIP_TYPE,
    MONITOR_IMPL_WIFISERVICE_TYPE,
    MONITOR_IMPL_DRIVER_TYPE,
    MONITOR_IMPL_MAX_TYPE,
} MonitorImplType;

static LnnInitEventMonitorImpl g_monitorImplInit[MONITOR_IMPL_MAX_TYPE] = {
    LnnInitNetlinkMonitorImpl,
    LnnInitProductMonitorImpl,
    LnnInitLwipMonitorImpl,
    LnnInitWifiServiceMonitorImpl,
    LnnInitDriverMonitorImpl,
};

int32_t LnnInitEventMonitor(void)
{
    for (uint32_t i = 0; i < MONITOR_IMPL_MAX_TYPE; ++i) {
        if (g_monitorImplInit[i] == NULL) {
            continue;
        }
        if (g_monitorImplInit[i]() != SOFTBUS_OK) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init event monitor impl(%u) failed", i);
            return SOFTBUS_ERR;
        }
    }
    return SOFTBUS_OK;
}