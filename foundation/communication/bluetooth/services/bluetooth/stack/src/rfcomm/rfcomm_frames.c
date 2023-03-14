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

#include "rfcomm_defs.h"

static const unsigned char G_CRCTABLE[256] = {  // Refer to B.3.5 Reversed CRC table of GSM 07.10,v6.3.0
    0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
    0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
    0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
    0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
    0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
    0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
    0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
    0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
    0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
    0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
    0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
    0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
    0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
    0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
    0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
    0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

static bool RfcommIsSabmDiscValid(RfcommCheckFrameValidInfo info);
static bool RfcommIsUaValid(RfcommCheckFrameValidInfo info);
static bool RfcommIsDmValid(RfcommCheckFrameValidInfo info);
static bool RfcommIsUihValid(RfcommCheckFrameValidInfo info);
static bool RfcommCheckFcs(uint8_t len, uint8_t recvfcs, const uint8_t *p);
static uint8_t RfcommCalculateFcs(uint8_t len, const uint8_t *p);
static void RfcommParseHeaderTail(Packet *pkt, RfcommFrameHeaderTailInfo *headTailInfo);
static RfcommEventType RfcommParseSabm(
    RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output);
static RfcommEventType RfcommParseDisc(
    RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output);
static RfcommEventType RfcommParseUa(
    RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output);
static RfcommEventType RfcommParseDm(
    RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output);
static RfcommEventType RfcommParseUih(RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, Packet *pkt,
    RfcommFlowControlType fcType, RfcommParseFrameResult output);
static RfcommEventType RfcommParseUihCmd(Packet *pkt, RfcommParseFrameResult output);
static RfcommEventType RfcommSetPnInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output);
static RfcommEventType RfcommSetMscInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output);
static RfcommEventType RfcommSetRpnInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output);
static RfcommEventType RfcommSetRlsInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output);
static RfcommEventType RfcommSetTestInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output);
static RfcommEventType RfcommSetFcOnOffInfo(Packet *pkt, uint8_t typeCr, uint8_t type, RfcommParseFrameResult output);
static RfcommEventType RfcommSetNscInfo(Packet *pkt, RfcommParseFrameResult output);

/**
 * @brief The function is used to calculate FCS.
 *        The function is referred to B.3.3 The transmitter code of GSM 07.10,v6.3.0.
 *
 * @param len The number of bytes to be calculated.
 * @param p   Pointer of the information to be calculated.
 * @return The FCS value.
 */
uint8_t RfcommCalculateFcs(uint8_t len, const uint8_t *p)
{
    LOG_INFO("%{public}s", __func__);

    // Init
    uint8_t fcs = 0xFF;

    // len is the number of bytes in the message, p points to message.
    while (len--) {
        fcs = G_CRCTABLE[fcs ^ *p++];
    }
    fcs = 0xFF - fcs;

    return fcs;
}

/**
 * @brief The function is used to check FCS.
 *        The function is referred to B.3.4 The receiver code of GSM 07.10,v6.3.0.
 *
 * @param len     The number of bytes to be calculated.
 * @param recvfcs Received fcs value.
 * @param p       Pointer of the information to be calculated.
 * @return Check result.true:FCS is OK,false:FCS is not OK.
 */
bool RfcommCheckFcs(uint8_t len, uint8_t recvfcs, const uint8_t *p)
{
    LOG_INFO("%{public}s", __func__);

    // Init
    uint8_t fcs = 0xFF;
    bool result = false;

    // len is the number of bytes in the message, p points to message.
    while (len--) {
        fcs = G_CRCTABLE[fcs ^ *p++];
    }
    fcs = G_CRCTABLE[fcs ^ recvfcs];
    if (fcs == 0xCF) {
        result = true;
    } else {
        LOG_ERROR("%{public}s Fcs check error, recvfcs is %hhu.", __func__, recvfcs);
    }

    return result;
}

