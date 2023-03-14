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
 * @file l2cap_def.h
 *
 * @brief Interface of bluetooth l2cap protocol BR/EDR part
 *
 */

#ifndef L2CAP_DEF_H
#define L2CAP_DEF_H

#include <stdint.h>
#include "packet.h"
#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// -------------------- Below is for BR/EDR ------------------------------

#define L2CAP_INFORMATION_TYPE_CONNECTIONLESS_MTU 0x0001  // not supported in this implementation
#define L2CAP_INFORMATION_TYPE_EXTENDED_FEATURE 0x0002
#define L2CAP_INFORMATION_TYPE_FIXED_CHANNEL 0x0003

// the first octet for extended feature mask
#define L2CAP_FEATURE_FLOW_CONTROL_MODE 0x01
#define L2CAP_FEATURE_RETRANSMISSION_MODE 0x02
#define L2CAP_FEATURE_BIDIRECTIONAL_QOS 0x04
#define L2CAP_FEATURE_ENHANCED_RETRANSMISSION_MODE 0x08
#define L2CAP_FEATURE_STREAMING_MODE 0x10
#define L2CAP_FEATURE_FCS_OPTION 0x20
#define L2CAP_FEATURE_EXTENDED_FLOW_SPECIFICATION 0x40
#define L2CAP_FEATURE_FIXED_CHANNELS 0x80

// the second octet for extended feature mask
#define L2CAP_FEATURE_EXTENDED_WINDOW_SIZE 0x01
#define L2CAP_FEATURE_UNICAST_CONNECTIONLESS_DATA 0x02

// the first octet for fixed channels supported
#define L2CAP_FEATURE_SIGNALING_CHANNEl 0x02
#define L2CAP_FEATURE_CONNECTIONLESS_RECEPTION 0x04
#define L2CAP_FEATURE_AMP_MANAGER_PROTOCOL 0x08
#define L2CAP_FEATURE_BREDR_SECURITY_MANAGER 0x80

// the eighth octet for fixed channels supported
#define L2CAP_FEATURE_AMP_TEST_MANAGER 0x80

// l2cap mode
#define L2CAP_BASIC_MODE 0x00
#define L2CAP_ENHANCED_RETRANSMISSION_MODE 0x03
#define L2CAP_STREAM_MODE 0x04

#define L2CAP_DEFAULT_MTU 672

// L2cap connection response result
#define L2CAP_CONNECTION_SUCCESSFUL 0x0000
#define L2CAP_CONNECTION_PENDING 0x0001
#define L2CAP_PSM_NOT_SUPPORTED 0x0002
#define L2CAP_SECURITY_BLOCK 0x0003
#define L2CAP_NO_RESOURCES_AVAILABLE 0x0004
#define L2CAP_INVALID_SOURCE_CID 0x0006
#define L2CAP_SOURCE_CID_ALREADY_ALLOCATED 0x0007

// L2cap connection response status
#define L2CAP_NO_FURTHER_INFORMATION_AVAILABLE 0x0000
#define L2CAP_AUTHENTICATION_PENDING 0x0001
#define L2CAP_AUTHORIZATION_PENDING 0x0002

// L2cap config response result
#define L2CAP_SUCCESS 0x0000
#define L2CAP_UNACCEPTABLE_PARAMETERS 0x0001
#define L2CAP_REJECTED 0x0002
#define L2CAP_UNKNOWN_OPTIONS 0x0003
#define L2CAP_PENDING 0x0004
#define L2CAP_FLOW_SPEC_REJECTED 0x0005

#define L2CAP_STATE_COLLISION 0xFF

