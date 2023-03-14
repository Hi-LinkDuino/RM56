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

#ifndef AVDTP_INT_H
#define AVDTP_INT_H

#include "avdtp.h"

/**
 * Transport channel state
 **/
#define AVDT_TRANS_ST_IDLE 0         /* Init status,no connection */
#define AVDT_TRANS_ST_ACP 1          /* Acceptor: waiting for connection */
#define AVDT_TRANS_ST_INT 2          /* Init connection */
#define AVDT_TRANS_ST_CONNECTION 3   /* Initator: waiting for connection confirm */
#define AVDT_TRANS_ST_CFG 4          /* Waiting for configure confirm */
#define AVDT_TRANS_ST_COMPLETE 5     /* L2cap channel opened ok */
#define AVDT_TRANS_ST_SECURITY_INT 6 /* Initator: waiting for security confirm */
#define AVDT_TRANS_ST_SECURITY_ACP 7 /* Acceptor:waiting for security confirm */

/**
 * AVDTP service capabilities
 */
#define AVDT_PSC_MSK (AVDT_PSC_MSK_TRANS | AVDT_PSC_MSK_DELAY_RPT | AVDT_PSC_MSK_CODEC)

/**
 * Media packet header
 */
#define AVDT_MEDIA_OCTET1 0x80      /* First octect */
#define AVDT_BUFFER_MEDIA_HEADER 12 /* Header size */

/**
 * Transport table max size
 */
#define AVDT_CH_TABLE_SIZE (AVDT_NUM_LINKS * 2)

/**
 * L2cap config state
 */
#define AVDT_CFG_IND 1 << 0
#define AVDT_CFG_CFM 1 << 1
#define AVDT_CFG_END 3

/**
 * Stream link number. It's only surport audio stream now.
 */
#define AVDT_NUM_STREAM_LINKS 6

/**
 * Max numbers of stream endpoints
 */
#define AVDT_MAX_NUM_SEP (AVDT_NUM_STREAM_LINKS * AVDT_NUM_SEPS)

#define AVDT_1BYTE 1
#define AVDT_2BYTE 2
#define AVDT_4BYTE 4
#define AVDT_32BYTE 32
#define AVDT_OFFSET_1BIT 1
#define AVDT_OFFSET_2BIT 2
#define AVDT_OFFSET_3BIT 3
#define AVDT_OFFSET_4BIT 4
#define AVDT_OFFSET_8BIT 8
#define AVDT_OFFSET_16BIT 16
#define AVDT_OFFSET_24BIT 24

#define AVDT_TRANS_LENGTH 2
#define AVDT_DELAY_LENGTH 2

#define AVDT_SIG_LENGTH_COMM 1
#define AVDT_SIG_LENGTH_SETCONFIG 2
#define AVDT_SIG_LENGTH_DELAY_RPT 3

#define WAIT_TIME (-1)

#define AVDT_LABEL_MAX 128
#define AVDT_MEDIA_SEQ_MAX 65536

#define BT_ADDR_FMT_ASC(addr) addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
#define BT_ADDR_FMT_DSC(addr) addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]
#define BT_ADDR_LENGTH 6

#define AVDT_CODEC_OFFSET_LENGHT 0
#define AVDT_CODEC_OFFSET_MEDIA_TYPE 1
#define AVDT_CODEC_OFFSET_CODEC_TYPE 2

#define AVDT_MAKE_SEQUENCE_NO(p, sequence) \
    do {                                   \
        *(p)++ = ((sequence) >> 8);        \
        *(p)++ = (uint8_t)(sequence);      \
    } while (0)

#define AVDT_MAKE_TIME_STAMP(p, timeStamp)     \
    do {                                       \
        *(p)++ = (uint8_t)((timeStamp) >> 24); \
        *(p)++ = (uint8_t)((timeStamp) >> 16); \
        *(p)++ = (uint8_t)((timeStamp) >> 8);  \
        *(p)++ = (uint8_t)(timeStamp);         \
    } while (0)

#define AVDT_MAKE_SSRC(p, ssrc)           \
    do {                                  \
        *(p)++ = (uint8_t)((ssrc) >> 24); \
        *(p)++ = (uint8_t)((ssrc) >> 16); \
        *(p)++ = (uint8_t)((ssrc) >> 8);  \
        *(p)++ = (uint8_t)(ssrc);         \
    } while (0)