/**
 * @brief The function is used to send SABM command.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendSabm(const RfcommSessionInfo *session, uint8_t dlci)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_SABM_HEADER_LEN] = {0};
    uint8_t cr = (session->isInitiator) ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (cr << RFCOMM_SHIFT_CR) | (dlci << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_SABM | PF;
    // Length
    header[RFCOMM_LENGTH_1] = EA;
    // FCS
    uint8_t tail = RfcommCalculateFcs(RFCOMM_NOT_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_SABM_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send DISC command.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendDisc(const RfcommSessionInfo *session, uint8_t dlci)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_DISC_HEADER_LEN] = {0};
    uint8_t cr = (session->isInitiator) ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (cr << RFCOMM_SHIFT_CR) | (dlci << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_DISC | PF;
    // Length
    header[RFCOMM_LENGTH_1] = EA;
    // FCS
    uint8_t tail = RfcommCalculateFcs(RFCOMM_NOT_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_DISC_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send UA response.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUa(const RfcommSessionInfo *session, uint8_t dlci)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_UA_HEADER_LEN] = {0};
    uint8_t cr = (!session->isInitiator) ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (cr << RFCOMM_SHIFT_CR) | (dlci << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UA | PF;
    // Length
    header[RFCOMM_LENGTH_1] = EA;
    // FCS
    uint8_t tail = RfcommCalculateFcs(RFCOMM_NOT_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_UA_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send DM response.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @param pf      Is the pf value 1.true:yes, false:no
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendDm(const RfcommSessionInfo *session, uint8_t dlci, bool pf)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_DM_HEADER_LEN] = {0};
    uint8_t cr = (!session->isInitiator) ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (cr << RFCOMM_SHIFT_CR) | (dlci << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_DM | (pf ? PF : 0);
    // Length
    header[RFCOMM_LENGTH_1] = EA;
    // FCS
    uint8_t tail = RfcommCalculateFcs(RFCOMM_NOT_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_DM_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send UIH(PN) command/response.
 *
 * @param session The pointer of the session in the session list.
 * @param dlci    The dlci value.
 * @param isCmd   <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @param pnInfo  The PN information to send to peer.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihPn(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd, const RfcommSendPnInfo *pnInfo)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_PN_HEADER_LEN] = {0};
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = isCmd ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + PN info 8byte)
    header[RFCOMM_LENGTH_1] = EA | (RFCOMM_PN_FRAME_LEN) << 1;
    // Information(Type)
    header[RFCOMM_INFO_TYPE] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_PN << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (RFCOMM_PN_INFO_LEN << 1);
    // Information(D1~D6)
    header[RFCOMM_PN_DLCI] = dlci;
    // Information(I1~I4,CL1~CL4)
    header[RFCOMM_PN_CL] = 0 | (pnInfo->cl << RFCOMM_PN_SHIFT_CL);
    // Information(P1~P6)
    header[RFCOMM_PN_PRIORITY] = pnInfo->priority;
    // Information(T1~T8)
    header[RFCOMM_PN_TIME] = 0;
    // Information(N1~N8)
    header[RFCOMM_PN_MTU1] = pnInfo->mtu & 0xFF;
    // Information(N9~N16)
    header[RFCOMM_PN_MTU2] = (pnInfo->mtu & 0xFF00) >> RFCOMM_PN_SHIFT_MTU;
    // Information(NA1~NA8)
    header[RFCOMM_PN_NA] = 0;
    // Information(K1~K3)
    header[RFCOMM_PN_CREDIT] = pnInfo->credits;
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_PN_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send UIH(MSC) command/response.
 *
 * @param session  The pointer of the session in the session list.
 * @param dlci     The dlci value.
 * @param isCmd    <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @param modemSts Modem status information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihMsc(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd, const RfcommModemStatusInfo *modemSts)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_MSC_HAVEBREAK_HEADER_LEN] = {0};
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = (isCmd) ? 1 : 0;
    uint8_t headerLength = (modemSts->breakSignal) ? RFCOMM_MSC_HAVEBREAK_HEADER_LEN : RFCOMM_MSC_NO_BREAK_HEADER_LEN;
    uint8_t frameLength = (modemSts->breakSignal) ? RFCOMM_MSC_FRAME_HAVEBREAK_LEN : RFCOMM_MSC_FRAME_NO_BREAK_LEN;
    uint8_t infoLength = (modemSts->breakSignal) ? RFCOMM_MSC_INFO_HAVEBREAK_LEN : RFCOMM_MSC_INFO_NO_BREAK_LEN;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + DLCI 1byte + V.24signals 1bytes +(breakSignal))
    header[RFCOMM_LENGTH_1] = EA | (frameLength << 1);
    // Information(Type)
    header[RFCOMM_INFO_TYPE] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_MSC << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (infoLength << 1);
    // Information(DLCI)
    header[RFCOMM_MSC_DLCI] = EA | (1 << 1) | (dlci << RFCOMM_SHIFT_DLCI);
    // Information(V.24 signals)
    header[RFCOMM_MSC_SIGNAL] = EA | (modemSts->signals << 1);
    // Information(Break signals)
    if (modemSts->breakSignal) {
        header[RFCOMM_MSC_BREAK] = EA | (modemSts->breakSignal << 1);
    }
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, headerLength, tail, NULL);
}

/**
 * @brief The function is used to send UIH(RLS) command/response.
 *
 * @param session    The pointer of the session in the session list.
 * @param dlci       The dlci value.
 * @param isCmd      <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @param lineStatus Line status information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihRls(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd, uint8_t lineStatus)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_RLS_HEADER_LEN] = {0};
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = isCmd ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + DLCI 1byte + rls 1bytes)
    header[RFCOMM_LENGTH_1] = EA | (RFCOMM_RLS_FRAME_LEN << 1);
    // Information(Type)
    header[RFCOMM_INFO_TYPE] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_RLS << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (RFCOMM_RLS_INFO_LEN << 1);
    // Information(DLCI)
    header[RFCOMM_RLS_DLCI] = EA | (1 << 1) | (dlci << RFCOMM_SHIFT_DLCI);
    // Information(rls)
    header[RFCOMM_RLS_STATUS] = lineStatus;
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_RLS_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send UIH(RPN) command/response.
 *
 * @param session    The pointer of the session in the session list.
 * @param dlci       The dlci value.
 * @param isCmd      <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @param portConfig Remote port setting information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihRpn(const RfcommSessionInfo *session, uint8_t dlci, bool isCmd,
                     const RfcommRemotePortConfig *portConfig)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_RPN_CMDHEADER_LEN] = {0};
    uint8_t headerLength = portConfig ? RFCOMM_RPN_CMDHEADER_LEN : RFCOMM_RPN_REQHEADER_LEN;
    uint8_t frameLength = portConfig ? RFCOMM_RPN_CMDFRAME_LEN : RFCOMM_RPN_REQFRAME_LEN;
    uint8_t infoLength = portConfig ? RFCOMM_RPN_CMDINFO_LEN : RFCOMM_RPN_REQINFO_LEN;
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = isCmd ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + DLCI 1byte + (rpn 7bytes))
    header[RFCOMM_LENGTH_1] = EA | (frameLength << 1);
    // Information(Type)
    header[RFCOMM_INFO_TYPE] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_RPN << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (infoLength << 1);
    // Information(DLCI)
    header[RFCOMM_RPN_DLCI] = EA | (1 << 1) | (dlci << RFCOMM_SHIFT_DLCI);
    // Information(rpn)
    if (portConfig != NULL) {
        header[RFCOMM_RPN_BAUDRATE] = portConfig->baudrate;
        header[RFCOMM_RPN_BITINFO] = portConfig->data_bits |
                                    ((portConfig->stop_bit) << RFCOMM_RPN_SHIFT_STOP_BIT) |
                                    ((portConfig->parity) << RFCOMM_RPN_SHIFT_PARITY) |
                                    ((portConfig->parity_type) << RFCOMM_RPN_SHIFT_PARITY_TYPE);
        header[RFCOMM_RPN_FLC] = portConfig->fc;
        header[RFCOMM_RPN_XON] = portConfig->xon_char;
        header[RFCOMM_RPN_XOFF] = portConfig->xoff_char;
        header[RFCOMM_RPN_MASK1] = portConfig->parameter_mask1;
        header[RFCOMM_RPN_MASK2] = portConfig->parameter_mask2;
    }
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, headerLength, tail, NULL);
}

/**
 * @brief The function is used to send UIH(FCon) command/response.
 *
 * @param session The pointer of the session in the session list.
 * @param isCmd   <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihFcon(const RfcommSessionInfo *session, bool isCmd)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_FCON_HEADER_LEN] = {0};
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = isCmd ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + 0byte)
    header[RFCOMM_LENGTH_1] = EA | (RFCOMM_FCON_FRAME_LEN << 1);
    // Information(Type)
    header[RFCOMM_INFO_TYPE] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_FCON << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (0 << 1);
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_FCON_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send UIH(FCoff) command/response.
 *
 * @param session The pointer of the session in the session list.
 * @param isCmd   <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihFcoff(const RfcommSessionInfo *session, bool isCmd)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_FCOFF_HEADER_LEN] = {0};
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = isCmd ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + 0byte)
    header[RFCOMM_LENGTH_1] = EA | (RFCOMM_FCOFF_FRAME_LEN << 1);
    // Information(Type)
    header[RFCOMM_INFO_TYPE] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_FCOFF << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (0 << 1);
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_FCOFF_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send UIH(Test) command/response.
 *
 * @param session The pointer of the session in the session list.
 * @param isCmd   <b>true</b> if this is a command; <b>false</b> if this is a response.
 * @param pkt     The payload information.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihTest(const RfcommSessionInfo *session, bool isCmd, Packet *pkt)
{
    LOG_INFO("%{public}s", __func__);

    size_t size = PacketPayloadSize(pkt);
    uint8_t header[RFCOMM_TEST_HEADER_LEN_MAX] = {0};
    uint8_t len = 0;
    uint8_t frameLength = (size <= 0x7F) ? (RFCOMM_TEST_FRAME_LEN_MIN + size) :
                                           (RFCOMM_TEST_FRAME_LEN_MIN + 1 + size);
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t infoCR = isCmd ? 1 : 0;

    // Address
    header[len] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    len++;
    // Control
    header[len] = FRAME_TYPE_UIH | 0;
    len++;
    // Length(info:type 1byte + len 1/2byte + info)
    if ((frameLength) <= 0x7F) {
        header[len] = EA | ((frameLength) << 1);
        len++;
    } else {
        header[len] = ((frameLength) & 0x7F) << 1;
        len++;
        header[len] = (frameLength) >> RFCOMM_SHIFT_LENGTH2;
        len++;
    }
    // Information(Type)
    header[len] = EA | (infoCR << RFCOMM_SHIFT_CR) | (UIH_TYPE_TEST << RFCOMM_SHIFT_TYPE);
    len++;
    // Information(information Length)
    if (size <= 0x7F) {
        header[len] = EA | (size << 1);
        len++;
    } else {
        header[len] = (size & 0x7F) << 1;
        len++;
        header[len] = size >> RFCOMM_SHIFT_LENGTH2;
        len++;
    }
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, len, tail, pkt);
}

/**
 * @brief The function is used to send UIH(NSC) response.
 *
 * @param session The pointer of the session in the session list.
 * @param ea      The EA bit in the type field of the not supported command frame.
 * @param cr      The C/R bit in the type field of the not supported command frame.
 * @param type    The not supported command type.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihNsc(const RfcommSessionInfo *session, uint8_t ea, uint8_t cr, uint8_t type)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t header[RFCOMM_NSC_HEADER_LEN] = {0};
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;

    // Address
    header[RFCOMM_ADDRESS] = EA | (addressCR << RFCOMM_SHIFT_CR) | (CONTROL_DLCI << RFCOMM_SHIFT_DLCI);
    // Control
    header[RFCOMM_CONTROL] = FRAME_TYPE_UIH | 0;
    // Length(info:type 1byte + len 1byte + info 1byte)
    header[RFCOMM_LENGTH_1] = EA | (RFCOMM_NSC_FRAME_LEN << 1);
    // Information(Type)
    // If the C/R bit is set to 1 the message is a command,
    // if it is set to 0 the message is a response.
    header[RFCOMM_INFO_TYPE] = EA | 0 | (UIH_TYPE_NSC << RFCOMM_SHIFT_TYPE);
    // Information(Length)
    header[RFCOMM_INFO_LEN] = EA | (RFCOMM_NSC_INFO_LEN << 1);
    // Information(Non supported command type)
    header[RFCOMM_NSC_TYPE] = ea | (cr << RFCOMM_SHIFT_CR) | (type << RFCOMM_SHIFT_TYPE);
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, RFCOMM_NSC_HEADER_LEN, tail, NULL);
}

/**
 * @brief The function is used to send data(or new credits) to peer.
 *
 * @param session    The pointer of the session in the session list.
 * @param dlci       The dlci value.
 * @param newCredits New credit count.
 * @param packet     The payload.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 */
