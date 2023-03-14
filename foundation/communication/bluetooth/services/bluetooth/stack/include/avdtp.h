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
 * @file avdtp.h
 *
 * @brief AVDT protocol Interface.
 *
 * @since 6.0
 */
#ifndef AVDTP_H
#define AVDTP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "btstack.h"
#include "packet.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  Constants
 */
#define AVDT_VERSION_1_3 0x0103  // AVDTP.ICS #Table 14a-3,15a-3 version 1.3

#ifndef AVDT_NUM_SEPS
#define AVDT_NUM_SEPS 6  // SOW Max Number of stream endpoints
#endif

#ifndef AVDT_NUM_LINKS
#define AVDT_NUM_LINKS 6  // SOW Max Number of link devices
#endif

/**
 * AVDTP.ICS #Table 14-1, 15-1 Basics Service.
 * The Max size of the codec capabilities information element.
 */
#define AVDT_CODEC_SIZE 20

/**
 * The return value of the API function
 */
#define AVDT_SUCCESS 0       // Successful
#define AVDT_BAD_PARAMS 1    // Invalid parameters
#define AVDT_NO_RESOURCES 2  // Not resources
#define AVDT_BAD_HANDLE 3    // Invalid handle
#define AVDT_BUSY 4          // A procedure is doing,such as discovery seps
#define AVDT_FAILED 5        // Procedure failed

/**
 * AVDTP.ICS #Table 1-1,1-2 SEP Type.
 */
#define AVDT_SEP_SRC 0      // Source SEP
#define AVDT_SEP_SNK 1      // Sink SEP
#define AVDT_SEP_INVALID 2  // Invalid SEP

/**
 * AVDTP Initiator(INT)and Acceptor(ACP).
 */
#define AVDT_INT 0  // Initiator
#define AVDT_ACP 1  // Acceptor

/**
 * AVDTP Source(SRC)and Sink(SNK)
 */
#define AVDT_ROLE_SRC 0     // source
#define AVDT_ROLE_SNK 1     // sink
#define AVDT_ROLE_UNKOWN 2  // unknown

/**
 * Media Type of the SEP
 */
#define AVDT_MEDIA_TYPE_AUDIO 0  // Audio SEP
#define AVDT_MEDIA_TYPE_VIDEO 1  // Retain: Video SEP

/**
 * Error codes defined in the AVDTP and GAVDP specifications.
 * AVDTP.ICS #Table 16-1, Reporting Capability Error
 */
#define AVDT_ERR_BAD_HEADER_FORMAT 0x01           // Packet header format is error
#define AVDT_ERR_BAD_LENGTH 0x11                  // The length of packet is error
#define AVDT_ERR_BAD_ACP_SEID 0x12                // Invalid SEID
#define AVDT_ERR_SEP_IN_USE 0x13                  // The SEP use
#define AVDT_ERR_SEP_NOT_IN_USE 0x14              // The SEP used
#define AVDT_ERR_BAD_SERV_CATEGORY 0x17           // The service category is error
#define AVDT_ERR_BAD_PAYLOAD_FORMAT 0x18          // The payload format is error
#define AVDT_ERR_UNSUPPORTED_COMMAND 0x19         // Requested command is not supported
#define AVDT_ERR_INVALID_CAPABILITIES 0x1A        // Invalid capabilities
#define AVDT_ERR_BAD_RECOVERY_TYPE 0x22           // The requested Recovery Type is not defined in AVDTP.
#define AVDT_ERR_BAD_MEDIA_TRANSPORT_FORMAT 0x23  // Media transport capability is error
#define AVDT_ERR_BAD_RECOVERY_FORMAT 0x25         // The format of Recovery Service Capability is not correct.
#define AVDT_ERR_UNSUPPORTED_CONFIGURATION 0x29   // Configuration is not supported
#define AVDT_ERR_BAD_STATE 0x31                   // Message can't be processed in this state
#define AVDT_ERR_SERVICE 0x80                     // Invalid service category
#define AVDT_ERR_RESOURCE 0x81                    // Not enough resources
#define AVDT_ERR_INVALID_MCT 0xC1                 // Invalid Media Codec Type
#define AVDT_ERR_UNSUP_MCT 0xC2                   // Unsupported Media Codec Type

