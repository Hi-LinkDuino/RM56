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

#ifndef AVDTP_MESSAGE_H
#define AVDTP_MESSAGE_H

#include "avdtp_int.h"

/**
 * Buffer size
 */
#define AVDT_BUFFER_CMD_MAX 660      // Mtu 672 ,L2CAP Header:12
#define AVDT_BUFFER_SINGLE 2         // Single message header
#define AVDT_BUFFER_START 3          // Fragment message, start
#define AVDT_BUFFER_REJECT 3         // reject message
#define AVDT_BUFFER_REJECT_4 4       // reject message
#define AVDT_BUFFER_GENERAL_REJ 2    // general reject
#define AVDT_BUFFER_END 1            // Fragment message,continue or end
#define AVDT_BUFFER_ERR_CODE 1       // Error code
#define AVDT_BUFFER_SEID 1           // Each seid size
#define AVDT_BUFFER_HEADER_MAX 5     // Max size of message header
#define AVDT_BUFFER_DISCOVER_RSP 30  //
#define AVDT_BUFFER_ALLCAP_RSP 128   //

/**
 * 0x00 – Forbidden 0x01 – 0x3E valid SEID values 0x3F – RFD
 */
#define AVDT_SEID_MIN 0x01
#define AVDT_SEID_MAX 0x3E
#define AVDT_SEID_FORBID 0x00
#define AVDT_SEID_INVLID 0x3F

/**
 * Packet type
 */
#define AVDT_PKT_TYPE_SINGLE 0    // Single packet
#define AVDT_PKT_TYPE_START 1     // Start packet
#define AVDT_PKT_TYPE_CONTINUE 2  // Continue packet
#define AVDT_PKT_TYPE_END 3       // End packet

/**
 * Message type
 */
#define AVDT_MSG_TYPE_CMD 0  // Command
#define AVDT_MSG_TYPE_GRJ 1  // General reject
#define AVDT_MSG_TYPE_RSP 2  // Response accept
#define AVDT_MSG_TYPE_REJ 3  // Response reject

/**
 * Signaling command
 */
#define AVDT_SIG_INVALID 0x00     // Invalid signal
#define AVDT_SIG_DISCOVER 0x01    // Discover
#define AVDT_SIG_GETCAP 0x02      // Get capabilities
#define AVDT_SIG_SETCONFIG 0x03   // Set configuration
#define AVDT_SIG_GETCONFIG 0x04   // Get configuration
#define AVDT_SIG_RECONFIG 0x05    // Reconfigure
#define AVDT_SIG_OPEN 0x06        // Open
#define AVDT_SIG_START 0x07       // Start
#define AVDT_SIG_CLOSE 0x08       // Close
#define AVDT_SIG_SUSPEND 0x09     // Suspend
#define AVDT_SIG_ABORT 0x0A       // Abort
#define AVDT_SIG_SECURITY 0x0B    // Security control
#define AVDT_SIG_GET_ALLCAP 0x0C  // Get all capabilities
#define AVDT_SIG_DELAY_RPT 0x0D   // Delay report

/**
 * Maximum signal value
 */
#define AVDT_SIG_MAX AVDT_SIG_DELAY_RPT

/**
 * Profile Service Category
 */
#define AVDT_PSC_TRANS 1      // Media Transport
#define AVDT_PSC_REPORT 2     // Reporting
#define AVDT_PSC_RECOV 3      // Recovery
#define AVDT_PSC_PROTECT 4    // Content Protection
#define AVDT_PSC_HDRCMP 5     // Header Compression
#define AVDT_PSC_MUX 6        // Multiplexing
#define AVDT_PSC_CODEC 7      // Media Codec
#define AVDT_PSC_DELAY_RPT 8  // Delay Reporting
#define AVDT_PSC_MAX_CUR AVDT_PSC_DELAY_RPT

#define MOVE_4BIT 4
#define MOVE_2BIT 2

/**
 * Channel types
 */
enum {
    AVDT_CH_TYPE_SIG,     // Signaling channel
    AVDT_CH_TYPE_STREAM,  // Streaming Media channel
    AVDT_CH_TYPE_NUM_TYPES
};

/**
 * Function declarations
 */
