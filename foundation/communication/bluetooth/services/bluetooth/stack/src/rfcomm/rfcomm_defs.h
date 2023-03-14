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

#ifndef RFCOMM_DEFS_H
#define RFCOMM_DEFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "btm/btm_thread.h"
#include "btstack.h"
#include "gap_if.h"
#include "l2cap_if.h"
#include "log.h"
#include "packet.h"
#include "platform/include/alarm.h"
#include "platform/include/event.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/random.h"
#include "rfcomm.h"
#include "securec.h"

#define BT_PSM_RFCOMM 3
#define RFCOMM_L2CAP_MTU 1500
#define RFCOMM_L2CAP_DEFAUT_MTU L2CAP_DEFAULT_MTU
#define RFCOMM_HEAD_MAX_LEN 6  // Address(1),control(1),length(2),credit(1),tail(1)
#define RFCOMM_LOCAL_DEFAULT_MTU (RFCOMM_L2CAP_MTU - RFCOMM_HEAD_MAX_LEN)
#define RFCOMM_PEER_DEFAULT_MTU 127
#define MAX_SESSION_COUNT BT_CONNECT_NUM_MAX
#define MAX_SERVER_COUNT 30
#define MAX_DLC_COUNT 36
#define MAX_CREDIT_COUNT 10
#define MAX_QUEUE_COUNT MAX_CREDIT_COUNT
#define MAX_ONCE_NEWCREDIT 255

#define FRAME_TYPE_SABM 0b00101111
#define FRAME_TYPE_UA 0b01100011
#define FRAME_TYPE_DM 0b00001111
#define FRAME_TYPE_DISC 0b01000011
#define FRAME_TYPE_UIH 0b11101111

#define UIH_TYPE_PN 0b00100000
#define UIH_TYPE_MSC 0b00111000
#define UIH_TYPE_RPN 0b00100100
#define UIH_TYPE_RLS 0b00010100
#define UIH_TYPE_TEST 0b00001000
#define UIH_TYPE_FCON 0b00101000
#define UIH_TYPE_FCOFF 0b00011000
#define UIH_TYPE_NSC 0b00000100

#define EA 0b00000001
#define PF 0b00010000
#define CONTROL_DLCI 0x00

// Common command header information
#define RFCOMM_ADDRESS 0x00
#define RFCOMM_CONTROL 0x01
#define RFCOMM_LENGTH_1 0x02
#define RFCOMM_LENGTH_2 0x03
#define RFCOMM_INFO_TYPE 0x03
#define RFCOMM_INFO_LEN 0x04
// PN command
#define RFCOMM_PN_DLCI 0x05
#define RFCOMM_PN_CL 0x06
#define RFCOMM_PN_PRIORITY 0x07
#define RFCOMM_PN_TIME 0x08
#define RFCOMM_PN_MTU1 0x09
#define RFCOMM_PN_MTU2 0x0A
#define RFCOMM_PN_NA 0x0B
#define RFCOMM_PN_CREDIT 0x0C
// MSC command
#define RFCOMM_MSC_DLCI 0x05
#define RFCOMM_MSC_SIGNAL 0x06
#define RFCOMM_MSC_BREAK 0x07
// RLS command
#define RFCOMM_RLS_DLCI 0x05
#define RFCOMM_RLS_STATUS 0x06
// RPN command
#define RFCOMM_RPN_DLCI 0x05
#define RFCOMM_RPN_BAUDRATE 0x06
#define RFCOMM_RPN_BITINFO 0x07
#define RFCOMM_RPN_FLC 0x08
#define RFCOMM_RPN_XON 0x09
#define RFCOMM_RPN_XOFF 0x0A
#define RFCOMM_RPN_MASK1 0x0B
#define RFCOMM_RPN_MASK2 0x0C
// NSC command
#define RFCOMM_NSC_TYPE 0x05

#define RFCOMM_OFFSET_0 0x00
#define RFCOMM_OFFSET_1 0x01
#define RFCOMM_OFFSET_2 0x02
#define RFCOMM_OFFSET_3 0x03
#define RFCOMM_OFFSET_4 0x04
#define RFCOMM_OFFSET_5 0x05
#define RFCOMM_OFFSET_6 0x06
#define RFCOMM_OFFSET_7 0x07

