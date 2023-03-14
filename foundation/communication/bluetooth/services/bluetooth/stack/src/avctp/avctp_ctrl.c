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
#include "avctp_ctrl.h"
#include "avctp_ctrl_act.h"
#include "avctp_st.h"
#include "log.h"

/*****************************************************************************
 * Type  Define
 ****************************************************************************/
/* type for state table action functions */
typedef uint16_t (*AvctCtrlAction_t)(AvctCbDev *cbDev, const AvctEvtData *data);

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
const AvctCtrlAction_t C_AVCT_CTRL_ACTION[] = {
    AvctCbCtrlChConn,         /* AVCT_CHANNEL_CONN */
    AvctCbCtrlChBind,         /* AVCT_CHANNEL_BIND */
    AvctCbCtrlChBindFail,     /* AVCT_CHANNEL_BIND_FAIL */
    AvctCbCtrlChDisconn,      /* AVCT_CHANNEL_DISCONNECT */
    AvctCbCtrlChUnbind,       /* AVCT_CHANNEL_UNBIND */
    AvctCbCtrlChCheckDisconn, /* AVCT_CHANNEL_CHECK_DISCONNECT */
    AvctCbCtrlSendMsg,        /* AVCT_SEND_MSG */
    AvctCbCtrlDiscardMsg,     /* AVCT_DISCARD_MSG */
    AvctCbCtrlChConnInd,      /* AVCT_CONN_IND */
    AvctCbCtrlChConnFail,     /* AVCT_CONN_FAIL */
    AvctCbCtrlChCloseInd,     /* AVCT_DISCONN_IND */
    AvctCbCtrlChCloseCfm,     /* AVCT_DISCONN_CFM */
    AvctCbCtrlRevMsg,         /* AVCT_REV_MSG */
    AvctCbCtrlDiscardRevMsg,  /* AVCT_DISCARD_REV_MSG */
    AvctCbCtrlChBusyInd,      /* AVCT_BUSY_IND */
};

/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AvctCbCtrlEvt
 * Description  This function is called to sent event to event table,and call
 * the action. Param[in]   cbCtrl  point of the control channel block
 * Param[in]   event  event iD
 * Param[in]   data  date to send
 * Return      void
 */
uint16_t AvctCbCtrlEvt(AvctCbDev *cbDev, uint8_t event, const AvctEvtData *data)
{
    LOG_DEBUG("[AVCT] %{public}s: EventID(%hhu),curState(%hhu)", __func__, event, cbDev->cbCtrl->stState);
    uint16_t ret = AVCT_SUCCESS;
    AvctCbCh *cbCtrl = cbDev->cbCtrl;
    uint8_t curSt = cbCtrl->stState;
    uint8_t nextSt = AvctGetNextStation(curSt, event);
    uint8_t action = AvctGetEvtAction(curSt, event);
    /* Set next state */
    cbCtrl->stState = nextSt;

    /* excute actoin */
    if (action != AVCT_IGNORE) {
        ret = (*C_AVCT_CTRL_ACTION[action])(cbDev, data);
    }
    return ret;
}