/**
 * Stream state machine states
 */
enum {
    AVDT_IDLE_ST,
    AVDT_CONFIGURE_ST,
    AVDT_OPENING_ST,
    AVDT_OPEN_ST,
    AVDT_STREAMING_ST,
    AVDT_CLOSING_ST,
};

/**
 * Signal state machine states
 */
enum {
    AVDT_SIG_IDLE_ST,
    AVDT_SIG_CONFIGURE_ST,
    AVDT_SIG_OPEN_ST,
    AVDT_SIG_CLOSING_ST,
};

/**
 * Stream State machine action enumeration list
 */
enum {
    AVDT_SETCONFIGURE_REQ, /* 0 */
    AVDT_SETCONFIGURE_RSP, /* 1 */
    AVDT_SETCONFIGURE_IND, /* 2 */
    AVDT_SETCONFIGURE_CFM, /* 3 */
    AVDT_GETCONFIGURE_REQ, /* 4 */
    AVDT_GETCONFIGURE_RSP, /* 5 */
    AVDT_GETCONFIGURE_IND, /* 6 */
    AVDT_GETCONFIGURE_CFM, /* 7 */
    AVDT_OPEN_REQ,         /* 8 */
    AVDT_OPEN_RSP,         /* 9 */
    AVDT_OPEN_IND,         /* 10 */
    AVDT_OPEN_CFM,         /* 11 */
    AVDT_RECONFIGURE_REQ,  /* 12 */
    AVDT_RECONFIGURE_RSP,  /* 13 */
    AVDT_RECONFIGURE_IND,  /* 14 */
    AVDT_RECONFIGURE_CFM,  /* 15 */
    AVDT_START_REQ,        /* 16 */
    AVDT_START_RSP,        /* 17 */
    AVDT_START_IND,        /* 18 */
    AVDT_START_CFM,        /* 19 */
    AVDT_SUSPEND_REQ,      /* 20 */
    AVDT_SUSPEND_RSP,      /* 21 */
    AVDT_SUSPEND_IND,      /* 22 */
    AVDT_SUSPEND_CFM,      /* 23 */
    AVDT_CLOSE_REQ,        /* 24 */
    AVDT_CLOSE_RSP,        /* 25 */
    AVDT_CLOSE_IND,        /* 26 */
    AVDT_CLOSE_CFM,        /* 27 */
    AVDT_ABORT_REQ,        /* 28 */
    AVDT_ABORT_RSP,        /* 29 */
    AVDT_ABORT_IND,        /* 30 */
    AVDT_ABORT_CFM,        /* 31 */
    AVDT_DELAY_REQ,        /* 32 */
    AVDT_DELAY_RSP,        /* 33 */
    AVDT_DELAY_IND,        /* 34 */
    AVDT_DELAY_CFM,        /* 35 */
    AVDT_WRITE_REQ,        /* 36 */
    AVDT_WRITE_CFM,        /* unuse */
    AVDT_READ_IND,         /* unuse */
    AVDT_STREAM_OPEN_REQ,  /* 39 */
    AVDT_STREAM_OPEN_RSP,  /* 40 */
    AVDT_STREAM_OPEN_IND,  /* 41 */
    AVDT_STREAM_OPEN_CFM,  /* 42 */
    AVDT_STREAM_CLOSE_REQ, /* unuse */
    AVDT_STREAM_CLOSE_RSP, /* unuse */
    AVDT_STREAM_CLOSE_IND, /* 45 */
    AVDT_STREAM_CLOSE_CFM, /* 46 */
    AVDT_BAD_STATE_REJ,    /* 47 */
    AVDT_STREAM_ACTION_NUM /* 48 */
};
/**
 * Signal State machine action enumeration list
 */