// FCS calculation length
#define RFCOMM_IS_UIH_FSC_LEN 0x02
#define RFCOMM_NOT_UIH_FSC_LEN 0x03

#define RFCOMM_SHIFT_CR 0x01
#define RFCOMM_SHIFT_DLCI 0x02
#define RFCOMM_SHIFT_PF 0x04
#define RFCOMM_SHIFT_TYPE 0x02
#define RFCOMM_SHIFT_LENGTH2 0x07
#define RFCOMM_DLCI_SHIFT_SCN 0x01
#define RFCOMM_RLS_SHIFT_OVERRUN 0x01
#define RFCOMM_RLS_SHIFT_PARITY 0x02
#define RFCOMM_RLS_SHIFT_FRAMING 0x03
#define RFCOMM_MSC_SHIFT_BREAKLEN 0x03
#define RFCOMM_PN_SHIFT_CL 0x04
#define RFCOMM_PN_SHIFT_MTU 0x08
#define RFCOMM_RPN_SHIFT_STOP_BIT 0x02
#define RFCOMM_RPN_SHIFT_PARITY 0x03
#define RFCOMM_RPN_SHIFT_PARITY_TYPE 0x04

#define IS_DLCI_VALID(dlci) ((dlci) >= 2 && (dlci) <= 61)
#define IS_MTU_VALID(mtu) ((mtu) >= 23 && (mtu) <= 32767)
#define IS_CMD(isInitiator, cr) (((isInitiator) && !(cr)) || (!(isInitiator) && (cr)))
#define IS_RSP(isInitiator, cr) (((isInitiator) && (cr)) || (!(isInitiator) && !(cr)))
#define IS_FIRST_CMD(event)                                                                                        \
    ((event) == EV_CHANNEL_RECV_SABM || (event) == EV_CHANNEL_RECV_PN_REQ || (event) == EV_CHANNEL_RECV_RPN_CMD || \
        (event) == EV_CHANNEL_RECV_RPN_REQ)
#define IS_DISCONNECTED(event)                                                                 \
    ((event) == RFCOMM_CHANNEL_EV_CONNECT_FAIL || (event) == RFCOMM_CHANNEL_EV_DISCONNECTED || \
        (event) == RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS)

#define CL_REQ_SUPPORT_CREDIT 0x0F
#define CL_RSP_SUPPORT_CREDIT 0x0E
#define CL_REQ_UNSUPPORTED_CREDIT 0x00
#define CL_RSP_UNSUPPORTED_CREDIT 0x00
#define DEFAULT_CREDITS_VALUE 7

// Timer value(s)
#define RFCOMM_PER_SEC 1000
#define T1_SABM_DISC 20      // The timeout for SABM and DISC frames in RFCOMM).
#define T1_SABM_OPEN_DLC 60  // The timeout for SABM frame to start a new DLC (with DLCI > 0).
#define T2_UIH_DLCI0 20      // The timeout for commands sent in UIH frames on DLCI 0.

#define RFCOMM_NUM_TEN 10

// Transfer status
#define MSC_CMD_SEND 0b001
#define MSC_CMD_RECV 0b010
#define MSC_RSP_RECV 0b100
#define TRANSFER_READY 0b111

