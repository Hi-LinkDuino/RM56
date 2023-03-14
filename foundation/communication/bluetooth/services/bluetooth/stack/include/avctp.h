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
/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief This file is a part of BTStack.
 * @since 6.0
 */

/**
 * @file avctp.h
 *
 * @brief AVCT protocal Interface.
 *
 * @since 6.0
 */
#ifndef AVCTP_H
#define AVCTP_H

#include <stdint.h>

#include "btstack.h"
#include "gap_if.h"
#include "packet.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *  Constants
 ****************************************************************************/
/** PSM for AVCT. */
#define AVCT_PSM 0x0017
#define AVCT_BR_PSM 0x001B

/** Protocol Revision Numbers */
#define AVCT_REV_1_4 0x0104

/** The role to create the connection. */
#define AVCT_INIT 0  // Initiator
#define AVCT_ACPT 1  // Acceptor
#define AVCT_CT 0    // Control Role
#define AVCT_TG 1    // Target Role

/** The Channel Type */
#define AVCT_DATA_CTRL 0x01  // The control channel
#define AVCT_DATA_BR 0x02    // The browse channel

/** The C/R value. */
#define AVCT_COMMAND 0   // command
#define AVCT_RESPONSE 1  // response

/** Control channel callback events. */
typedef enum {
    AVCT_CONNECT_IND_EVT = 0,    // Connection indication
    AVCT_CONNECT_CFM_EVT,        // Connection confirm
    AVCT_DISCONNECT_IND_EVT,     // Disconnect indication
    AVCT_DISCONNECT_CFM_EVT,     // Disconnect confirm
    AVCT_CHANNEL_BUSY_EVT,       // Channel is busy on Enhance Mode
    AVCT_CHANNEL_UNBUSY_EVT,     // Channel is unbusy on Enhance Mode
    AVCT_BR_CONNECT_IND_EVT,     // Browse Connection indication, Browser auto connect in background,so it's unused.
    AVCT_BR_CONNECT_CFM_EVT,     // Browse Connection confirm
    AVCT_BR_DISCONNECT_IND_EVT,  // Browse Disconnect indication
    AVCT_BR_DISCONNECT_CFM_EVT,  // Browse Disconnect confirm
    AVCT_BR_CHANNEL_BUSY_EVT,    // Browse Channel is busy on Enhance Mode
    AVCT_BR_CHANNEL_UNBUSY_EVT,  // Browse Channel is unbusy on Enhance Mode
} AvctCallbackEvt;

/** API function return value result codes. */
typedef enum {
    AVCT_SUCCESS = 0,        // Function Success
    AVCT_ERR_NO_RESOURCES,   // Not enough resources
    AVCT_ERR_CONN_BAD,       // Bad channel can't be used
    AVCT_ERR_CONN_NOT_OPEN,  // Connection not open
    AVCT_ERR_PID_USED,       // PID already in used
    AVCT_ERR_PARAM,          // Paramas error
    AVCT_FAILED,             // Function Failed
} AvctRet;
/***********************************************************************************
 *  Type Definitions
 ***********************************************************************************/
/** @brief      HCI Event callback function.
 * @details     The aim of this primitive is to request an application callback when the selected indication Event
 *              occurs. Each profile shall register for being called back separately.
 * @param[in]   connId    Connection channel Id
 * @param[in]   event     Control link callback events
 * @param[in]   result    Control link callback result
 * @param[in]   peerAddr  Peer device address
 * @reutrn      @c void
 */
typedef void (*AvctChannelEventCallback)(
    uint8_t connId, uint8_t event, uint16_t result, const BtAddr *peerAddr, void *context);

/** @brief      Protocal Message data callback function
 * @details     This is the callback function used by AVCTP to report received AV control messages.
 * @param[in]   connId    Connection channel Id
 * @param[in]   lable     Message lable from app
 * @param[in]   cr        Command/Response
 * @param[in]   ch_type   Message come from (AVCT_DATA_CTRL or AVCT_DATA_BROWSE)
 * @param[in]   pkt       The point of message data
 * @reutrn      @c void
 */
typedef void (*AvctMsgCallback)(uint8_t connId, uint8_t label, uint8_t cr, uint8_t ch_type, Packet *pkt, void *context);

/**
 * @brief Connect parameter used by avct_connect_req.
 */
typedef struct {
    AvctChannelEventCallback chEvtCallback;  // HCI Event callback
    AvctMsgCallback msgCallback;             // Message Data callback
    uint16_t pid;                            // Profile ID
    uint8_t role;                            // Initiator/Acceptor
    void *context;                           // context from  app
} AvctConnectParam;

/***********************************************************************************
 *  External Function Declarations
 ***********************************************************************************/

/**
 * @brief       Function    AVCT_Register
 *
 * @details     This is the system level registration function for the AVCTP
 *              protocol. This function initializes AVCTP protocal and prepares
 *              the protocol stack for its use.  This function must be called
 *              once by the system before the other functions of the API can be
 *              used.
 *
 * @param[in]   mtu     Max transcation unit for control.
 * @param[in]   mtuBr   Max transcation unit for browser.
 *
 * @return      @c void
 *
 */