int RfcommSendUihData(const RfcommSessionInfo *session, uint8_t dlci, uint8_t newCredits, Packet *pkt)
{
    LOG_INFO("%{public}s", __func__);

    size_t size = 0;
    uint8_t pf = newCredits ? 1 : 0;
    uint8_t addressCR = (session->isInitiator) ? 1 : 0;
    uint8_t header[RFCOMM_DATA_HEADER_LEN_MAX] = {0};
    uint8_t len = 0;

    if (pkt != NULL) {
        size = PacketPayloadSize(pkt);
    }

    // Address
    header[len] = EA | (addressCR << RFCOMM_SHIFT_CR) | (dlci << RFCOMM_SHIFT_DLCI);
    len++;
    // Control
    header[len] = FRAME_TYPE_UIH | (pf << RFCOMM_SHIFT_PF);
    len++;
    // Length(info:type 1byte + len 1byte + info 1byte)
    if (size <= 0x7F) {
        header[len] = EA | (size << 1);
        len++;
    } else {
        header[len] = (size & 0x7F) << 1;
        len++;
        header[len] = size >> RFCOMM_SHIFT_LENGTH2;
        len++;
    }
    // credits
    if (newCredits) {
        header[len] = newCredits;
        len++;
    }
    // FCS(For UIH frames: on Address and Control field.)
    uint8_t tail = RfcommCalculateFcs(RFCOMM_IS_UIH_FSC_LEN, header);

    return RfcommSendData(session->l2capId, header, len, tail, pkt);
}