// Information length
#define RFCOMM_PN_INFO_LEN 0x08
#define RFCOMM_RLS_INFO_LEN 0x02
#define RFCOMM_RPN_REQINFO_LEN 0x01
#define RFCOMM_RPN_CMDINFO_LEN 0x08
#define RFCOMM_MSC_INFO_HAVEBREAK_LEN 0x03
#define RFCOMM_MSC_INFO_NO_BREAK_LEN 0x02
#define RFCOMM_NSC_INFO_LEN 0x01
// Frame length(info:type 1byte + len 1byte + info length)
#define RFCOMM_PN_FRAME_LEN (RFCOMM_PN_INFO_LEN + 2)
#define RFCOMM_RLS_FRAME_LEN (RFCOMM_RLS_INFO_LEN + 2)
#define RFCOMM_RPN_REQFRAME_LEN (RFCOMM_RPN_REQINFO_LEN + 2)
#define RFCOMM_RPN_CMDFRAME_LEN (RFCOMM_RPN_CMDINFO_LEN + 2)
#define RFCOMM_MSC_FRAME_HAVEBREAK_LEN (RFCOMM_MSC_INFO_HAVEBREAK_LEN + 2)
#define RFCOMM_MSC_FRAME_NO_BREAK_LEN (RFCOMM_MSC_INFO_NO_BREAK_LEN + 2)
#define RFCOMM_FCON_FRAME_LEN 0x02
#define RFCOMM_FCOFF_FRAME_LEN 0x02
#define RFCOMM_NSC_FRAME_LEN (RFCOMM_NSC_INFO_LEN + 2)
#define RFCOMM_TEST_FRAME_LEN_MIN 0x02
// Total header length
#define RFCOMM_SABM_HEADER_LEN 0x03
#define RFCOMM_DISC_HEADER_LEN 0x03
#define RFCOMM_UA_HEADER_LEN 0x03
#define RFCOMM_DM_HEADER_LEN 0x03
#define RFCOMM_PN_HEADER_LEN (RFCOMM_PN_FRAME_LEN + 3)
#define RFCOMM_RLS_HEADER_LEN (RFCOMM_RLS_FRAME_LEN + 3)
#define RFCOMM_RPN_REQHEADER_LEN (RFCOMM_RPN_REQFRAME_LEN + 3)
#define RFCOMM_RPN_CMDHEADER_LEN (RFCOMM_RPN_CMDFRAME_LEN + 3)
#define RFCOMM_MSC_HAVEBREAK_HEADER_LEN (RFCOMM_MSC_FRAME_HAVEBREAK_LEN + 3)
#define RFCOMM_MSC_NO_BREAK_HEADER_LEN (RFCOMM_MSC_FRAME_NO_BREAK_LEN + 3)
#define RFCOMM_FCON_HEADER_LEN (RFCOMM_FCON_FRAME_LEN + 3)
#define RFCOMM_FCOFF_HEADER_LEN (RFCOMM_FCOFF_FRAME_LEN + 3)
#define RFCOMM_NSC_HEADER_LEN (RFCOMM_NSC_FRAME_LEN + 3)
#define RFCOMM_TEST_HEADER_LEN_MAX 0x07
#define RFCOMM_DATA_HEADER_LEN_MAX 0x05

// Default rpn value
#define BAUDRATE_9600 0x03
#define DATA_BIT_8 0x08
#define STOP_BIT_1 0x00
#define NO_PARITY 0x00
#define ODD_PARITY 0x00
#define NO_FLC 0x00
#define XON_DC1 0x11
#define XOFF_DC3 0x13

// Remote port line status
#define OVERRUN_ERROR 0b001  // Received character overwrote an unread character
#define PARITY_ERROR 0b010   // Received character’s parity was incorrect
#define FRAMING_ERROR 0b100  // a character did not terminate with a stop bit

typedef enum {
    EVENT_UNKNOWN = 0,
    EVENT_SESSION,
    EVENT_CHANNEL,
    EVENT_FRAME_ERR,
    EVENT_UNSUPPORTED_CMD
} RfcommEventType;

typedef enum {
    EV_SESSION_SEND_OPEN_REQ = 0,
    EV_SESSION_SEND_CLOSE_REQ,
    EV_SESSION_RECV_REQ_CONNECT_RESULT,
    EV_SESSION_RECV_SECURITY_RESULT,
    EV_SESSION_RECV_CONNECT_REQ,
    EV_SESSION_RECV_CONNECT_RSP,
    EV_SESSION_RECV_CONFIG_REQ,
    EV_SESSION_RECV_CONFIG_RSP,
    EV_SESSION_RECV_DISCONNECT_REQ,
    EV_SESSION_RECV_SABM0,
    EV_SESSION_RECV_DISC0,
    EV_SESSION_RECV_UA0,
    EV_SESSION_RECV_DM0,
    EV_SESSION_RECV_FCON_REQ,
    EV_SESSION_RECV_FCON_RSP,
    EV_SESSION_RECV_FCOFF_REQ,
    EV_SESSION_RECV_FCOFF_RSP,
    EV_SESSION_RECV_TEST_REQ,
    EV_SESSION_RECV_TEST_RSP,
    EV_SESSION_RECV_NSC,
    EV_SESSION_RECV_LINK_LOSS,
    EV_SESSION_TIMEOUT,
    EV_SESSION_EV_MAX
} RfcommSessionEvent;

