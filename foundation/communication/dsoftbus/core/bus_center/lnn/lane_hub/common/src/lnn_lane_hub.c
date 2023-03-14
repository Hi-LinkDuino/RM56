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

#include "lnn_lane_hub.h"

#include "bus_center_event.h"
#include "lnn_heartbeat_strategy.h"
#include "lnn_lane_manager.h"
#include "lnn_time_sync_manager.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

int32_t LnnInitLaneHub(void)
{
    if (LnnInitLaneManager() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init lane manager fail");
        return SOFTBUS_ERR;
    }
    if (LnnInitTimeSync() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init time sync fail");
        return SOFTBUS_ERR;
    }
    if (LnnInitHeartbeat() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init heart beat fail");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t LnnInitLaneHubDelay(void)
{
    if (LnnStartHeartbeatDelay() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "start heartbeat delay fail");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

void LnnDeinitLaneHub(void)
{
    LnnDeinitTimeSync();
    LnnDeinitHeartbeat();
}