/**
 * @brief The function is used to check if the SABM or DISC frames is valid.
 *
 * @param info The information to be checked validity.
 * @return Check result.true:valid,false:invalid.
 */
bool RfcommIsSabmDiscValid(RfcommCheckFrameValidInfo info)
{
    LOG_INFO("%{public}s", __func__);

    if ((info.dlci != 0) && !IS_DLCI_VALID(info.dlci)) {
        return false;
    }

    if ((!info.pf) || !IS_CMD(info.isInitiator, info.cr) || info.length ||
        !RfcommCheckFcs(RFCOMM_NOT_UIH_FSC_LEN, info.fcs, info.calcInfo)) {
        return false;
    }

    return true;
}

/**
 * @brief The function is used to check if the UA frames is valid.
 *
 * @param info The information to be checked validity.
 * @return Check result.true:valid,false:invalid.
 */
bool RfcommIsUaValid(RfcommCheckFrameValidInfo info)
{
    LOG_INFO("%{public}s", __func__);

    if ((info.dlci != 0) && !IS_DLCI_VALID(info.dlci)) {
        return false;
    }

    if ((!info.pf) || !IS_RSP(info.isInitiator, info.cr) || info.length ||
        !RfcommCheckFcs(RFCOMM_NOT_UIH_FSC_LEN, info.fcs, info.calcInfo)) {
        return false;
    }

    return true;
}

/**
 * @brief The function is used to check if the DM frames is valid.
 *
 * @param info The information to be checked validity.
 * @return Check result.true:valid,false:invalid.
 */
bool RfcommIsDmValid(RfcommCheckFrameValidInfo info)
{
    LOG_INFO("%{public}s", __func__);

    if ((info.dlci != 0) && !IS_DLCI_VALID(info.dlci)) {
        return false;
    }

    if (!IS_RSP(info.isInitiator, info.cr) || info.length ||
        !RfcommCheckFcs(RFCOMM_NOT_UIH_FSC_LEN, info.fcs, info.calcInfo)) {
        return false;
    }

    return true;
}

/**
 * @brief The function is used to check if the UIH frames is valid.
 *
 * @param info The information to be checked validity.
 * @return Check result.true:valid,false:invalid.
 */