typedef enum {
    EV_CHANNEL_SEND_OPEN_REQ = 0,
    EV_CHANNEL_SEND_CLOSE_REQ,
    EV_CHANNEL_RECV_SECURITY_RESULT,
    EV_CHANNEL_RECV_SABM,
    EV_CHANNEL_RECV_DISC,
    EV_CHANNEL_RECV_UA,
    EV_CHANNEL_RECV_DM,
    EV_CHANNEL_RECV_PN_REQ,
    EV_CHANNEL_RECV_PN_RSP,
    EV_CHANNEL_RECV_MSC_REQ,
    EV_CHANNEL_RECV_MSC_RSP,
    EV_CHANNEL_RECV_RPN_CMD,
    EV_CHANNEL_RECV_RPN_REQ,
    EV_CHANNEL_RECV_RPN_RSP,
    EV_CHANNEL_RECV_RLS_REQ,
    EV_CHANNEL_RECV_RLS_RSP,
    EV_CHANNEL_ACCEPT,
    EV_CHANNEL_REJECT,
    EV_CHANNEL_RECV_DATA,
    EV_CHANNEL_WRITE_DATA,
    EV_CHANNEL_SEND_RPN_CMD,
    EV_CHANNEL_SEND_RPN_REQ,
    EV_CHANNEL_SEND_RLS_REQ,
    EV_CHANNEL_SEND_MSC_REQ,
    EV_CHANNEL_TIMEOUT,
    EV_CHANNEL_EV_MAX
} RfcommChannelEvent;

typedef enum { FC_TYPE_UNKNOWN = 0, FC_TYPE_NORMAL, FC_TYPE_CREDIT } RfcommFlowControlType;

typedef enum { PN_REQ = 0, PN_RSP_BEFORE_CREATE, PN_RSP_AFTER_CREATE } RfcommPnCmdKind;

typedef enum {
    ST_SESSION_CLOSED = 0,
    ST_SESSION_INITIATOR_WAIT_SECURITY_RESULT,
    ST_SESSION_WAIT_CONNECT_REQ_RESULT,
    ST_SESSION_WAIT_CONNECT_RSP,
    ST_SESSION_INITIATOR_WAIT_CONFIG_REQ_AND_RSP,
    ST_SESSION_INITIATOR_WAIT_CONFIG_REQ,
    ST_SESSION_INITIATOR_WAIT_CONFIG_RSP,
    ST_SESSION_SABM0_REQ_WAIT_UA0,
    ST_SESSION_RESPONDER_WAIT_SECURITY_RESULT,
    ST_SESSION_RESPONDER_WAIT_CONFIG_REQ_AND_RSP,
    ST_SESSION_RESPONDER_WAIT_CONFIG_REQ,
    ST_SESSION_RESPONDER_WAIT_CONFIG_RSP,
    ST_SESSION_WAIT_SABM0,
    ST_SESSION_DISC0_REQ_WAIT_UA0,
    ST_SESSION_WAIT_RESTART,
    ST_SESSION_CLOSING,
    ST_SESSION_CONNECTED
} RfcommSessionState;

typedef enum {
    ST_CHANNEL_CLOSED = 0,
    ST_CHANNEL_WAIT_PN_RSP,
    ST_CHANNEL_CLIENT_WAIT_SECURITY_RESULT,
    ST_CHANNEL_SABM_REQ_WAIT_UA,
    ST_CHANNEL_WAIT_UPPER_RESPONSE,
    ST_CHANNEL_WAIT_SABM,
    ST_CHANNEL_SERVER_WAIT_SECURITY_RESULT,
    ST_CHANNEL_DISC_REQ_WAIT_UA,
    ST_CHANNEL_WAIT_RESTART,
    ST_CHANNEL_CONNECTED
} RfcommChannelState;

typedef struct {
#define MSC_FC 0b00000001
#define MSC_RTC 0b00000010
#define MSC_RTR 0b00000100
#define MSC_IC 0b00100000
#define MSC_DV 0b01000000
    uint8_t signals;
#define MSC_BREAK 0b00000001
#define MSC_BREAK_LEN 0b01111000
    uint8_t breakSignal;
} RfcommModemStatusInfo;

typedef struct {
    uint8_t scn;
    uint16_t maxRevSize;
    uint32_t eventMask;
    RFCOMM_EventCallback callBack;
    void *context;
} RfcommServerInfo;