enum {
    AVDT_DISCOVER_REQ,   /* 0 */
    AVDT_DISCOVER_RSP,   /* 1 */
    AVDT_DISCOVER_IND,   /* 2 */
    AVDT_DISCOVER_CFM,   /* 3 */
    AVDT_GETCAP_REQ,     /* 4 */
    AVDT_GETCAP_RSP,     /* 5 */
    AVDT_GETCAP_IND,     /* 6 */
    AVDT_GETCAP_CFM,     /* 7 */
    AVDT_GETALLCAP_REQ,  /* 8 */
    AVDT_GETALLCAP_RSP,  /* 9 */
    AVDT_GETALLCAP_IND,  /* 10 */
    AVDT_GETALLCAP_CFM,  /* 11 */
    AVDT_CONNECT_REQ,    /* 12 */
    AVDT_CONNECT_RSP,    /* 13 */
    AVDT_CONNECT_IND,    /* 14 */
    AVDT_CONNECT_CFM,    /* 15 */
    AVDT_DISCONNECT_REQ, /* 16 */
    AVDT_DISCONNECT_RSP, /* 17 */
    AVDT_DISCONNECT_IND, /* 18 */
    AVDT_DISCONNECT_CFM, /* 19 */
    AVDT_SND_MSG,        /* 20 */
    AVDT_SIG_ACTION_NUM
};

#define AVDT_SSM_IGNORE (AVDT_STREAM_ACTION_NUM + 10)

/**
 * Stream State machine events
 */
enum {
    AVDT_SETCONFIGURE_CMD_REQ_EVENT, /* 0 */
    AVDT_SETCONFIGURE_CMD_RSP_EVENT, /* 1 */
    AVDT_SETCONFIGURE_CMD_IND_EVENT, /* 2 */
    AVDT_SETCONFIGURE_CMD_CFM_EVENT, /* 3 */
    AVDT_GETCONFIGURE_CMD_REQ_EVENT, /* 4 */
    AVDT_GETCONFIGURE_CMD_RSP_EVENT, /* 5 */
    AVDT_GETCONFIGURE_CMD_IND_EVENT, /* 6 */
    AVDT_GETCONFIGURE_CMD_CFM_EVENT, /* 7 */
    AVDT_OPEN_CMD_REQ_EVENT,         /* 8 */
    AVDT_OPEN_CMD_RSP_EVENT,         /* 9 */
    AVDT_OPEN_CMD_IND_EVENT,         /* 10 */
    AVDT_OPEN_CMD_CFM_EVENT,         /* 11 */
    AVDT_RECONFIGURE_CMD_REQ_EVENT,  /* 12 */
    AVDT_RECONFIGURE_CMD_RSP_EVENT,  /* 13 */
    AVDT_RECONFIGURE_CMD_IND_EVENT,  /* 14 */
    AVDT_RECONFIGURE_CMD_CFM_EVENT,  /* 15 */
    AVDT_START_CMD_REQ_EVENT,        /* 16 */
    AVDT_START_CMD_RSP_EVENT,        /* 17 */
    AVDT_START_CMD_IND_EVENT,        /* 18 */
    AVDT_START_CMD_CFM_EVENT,        /* 19 */
    AVDT_SUSPEND_CMD_REQ_EVENT,      /* 20 */
    AVDT_SUSPEND_CMD_RSP_EVENT,      /* 21 */
    AVDT_SUSPEND_CMD_IND_EVENT,      /* 22 */
    AVDT_SUSPEND_CMD_CFM_EVENT,      /* 23 */
    AVDT_CLOSE_CMD_REQ_EVENT,        /* 24 */
    AVDT_CLOSE_CMD_RSP_EVENT,        /* 25 */
    AVDT_CLOSE_CMD_IND_EVENT,        /* 26 */
    AVDT_CLOSE_CMD_CFM_EVENT,        /* 27 */
    AVDT_ABORT_CMD_REQ_EVENT,        /* 28 */
    AVDT_ABORT_CMD_RSP_EVENT,        /* 29 */
    AVDT_ABORT_CMD_IND_EVENT,        /* 30 */
    AVDT_ABORT_CMD_CFM_EVENT,        /* 31 */
    AVDT_DELAY_CMD_REQ_EVENT,        /* 32 */
    AVDT_DELAY_CMD_RSP_EVENT,        /* 33 */
    AVDT_DELAY_CMD_IND_EVENT,        /* 34 */
    AVDT_DELAY_CMD_CFM_EVENT,        /* 35 */
    AVDT_WRITE_CMD_REQ_EVENT,        /* 36 */
    AVDT_WRITE_CMD_CFM_EVENT,        /* 37 */
    AVDT_READ_CMD_IND_EVENT,         /* 38 */
    AVDT_STREAM_OPEN_CMD_REQ_EVENT,  /* 39 */
    AVDT_STREAM_OPEN_CMD_RSP_EVENT,  /* 40 */
    AVDT_STREAM_OPEN_CMD_IND_EVENT,  /* 41 */
    AVDT_STREAM_OPEN_CMD_CFM_EVENT,  /* 42 */
    AVDT_STREAM_CLOSE_CMD_REQ_EVENT, /* 43 */
    AVDT_STREAM_CLOSE_CMD_RSP_EVENT, /* 44 */
    AVDT_STREAM_CLOSE_CMD_IND_EVENT, /* 45 */
    AVDT_STREAM_CLOSE_CMD_CFM_EVENT, /* 46 */
    AVDT_BAD_STATE_REJ_EVENT,        /* 47 */
    AVDT_STREAM_ACTIONS_NUM_EVENT    /* 48 */
};
/**
 * Signal State machine events
 */