/**
 * AVDTP 13.1.1 Control callback events
 */
#define AVDT_DISCOVER_CFM_EVT 0       // Discover confirm
#define AVDT_DISCOVER_IND_EVT 1       // Discover indication
#define AVDT_GETCAP_CFM_EVT 2         // Get capabilities confirm
#define AVDT_GETCAP_IND_EVT 3         // Get capabilities indication
#define AVDT_OPEN_CFM_EVT 4           // Open confirm
#define AVDT_OPEN_IND_EVT 5           // Open indication
#define AVDT_CONFIG_CFM_EVT 6         // Configuration confirm
#define AVDT_CONFIG_IND_EVT 7         // Configuration indication
#define AVDT_START_CFM_EVT 8          // Start confirm
#define AVDT_START_IND_EVT 9          // Start indication
#define AVDT_SUSPEND_CFM_EVT 10       // Suspend confirm
#define AVDT_SUSPEND_IND_EVT 11       // Suspend indication
#define AVDT_CLOSE_CFM_EVT 12         // Close confirm
#define AVDT_CLOSE_IND_EVT 13         // Close indication
#define AVDT_RECONFIG_CFM_EVT 14      // Reconfiguration confirm
#define AVDT_RECONFIG_IND_EVT 15      // Reconfiguration indication
#define AVDT_WRITE_CFM_EVT 16         // Write confirm
#define AVDT_READ_IND_EVT 17          // Read indication
#define AVDT_CONNECT_CFM_EVT 18       // Connected confirm
#define AVDT_CONNECT_IND_EVT 19       // Connected indication
#define AVDT_DISCONNECT_CFM_EVT 20    // Disconnected confirm
#define AVDT_DISCONNECT_IND_EVT 21    // Disconnected indication
#define AVDT_DELAY_REPORT_IND_EVT 22  // Delay report indication
#define AVDT_DELAY_REPORT_CFM_EVT 23  // Delay report confirm
#define AVDT_ABORT_CFM_EVT 24         // Abort confirm
#define AVDT_ABORT_IND_EVT 25         // Abort indication
#define AVDT_GETCONFIG_CFM_EVT 26     // get Configuration confirm
#define AVDT_GETCONFIG_IND_EVT 27     // get Configuration indication
#define AVDT_GET_ALLCAP_CFM_EVT 28    // Get all capabilities confirm
#define AVDT_GET_ALLCAP_IND_EVT 29    // Get all capabilities indication
#define AVDT_CLOSE_TRANS_IND_EVT 30   // Receive transport released inication
#define AVDT_MAX_EVT (AVDT_CLOSE_TRANS_IND_EVT)

#define AVDT_PSM 0x0019  // PSM for AVDT
/**
 * Capability mask
 * AVDTP.ICS #Table 14-1, 15-1 AVDT protocol service capabilities
 */

#define AVDT_PSC_MSK_TRANS (1 << 0)      // Media Transport
#define AVDT_PSC_MSK_REPORT (1 << 1)     // Reporting
#define AVDT_PSC_MSK_RECOV (1 << 2)      // Recovery
#define AVDT_PSC_MSK_PROTECT (1 << 3)    // Content Protection
#define AVDT_PSC_MSK_HDRCMP (1 << 4)     // Header Compression
#define AVDT_PSC_MSK_MUX (1 << 5)        // Header Compression
#define AVDT_PSC_MSK_CODEC (1 << 6)      // Media Codec
#define AVDT_PSC_MSK_DELAY_RPT (1 << 7)  // Delay Reporting
/**
 * Type Definitions
 */