typedef struct {
    // The requested mode of the link.
    // Possible values are,
    // 0x00(Default) - Basic Mode
    // 0x03 - Enhanced Retransmission mode
    // 0x04 - Streaming mode
    uint8_t mode;

    // Valid in Enhanced Retransmission mode, the value should be set to 0.
    uint8_t maxTransmit;

    // Valid in Enhanced Retransmission mode, the value should be set to 0.
    uint8_t txWindowSize;

    // Valid in Enhanced Retransmission mode, refer to the size of transmission window.
    // The value range is 1 to 63
    // If the value is set to 0, then l2cap will determine the real value.
    uint8_t rxWindowSize;

    // Valid in Enhanced Retransmission mode, the value should be set to 0.
    uint16_t retransmissionTimeout;

    // Valid in Enhanced Retransmission mode, the value should be set to 0.
    uint16_t monitorTimeout;

    // Valid in Enhanced Retransmission mode or Streaming mode, the value should be set to 0.
    uint16_t mps;
} L2capOptionRfc;

typedef struct {
    // The maximum SDU size the sender of this option is capable of accepting for a channel.
    // The minimum value is 48.
    // The default value is 672.
    uint16_t mtu;

    // The Flush Timeout the sender is going to use.
    // It should be set to default value if profile/protocol does not specially refer.
    // Possible values are,
    // 0x0001 - no retransmissions at the baseband level should be performed.
    // 0x0002 to 0xFFFE - Flush Timeout in milliseconds used by the baseband.
    // 0xFFFF(Default) - an infinite amount of retransmissions.
    uint16_t flushTimeout;

    // Retransmission and flow control option
    L2capOptionRfc rfc;

    // Valid in Enhanced Retransmission mode or Streaming mode, specify the type of Frame Check Sequence (FCS).
    // It should be set to default value if profile/protocol does not specially refer.
    // Possible values are,
    // 0x00 - No FCS
    // 0x01(Default) - 16-bit FCS
    uint8_t fcs;
} L2capConfigInfo;

typedef struct {
    BtAddr addr;
    uint16_t handle;
} L2capConnectionInfo;

typedef struct {
    // Connection Request packets received
    void (*recvConnectionReq)(uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, uint16_t lpsm, void *ctx);

    // Connection Response packet received
    void (*recvConnectionRsp)(
        uint16_t lcid, const L2capConnectionInfo *info, uint16_t result, uint16_t status, void *ctx);

    // Configuration Request packet received
    void (*recvConfigReq)(uint16_t lcid, uint8_t id, const L2capConfigInfo *cfg, void *ctx);

    // Configuration Response packet received
    void (*recvConfigRsp)(uint16_t lcid, const L2capConfigInfo *cfg, uint16_t result, void *ctx);

    // Disconnection Request packet received
    void (*recvDisconnectionReq)(uint16_t lcid, uint8_t id, void *ctx);

    // Disconnection Response packet received
    void (*recvDisconnectionRsp)(uint16_t lcid, void *ctx);

    // Disconnected abnormal, such as acl disconnected or link loss
    void (*disconnectAbnormal)(uint16_t lcid, uint8_t reason, void *ctx);

    // L2cap data packet received
    void (*recvData)(uint16_t lcid, Packet *pkt, void *ctx);

    // In Enhanced Retransmission mode, when RNR received or tx_window overflow, this callback is generated
    void (*remoteBusy)(uint16_t lcid, uint8_t isBusy, void *ctx);
} L2capService;

typedef struct {
    // Echo Request packet received
    void (*recvEchoReq)(uint16_t aclHandle, uint8_t id, const uint8_t *data, uint16_t dataLen, void *ctx);
    // Echo Response packet received
    void (*recvEchoRsp)(uint16_t aclHandle, const uint8_t *data, uint16_t dataLen, void *ctx);
} L2capEcho;

// -------------------- Below is for LE ------------------------------

#define L2CAP_LE_ROLE_MASTER 0x00
#define L2CAP_LE_ROLE_SLAVE 0x01

#define L2CAP_LE_ATT_CHANNEL 0x0004
#define L2CAP_LE_SMP_CHANNEL 0x0006

// L2CAP CONNECTION PARAMETER UPDATE RESPONSE result
#define L2CAP_LE_CONNECTION_PARAMETERS_ACCEPTED 0x0000
#define L2CAP_LE_CONNECTION_PARAMETERS_REJECTED 0x0001