bool RfcommIsUihValid(RfcommCheckFrameValidInfo info)
{
    LOG_INFO("%{public}s", __func__);

    if ((info.dlci != 0) && !IS_DLCI_VALID(info.dlci)) {
        LOG_ERROR("%{public}s Invalid dlci:%hhu.", __func__, info.dlci);
        return false;
    }

    if (!IS_CMD(info.isInitiator, info.cr) || !RfcommCheckFcs(RFCOMM_IS_UIH_FSC_LEN, info.fcs, info.calcInfo)) {
        LOG_ERROR("%{public}s Uih is invalid, isInitiator:%{public}d, cr:%hhu.", __func__, info.isInitiator, info.cr);
        return false;
    }

    return true;
}

/**
 * @brief The function is used to analysis received data.
 *
 * @param session The pointer of the session in the session list.
 * @param pkt     The data received from L2CAP.
 * @param output  Parsed data.
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseFrames(const RfcommSessionInfo *session, Packet *pkt, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    RfcommEventType ret;
    RfcommFrameHeaderTailInfo headTailInfo;
    RfcommCheckFrameValidInfo checkInfo;
    (void)memset_s(&headTailInfo, sizeof(RfcommFrameHeaderTailInfo), 0x00, sizeof(RfcommFrameHeaderTailInfo));
    (void)memset_s(&checkInfo, sizeof(RfcommCheckFrameValidInfo), 0x00, sizeof(RfcommCheckFrameValidInfo));

    // Parse frames header and tail information.
    RfcommParseHeaderTail(pkt, &headTailInfo);

    // Set ht information to be checked for validity.
    checkInfo.isInitiator = session->isInitiator;
    checkInfo.cr = headTailInfo.cr;
    checkInfo.pf = headTailInfo.pf;
    checkInfo.dlci = headTailInfo.dlci;
    checkInfo.length = headTailInfo.length;
    checkInfo.fcs = headTailInfo.fcs;
    (void)memcpy_s(checkInfo.calcInfo, sizeof(checkInfo.calcInfo), headTailInfo.calcInfo, sizeof(checkInfo.calcInfo));

    switch (headTailInfo.type) {
        case FRAME_TYPE_SABM:
            ret = RfcommParseSabm(checkInfo, headTailInfo.dlci, output);
            break;
        case FRAME_TYPE_UA:
            ret = RfcommParseUa(checkInfo, headTailInfo.dlci, output);
            break;
        case FRAME_TYPE_DISC:
            ret = RfcommParseDisc(checkInfo, headTailInfo.dlci, output);
            break;
        case FRAME_TYPE_DM:
            ret = RfcommParseDm(checkInfo, headTailInfo.dlci, output);
            break;
        case FRAME_TYPE_UIH:
            ret = RfcommParseUih(checkInfo, headTailInfo.dlci, pkt, session->fcType, output);
            break;
        default:
            ret = EVENT_FRAME_ERR;
            break;
    }

    return ret;
}

/**
 * @brief The function is used to analysis frames header and tail information.
 *
 * @param pkt           The data received from L2CAP.
 * @param headTailInfo  Parsed data.
 * @return event type. @see RfcommEventType
 */
void RfcommParseHeaderTail(Packet *pkt, RfcommFrameHeaderTailInfo *headTailInfo)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t fcs = 0;
    uint8_t header[4] = {0};

    // Frame header
    PacketExtractHead(pkt, header, 0x03);
    // Address's CR
    uint8_t cr = (header[RFCOMM_ADDRESS] >> RFCOMM_SHIFT_CR) & 1;
    // Address's dlci
    uint8_t dlci = header[RFCOMM_ADDRESS] >> RFCOMM_SHIFT_DLCI;
    // Control's frame type
    uint8_t type = header[RFCOMM_CONTROL] & (~PF);
    // Control's PF
    uint8_t pf = (header[RFCOMM_CONTROL] & PF) >> RFCOMM_SHIFT_PF;
    // Info's length
    uint8_t ea = header[RFCOMM_LENGTH_1] & EA;
    uint16_t length = header[RFCOMM_LENGTH_1] >> 1;
    if (!ea) {
        PacketExtractHead(pkt, &header[RFCOMM_LENGTH_2], 0x01);
        length += (header[RFCOMM_LENGTH_2] << RFCOMM_SHIFT_LENGTH2);
    }
    // fcs
    PacketExtractTail(pkt, &fcs, 0x01);

    headTailInfo->fcs = fcs;
    headTailInfo->cr = cr;
    headTailInfo->dlci = dlci;
    headTailInfo->type = type;
    headTailInfo->pf = pf;
    headTailInfo->length = length;
    headTailInfo->calcInfo[RFCOMM_OFFSET_0] = header[RFCOMM_OFFSET_0];
    headTailInfo->calcInfo[RFCOMM_OFFSET_1] = header[RFCOMM_OFFSET_1];
    headTailInfo->calcInfo[RFCOMM_OFFSET_2] = header[RFCOMM_OFFSET_2];
}

