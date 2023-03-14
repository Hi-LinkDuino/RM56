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
#include "avctp_int.h"
#include "avctp_dev.h"
#include "avctp_ctrl_l2cap.h"
#include "avctp_br_l2cap.h"
#include "securec.h"
#include "log.h"
#include "event.h"
#include "btstack.h"
#include "btm/btm_thread.h"
#include "module.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/* Manager block for AVCT */
AvctChMng g_avctMng;

void AvctStartup(void *context);
void AvctShutdown(void *context);
/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AVCT_Init
 * Description  This function is called to start avctp.
 * Param[in]    void.
 * Return       void.
 */
static void AVCT_Init()
{
    LOG_INFO("[AVCT] %{public}s", __func__);
    return;
}

/*
 * Function     AVCT_Cleanup
 * Description  This function is called to stop avctp.
 * Param[in]   void
 * Return      void
 */
static void AVCT_Cleanup()
{
    LOG_INFO("[AVCT] %{public}s", __func__);
    return;
}

void AvctStartup(void *context)
{
    Event *event = (Event *)context;
    /* Initialize channel manager block */
    (void)memset_s(&g_avctMng, sizeof(AvctChMng), 0, sizeof(AvctChMng));
    /* regist psm */
    if (L2CIF_RegisterService(AVCT_PSM, (L2capService *)&G_AVCT_L2C_SVR, NULL, NULL)) {
        LOG_ERROR("[AVCT] %{public}s:L2CIF_RegisterService AVCT_PSM failed.", __func__);
    }
    /* regist browser psm */
    if (L2CIF_RegisterService(AVCT_BR_PSM, (L2capService *)&G_AVCT_BR_L2C_SVR, NULL, NULL)) {
        LOG_ERROR("[AVCT] %{public}s:L2CIF_RegisterService AVCT_BR_PSM failed.", __func__);
    }
    EventSet(event);
}

static void AVCT_Startup()
{
    LOG_INFO("[AVCT] %{public}s", __func__);
    /* Create queue resource */
    if (BTM_CreateProcessingQueue(PROCESSING_QUEUE_ID_AVCTP, BTM_PROCESSING_QUEUE_SIZE_DEFAULT)) {
        return;
    }
    Event *event = EventCreate(true);
    if (!AvctAsyncProcess(AvctStartup, event)) {
        EventWait(event, WAIT_TIME);
    }
    EventDelete(event);
}

void AvctShutdown(void *context)
{
    LOG_INFO("[AVCT] %{public}s", __func__);
    Event *event = (Event *)context;
    L2CIF_DeregisterService(AVCT_PSM, NULL);
    L2CIF_DeregisterService(AVCT_BR_PSM, NULL);
    /* Delete queue resource */
    BTM_DeleteProcessingQueue(PROCESSING_QUEUE_ID_AVCTP);
    /* Release device resource */
    for (uint8_t i = 0; i < AVCT_MAX_DEVICES; i++) {
        AvctCbDevDealloc(&g_avctMng.cbDev[i]);
    }
    EventSet(event);
}

static void AVCT_Shutdown()
{
    LOG_INFO("[AVCT] %{public}s", __func__);
    Event *event = EventCreate(true);
    if (!AvctAsyncProcess(AvctShutdown, event)) {
        EventWait(event, WAIT_TIME);
    }
    EventDelete(event);
}

static Module g_avctp = {
    .name = MODULE_NAME_AVCTP,
    .init = AVCT_Init,
    .startup = AVCT_Startup,
    .shutdown = AVCT_Shutdown,
    .cleanup = AVCT_Cleanup,
    .dependencies = {MODULE_NAME_L2CAP, MODULE_NAME_GAP},
};

MODULE_DECL(g_avctp)