/* Msg function declarations */
extern void AvdtSendSig(AvdtSigCtrl *sigCtrl);
extern void AvdtMsgSendCmd(AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
extern void AvdtMsgSendRsp(AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
extern void AvdtMsgSendRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
extern void AvdtMsgSendGeneralRej(AvdtSigCtrl *sigCtrl, uint8_t sigId);

/**
 * Inner function declaration
 */
/**
 * Build command
 */
void AvdtBuildSingleHeader(uint8_t *data, uint8_t label, uint8_t pktType, uint8_t msgType);
void AvdtBuildSignal(uint8_t *data, uint8_t signal);
void AvdtBuildSignalSeid(uint8_t *data, uint8_t seid);
void AvdtBuildDelayCmd(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildDiscoverCmd(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildSingleCmd(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildSetConfigureCmd(
    AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildReconfigureCmd(
    AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtSendFragPacket(uint16_t lcid, Packet *pkt, uint16_t peerMtu);
uint16_t AvdtBuildCapability(const Packet *pkt, const AvdtSepConfig *cfg, uint8_t off, uint8_t sigId);
uint16_t AvdtSendFragStart(uint16_t lcid, Packet *fragmentPacket, uint8_t signal, uint8_t number, uint8_t msgType,
    uint8_t label, uint16_t peerMtu);
uint16_t AvdtSendFragContinueEnd(
    uint16_t lcid, Packet *fragmentPacket, uint8_t pktType, uint8_t msgType, uint8_t label, uint16_t peerMtu);
/**
 * Build respond
 */
void AvdtBuildGetCapRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildDiscoverRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildSingleRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildGetConfigureRsp(
    AvdtSigCtrl *sigCtrl, AvdtStreamCtrl *streamCtrl, uint8_t sigId, const AvdtMsg *avdtMsgData);
void AvdtBuildSeps(const Packet *pkt, const AvdtMsg *avdtMsgData);

/**
 * Build reject
 */
void AvdtBuildCommonRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t errCode);
void AvdtBuildConfigureRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t category, uint8_t errCode);
void AvdtSignalGeneralRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label);
/**
 * Parse indication message
 */
void AvdtParseMsg(AvdtSigCtrl *sigCtrl, Packet *pkt);
void AvdtParseSingleMsg(AvdtSigCtrl *sigCtrl, Packet *pkt, uint8_t transLabel, uint8_t messageType);
void AvdtParseSignalInd(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label, const Packet *pkt);
void AvdtParseCommonInd(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label, const Packet *pkt, uint8_t event);
void AvdtParseReconfigInd(AvdtSigCtrl *sigCtrl, uint8_t label, const Packet *pkt, uint8_t event);
void AvdtParseSetconfigInd(AvdtSigCtrl *sigCtrl, uint8_t label, const Packet *pkt, uint8_t event);
void AvdtParseDelayRptInd(AvdtSigCtrl *sigCtrl, uint8_t label, const Packet *pkt, uint8_t event);
void AvdtParseSignalRsp(AvdtSigCtrl *sigCtrl, uint8_t sigId, const Packet *pkt);
void AvdtAssembleMsg(AvdtSigCtrl *sigCtrl, uint8_t msgType, uint8_t pktType, Packet *pkt);
void AvdtParseSignalRej(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t label, Packet *pkt);
void AvdtParseSetconfig(uint16_t streamHandle, const Packet *pkt, uint16_t off, uint8_t *errCode, uint8_t *errParam);
void AvdtParseReconfig(uint16_t streamHandle, const Packet *pkt, uint16_t off, uint8_t *errCode, uint8_t *errParam);
uint8_t AvdtGetEventBySigidInd(uint8_t sigId);
uint8_t AvdtParseTransLabel(uint8_t header);
uint8_t AvdtParsePacketType(uint8_t header);
uint8_t AvdtParseMessageType(uint8_t header);
uint8_t AvdtParseSignal(uint8_t header);
uint8_t AvdtParseSeid(uint8_t data);
uint16_t AvdtParseDelayValue(uint8_t maxSB, uint8_t lowSB);

/**
 * Parse confirm message
 */
void AvdtParseGetCapRsp(AvdtCtrlData *data, const Packet *pkt);
void AvdtParseGetConfigRsp(AvdtCtrlData *data, const Packet *pkt);
void AvdtParseDiscoverRsp(AvdtSigCtrl *sigCtrl, AvdtCtrlData *data, const Packet *pkt);
void AvdtParseMsgAssembled(AvdtSigCtrl *sigCtrl, Packet *pkt);

uint8_t AvdtIndPktLenCheck(AvdtSigCtrl *sigCtrl, uint8_t sigId, const Packet *pkt);
void AvdtMakeRejMsg(uint8_t sigId, uint8_t errCode, uint8_t errParam, AvdtMsg *msg);
uint8_t AvdtIndSeidCheck(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t data);
uint8_t AvdtServiceCapCheck(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t data);
void AvdtIndReject(AvdtSigCtrl *sigCtrl, uint8_t sigId, uint8_t errCode, uint8_t errParam);
uint8_t AvdtCheckSignal(uint8_t sigId);
uint8_t AvdtParseCategoryCodec(AvdtSepConfig *sepConfig, const Packet *pkt, uint16_t offset);
uint8_t AvdtParseCategoryTrans(AvdtSepConfig *sepConfig, const Packet *pkt, uint16_t offset, uint8_t *data);
uint8_t AvdtParseCategoryDelay(AvdtSepConfig *sepConfig, const Packet *pkt, uint16_t offset, uint8_t *data);
uint8_t AvdtParseCategorySkip(const Packet *pkt, uint16_t offset);
void AvdtParseCategory(AvdtCtrlData *data, const Packet *pkt);

#endif /* AVDTP_MESSAGE_H */