typedef struct {
    uint16_t l2capId;
    uint8_t id; // Only use in recvConnectionReq callback
    uint16_t pendingL2capId;
    uint8_t pendingId; 
    BtAddr btAddr;
    bool isInitiator;
    bool peerSessionFc;
    RfcommFlowControlType fcType;
    RfcommSessionState sessionState;
    Alarm *timer;
    uint16_t l2capPeerMtu;
    uint16_t l2capLocalMtu;
} RfcommSessionInfo;

typedef struct {
    RfcommSessionInfo *session;
    bool isServer;
    uint8_t dlci;
    uint16_t mtu;
    uint32_t eventMask;
    RFCOMM_EventCallback callback;
    void *context;
} RfcommCreateChannelInfo;

typedef struct {
    uint8_t cl;
    uint8_t credits;
    uint16_t mtu;
    uint8_t priority;
} RfcommSendPnInfo;

typedef struct {
    uint16_t handle;
    uint8_t dlci;
    uint8_t scn;
    bool isServer;
    RfcommSessionInfo *session;
    uint16_t peerMtu;
    uint16_t localMtu;
    RfcommChannelState channelState;
    uint8_t transferReady;
    bool peerChannelFc;
    bool localFcToPeer;
    bool localFcToUpper;
    uint8_t peerCredit;
    uint8_t localCredit;
    uint8_t localCreditMax;
    List *sendQueue;
    List *recvQueue;
    uint8_t lineStatus;
    RfcommRemotePortConfig portConfig;
    RfcommModemStatusInfo peerModemSt;
    uint32_t eventMask;
    uint32_t receivedBytes;
    uint32_t transmittedBytes;
    Alarm *timer;
    RFCOMM_EventCallback callBack;
    void *context;
} RfcommChannelInfo;

typedef struct {
    L2capConfigInfo cfg;
    uint16_t result;
} RfcommConfigRspInfo;

typedef struct {
    L2capConfigInfo cfg;
    uint8_t id;
} RfcommConfigReqInfo;

typedef struct {
    uint16_t lcid;
    int result;
} RfcommReqConnectRslt;

typedef union {
    struct {
        uint8_t dlci;
        uint8_t cl;
        uint8_t priority;
        uint16_t mtu;
        uint8_t k;
    } pn;
    struct {
        uint8_t dlci;
        uint8_t signal;
        uint8_t breakSignal;
    } msc;
    struct {
        uint8_t dlci;
        bool isCmd;
        RfcommRemotePortConfig rpnInfo;
    } rpn;
    struct {
        uint8_t dlci;
        uint8_t lineStatus;
    } rls;
    struct {
        uint8_t ea;
        uint8_t cr;
        uint8_t type;
    } nsc;
    struct {
        Packet *pkt;
        uint16_t length;
    } test;
    struct {
        uint8_t credits;
        Packet *payload;
        size_t size;
    } data;
    struct {
        uint8_t ea;
        uint8_t cr;
        uint8_t type;
    } invalidCmd;
} RfcommUihInfo;

typedef struct {
    bool isInitiator;
    uint8_t cr;
    uint8_t dlci;
    uint8_t pf;
    uint8_t fcs;
    uint8_t calcInfo[3];
    uint16_t length;
} RfcommCheckFrameValidInfo;

typedef struct {
    uint8_t cr;
    uint8_t dlci;
    uint8_t pf;
    uint8_t type;
    uint8_t fcs;
    uint8_t calcInfo[3];
    uint16_t length;
} RfcommFrameHeaderTailInfo;

typedef struct {
    uint8_t *dlci;
    int *event;
    RfcommUihInfo *info;
} RfcommParseFrameResult;

typedef struct {
    uint16_t lcid;
    uint8_t id;
} RfcommRecvConnectedInfo;

typedef struct {
    uint16_t result;
    GAP_ServiceConnectDirection direction;
} RfcommSessionSecurityRslt;

