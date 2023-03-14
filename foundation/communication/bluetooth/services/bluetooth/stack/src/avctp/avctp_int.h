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
#ifndef AVCTP_INT_H
#define AVCTP_INT_H

#include "avctp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro Define
 ****************************************************************************/
#define AVCT_MAX_DEVICES 6   /* The max number of peer devices tha can connected. */
#define AVCT_MAX_CONNECTS 14 /* The max number of connect session for app. */

/* State define for L2CAP channel */
typedef enum {
    AVCT_CH_IDLE = 0, /* no connection */
    AVCT_CH_CONN,     /* connection is requested */
    AVCT_CH_CFG,      /* config state */
    AVCT_CH_OPENED,   /* opened */
    AVCT_CH_BUSY,     /* l2cap channel is busy.can't send message */
} AvctChSt;

/* message packet header length */
#define AVCT_PKG_HDR_LEN_SINGLE 3
#define AVCT_PKG_HDR_LEN_START 4
#define AVCT_PKG_HDR_LEN_CONTINUE 1
#define AVCT_PKG_HDR_LEN_END 1

/* message packet type */
#define AVCT_PKT_TYPE_SINGLE 0   /* single packet */
#define AVCT_PKT_TYPE_START 1    /* start packet */
#define AVCT_PKT_TYPE_CONTINUE 2 /* continue packet */
#define AVCT_PKT_TYPE_END 3      /* end packet */

/* Command/Response/REJ indicator. */
#define AVCT_CMD 0 /* Command message */
#define AVCT_RSP 2 /* Response message */
#define AVCT_REJ 3 /* Message rejected */

#define AVCT_CONN_UNALLOC 0 /* connect unallocated  */
#define AVCT_CONN_ALLOC 1   /* connect allocated  */
#define AVCT_CONN_BIND 2    /* connect Binded */

#define AVCT_ALLOC_CTRL (1 << 0) /* allocated  ctrl */
#define AVCT_ALLOC_BR (1 << 1)   /* allocated br */

#define AVCT_L2C_CFG_REQ (1 << 0) /* config requesed */
#define AVCT_L2C_CFG_RSP (1 << 1) /* config responsed */

#define AVCT_CH_CTRL 0 /* Control channel */
#define AVCT_CH_BR 1   /* Browsing channel */

#define WAIT_TIME (-1)

#define AVCT_PARSE_SIGNLE_HDR(p, label, cr, ipid, pid)        \
    do {                                                      \
        (label) = *(p) >> 4;                                  \
        (cr) = (*(p) >> 1) & 1;                               \
        (ipid) = (*(p)) & 1;                                  \
        (pid) = (uint16_t)((p)[1] << 8) + (uint16_t)((p)[2]); \
    } while (0)

#define BT_ADDR_FMT_ASC(addr) addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
#define BT_ADDR_FMT_DSC(addr) addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]

#define AVCT_START_PKT_PID_OFFSET 2
#define AVCT_START_PKT_PID_SIZE 2

#define AVCT_32BYTE 32
#define BT_ADDR_LENGTH 6

/*****************************************************************************
 * Data Types
 ****************************************************************************/
/* channel control block type */
typedef struct {
    uint16_t peerMtu;  /* Control l2c mtu from peer device */
    uint16_t chResult; /* Connection result from L2CAP */
    uint16_t chId;     /* Channel ID from L2CAP */
    uint8_t chState;   /* Channel state */
    uint8_t chCfgSt;   /* Channel config state.There are two state(ind/cfm) */
    uint8_t stState;   /* The state table state */
    uint8_t ia;        /* Int(AVCT_INIT) or Acp(AVCT_ACP) */
    Packet *rxMsg;     /* The point to the message being reassembled */
} AvctCbCh;

/* device control block type */
typedef struct {
    BtAddr peerAddr;  /* BD address of peer device */
    AvctCbCh *cbCtrl; /* Associated ctrl channel ControlBlock */
    AvctCbCh *cbBr;   /* Associated browser channel ControlBlock */
    uint8_t bindCnt;  /* The count of connection which is binded to the device */
    uint8_t chLink;   /* 0:unlink, 1: ctrl linked 3:ctrl and br channe has been linked. */
    bool alloced;     /* false:unalloc, true :alloced */
} AvctCbDev;

/* connection session control block */
typedef struct {
    AvctConnectParam connParam; /* Connection creation parameters from app  */
    AvctCbDev *cbDev;           /* The point to the device ControlBlock */
    uint8_t status;             /* 0:unalloc; 1:alloc;2 :binded */
    uint8_t connId;             /* Connection Id */
} AvctCbConn;

/* Channel Management service for AVCT */
typedef struct {
    AvctCbConn cbConn[AVCT_MAX_CONNECTS]; /* Connection control blocks */
    AvctCbDev cbDev[AVCT_MAX_DEVICES];    /* Device control blocks */
    uint16_t rxMtu;                       /* Receive MTU for control channel */
    uint16_t rxMtuBr;                     /* Receive MTU for browsing channel */
    uint16_t role;
    bool registered;
} AvctChMng;

/* data type to send */
typedef struct {
    Packet *buf;        /* Packet buffer point */
    AvctCbConn *cbConn; /* The point to the connection control block */
    uint8_t label;      /* Transaction lable from app */
    uint8_t cr;         /* command / response */
} AvctTxMsg;

/* state table event data */
typedef union {
    AvctTxMsg txMsg;    /* Message to transfer */
    Packet *buf;        /* Packet buffer point */
    AvctCbConn *cbConn; /* The point to the connection control block */
    uint16_t result;    /* Deal result */
} AvctEvtData;

typedef struct {
    BtAddr addr;        /* peer address */
    uint16_t aclHandle; /* aclHandle */
    uint16_t lcid;      /* L2CAP link channel ID */
    uint8_t id;         /* id */
    uint16_t psm;       /* psm */
} AvctRevConnParam;

/*****************************************************************************
 * Globle Variable
 ****************************************************************************/
extern AvctChMng g_avctMng;

/*****************************************************************************
 * Extern Function Declare
 ****************************************************************************/
extern uint8_t AvctAsyncProcess(void (*callback)(void *context), void *context);

#ifdef __cplusplus
}
#endif

#endif /* AVCTP_INT_H */