/**
 * Service category
 */
typedef struct {
    uint8_t errCode;   // error code
    uint8_t category;  // category
} AvdtCatetory;

/**
 * SEP information. transportred during the discovery procedure.
 */
typedef struct {
    uint8_t seid;       // The identifier of SEP
    uint8_t mediaType;  // Media type: AVDT_MEDIA_TYPE_AUDIO
    uint8_t sepType;    // SEP type
    uint8_t role;       // actor role
    bool isUsed;        // true if stream is used
} AvdtSepInfo;

/**
 * Reject error information.
 */
typedef struct {
    uint8_t errCode;   // error code
    uint8_t errParam;  // error param
} AvdtRejErrInfo;

/*
 * AVDTP SEP Configuration.
 */
typedef struct {
    uint8_t codecInfo[AVDT_CODEC_SIZE];  // Codec capabilities
    uint8_t numCodec;                    // Number of media codec information elements
    uint8_t mediaType;                   // AVDT_MEDIA_TYPE
    uint16_t pscMask;                    // Protocol service capabilities mask
} AvdtSepConfig;

/**
 * Header structure for callback event parameters
 */
typedef struct {
    uint8_t errCode;    // Zero if operation succeeded;otherwise failed
    uint8_t errParam;   // Error parameter included for some events
    uint8_t label;      // Transaction label
    uint8_t seid;       // Used inner AVDTp only
    uint8_t sigId;      // Used inner AVDTponly
    uint8_t sigHandle;  // Used inner AVDTponly
    uint16_t mtu;       // Mtu size
} AvdtEvtHeader;

/**
 * Configure structure
 * * */
typedef struct {
    AvdtEvtHeader hdr;  // Event header
    AvdtSepConfig cfg;  // Pointer to configuration for this SEP
} AvdtConfig;

/**
 * Set Configure structure
 * * */
typedef struct {
    AvdtEvtHeader hdr;  // Event header
    AvdtSepConfig cfg;  // Pointer to configuration for this SEP
    uint8_t intSeid;    // The SEP ID initiated
    uint8_t acpSeid;    // The SEP ID acceptor
} AvdtSetConfig;

/**
 * Open stream structure
 */
typedef struct {
    AvdtEvtHeader hdr;  // Event header
    uint16_t peerMtu;   // L2CAP MTU of the peer
    uint16_t lcid;      // L2CAP LCID for media channel.recorded by AVDTP
} AvdtOpen;

/**
 * Seps structure
 */
typedef struct {
    AvdtEvtHeader hdr;                // Event header
    uint8_t seidList[AVDT_NUM_SEPS];  // Array of SEID values
    uint8_t numSeps;                  // Number of values in array
} AvdtMulti;

typedef struct {
    AvdtEvtHeader hdr;                // Event header
    AvdtSepInfo seps[AVDT_NUM_SEPS];  // Pointer to SEP information
    uint8_t numSeps;                  // Number of stream endpoints
} AvdtDiscover;

typedef struct {
    AvdtEvtHeader hdr;  // Event header
    uint16_t delay;     // Delay value
} AvdtDelayRpt;

/**
 * Union of all control callback event data structures
 */
typedef union {
    AvdtEvtHeader hdr;
    AvdtDiscover discoverCfm;
    AvdtDiscover discoverInd;
    AvdtConfig getCapCfm;
    AvdtConfig getCapInd;
    AvdtOpen openCfm;
    AvdtOpen openInd;
    AvdtSetConfig configInd;
    AvdtSetConfig configCfm;
    AvdtEvtHeader startCfm;
    AvdtEvtHeader startInd;
    AvdtEvtHeader suspendCfm;
    AvdtEvtHeader suspendInd;
    AvdtEvtHeader closeCfm;
    AvdtEvtHeader closeInd;
    AvdtConfig reconfigCfm;
    AvdtConfig reconfigInd;
    AvdtConfig getconfigCfm;
    AvdtConfig getconfigInd;
    AvdtEvtHeader connectInd;
    AvdtEvtHeader connectCfm;
    AvdtEvtHeader disconnectInd;
    AvdtEvtHeader disconnectCfm;
    AvdtDelayRpt delayRptInd;
    AvdtDelayRpt delayRptCfm;
    AvdtEvtHeader abortInd;
    AvdtEvtHeader abortCfm;
} AvdtCtrlData;