// L2cap Credit Based Connection response result
#define L2CAP_LE_CONNECTION_SUCCESSFUL 0x0000
#define L2CAP_LE_PSM_NOT_SUPPORTED 0x0002
#define L2CAP_LE_NO_RESOURCES_AVAILABLE 0x0004
#define L2CAP_LE_INSUFFICIENT_AUTHENTICATION 0x0005
#define L2CAP_LE_INSUFFICIENT_AUTHORIZATION 0x0006
#define L2CAP_LE_INSUFFICIENT_ENCRYPTION_KEY_SIZE 0x0007
#define L2CAP_LE_INSUFFICIENT_ENCRYPTION 0x0008
#define L2CAP_LE_INVALID_SOURCE_CID 0x0009
#define L2CAP_LE_SOURCE_CID_ALREADY_ALLOCATED 0x000A
#define L2CAP_LE_UNACCEPTABLE_PARAMETERS 0x000B

typedef struct {
    uint16_t mtu;
    uint16_t mps;
    uint16_t credit;
} L2capLeConfigInfo;

typedef struct {
    // LE Credit Based Connection Request packet received
    // Refer to charter 4.22 of Core 5.0
    void (*recvLeCreditBasedConnectionReq)(
        uint16_t lcid, uint8_t id, const L2capConnectionInfo *info, const L2capLeConfigInfo *cfg, void *ctx);
    // LE Credit Based Connection Response packet received
    // Refer to charter 4.23 of Core 5.0
    void (*recvLeCreditBasedConnectionRsp)(
        uint16_t lcid, const L2capConnectionInfo *info, const L2capLeConfigInfo *cfg, uint16_t result, void *ctx);

    // Disconnection Request packet received
    // Refer to charter 4.6 of Core 5.0
    void (*recvLeDisconnectionReq)(uint16_t lcid, uint8_t id, void *ctx);
    // Disconnection Response packet received
    // Refer to charter 4.7 of Core 5.0
    void (*recvLeDisconnectionRsp)(uint16_t lcid, void *ctx);

    // Disconnected abnormal, such as le acl disconnected or link loss
    // Refer to HCI specification charter 7.7.5 of Core 5.0
    void (*leDisconnectAbnormal)(uint16_t lcid, uint8_t reason, void *ctx);

    // LE data packet received
    void (*recvLeData)(uint16_t lcid, Packet *pkt, void *ctx);

    void (*leRemoteBusy)(uint16_t lcid, uint8_t busy, void *ctx);
} L2capLeService;

typedef struct {
    uint16_t cid;

    // LE ACL connected
    void (*leConnected)(const BtAddr *addr, uint16_t aclHandle, uint8_t role, uint8_t status);

    // LE ACL disconnected
    void (*leDisconnected)(uint16_t aclHandle, uint8_t status, uint8_t reason);

    // LE Fix Channel data received
    void (*recvLeData)(uint16_t aclHandle, const Packet *pkt);
} L2capLeFixChannel;

typedef struct {
    uint16_t connIntervalMin;     // Range: 0x0006 to 0x0C80, Time = N * 1.25 ms, Time Range: 7.5 ms to 4 s.
    uint16_t connIntervalMax;     // Range: 0x0006 to 0x0C80, Time = N * 1.25 ms, Time Range: 7.5 ms to 4 s.
    uint16_t connLatency;         // the range 0 to ((supervisionTimeout / (connIntervalMax * 2)) - 1) less than 500.
    uint16_t supervisionTimeout;  // Range: 0x000A to 0x0C80, Time = N * 10 ms, Time Range: 100 ms to 32 s
} L2capLeConnectionParameter;

typedef struct {
    // Connection Parameter Update Request packet received
    void (*recvLeConnectionParameterUpdateReq)(
        uint16_t aclHandle, uint8_t id, const L2capLeConnectionParameter *param, void *ctx);

    // Connection Parameter Update Response packet received
    void (*recvLeConnectionParameterUpdateRsp)(uint16_t aclHandle, uint16_t result, void *ctx);
} L2capLeConnectionParameterUpdate;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // L2CAP_DEF_H