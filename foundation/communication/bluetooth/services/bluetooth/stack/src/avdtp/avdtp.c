/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "avdtp_ctrl.h"
#include "avdtp_l2cap.h"
#include "btm/btm_thread.h"
#include "btstack.h"
#include "event.h"
#include "log.h"
#include "module.h"

void AvdtStartup(void *context);
void AvdtShutdown(void *context);

static void AVDT_Init()
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    return;
}

static void AVDT_Cleanup()
{
    LOG_DEBUG("[AVDT]%{public}s:", __func__);
    return;
}

static void AVDT_Startup()
{
    LOG_INFO("[AVDT] %{public}s", __func__);
    /* Create queue resource */
    if (BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_AVDTP, BTM_PROCESSING_QUEUE_SIZE_DEFAULT)) {
        return;
    }
    Event *event = EventCreate(true);
    if (!AvdtAsyncProcess(AvdtStartup, event)) {
        EventWait(event, WAIT_TIME);
    }
    EventDelete(event);
    return;
}

void AvdtStartup(void *context)
{
    Event *event = (Event *)context;
    AvdtControlBlockInit();
    /* regist psm */
    if (L2CIF_RegisterService(AVDT_PSM, (L2capService *)&G_AVDT_L2C_APPL, NULL, NULL)) {
        LOG_DEBUG("[AVDT]%{public}s:L2CIF_RegisterService failed! ", __func__);
    }
    EventSet(event);
    return;
}

static void AVDT_Shutdown()
{
    LOG_INFO("[AVDT] %{public}s", __func__);
    Event *event = EventCreate(true);
    if (!AvdtAsyncProcess(AvdtShutdown, event)) {
        EventWait(event, WAIT_TIME);
    }
    EventDelete(event);
    return;
}

void AvdtShutdown(void *context)
{
    Event *event = (Event *)context;
    L2CIF_DeregisterService(AVDT_PSM, NULL);
    AvdtSigDealloc();
    AvdtTransChDeallocAll();
    /* Delete queue resource */
    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_AVDTP);
    EventSet(event);
    return;
}

static Module g_avdtp = {
    .name = MODULE_NAME_AVDTP,
    .init = AVDT_Init,
    .startup = AVDT_Startup,
    .shutdown = AVDT_Shutdown,
    .cleanup = AVDT_Cleanup,
    .dependencies = {MODULE_NAME_L2CAP, MODULE_NAME_GAP},
};

MODULE_DECL(g_avdtp)