/**
 * @brief       Protocol Message data callback function
 * @details     This is the callback function used by AVDTP to report received AV control messages.
 * @param[in]   handle    Stream handle
 * @param[in]   bdAddr    Peer device address
 * @param[in]   event     Indication/Confirm event of message
 * @param[in]   data      Avdt event data
 * @param[in]   role      Local instance role
 * @return      @c void
 */
typedef void (*AvdtCtrlCallback)(
    uint16_t handle, const BtAddr *bdAddr, uint8_t event, const AvdtCtrlData *data, uint8_t role);

/**
 * @brief       Callback function for sink to read MediaData
 * @details     This is the callback function used by AVDTP to report sink to received AV stream data.
 * @param[in]   handle          Block handle
 * @param[in]   pkt             Stream data
 * @param[in]   timeStamp       Timestamp of frame
 * @param[in]   pt              Packet type
 * @param[in]   streamHandle    Stream handle
 * @return      @c void
 */
typedef void (*AVDT_SinkDataCallback)(
    uint16_t handle, Packet *pkt, uint32_t timeStamp, uint8_t pt, uint16_t streamHandle);

/**
 * AVDT Registration Control Block.
 */
typedef struct {
    uint16_t mtu;                   // L2CAP MTU for AVDT signaling channel
    uint8_t role;                   // Source or Sink
    AvdtCtrlCallback ctrlCallback;  // Control Callback function
} AvdtRegisterParam;

/**
 * Used before to create a stream
 */
typedef struct {
    AvdtSepConfig cfg;                    // SEP configuration
    uint16_t codecIndex;                  // The handle to the a2dp stream control entry
    AVDT_SinkDataCallback sinkDataCback;  // Sink data callback function
    uint16_t mtu;                         // The L2CAP MTU of the transport channel
    uint16_t flushTo;                     // The L2CAP flush timeout of the transport channel
    uint8_t sepType;                      // SEP type
    uint16_t nscMask;                     // Protocol command messages is not supported
} AvdtStreamConfig;

/**
 *
 * @brief       Function AVDT_Register
 * @details     This function initializes AVDTP and prepares the protocol stack for its use.
 *              It must be called once by the system or platform using AVDT before the other functions of the API an be
 *              used.
 * @param[in]   reg  Register information
 * @return      void
 *
 */
BTSTACK_API void AVDT_Register(const AvdtRegisterParam *reg);

/**
 *
 * @brief   Function AVDT_Deregister
 * @details This function is called to deregister AVDTP protocol.
 *          This function can be called after all streams must be removed.
 * @return  void
 *
 */
BTSTACK_API void AVDT_Deregister(void);