enum {
    AVDT_DISCOVER_CMD_REQ_EVENT,   /* 0 */
    AVDT_DISCOVER_CMD_RSP_EVENT,   /* 1 */
    AVDT_DISCOVER_CMD_IND_EVENT,   /* 2 */
    AVDT_DISCOVER_CMD_CFM_EVENT,   /* 3 */
    AVDT_GETCAP_CMD_REQ_EVENT,     /* 4 */
    AVDT_GETCAP_CMD_RSP_EVENT,     /* 5 */
    AVDT_GETCAP_CMD_IND_EVENT,     /* 6 */
    AVDT_GETCAP_CMD_CFM_EVENT,     /* 7 */
    AVDT_GETALLCAP_CMD_REQ_EVENT,  /* 8 */
    AVDT_GETALLCAP_CMD_RSP_EVENT,  /* 9 */
    AVDT_GETALLCAP_CMD_IND_EVENT,  /* 10 */
    AVDT_GETALLCAP_CMD_CFM_EVENT,  /* 11 */
    AVDT_CONNECT_CMD_REQ_EVENT,    /* 12 */
    AVDT_CONNECT_CMD_RSP_EVENT,    /* 13 */
    AVDT_CONNECT_CMD_IND_EVENT,    /* 14 */
    AVDT_CONNECT_CMD_CFM_EVENT,    /* 15 */
    AVDT_DISCONNECT_CMD_REQ_EVENT, /* 16 */
    AVDT_DISCONNECT_CMD_RSP_EVENT, /* 17 */
    AVDT_DISCONNECT_CMD_IND_EVENT, /* 18 */
    AVDT_DISCONNECT_CMD_CFM_EVENT, /* 19 */
    AVDT_SND_MSG_EVENT,            /* 20 */
    AVDT_SIG_ACTIONS_NUM_EVENT     /* 21 */
};
/**
 * Define the header of each buffer used in the Avdt.
 */
typedef struct {
    Packet *data;
    uint16_t event;
    uint16_t len;
    uint8_t transLabel;
} AvdtHeader;

/**
 * Data type for AVDT_DISCOVER_REQ_EVENT
 */
typedef struct {
    AvdtSepInfo *sepInformation;
    uint8_t numSeps;
} AvdtDiscoverData;

/**
 * Data type for AVDT_GETCAP_REQ_EVENT
 */
typedef struct {
    AvdtEvtHeader single;
    AvdtSepConfig *cfg;
} AvdtGetCapData;

/**
 * Type for AVDT_SCB_API_WRITE_REQ_EVENT
 */
typedef struct {
    Packet *pkt;
    uint32_t timeStamp;
    uint8_t payloadType;
    uint8_t marker;
} AvdtStreamData;

/**
 * Msg union of all message parameter types
 */
typedef union {
    AvdtEvtHeader single;
    AvdtSetConfig configureCmd;
    AvdtConfig reconfigureCmd;
    AvdtDiscover discoverRsp;
    AvdtDelayRpt delayRptCmd;
    AvdtGetCapData getcap;
} AvdtMsg;
/**
 * Union associated with sigCtrl state machine events
 */
typedef union {
    AvdtDiscoverData discover;
    AvdtGetCapData getcap;
    AvdtMsg msg;
    AvdtStreamData apiWrite;
    AvdtDelayRpt apiDelay;
    Packet *pkt;
    AvdtCtrlData ctrlData;
    uint8_t errCode;
} AvdtEventData;

