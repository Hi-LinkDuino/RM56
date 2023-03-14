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
#include "avctp_st.h"

/*****************************************************************************
 * Globle Data  Define
 ****************************************************************************/
/* state table for idle state */
const uint8_t C_AVCT_ST_IDLE[AVCT_MAX_EVT][AVCT_NUM_COLUMNS] = {
    /* Action          Next state         Event */
    {AVCT_CHANNEL_CONN, AVCT_OPENING_ST}, /* AVCT_BIND_EVT */
    {AVCT_CHANNEL_UNBIND, AVCT_IDLE_ST},  /* AVCT_UNBIND_EVT */
    {AVCT_DISCARD_MSG, AVCT_IDLE_ST},     /* AVCT_SED_MSG_EVT */
    {AVCT_IGNORE, AVCT_IDLE_ST},          /* AVCT_DISCONN_EVT */
    {AVCT_CONN_IND, AVCT_OPENED_ST},      /* AVCT_REV_CONN_EVT */
    {AVCT_DISCONN_IND, AVCT_IDLE_ST},     /* AVCT_REV_DISCONN_EVT */
    {AVCT_DISCARD_REV_MSG, AVCT_IDLE_ST}, /* AVCT_REV_MSG_EVT */
    {AVCT_IGNORE, AVCT_IDLE_ST}           /* AVCT_REV_BUSY_EVT */
};

const uint8_t C_AVCT_ST_OPENING[AVCT_MAX_EVT][AVCT_NUM_COLUMNS] = {
    /* Action          Next state               Event */
    {AVCT_IGNORE, AVCT_OPENING_ST},             /* AVCT_BIND_EVT */
    {AVCT_CHANNEL_UNBIND, AVCT_OPENING_ST},     /* AVCT_UNBIND_EVT */
    {AVCT_DISCARD_MSG, AVCT_OPENING_ST},        /* AVCT_SED_MSG_EVT */
    {AVCT_CHANNEL_DISCONNECT, AVCT_CLOSING_ST}, /* AVCT_DISCONN_EVT */
    {AVCT_CONN_IND, AVCT_OPENED_ST},            /* AVCT_REV_CONN_EVT */
    {AVCT_CONN_FAIL, AVCT_IDLE_ST},             /* AVCT_REV_DISCONN_EVT */
    {AVCT_DISCARD_REV_MSG, AVCT_OPENING_ST},    /* AVCT_REV_MSG_EVT */
    {AVCT_IGNORE, AVCT_OPENING_ST},             /* AVCT_REV_BUSY_EVT */
};

const uint8_t C_AVCT_ST_OPENED[AVCT_MAX_EVT][AVCT_NUM_COLUMNS] = {
    /* Action          Next state                    Event */
    {AVCT_CHANNEL_BIND, AVCT_OPENED_ST},             /* AVCT_BIND_EVT */
    {AVCT_CHANNEL_CHECK_DISCONNECT, AVCT_OPENED_ST}, /* AVCT_UNBIND_EVT */
    {AVCT_SEND_MSG, AVCT_OPENED_ST},                 /* AVCT_SED_MSG_EVT */
    {AVCT_CHANNEL_DISCONNECT, AVCT_CLOSING_ST},      /* AVCT_DISCONN_EVT */
    {AVCT_IGNORE, AVCT_OPENED_ST},                   /* AVCT_REV_CONN_EVT */
    {AVCT_DISCONN_IND, AVCT_IDLE_ST},                /* AVCT_REV_DISCONN_EVT */
    {AVCT_REV_MSG, AVCT_OPENED_ST},                  /* AVCT_REV_MSG_EVT */
    {AVCT_BUSY_IND, AVCT_BUSY_ST},                   /* AVCT_REV_BUSY_EVT */
};

const uint8_t C_AVCT_ST_CLOSING[AVCT_MAX_EVT][AVCT_NUM_COLUMNS] = {
    /* Action          Next state                Event */
    {AVCT_CHANNEL_BIND_FAIL, AVCT_CLOSING_ST}, /* AVCT_BIND_EVT */
    {AVCT_IGNORE, AVCT_CLOSING_ST},            /* AVCT_UNBIND_EVT */
    {AVCT_DISCARD_MSG, AVCT_CLOSING_ST},       /* AVCT_SED_MSG_EVT */
    {AVCT_IGNORE, AVCT_CLOSING_ST},            /* AVCT_DISCONN_EVT */
    {AVCT_IGNORE, AVCT_CLOSING_ST},            /* AVCT_REV_CONN_EVT */
    {AVCT_DISCONN_CFM, AVCT_IDLE_ST},          /* AVCT_REV_DISCONN_EVT */
    {AVCT_DISCARD_REV_MSG, AVCT_CLOSING_ST},   /* AVCT_REV_MSG_EVT */
    {AVCT_IGNORE, AVCT_CLOSING_ST},            /* AVCT_REV_BUSY_EVT */
};

const uint8_t C_AVCT_ST_BUSY[AVCT_MAX_EVT][AVCT_NUM_COLUMNS] = {
    /* Action          Next state                   Event */
    {AVCT_CHANNEL_BIND, AVCT_BUSY_ST},             /* AVCT_BIND_EVT */
    {AVCT_CHANNEL_CHECK_DISCONNECT, AVCT_BUSY_ST}, /* AVCT_UNBIND_EVT */
    {AVCT_DISCARD_MSG, AVCT_BUSY_ST},              /* AVCT_SED_MSG_EVT */
    {AVCT_CHANNEL_DISCONNECT, AVCT_CLOSING_ST},    /* AVCT_DISCONN_EVT */
    {AVCT_IGNORE, AVCT_BUSY_ST},                   /* AVCT_REV_CONN_EVT */
    {AVCT_DISCONN_IND, AVCT_IDLE_ST},              /* AVCT_REV_DISCONN_EVT */
    {AVCT_REV_MSG, AVCT_BUSY_ST},                  /* AVCT_REV_MSG_EVT */
    {AVCT_BUSY_IND, AVCT_BUSY_ST},                 /* AVCT_REV_BUSY_EVT */
};

/* state table */
const AvctStateTable C_AVCT_ST_TBL[AVCT_MAX_ST] = {
    C_AVCT_ST_IDLE,    /* Idle */
    C_AVCT_ST_OPENING, /* Opening */
    C_AVCT_ST_OPENED,  /* Opend */
    C_AVCT_ST_CLOSING, /* Closing */
    C_AVCT_ST_BUSY     /* Busy */
};

/*****************************************************************************
 * Function
 ****************************************************************************/
/*
 * Function     AvctGetNextStation
 * Description  Get next staton.
 * Param[in]    curState Current station.
 * Param[in]    event    Input event.
 * Return       uint8_t   Next station.
 */
uint8_t AvctGetNextStation(uint8_t curState, uint8_t event)
{
    AvctStateTable sTbl = C_AVCT_ST_TBL[curState];
    return sTbl[event][AVCT_NEXT_STATE];
}

/*
 * Function     AvctGetEvtAction
 * Description  Get the action index of the event.
 * Param[in]    curState Current station.
 * Param[in]    event    Input event.
 * Return       uint8_t  Action index.
 */
uint8_t AvctGetEvtAction(uint8_t curState, uint8_t event)
{
    AvctStateTable sTbl = C_AVCT_ST_TBL[curState];
    uint8_t action = sTbl[event][AVCT_ACTION];
    return action;
}