void RfcommInitialize();
void RfcommFinalize();
uint8_t RfcommAssignServerNum();
int RfcommFreeServerNum(uint8_t scn);
int RfcommConnectChannel(const RfcommConnectReqInfo *reqInfo, uint16_t *handle);
int RfcommRegisterServer(uint8_t scn, uint16_t mtu, uint32_t eventMask, RFCOMM_EventCallback callback, void *context);
int RfcommDeregisterServer(uint8_t scn, bool isRemoveCallback);
int RfcommAcceptConnection(uint16_t handle);
int RfcommRejectConnection(uint16_t handle);
int RfcommDisconnectChannel(uint16_t handle);
int RfcommSetPortConfig(uint16_t handle, const RfcommRemotePortConfig *config);
int RfcommReqPortConfig(uint16_t handle);
int RfcommSendRemoteLineStatus(uint16_t handle, const RfcommRemoteLineStatus *lineStatus);
int RfcommSetModemStatus(uint16_t handle, const RfcommModemStatus *modemStatus);
int RfcommGetPortState(uint16_t handle, RfcommPortState *state);
int RfcommRead(uint16_t handle, Packet **pkt);
int RfcommWrite(uint16_t handle, Packet *pkt);
int RfcommSendTestCmd(uint16_t handle, Packet *pkt);

// Call L2CAP interface.
// Register and deregister with L2CAP.
void RfcommRegisterL2cap();
void RfcommDeregisterL2cap();
// Send command to L2CAP.
int RfcommSendConnectReq(const BtAddr *addr);
int RfcommSendConnectRsp(uint16_t lcid, uint8_t id, uint16_t result, uint16_t status);
int RfcommSendConfigReq(uint16_t lcid, const L2capConfigInfo *cfg);
int RfcommSendConfigRsp(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, uint16_t result);
int RfcommSendDisconnectReq(uint16_t lcid);
int RfcommSendDisconnectRsp(uint16_t lcid, uint8_t id);
int RfcommSendData(uint16_t lcid, const uint8_t *header, uint8_t headSize, uint8_t tail, Packet *pkt);
// Process callback
void RfcommRecvConnectReqCback(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm);
void RfcommRecvConnectRspCback(uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status);
void RfcommRecvConfigReqCback(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg);
void RfcommRecvConfigRspCback(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result);
void RfcommRecvDisconnectReqCback(uint16_t lcid, uint8_t id);
void RfcommDisconnectAbnormalCback(uint16_t lcid, uint8_t reason);
void RfcommRecvDataCback(uint16_t lcid, Packet *pkt);
void RfcommSendConnectReqCback(const BtAddr *addr, uint16_t lcid, int result);

// Call GAP interface.
// Check security.
void RfcommRegisterSecurity();
void RfcommDeregisterSecurity();
int RfcommCheckChannelSecurity(RfcommChannelInfo *channel, bool isServer);
int RfcommCheckSessionSecurity(RfcommSessionInfo *session, uint8_t scn, bool isServer);
void RfcommRecvChannelSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);
void RfcommRecvSessionSecurityResult(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);

// Create and destroy session list.
void RfcommCreateSessionList();
void RfcommDestroySessionList();
uint8_t RfcommGetSessionLstCnt();
// About the method of operating session.
RfcommSessionInfo *RfcommCreateSession(const BtAddr *addr, uint16_t lcid, uint8_t id, bool isInitiator);
RfcommSessionInfo *RfcommGetSessionByLcid(uint16_t lcid);
RfcommSessionInfo *RfcommGetSessionByAddr(const BtAddr *addr);
void RfcommResetSessionInfo(RfcommSessionInfo *session);
void RfcommRemoveSession(RfcommSessionInfo *session);
void RfcommStartSessionTimer(RfcommSessionInfo *session, uint8_t timeout);
void RfcommStopSessionTimer(RfcommSessionInfo *session);
void RfcommSessionTimeout(void *parameter);
bool RfcommIsSessionValid(RfcommSessionInfo *session);
void RfcommSetFlowControlType(RfcommSessionInfo *session, bool isPnReq, uint8_t cl);

// Session state machine.
int RfcommSessionEvtFsm(RfcommSessionInfo *session, RfcommSessionEvent event, const void *data);