/**
 *
 * @brief       Function AVDT_AbortReq
 * @details     This function is called to request to pass AVDT Abort.
 *              AVDTP.ICS #Table 5-5, 5b-5 Abort stream command.
 * @param[in]   handle      Handle of stream
 * @param[out]  transLabel  The label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_AbortReq(uint16_t handle, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_AbortRsp
 * @details     The response of  Abort request from peer device.
 *              AVDTP.ICS #Table 11-5, 11b-5 Abort stream response
 * @param[in]   handle      Handle of stream
 * @param[in]   transLabel  The label of this message
 * @param[in]   errCode     Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_AbortRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_CreateStream
 * @details     Create a stream endpoint. An application can initiate a connection between this endpoint and an endpoint
 *              on a peer device after a stream endpoint is created.
 * @param[in]   bdAddr              Peer device address
 * @param[in]   avdtStreamConfig    Stream  endpoint configure information
 * @param[out]  handle              Sep handle
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_CreateStream(const BtAddr *bdAddr, uint16_t *handle, uint16_t codeIndex);

/**
 *
 * @brief       Function AVDT_RegisterLocalSEP
 * @details     Register local stream endpoint information.
 * @param[in]   avdtStreamConfig  Stream  endpoint configure information
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_RegisterLocalSEP(AvdtStreamConfig *avdtStreamConfig, uint8_t number);

/**
 *
 * @brief       Function AVDT_RemoveStream (Retain)
 * @details     Remove a stream endpoint.
 * @param[in]   handle   Handle of stream.
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_RemoveStream(uint16_t handle);

/**
 *
 * @brief       Function AVDT_DiscoverReq
 * @details     This function send AVDT_DISCOVER_CMD from INT to ACP and it request to get the overview of all SEP
 *              information of the ACP. When discovery is complete, an AVDT_DISCOVER_CFM_EVT is sent to the application
 *              via its callback function.The application must not call AVDT_GetCapReq() or AVDT_DiscoverReq() again to
 *              the same device until discovery is complete. AVDTP.ICS #Table 4-1, 4b-1 Stream discover command
 * @param[in]   bdAddr        Peer device address
 * @param[in]   maxSeps       Max number to get SEP
 * @param[out]  transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_DiscoverReq(const BtAddr *bdAddr, uint8_t maxSeps, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_DiscoverRsp(Retain)
 * @details     Respond to a discover request from ACP.
 *              AVDTP.ICS #Table 10-1, 10b-1 Stream discover response
 * @param[in]   bdAddr        Peer device address
 * @param[in]   transLabel    Label of Disvover message
 * @param[in]   errCode       Error code
 * @param[in]   sepInfo       SEP information
 * @param[in]   numSeps       Number of SEP
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_DiscoverRsp(
    const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepInfo *sepInfo, uint8_t numSeps);

/**
 *
 * @brief       Function AVDT_GetCapReq
 * @details     This function send AVDT_GETCAP_REQ from INT to ACP and it used to get the information of a SEP of
 *              ACP.When the procedure is complete, an AVDT_GETCAP_CFM_EVT is sent to the application via its callback
 *              function.  The application must not call AVDT_GetCapReq() or AVDT_DiscoverReq() again until the
 *              procedure is complete. AVDTP.ICS #Table 4-2, 4b-2 Stream get capabilities command AVDTP.ICS #Table 4-6,
 *              4b-6 Stream get all capabilities command.
 * @param[in]   bdAddr        Peer device address
 * @param[in]   acpSeid       Seid of Acceptor
 * @param[out]  transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_GetCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_GetCapRsp(Retain)
 * @details     Respond to a getcap request from ACP.
 *              AVDTP.ICS #Table 10-2, 10b-2 Stream get capabilities response.
 * @param[in]   bdAddr        Peer device address
 * @param[in]   transLabel    Lable of getcapability cmd
 * @param[in]   errCode       Error code
 * @param[in]   sepCap        SEP capability information
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_GetCapRsp(const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap);

/**
 *
 * @brief       Function AVDT_GetAllCapReq
 * @details     This function send AVDT_GETALLCAP_REQ from INT to ACP and it used to get the information of a SEP of
 *              ACP.When the procedure is complete, an AVDT_GETALLCAP_CFM_EVT is sent to the application via its
 *              callback function. The application must not call AVDT_GetAllCapReq() or AVDT_DiscoverReq() again until
 *              the procedure is complete. AVDTP.ICS #Table 4-2, 4b-2 Stream get capabilities command AVDTP.ICS #Table
 *              4-6, 4b-6 Stream get all capabilities command.
 * @param[in]   bdAddr        Peer device address
 * @param[in]   acpSeid       Seid of Acceptor
 * @param[out]  transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_GetAllCapReq(const BtAddr *bdAddr, uint8_t acpSeid, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_GetAllCapRsp(Retain)
 * @details     Respond to a getcap request from ACP.
 *              AVDTP.ICS #Table 10-2, 10b-2 Stream get capabilities response.
 * @param[in]   bdAddr        Peer device address
 * @param[in]   transLabel    Lable of getcapability cmd
 * @param[in]   errCode       Error code
 * @param[in]   sepCap        SEP capability information
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_GetAllCapRsp(
    const BtAddr *bdAddr, uint8_t transLabel, uint8_t errCode, AvdtSepConfig *sepCap);

/**
 *
 * @brief       Function AVDT_DelayReq
 * @details     This functions sends a Delay Report to ACP that is associated with a particular SEID.This function is
 *              called by SNK device. AVDTP.ICS #Table 15-6 Delay Reporting
 * @param[in]   bdAddr        Peer device address
 * @param[in]   delayValue    Delay data
 * @param[out]  transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_DelayReq(uint16_t handle, uint8_t *transLabel, uint16_t delayValue);

/**
 *
 * @brief       Function AVDT_DelayRsp(Retain)
 * @details     Respond to a delay request from the peer device.
 *              AVDTP.ICS #Table 14-6 Delay Reporting.
 * @param[in]   handle        Handle of stream
 * @param[in]   transLabel    Label of delay cmd
 * @param[in]   errCode       Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_DelayRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_OpenReq
 * @details     This function send the AVDT_OPEN_REQ from INT to ACP and create the stream between INT and ACP.When the
 *              connection is completed, an AVDT _OPEN_CFM_EVT is sent to the application via the control callback
 *              function for this handle. AVDTP.ICS #Table 5-1,5b-1 Open stream command
 * @param[in]   handle        Handle of stream
 * @param[out]   transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_OpenReq(uint16_t handle, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_OpenRsp(Retain)
 * @details     Respond to an open request from ACP.
 *              AVDTP.ICS #Table 11-1, 11b-1 Open stream response
 * @param[in]   handle        Handle of stream
 * @param[in]   transLabel    Label of this message
 * @param[in]   errCode       Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_OpenRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_SetConfigReq
 * @details     This function send AVDT_CONFIG_CMD from INT to ACP. When the config is completed, an AVDT_CONFIG_CFM_EVT
 *              is sent to the application via the control callback function for this handle. AVDTP.ICS #Table 4-3, 4b-3
 *              Set configuration command
 * @param[in]   handle        Handle of stream
 * @param[in]   seid          The id of SEP in acceptor
 * @param[in]   sepConfig     Configure information
 * @param[out]   transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_SetConfigReq(uint16_t handle, uint8_t seid, AvdtSepConfig *sepConfig, uint8_t *transLabel);

/**
 *
 * @brief       AVDT_SetConfigRsp
 * @details     Respond to a configure request from ACP.  This function must be called if the application receives an
 *              AVDT_CONFIG_IND_EVT through its control callback. AVDTP.ICS #Table 10-3, 10b-3 Set configuration
 *              response
 * @param[in]   handle        Handle of stream
 * @param[in]   transLabel    Lable of setconfigure cmd
 * @param[in]   category      Configure result information
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_SetConfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category);

/**
 *
 * @brief       Function AVDT_GetConfigReq
 * @details     This function send AVDT_GETCONFIG_CMD from INT to ACP to get the current configuration of a SEP in ACP.
 *              When the config is completed, an AVDT_GETCONFIG_CFM _EVT is sent to the application via the control
 *              callback function. AVDTP.ICS #Table 4-4, 4b-4 Get configuration command
 * @param[in]   handle        Handle of stream
 * @param[out]  transLabel    Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_GetConfigReq(uint16_t handle, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_StartReq
 * @details     Start one or more stream endpoints(audio/video). All stream endpoints must previously be opened.  When
 *              the streams are started, an AVDT_START_CFM_EVT is sent to the application via the control callback
 *              function for each stream. AVDTP.ICS #Table 5-2, 5b-2 Start stream command
 * @param[in]   handles       Handles of stream, use the first one usually
 * @param[in]   numHandles    Number of stream
 * @param[out]  transLabel    Lable of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_StartReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_StartRsp
 * @details     Respond to a start request from ACP.  This function must be called if the application receives an
 *              AVDT_START_IND_EVT through its control callback. AVDTP.ICS #Table 11-2, 11b-2 Start stream command
 * @param[in]   firstHandle       First stream handle successful
 * @param[in]   transLabel        Label of start cmd
 * @param[in]   firstFailHandle   First stream handle failed
 * @param[in]   errCode           Error code ot the failed handle
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_StartRsp(uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_SuspendReq
 * @details     Suspend one or more stream endpoints. This suspends the transport of media packets for the streams.  All
 *              stream endpoints must previously be open and started.  When the streams are suspended, an
 *              AVDT_SUSPEND_CFM_EVT is sent to the application via the control callback function for each stream.
 *              AVDTP.ICS #Table 5-4, 5b-4 Suspend command
 * @param[in]   handles       Handles of stream, use the first one usually
 * @param[in]   numHandles    Number of stream
 * @param[out]  transLabel    Lable of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_SuspendReq(const uint16_t *handles, uint8_t numHandles, uint8_t *transLabel);

/**
 *
 * @brief       Function AVDT_SuspendRsp
 * @details     Respond to a suspend request from ACP.  This function must be called if the application receives an
 *              AVDT_SUSPEND_IND_EVT through its control callback. AVDTP.ICS #Table 11-4, 11b-4 Suspend response
 * @param[in]   firstHandle       First stream handle successful
 * @param[in]   transLabel        Label of start cmd
 * @param[in]   firstFailHandle   First stream handle failed
 * @param[in]   errCode           Error code ot the failed handle
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_SuspendRsp(
    uint16_t firstHandle, uint8_t transLabel, uint16_t firstFailHandle, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_CloseReq
 * @details     It stops the transport of media packets and close the transport channel associated with this stream
 *              endpoint.  When the stream is closed,an AVDT_CLOSE_CFM _EVT is sent to the application via the control
 *              callback function for this handle. AVDTP.ICS #Table 5-3, 5b-3 Close stream command
 * @param[in]   handle       Handle of stream
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_CloseReq(uint16_t handle);

/**
 *
 * @brief       Function AVDT_CloseRsp
 * @details     Respond to a close request from ACP.  This function must be called if the application receives an
 *              AVDT_CLOSE_IND_EVT through its control callback. AVDTP.ICS #Table 11-3, 11b-c Close stream response
 * @param[in]   handle       Handle of stream
 * @param[in]   transLabel   Label of close cmd
 * @param[in]   errCode      Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_CloseRsp(uint16_t handle, uint8_t transLabel, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_ReconfigReq
 * @details     Reconfigure a stream endpoint.  It allows the application can only be called if the stream is opened but
 *              not started to change the codec capabilities of a stream endpoint after it has been opened.  When the
 *              procedure is completed, an AVDT_RECONFIG _CFM_EVT is sent to the application via the control callback
 *              function for this handle. AVDTP.ICS #Table 4-5, 4b-5 Reconfigure command
 * @param[in]   handle       Handle of stream
 * @param[in]   cfg          Reconfigure information
 * @param[out]  transLabel   Label of this message
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_ReconfigReq(uint16_t handle, AvdtSepConfig *cfg, uint8_t *transLabel);

/**
 *
 * @brief       Funciton AVDT_ReconfigRsp
 * @details     Respond to a reconfigure request from ACP.This function must be called if the application receives an
 *              AVDT_RECONFIG_IND_EVT through its control callback. AVDTP.ICS #Table 10-5, 10b-5 Reconfigure response
 * @param[in]   handle       Handle of stream
 * @param[in]   transLabel   Label of reconfigure cmd
 * @param[in]   errCode      Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_ReconfigRsp(uint16_t handle, uint8_t transLabel, AvdtCatetory category);

/**
 *
 * @brief       Function AVDT_WriteReq
 * @details     Send a media packet from SOURCE to the SINK and the status of the stream must be streaming. AVDTP 13.2.1
 * @param[in]   handle       Handle of stream
 * @param[in]   pkt          Stream data
 * @param[in]   timeStamp    Timestamp of this stream data sent
 * @param[in]   payloadType  Payload type
 * @param[in]   marker       Marker: such ad frame boundaries in the data stream
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_WriteReq(
    uint16_t handle, const Packet *pkt, uint32_t timeStamp, uint8_t payloadType, uint16_t marker);
/**
 *
 * @brief       Function AVDT_ConnectRsp(Retain)
 * @details     Respond to a connect request from ACP. AVDTP.ICS #Table 9-1, 9b-1 Establish signaling channel
 * @param[in]   bdAddr       Peer device address
 * @param[in]   errCode      Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_ConnectRsp(const BtAddr *bdAddr, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_ConnectReq
 * @details     This function send an AVDT_CONNECT_REQ from INT to ACP.When the connection is completed, an
 *              AVDT_CONNECT_CFM_EVT is sent to the application via its control callback function. AVDTP.ICS #Table 3-1,
 *              3b-1 Establish signaling channel
 * @param[in]   bdAddr   Peer device address
 * @param[in]   role     Local role
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_ConnectReq(const BtAddr *bdAddr, uint8_t role);

/**
 *
 * @brief       Function AVDT_DisconnectReq
 * @details     This function send an AVDT_DISCONNECT_REQ from INT to ACP.When the connection is completed, an
 *              AVDT_DISCONNECT_CFM_EVT is sent to the application via its control callback function. AVDTP.ICS #Table
 *              3-2, 3b-2 Establish signaling channel
 * @param[in]   bdAddr       Peer device address
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_DisconnectReq(const BtAddr *bdAddr);

/**
 *
 * @brief       Function AVDT_DisconnectRsp(Retain)
 * @details     Respond to a disconnect request from ACP.
 *              AVDTP.ICS #Table 9-2, 9b-2 Establish signaling channel
 * @param[in]   bdAddr       Peer device address
 * @param[in]   errCode      Error code
 * @return      AVDT_SUCCESS if successful, otherwise error.
 *
 */
BTSTACK_API uint16_t AVDT_DisconnectRsp(const BtAddr *bdAddr, uint8_t errCode);

/**
 *
 * @brief       Function AVDT_GetL2capChannel(Retain)
 * @details     Get the L2CAP CID used by the handle.
 * @param[in]   handle       Handle of stream
 * @return      return channel id if successful, otherwise 0.
 *
 */
BTSTACK_API uint16_t AVDT_GetL2capChannel(uint16_t handle);

/**
 *
 * @brief       Function AVDT_Rej(Retain)
 * @details     Send a reject message from ACP to INT.
 *              AVDTP.ICS #Table 11-6,11b-6,16-3 General Reject Response Includes Signal ID,General reject message
 * @param[in]   handle       Handle of stream
 * @param[in]   bdAddr       Peer device address
 * @param[in]   cmd          Signaling of AVDT
 * @param[in]   transLabel    Label of cmd received
 * @param[in]   errInfo      Error info rejected for command received
 * @return      void
 *
 */
BTSTACK_API void AVDT_Rej(
    uint16_t handle, const BtAddr *bdAddr, uint8_t cmd, uint8_t transLabel, AvdtRejErrInfo errInfo);

#ifdef __cplusplus
}
#endif

#endif /* AVDTP_H */