/**
 * @brief The function is used to analysis SABM information.
 *
 * @param checkInfo The info to be checked validity.
 * @param addrDlci  The dlci value.
 * @param output    Parsed data.
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseSabm(RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    if (!RfcommIsSabmDiscValid(checkInfo)) {
        return EVENT_FRAME_ERR;
    }

    if (addrDlci == 0) {
        *output.event = EV_SESSION_RECV_SABM0;
        return EVENT_SESSION;
    }

    *output.dlci = addrDlci;
    *output.event = EV_CHANNEL_RECV_SABM;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to analysis DISC information.
 *
 * @param checkInfo The info to be checked validity.
 * @param addrDlci  The dlci value.
 * @param output    Parsed data.
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseDisc(RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    if (!RfcommIsSabmDiscValid(checkInfo)) {
        return EVENT_FRAME_ERR;
    }

    if (addrDlci == 0) {
        *output.event = EV_SESSION_RECV_DISC0;
        return EVENT_SESSION;
    }

    *output.dlci = addrDlci;
    *output.event = EV_CHANNEL_RECV_DISC;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to analysis UA information.
 *
 * @param checkInfo The info to be checked validity.
 * @param addrDlci  The dlci value.
 * @param output    Parsed data.
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseUa(RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    if (!RfcommIsUaValid(checkInfo)) {
        return EVENT_FRAME_ERR;
    }

    if (addrDlci == 0) {
        *output.event = EV_SESSION_RECV_UA0;
        return EVENT_SESSION;
    }

    *output.dlci = addrDlci;
    *output.event = EV_CHANNEL_RECV_UA;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to analysis DM information.
 *
 * @param checkInfo The info to be checked validity.
 * @param addrDlci  The dlci value.
 * @param output    Parsed data.
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseDm(RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    if (!RfcommIsDmValid(checkInfo)) {
        return EVENT_FRAME_ERR;
    }

    if (addrDlci == 0) {
        *output.event = EV_SESSION_RECV_DM0;
        return EVENT_SESSION;
    }

    *output.dlci = addrDlci;
    *output.event = EV_CHANNEL_RECV_DM;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to analysis UIH information.
 *
 * @param checkInfo The info to be checked validity.
 * @param addrDlci  The dlci value.
 * @param output    Parsed data.
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseUih(RfcommCheckFrameValidInfo checkInfo, uint8_t addrDlci, Packet *pkt,
    RfcommFlowControlType fcType, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    size_t size = 0;
    uint8_t credit = 0;

    if (!RfcommIsUihValid(checkInfo)) {
        return EVENT_FRAME_ERR;
    }

    if (addrDlci == 0) {
        return RfcommParseUihCmd(pkt, output);
    }

    // When the DLCI in address is not 0, it is considered as data transmission.
    // UIH frames with P/F-bit = 1 and credit based flow control used.
    if ((checkInfo.pf == 1) && (fcType == FC_TYPE_CREDIT)) {
        PacketExtractHead(pkt, &credit, 1);
        output.info->data.credits = credit;
    }

    if (pkt != NULL) {
        size = PacketSize(pkt);
    }

    if (checkInfo.length != size) {
        LOG_ERROR("%{public}s Invalid length.Length is %hu,received size is %zu.", __func__, checkInfo.length, size);
        return EVENT_FRAME_ERR;
    }

    output.info->data.payload = pkt;
    output.info->data.size = checkInfo.length;

    *output.dlci = addrDlci;
    *output.event = EV_CHANNEL_RECV_DATA;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to analysis UIH data.
 *
 * @param pkt    The data received from L2CAP.
 * @param dlci   The dlci where the data is transmitted.
 * @param event  id: @see RfcommChannelEvent, RfcommSessionEvent
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommParseUihCmd(Packet *pkt, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data = 0;
    RfcommEventType ret;

    // If DLCI is 0, it is considered as control information transmission on the data link.
    PacketExtractHead(pkt, &data, 0x01);
    uint8_t ea = data & EA;
    uint8_t cr = (data >> RFCOMM_SHIFT_CR) & 1;
    uint8_t type = data >> RFCOMM_SHIFT_TYPE;

    if (!ea) {
        return EVENT_FRAME_ERR;
    }

    switch (type) {
        case UIH_TYPE_PN:
            ret = RfcommSetPnInfo(pkt, cr, output);
            break;
        case UIH_TYPE_MSC:
            ret = RfcommSetMscInfo(pkt, cr, output);
            break;
        case UIH_TYPE_RPN:
            ret = RfcommSetRpnInfo(pkt, cr, output);
            break;
        case UIH_TYPE_RLS:
            ret = RfcommSetRlsInfo(pkt, cr, output);
            break;
        case UIH_TYPE_TEST:
            ret = RfcommSetTestInfo(pkt, cr, output);
            break;
        case UIH_TYPE_FCON:
            ret = RfcommSetFcOnOffInfo(pkt, cr, UIH_TYPE_FCON, output);
            break;
        case UIH_TYPE_FCOFF:
            ret = RfcommSetFcOnOffInfo(pkt, cr, UIH_TYPE_FCOFF, output);
            break;
        case UIH_TYPE_NSC:
            ret = RfcommSetNscInfo(pkt, output);
            break;
        default:
            output.info->invalidCmd.ea = ea;
            output.info->invalidCmd.cr = cr;
            output.info->invalidCmd.type = type;
            ret = EVENT_UNSUPPORTED_CMD;
            break;
    }

    return ret;
}

/**
 * @brief The function is used to set PN information.
 *
 * @param pkt    The data received from L2CAP.
 * @param typeCr The cr value in type of the frames.
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetPnInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data[8] = {0};

    PacketExtractHead(pkt, data, 0x01);
    uint8_t ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t len = data[RFCOMM_OFFSET_0] >> 1;
    if (!ea || (len != RFCOMM_PN_INFO_LEN)) {
        return EVENT_FRAME_ERR;
    }
    (void)memset_s(data, sizeof(data), 0x00, sizeof(data));
    PacketExtractHead(pkt, data, 0x08);
    output.info->pn.dlci = data[RFCOMM_OFFSET_0] & 0x3f;
    output.info->pn.cl = data[RFCOMM_OFFSET_1] >> RFCOMM_PN_SHIFT_CL;
    output.info->pn.priority = data[RFCOMM_OFFSET_2] & 0x3f;
    // T1-T8,(Acknowledgment timer T1, which is not negotiable in RFCOMM.)
    output.info->pn.mtu = data[RFCOMM_OFFSET_4];
    output.info->pn.mtu += data[RFCOMM_OFFSET_5] << RFCOMM_PN_SHIFT_MTU;
    output.info->pn.k = data[RFCOMM_OFFSET_7] & 0x07;

    if (!IS_DLCI_VALID(output.info->pn.dlci) || !IS_MTU_VALID(output.info->pn.mtu)) {
        return EVENT_FRAME_ERR;
    }

    *output.dlci = output.info->pn.dlci;
    *output.event = (typeCr == 1) ? EV_CHANNEL_RECV_PN_REQ : EV_CHANNEL_RECV_PN_RSP;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to set MSC information.
 *
 * @param pkt    The data received from L2CAP.
 * @param typeCr The cr value in type of the frames.
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetMscInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data[3] = {0};

    PacketExtractHead(pkt, data, 1);
    uint8_t ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t len = data[RFCOMM_OFFSET_0] >> 1;
    if (!ea || ((len != RFCOMM_MSC_INFO_NO_BREAK_LEN) && (len != RFCOMM_MSC_INFO_HAVEBREAK_LEN))) {
        return EVENT_FRAME_ERR;
    }
    (void)memset_s(data, sizeof(data), 0x00, sizeof(data));
    PacketExtractHead(pkt, data, 0x02);
    ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t cr = (data[RFCOMM_OFFSET_0] >> RFCOMM_SHIFT_CR) & 1;
    output.info->msc.dlci = data[RFCOMM_OFFSET_0] >> RFCOMM_SHIFT_DLCI;
    output.info->msc.signal = data[RFCOMM_OFFSET_1] >> 1;
    if (len == RFCOMM_MSC_INFO_HAVEBREAK_LEN) {
        PacketExtractHead(pkt, &data[RFCOMM_OFFSET_2], 0x01);
        output.info->msc.breakSignal = data[RFCOMM_OFFSET_2] >> 1;
    }
    if (!ea || !cr || !IS_DLCI_VALID(output.info->msc.dlci)) {
        return EVENT_FRAME_ERR;
    }

    *output.dlci = output.info->msc.dlci;
    *output.event = (typeCr == 1) ? EV_CHANNEL_RECV_MSC_REQ : EV_CHANNEL_RECV_MSC_RSP;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to set RPN information.
 *
 * @param pkt    The data received from L2CAP.
 * @param typeCr The cr value in type of the frames.
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetRpnInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data[8] = {0};

    PacketExtractHead(pkt, data, 1);
    uint8_t ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t len = data[RFCOMM_OFFSET_0] >> 1;
    if (!ea || ((len != RFCOMM_RPN_REQINFO_LEN) && (len != RFCOMM_RPN_CMDINFO_LEN))) {
        return EVENT_FRAME_ERR;
    }
    (void)memset_s(data, sizeof(data), 0x00, sizeof(data));
    PacketExtractHead(pkt, data, 1);
    ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t cr = (data[RFCOMM_OFFSET_0] >> RFCOMM_SHIFT_CR) & 1;
    output.info->rpn.dlci = data[RFCOMM_OFFSET_0] >> RFCOMM_SHIFT_DLCI;
    if (!ea || !cr || !IS_DLCI_VALID(output.info->rpn.dlci)) {
        return EVENT_FRAME_ERR;
    }
    if (len != RFCOMM_RPN_CMDINFO_LEN) {
        output.info->rpn.isCmd = false;
    } else {
        PacketExtractHead(pkt, &data[RFCOMM_OFFSET_1], 0x07);
        output.info->rpn.isCmd = true;
        output.info->rpn.rpnInfo.baudrate = data[RFCOMM_OFFSET_1];
        output.info->rpn.rpnInfo.data_bits = data[RFCOMM_OFFSET_2] & 0x03;
        output.info->rpn.rpnInfo.stop_bit = (data[RFCOMM_OFFSET_2] >> RFCOMM_RPN_SHIFT_STOP_BIT) & 1;
        output.info->rpn.rpnInfo.parity = (data[RFCOMM_OFFSET_2] >> RFCOMM_RPN_SHIFT_PARITY) & 1;
        output.info->rpn.rpnInfo.parity_type = (data[RFCOMM_OFFSET_2] >> RFCOMM_RPN_SHIFT_PARITY_TYPE) & 0x03;
        output.info->rpn.rpnInfo.fc = data[RFCOMM_OFFSET_3] & 0x3f;
        output.info->rpn.rpnInfo.xon_char = data[RFCOMM_OFFSET_4];
        output.info->rpn.rpnInfo.xoff_char = data[RFCOMM_OFFSET_5];
        output.info->rpn.rpnInfo.parameter_mask1 = data[RFCOMM_OFFSET_6] & 0x7f;
        output.info->rpn.rpnInfo.parameter_mask2 = data[RFCOMM_OFFSET_7] & 0x3f;
    }

    *output.dlci = output.info->rpn.dlci;
    if (typeCr == 1) {
        *output.event = (output.info->rpn.isCmd) ? EV_CHANNEL_RECV_RPN_CMD : EV_CHANNEL_RECV_RPN_REQ;
    } else {
        *output.event = EV_CHANNEL_RECV_RPN_RSP;
    }

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to set RLS information.
 *
 * @param pkt    The data received from L2CAP.
 * @param typeCr The cr value in type of the frames.
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetRlsInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data[2] = {0};

    PacketExtractHead(pkt, data, 0x01);
    uint8_t ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t len = data[RFCOMM_OFFSET_0] >> 1;
    if (!ea || (len != RFCOMM_RLS_INFO_LEN)) {
        return EVENT_FRAME_ERR;
    }
    (void)memset_s(data, sizeof(data), 0x00, sizeof(data));
    PacketExtractHead(pkt, data, 0x02);
    ea = data[RFCOMM_OFFSET_0] & EA;
    uint8_t cr = (data[RFCOMM_OFFSET_0] >> RFCOMM_SHIFT_CR) & 1;
    output.info->rls.dlci = data[RFCOMM_OFFSET_0] >> RFCOMM_SHIFT_DLCI;
    output.info->rls.lineStatus = data[RFCOMM_OFFSET_1];

    if (!ea || !cr || !IS_DLCI_VALID(output.info->rls.dlci)) {
        return EVENT_FRAME_ERR;
    }

    *output.dlci = output.info->rls.dlci;
    *output.event = (typeCr == 1) ? EV_CHANNEL_RECV_RLS_REQ : EV_CHANNEL_RECV_RLS_RSP;

    return EVENT_CHANNEL;
}

/**
 * @brief The function is used to set NSC information.
 *
 * @param pkt     The data received from L2CAP.
 * @param output  Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetNscInfo(Packet *pkt, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data = 0;

    PacketExtractHead(pkt, &data, 0x01);
    uint8_t ea = data & EA;
    uint8_t len = data >> 1;
    if (!ea || (len != RFCOMM_NSC_INFO_LEN)) {
        return EVENT_FRAME_ERR;
    }
    PacketExtractHead(pkt, &data, 0x01);
    output.info->nsc.ea = data & 1;
    output.info->nsc.cr = (data >> RFCOMM_SHIFT_CR) & 1;
    output.info->nsc.type = data >> RFCOMM_SHIFT_TYPE;

    *output.event = EV_SESSION_RECV_NSC;

    return EVENT_SESSION;
}

/**
 * @brief The function is used to set FC on information.
 *
 * @param pkt    The data received from L2CAP.
 * @param typeCr The cr value in type of the frames.
 * @param type   UIH command type.
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetFcOnOffInfo(Packet *pkt, uint8_t typeCr, uint8_t type, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data = 0;

    PacketExtractHead(pkt, &data, 1);
    uint8_t ea = data & EA;
    uint8_t len = data >> 1;

    if (!ea || (len != 0)) {
        return EVENT_FRAME_ERR;
    }

    if (type == UIH_TYPE_FCON) {
        *output.event = (typeCr == 1) ? EV_SESSION_RECV_FCON_REQ : EV_SESSION_RECV_FCON_RSP;
    } else if (type == UIH_TYPE_FCOFF) {
        *output.event = (typeCr == 1) ? EV_SESSION_RECV_FCOFF_REQ : EV_SESSION_RECV_FCOFF_RSP;
    } else {
        LOG_ERROR("%{public}s error type:%hhu.", __func__, type);
    }

    return EVENT_SESSION;
}

/**
 * @brief The function is used to set test information.
 *
 * @param pkt    The data received from L2CAP.
 * @param typeCr The cr value in type of the frames.
 * @param output Parsed data
 * @return event type. @see RfcommEventType
 */
RfcommEventType RfcommSetTestInfo(Packet *pkt, uint8_t typeCr, RfcommParseFrameResult output)
{
    LOG_INFO("%{public}s", __func__);

    uint8_t data = 0;

    PacketExtractHead(pkt, &data, 0x01);
    uint8_t ea = data & EA;
    uint8_t len = data >> 1;

    if (!ea) {
        PacketExtractHead(pkt, &data, 0x01);
        len += (data << RFCOMM_SHIFT_LENGTH2);
    }
    output.info->test.length = len;
    output.info->test.pkt = pkt;

    *output.event = (typeCr == 1) ? EV_SESSION_RECV_TEST_REQ : EV_SESSION_RECV_TEST_RSP;

    return EVENT_SESSION;
}