// Create and destroy channel list.
void RfcommCreateChannelList();
void RfcommDestroyChannelList();
// About the method of operating channel.
RfcommChannelInfo *RfcommCreateChannel(const RfcommCreateChannelInfo *createChannelInfo);
RfcommChannelInfo *RfcommCreateChannelOfServer(RfcommSessionInfo *session, uint8_t dlci, int event);
RfcommChannelInfo *RfcommGetChannelByDlci(const RfcommSessionInfo *session, uint8_t dlci);
RfcommChannelInfo *RfcommGetFirstChannelOnSession(const RfcommSessionInfo *session);
RfcommChannelInfo *RfcommGetChannelByHandle(uint16_t handle);
int RfcommOpenAllChannelOnSession(const RfcommSessionInfo *session);
void RfcommResetAllChannelOnSession(const RfcommSessionInfo *session);
void RfcommSetFlcToUpper(RfcommChannelInfo *channel);
void RfcommSetFlcToPeer(RfcommChannelInfo *channel, bool enable);
void RfcommNotifyEvtToUpper(const RfcommChannelInfo *channel, uint32_t eventId, const void *eventData);
void RfcommNotifyAllChannelEvtOnSession(const RfcommSessionInfo *session, uint32_t eventId);
void RfcommReleaseCachePkt(RfcommChannelInfo *channel);
void RfcommRemoveChannel(RfcommChannelInfo *channel);
void RfcommRemoveAllChannelOnSession(const RfcommSessionInfo *session);
void RfcommRemoveInvalidChannelOnSession(const RfcommSessionInfo *session);
void RfcommRemoveChannelCallback(uint8_t scn);
void RfcommSendCachePkt(RfcommChannelInfo *channel);
void RfcommSendAllCachePktOnSession(const RfcommSessionInfo *session);
bool RfcommCheckSessionValid(const RfcommSessionInfo *session);
void RfcommCloseInvalidSession(RfcommSessionInfo *session);
void RfcommResetChannelInfo(RfcommChannelInfo *channel);
void RfcommSetPeerModemStatus(RfcommChannelInfo *channel, const RfcommModemStatusInfo *modemSts);
void RfcommStartChannelTimer(RfcommChannelInfo *channel, uint8_t timeout);
void RfcommStopChannelTimer(RfcommChannelInfo *channel);
void RfcommChannelTimeout(void *parameter);
bool RfcommIsChannelValid(const RfcommChannelInfo *channel);
void RfcommDeterminePeerMtu(RfcommChannelInfo *channel);
void RfcommUpdateChannelDirectionBit(const RfcommSessionInfo *session, bool isInitiator);

void RfcommReadLock();
void RfcommReadUnlock();

// Channel state machine.
int RfcommChannelEvtFsm(RfcommChannelInfo *channel, RfcommChannelEvent event, const void *data);

// Create and destroy server list.
void RfcommCreateServerList();
void RfcommDestroyServerList();
// About the method of operating server.
RfcommServerInfo *RfcommGetServerByScn(uint8_t scn);
RfcommServerInfo *RfcommCreateServer(
    uint8_t scn, uint16_t mtu, uint32_t eventMask, RFCOMM_EventCallback callback, void *context);
void RfcommRemoveServer(RfcommServerInfo *server);

// Compose or parse Frame.
int RfcommSendSabm(const RfcommSessionInfo *session, uint8_t dlci);
int RfcommSendDisc(const RfcommSessionInfo *session, uint8_t dlci);
int RfcommSendUa(const RfcommSessionInfo *session, uint8_t dlci);
int RfcommSendDm(const RfcommSessionInfo *session, uint8_t dlci, bool pf);
int RfcommSendUihPn(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd, const RfcommSendPnInfo *pnInfo);
int RfcommSendUihMsc(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd,
                     const RfcommModemStatusInfo *modemSts);
int RfcommSendUihRls(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd, uint8_t lineStatus);
int RfcommSendUihRpn(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd,
                     const RfcommRemotePortConfig *portConfig);
int RfcommSendUihFcon(const RfcommSessionInfo *session, bool isCmd);
int RfcommSendUihFcoff(const RfcommSessionInfo *session, bool isCmd);
int RfcommSendUihTest(const RfcommSessionInfo *session, bool isCmd, Packet *pkt);
int RfcommSendUihNsc(const RfcommSessionInfo *session, uint8_t ea, uint8_t cr, uint8_t type);
int RfcommSendUihData(const RfcommSessionInfo *session, uint8_t dlci, uint8_t newCredits, Packet *pkt);
RfcommEventType RfcommParseFrames(const RfcommSessionInfo *session, Packet *pkt, RfcommParseFrameResult output);

#endif