/**
 * AVDTP Stream Control Block.
 */
typedef struct {
    BtAddr peerAddress;
    AvdtSetConfig currCfg; /* Current configuration */
    AvdtSepConfig reqCfg;  /* Requested configuration */
    Packet *pkt;           /* Packet waiting to be sent */
    uint16_t codecIndex;   /* index of local sep */
    uint16_t mediaSeq;     /* Media packet sequence number */
    uint8_t role;          /* Initiator/acceptor role in current procedure */
    uint8_t state;         /* State machine state */
    uint8_t peerSeid;      /* SEID of peer stream */
    uint8_t currEvt;       /* current event; set only by the state machine */
    uint8_t sigHandle;     /* Avdtpccb entry */
    uint16_t handle;       /* Unique handle for this AvdtpScb entry */
    bool isAllocated;      /* True if the SCB is isAllocated */
    bool isUsed;           /* True if used by peer */
} AvdtStreamCtrl;

/**
 * AVDTP Channel Control Block.
 */
typedef struct {
    BtAddr peerAddress;
    AvdtCtrlCallback procCback; /* Procedure callback function */
    Packet *currentMsg;         /* Current message being sent */
    Packet *rxMsg;              /* Current message being received */
    uint8_t state;              /* The channel state machine state */
    uint8_t procParam;          /* Either SEID for get capabilities or number of SEPS for discover */
    uint8_t rcvLabel;           /* Message header "label" (sequence number) */
    uint8_t label;
    uint8_t numbPakcket;
    uint8_t role;
    uint8_t ia; /* AVDT_INT or AVDT_ACP */
    uint8_t rcvSignal;
    uint8_t msgType;
    uint16_t handle;
    uint16_t streamHandle;
    uint16_t lcid;
    bool isProcBusy; /* True when a discover or get capabilities procedure in progress */
    bool isFailReport;
    AvdtStreamCtrl streamCtrl[AVDT_NUM_SEPS];
    uint8_t edr;
} AvdtSigCtrl;

typedef struct {
    uint16_t handle;
    bool isUsed;
} AvdtChannelHandle;

/**
 * AVDTP transport channel
 */
typedef struct {
    BtAddr addr;
    uint16_t peerMtu;     /* L2CAP MTU of the peer device */
    uint16_t peerFlushTo; /* Our flush timeout for this channel */
    uint16_t lcid;
    uint16_t sigHandle;    /* Channel control handle related with this channel */
    uint16_t streamHandle; /* Stream control handle related with the channel */
    uint8_t type;          /* Transport type: signaling channel, media channel */
    uint8_t state;         /* Transport channel state */
    uint8_t cfgFlags;      /* L2CAP configuration flags */
    uint8_t id;
} AvdtTransChannel;

typedef struct {
    AvdtStreamConfig localConfigure;
    bool isExited;
} AvdtStreamEndpoint;

/**
 * Types for action functions.
 */
typedef uint16_t (*AvdtStreamAction)(AvdtStreamCtrl *streamCtrl, AvdtEventData *data);
typedef uint16_t (*AvdtSigAction)(AvdtSigCtrl *sigCtrl, const AvdtEventData *data);

/**
 * Control block for AVDTP.
 */
typedef struct {
    AvdtRegisterParam regInfo;                         /* Registration control block */
    AvdtSigCtrl *sigCtrl[AVDT_NUM_LINKS];              /* Channel control blocks */
    AvdtTransChannel *transTable[AVDT_CH_TABLE_SIZE];  /* Record the lcid of Signaling and  media */
    const AvdtStreamAction *streamAction;              /* Pointer to stream action functions */
    const AvdtSigAction *sigAction;                    /* Pointer to signal action functions */
    AvdtChannelHandle streamHandles[AVDT_MAX_NUM_SEP]; /* SEP handles */
    AvdtChannelHandle sigHandles[AVDT_NUM_LINKS];      /* Channel ctrl handles */
    AvdtStreamEndpoint localSEP[AVDT_NUM_SEPS];        /* Local stream endpoint */
    bool avdtRegisted;
    bool sepRegisted;
} AvdtCB;

/**
 * Global data
 */

/**
 * L2CAP callback registration structure
 */

extern void AvdtPktDataPrint(const Packet *pkt);
#endif /* AVDTP_INT_H */