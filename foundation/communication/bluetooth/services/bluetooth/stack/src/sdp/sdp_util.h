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

#ifndef SDP_UTIL_H
#define SDP_UTIL_H

#include "sdp.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "packet.h"

#ifndef NO_SANITIZE
#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define NO_SANITIZE(type) __attribute__((no_sanitize(type)))
#endif
#endif
#endif

#ifndef NO_SANITIZE
#define NO_SANITIZE(type)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// The PSM that SDP uses.
#define SDP_PSM 0x0001
/// Max reserved ServiceRecordHandle
#define SDP_MAX_RESERVED_RECORD_HANDLE 0xFFFF
/// The maximum number of attributes in each record.
#define SDP_MAX_ATTRIBUTE_COUNT 20
/// The maximum length, in bytes, of an attribute.
#define SDP_MAX_ATTRIBUTE_LEN 400
/// The maximum number of UUID in the sequence.
#define SDP_MAX_UUID_COUNT 12
/// The maximum number of record handles.
#define SDP_MAX_RECORD_HANDLE_COUNT 60
/// The size of a scratchpad buffer, in bytes, for storing the response to an attribute request.
#define SDP_MAX_LIST_BYTE_COUNT 4096
/// The maximum number of parameters in an SDP protocol element.
#define SDP_MAX_PROTOCOL_PARAMS 4
/// Other attribute count
#define SDP_ATTRIBUTE_COUNT 8
/// Other sequence attribute count
#define SDP_SEQUENCE_ATTRIBUTE_COUNT 5
/// The number of SdpService arrays
#define SDP_SERVICE_ARRAY_NUMBER 60
/// The mtu size for the L2CAP configuration.
#define SDP_MTU_SIZE 672
/// The maximum attribute byte count
#define SDP_MAX_ATTRIBUTE_BYTE_COUNT 651
/// Header length for request and response
#define SDP_PDU_HEADER_LENGTH 5
/// Header and tail length
#define SDP_PACKET_HEADER_AND_TAIL_LENGTH 21
/// ContinuationState
#define SDP_MAX_CONTINUATION_LEN 16
/// Wait time for connect
#define SDP_CONNECT_WAIT_TIME 4000
/// Timeout (ms)
#define SDP_CONNECT_TIMEOUT 40000

#define SDP_STATE_IDLE 0
#define SDP_STATE_CONN_SETUP 1
#define SDP_STATE_CFG_REQ_SETUP 2
#define SDP_STATE_CFG_RSP_SETUP 3
#define SDP_STATE_CONNECTED 4
#define SDP_STATE_DISCONNECT 5
#define SDP_STATE_TIMEOUT 6

#define SDP_PACKET_WAIT 0
#define SDP_PACKET_SEND 1
#define SDP_PACKET_SEND_FRAGMENT 2

/// SDP response error code
#define SDP_INVALID_VERSION 0x0001       /// Invalid/unsupported SDP version
#define SDP_INVALID_SERV_REC_HDL 0x0002  /// Invalid Service Record Handle
#define SDP_INVALID_REQ_SYNTAX 0x0003    /// Invalid request syntax
#define SDP_INVALID_PDU_SIZE 0x0004      /// Invalid PDU Size
#define SDP_INVALID_CONT_STATE 0x0005    /// Invalid Continuation State
#define SDP_NO_RESOURCES 0x0006          /// Insufficient Resources to satisfy Request

#define SDP_PROTOCOL_DESCRIPTOR_LIST_MAX 4
#define SDP_PROTOCOL_DESCRIPTOR_MAX 4
#define SDP_LANGUAGE_ATTRIBUTE_MAX 5
#define SDP_LANGUAGE_ATTRIBUTE_LENGTH 9

#define SDP_BLUETOOTH_ADDRESS_LENGTH 6
#define SDP_SERVICE_RECORD_HANDLE_BYTE 4
#define SDP_DESCRIPTOR_SIZE_BIT 3
#define SDP_SERVICE_RECORD_OTHER 7

#define SDP_PACKET_TAIL_ONE_BYTE 1
#define SDP_PACKET_TAIL_TWO_BYTE 2
#define SDP_PACKET_TAIL_THREE_BYTE 3

#define SDP_UINT8_LENGTH 1
#define SDP_UINT16_LENGTH 2
#define SDP_UINT32_LENGTH 4
#define SDP_UINT64_LENGTH 8
#define SDP_UINT128_LENGTH 16

#define SDP_INT8_LENGTH 1
#define SDP_INT16_LENGTH 2
#define SDP_INT32_LENGTH 4

#define SDP_UUID16_LENGTH 2
#define SDP_UUID32_LENGTH 4
#define SDP_UUID128_LENGTH 16

typedef enum {
    DE_TYPE_NIL = 0,
    DE_TYPE_UINT,
    DE_TYPE_INT,
    DE_TYPE_UUID,
    DE_TYPE_STRING,
    DE_TYPE_BOOL,
    DE_TYPE_DES,
    DE_TYPE_DEA,
    DE_TYPE_URL
} SdpDescriptorType;

typedef enum {
    DE_SIZE_8 = 0,
    DE_SIZE_16,
    DE_SIZE_32,
    DE_SIZE_64,
    DE_SIZE_128,
    DE_SIZE_VAR_8,
    DE_SIZE_VAR_16,
    DE_SIZE_VAR_32
} SdpDescriptorSize;

typedef enum {
    SDP_INVALID = 0x00,
    SDP_ERROR_RESPONSE = 0x01,
    SDP_SERVICE_SEARCH_REQUEST,
    SDP_SERVICE_SEARCH_RESPONSE,
    SDP_SERVICE_ATTRIBUTE_REQUEST,
    SDP_SERVICE_ATTRIBUTE_RESPONSE,
    SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST,
    SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE
} SdpPduId;

typedef union {
    void (*ServiceSearchCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context);
    void (*ServiceAttributeCb)(const BtAddr *addr, const SdpService *service, void *context);
    void (*ServiceSearchAttributeCb)(
        const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context);
} SdpServiceCallback;

typedef struct {
    BtAddr addr;
    SdpPduId pduId;
    uint16_t transactionId;
    bool resentFlag;
    uint8_t packetState;
    Packet *packet;
    Packet *assemblePacket;
    SdpServiceCallback callback;
    void *context;
} SdpClientRequest;

void SdpSetEnableState();
void SdpSetDisableState();
bool SdpGetEnableState();
void SdpReverseForBigEndian(const uint8_t *src, uint8_t *dst, int length);
uint16_t SdpAddAttributeForUuid(uint8_t *buffer, uint16_t offset, const BtUuid *uuid);
uint16_t SdpGetLengthFromType(const uint8_t *buffer, uint8_t type, uint32_t *length);
uint16_t SdpGetUuid(uint8_t *buffer, BtUuid *uuid);

#if defined __cplusplus
}
#endif

#endif  // SDP_UTIL_H