BTSTACK_API void AVCT_Register(uint16_t mtu, uint16_t mtuBr, uint16_t role);

/**
 *
 * @brief       Function    AVCT_Deregister
 *
 * @details     This function is called to deregister AVCTP protocol.
 *              It is called when AVCTP is no longer being used by any
 *              application in the system. All connections must be
 *              disconned in advance.
 *
 * @param[in]   void
 *
 * @return      @c void
 *
 */
BTSTACK_API void AVCT_Deregister(void);

/**
 *
 * @brief       Function    AVCT_ConnectReq
 *
 * @details     This function is called to create an avctp control connection
 *              session for peer device.
 *              AVCT.ICS #Table 2/11 3/11 Connect Request.
 *              AVCT.ICS #Table 3/9 Event register for connection request.
 *              AVCT Profile #11.2.1 #11.1.
 *
 * @param[in]   connParam   The point to the param of the connection request.
 *                          Such as the Role、Profile ID、Callback func point.
 * @param[in]   peerAddr    The peer address to be connected.
 * @param[out]  connId      The point of the connection session handle.
 *
 * @return      @c AVCT_SUCCESS Success @c otherwise Error
 *
 */
BTSTACK_API uint16_t AVCT_ConnectReq(uint8_t *connId, const AvctConnectParam *connParam, const BtAddr *peerAddr);

/**
 *
 * @brief       Function    AVCT_DisconnectReq
 *
 * @details     This function is called to disconnect the avctp control
 *              connection session. AVCT.ICS #Table 2/12 3/12 Disconnect Request. AVCT.ICS
 *              #Table 3/10 Event register for disconnection request. AVCT Profile #11.2.2
 *              #11.2.3 #11.1.
 *
 * @param[in]   connId   The id of the connection session to be disconnected.
 *
 * @return      @c AVCT_SUCCESS Success @c otherwise Error
 *
 */
BTSTACK_API uint16_t AVCT_DisconnectReq(uint8_t connId);

/**
 *
 * @brief       Function    AVCT_SendMsgReq
 *
 * @details     This function is called to send an message to the control
 *              connection session. AVCT.ICS #Table 2/13 3/13 Send Message AVCT.ICS #Table
 *              3/10 Event register for message  reception. AVCT Profile #11.2.3 #11.2.4
 *              #11.1
 *
 * @param[in]   connId  The id of the connection session which the message is send to.
 * @param[in]   label   Message label from application.
 * @param[in]   cr      Command/Tesponse
 * @param[in]   msg     The point of Message data.
 *
 * @return      @c AVCT_SUCCESS Success @c otherwise Error
 *
 */
BTSTACK_API uint16_t AVCT_SendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg);

/**
 *
 * @brief       Function    AVCT_GetMtu
 *
 * @details     This function is called to get the mtu of the control connection session.
 *
 * @param[in]   connId   The id of the connection session.
 *
 * @return      @c MTU
 *
 */
BTSTACK_API uint16_t AVCT_GetPeerMtu(uint8_t connId);

/**
 *
 * @brief       Function    AVCT_BrConnectReq
 *
 * @details     This function is called to create an avctp browse connection session if the peer device has.
 *
 * @param[in]   connId  The control connection session id.
 * @param[in]   role    The connection session role.
 *
 * @return      @c AVCT_SUCCESS Success @c otherwise Error
 *
 */
BTSTACK_API uint16_t AVCT_BrConnectReq(uint8_t connId, uint8_t role);

/**
 *
 * @brief       Function    AVCT_BrDisconnectReq
 *
 * @details     Remove an AVCTP browse connection session.
 *
 * @param[in]   connId      The connection session id.
 *
 * @return      @c AVCT_SUCCESS Success @c otherwise Error
 *
 */
BTSTACK_API uint16_t AVCT_BrDisconnectReq(uint8_t connId);

/**
 *
 * @brief       Function    AVCT_BrSendMsgReq
 *
 * @details     This function is called to send an  message to the browse session.
 *              AVCT.ICS #Table 2/13 3/13 Send Message.
 *              AVCT.ICS #Table 3/10 Event register for message reception.
 *              AVCT Profile #11.2.3 #11.2.4 #11.1.
 *
 * @param[in]   connId  The id of the connection session which the message is send to.
 * @param[in]   label   Message label from application.
 * @param[in]   cr      Command/Tesponse
 * @param[in]   msg     Message data point.
 *
 * @return      @c AVCT_SUCCESS Success @c otherwise Error
 *
 */
BTSTACK_API uint16_t AVCT_BrSendMsgReq(uint8_t connId, uint8_t label, uint8_t cr, const Packet *msg);

/**
 *
 * @brief       Function    AVCT_BrGetMtu
 *
 * @details     This function is called to get the mtu of the browse connection session.
 *
 * @param[in]   connId   The id of the connection session.
 *
 * @return      @c MTU
 *
 */
BTSTACK_API uint16_t AVCT_BrGetPeerMtu(uint8_t connId);

#ifdef __cplusplus
}
#endif

#endif /* AVCTP_H */
