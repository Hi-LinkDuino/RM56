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
#include "avctp_br.h"
#include "avctp_br_act.h"
#include "avctp_dev.h"
#include "avctp_st.h"
#include "log.h"

/*****************************************************************************
 * Type  Define
 ****************************************************************************/
/* type for state table action functions */
typedef uint16_t (*AvctBrAction_t)(AvctCbDev *cbDev, const AvctEvtData *data);

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
const AvctBrAction_t C_AVCT_BR_ACTION[] = {
    AvctCbBrChConn,         /* AVCT_CHANNEL_CONN */
    AvctCbBrChBind,         /* AVCT_CHANNEL_BIND */
    AvctCbBrChBindFail,     /* AVCT_CHANNEL_BIND_FAIL */
    AvctCbBrChDisconn,      /* AVCT_CHANNEL_DISCONNECT */
    AvctCbBrChUnbind,       /* AVCT_CHANNEL_UNBIND */
    AvctCbBrChCheckDisconn, /* AVCT_CHANNEL_CHECK_DISCONNECT */
    AvctCbBrSendMsg,        /* AVCT_SEND_MSG */
    AvctCbBrDiscardMsg,     /* AVCT_DISCARD_MSG */
    AvctCbBrChConnInd,      /* AVCT_CONN_IND */
    AvctCbBrChConnFail,     /* AVCT_CONN_FAIL */
    AvctCbBrChCloseInd,     /* AVCT_DISCONN_IND */
    AvctCbBrChCloseCfm,     /* AVCT_DISCONN_CFM */
    AvctCbBrRevMsg,         /* AVCT_REV_MSG */
    AvctCbBrDiscardRevMsg,  /* AVCT_DISCARD_REV_MSG */
    AvctCbBrChBusyInd,      /* AVCT_BUSY_IND */
};
/*****************************************************************************
 * Function
 ****************************************************************************/
uint16_t AvctCbBrEvt(AvctCbDev *cbDev, uint8_t event, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s: EventID(%hhu) curState(%hhu)", __func__, event, cbDev->cbBr->stState);
    uint16_t ret = AVCT_SUCCESS;
    AvctCbCh *cbBr = cbDev->cbBr;
    int curSt = cbBr->stState;
    uint8_t nextSt = AvctGetNextStation(curSt, event);
    uint8_t action = AvctGetEvtAction(curSt, event);
    /* Set next state */
    cbBr->stState = nextSt;
    
    /* excute actoin */
    if (action != AVCT_IGNORE) {
        ret = (*C_AVCT_BR_ACTION[action])(cbDev, data);
    }
    return ret;
}

/*
 * Function     AvctBrConnectInitiate
 * Description  Browser Connect for initiate role
 * Param[out]  *cbConn    connection
 * Param[in]   *connParam  oint to the info of the connection request.
 *              Such as the Role、Profile ID、Callback func point.
 * Param[in]   peerAddr  The peer address to be connected.
 * Return      AVCT_SUCCESS if successful, otherwise error.
 */
uint16_t AvctBrConnectInitiate(AvctCbConn *cbConn)
{
    uint16_t ret = AVCT_SUCCESS;
    AvctCbCh *cbBr = NULL;
    /* check whether the ctrl channel has been connected. */
    if (cbConn->cbDev->cbCtrl == NULL) {
        LOG_ERROR("[AVCT]The ctrl connection is not open!");
        ret = AVCT_ERR_CONN_NOT_OPEN;
    } else if (cbConn->cbDev->chLink & AVCT_ALLOC_BR) {
        LOG_ERROR("[AVCT]The br connection is already opened!");
        ret = AVCT_ERR_CONN_BAD;
    } else {
        /* Alloc CbBr memory */
        if (cbConn->cbDev->cbBr == NULL) {
            cbBr = AvctCbChAlloc();
            if (cbBr == NULL) {
                LOG_ERROR("[AVCT]AvctCbChAlloc Failed!");
                ret = AVCT_ERR_NO_RESOURCES;
            }
        }
    }
    if (ret == AVCT_SUCCESS && cbBr != NULL) {
        /* create br channel */
        AvctEvtData evtData;
        cbConn->cbDev->cbBr = cbBr;
        cbConn->cbDev->chLink |= AVCT_ALLOC_BR;
        cbConn->cbDev->cbBr->ia = AVCT_INIT;
        evtData.cbConn = cbConn;
        ret = AvctCbBrEvt(cbConn->cbDev, AVCT_BIND_EVT, &evtData);
    }
    return ret;
}