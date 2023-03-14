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
#include "avctp_conn.h"
#include "avctp_st.h"
#include "log.h"
#include "securec.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AvctCbConnAlloc
 * Description  This function is called to alloc connection resource.
 * Param[in]   *connParam  point to the info of the connection request.
 *              Such as the Role、Profile ID、Callback func point.
 * Return      connection object point; NULL if there is no connection resource.
 */
AvctCbConn *AvctCbConnAlloc(const AvctConnectParam *connParm)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        if (g_avctMng.cbConn[i].status == 0) {
            g_avctMng.cbConn[i].status = AVCT_CONN_ALLOC;
            g_avctMng.cbConn[i].connId = i;
            g_avctMng.cbConn[i].cbDev = NULL;
            (void)memcpy_s(
                &g_avctMng.cbConn[i].connParam, sizeof(AvctConnectParam), connParm, sizeof(AvctConnectParam));
            cbConn = &g_avctMng.cbConn[i];
            LOG_DEBUG("[AVCT] %{public}s:connId is %hhu\n", __func__, i);
            break;
        }
    }
    return cbConn;
}

/*
 * Function     AvctCbConnDealloc
 * Description  This function is called to dealloc connection resource.
 * Param[in]    *cbConn  point to the info of the connection control block.
 * Return       void
 */
void AvctCbConnDealloc(AvctCbConn *cbConn)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    if (cbConn->connParam.role == AVCT_ACPT) {
        cbConn->cbDev = NULL;
        cbConn->status = AVCT_CONN_ALLOC;
    } else {
        (void)memset_s(cbConn, sizeof(AvctCbConn), 0, sizeof(AvctCbConn));
    }
    return;
}
/*
 * Function     AvctCbConnEvtCallback
 * Description  This function is called to send evet callback to app.
 * Param[in]    *cbConn  point to the info of the connection control block.
 * Param[in]    event
 * Param[in]    result
 * Param[in]    peerAddr  peer device address
 * Return       void
 */
NO_SANITIZE("cfi")
void AvctCbConnEvtCallback(const AvctCbConn *cbConn, uint8_t event, uint16_t result, const BtAddr *peerAddr)
{
    LOG_DEBUG("[AVCT]---%{public}s: connId(%hhu) EventId(%hhu), result(%hu)", __func__, cbConn->connId, event, result);
    AvctChannelEventCallback cback = cbConn->connParam.chEvtCallback;
    if (cback != NULL) {
        (*cback)(cbConn->connId, event, result, peerAddr, cbConn->connParam.context);
    }
    return;
}
/*
 * Function     AvctGetCbConnByConnId
 * Description  This function is called to get the connection by connId.
 * Param[in]   connId  connection ID.
 * Return      AvctCbConn  the point of connection control block
 */
AvctCbConn *AvctGetCbConnByConnId(uint8_t connId)
{
    LOG_DEBUG("[AVCT] %{public}s:", __func__);
    AvctCbConn *cbConn = NULL;
    if ((connId < AVCT_MAX_CONNECTS) && (g_avctMng.cbConn[connId].status != 0)) {
        cbConn = &g_avctMng.cbConn[connId];
        LOG_DEBUG("[AVCT] %{public}s:conn status(%hhu)", __func__, g_avctMng.cbConn[connId].status);
    }
    return cbConn;
}

/*
 * Function     AvctGetCbConnByPid
 * Description  This function is called to get the connection by cbCtrl and pid.
 * Param[in]   *cbDev  point to the device block.
 * Param[in]   pid  profile id.
 * Return      void
 */
AvctCbConn *AvctGetCbConnByPid(const AvctCbDev *cbDev, uint16_t pid)
{
    LOG_DEBUG("[AVCT] %{public}s:pid (0x%x)", __func__, pid);
    AvctCbConn *cbConn = NULL;
    for (uint8_t i = 0; i < AVCT_MAX_CONNECTS; i++) {
        if ((g_avctMng.cbConn[i].status == AVCT_CONN_BIND) && (g_avctMng.cbConn[i].cbDev == cbDev) &&
            (g_avctMng.cbConn[i].connParam.pid == pid)) {
            cbConn = &g_avctMng.cbConn[i];
            break;
        }
    }
    return cbConn;
}
