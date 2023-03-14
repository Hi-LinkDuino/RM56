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
#ifndef AVCTP_ST_H
#define AVCTP_ST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro Define
 ****************************************************************************/
/* state table events */
typedef enum {
    AVCT_BIND_EVT,
    AVCT_UNBIND_EVT,
    AVCT_SED_MSG_EVT,
    AVCT_DISCONN_EVT,
    AVCT_REV_CONN_EVT,
    AVCT_REV_DISCONN_EVT,
    AVCT_REV_MSG_EVT,
    AVCT_REV_BUSY_EVT,
    AVCT_MAX_EVT,
} AvctEvt;

/* state table states */
typedef enum {
    AVCT_IDLE_ST,
    AVCT_OPENING_ST,
    AVCT_OPENED_ST,
    AVCT_CLOSING_ST,
    AVCT_BUSY_ST,
    AVCT_MAX_ST,
} AvctSt;

/* state table action enumeration list */
typedef enum {
    AVCT_CHANNEL_CONN,
    AVCT_CHANNEL_BIND,
    AVCT_CHANNEL_BIND_FAIL,
    AVCT_CHANNEL_DISCONNECT,
    AVCT_CHANNEL_UNBIND,
    AVCT_CHANNEL_CHECK_DISCONNECT,
    AVCT_SEND_MSG,
    AVCT_DISCARD_MSG,
    AVCT_CONN_IND,
    AVCT_CONN_FAIL,
    AVCT_DISCONN_IND,
    AVCT_DISCONN_CFM,
    AVCT_REV_MSG,
    AVCT_DISCARD_REV_MSG,
    AVCT_BUSY_IND,
    AVCT_UNBUSY_IND,
    AVCT_IGNORE,
    AVCT_ACTION_MAX,
} AvctAct;

/* state table information */
#define AVCT_ACTION 0      /* position of action */
#define AVCT_NEXT_STATE 1  /* position of next state */
#define AVCT_NUM_COLUMNS 2 /* number of columns in state tables */

/*****************************************************************************
 * Data Define
 ****************************************************************************/
/* type for state table */
typedef const uint8_t (*AvctStateTable)[AVCT_NUM_COLUMNS];
/*****************************************************************************
 * Globle Variable
 ****************************************************************************/
extern const uint8_t C_AVCT_ST_IDLE[AVCT_MAX_EVT][AVCT_NUM_COLUMNS];
extern const uint8_t C_AVCT_ST_OPENING[AVCT_MAX_EVT][AVCT_NUM_COLUMNS];
extern const uint8_t C_AVCT_ST_OPENED[AVCT_MAX_EVT][AVCT_NUM_COLUMNS];
extern const uint8_t C_AVCT_ST_CLOSING[AVCT_MAX_EVT][AVCT_NUM_COLUMNS];
extern const uint8_t C_AVCT_ST_BUSY[AVCT_MAX_EVT][AVCT_NUM_COLUMNS];
extern const AvctStateTable C_AVCT_ST_TBL[AVCT_MAX_ST];
/*****************************************************************************
 * Extern Function Declare
 ****************************************************************************/
extern uint8_t AvctGetNextStation(uint8_t curState, uint8_t event);
extern uint8_t AvctGetEvtAction(uint8_t curState, uint8_t event);

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_ST_H */