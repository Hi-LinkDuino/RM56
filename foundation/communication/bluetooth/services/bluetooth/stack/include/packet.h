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
 * @brief Bluetooth Basic tool library, This file is part of BTStack.
 *        Data Struct packet declarations.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef PACKET_H
#define PACKET_H

#include "buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Packet Packet;
typedef uint16_t (*CalCrc16)(uint8_t data, uint16_t preCrc);

/**
 * @brief Create new specified size(headsize, tailsize, payloadsize) packet.
 *        Packet is Buffer type linked list.
 *
 * @param headSize Packet headSize, could be 0 or positive integer.
 * @param tailSize Packet tailSize, could be 0 or positive integer.
 * @param payloadSize Packet payloadSize, could be 0 or positive integer.
 * @return Packet pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Packet *PacketMalloc(uint16_t headSize, uint16_t tailSize, uint32_t payloadSize);

/**
 * @brief Copy malloc new packet by Existing packet. Do not copy data.
 *
 * @param pkt Existing packet pointer.
 * @return Packet pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Packet *PacketRefMalloc(const Packet *pkt);

/**
 * @brief Inherit malloc new packet by Existing packet.
 *        headSize and tailSize are added.
 *        Used in data downstream.
 *
 * @param pkt Packet pointer.
 * @param headSize Packet headSize, could be 0 or positive integer.
 * @param tailSize Packet tailSize, could be 0 or positive integer.
 * @return Packet pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Packet *PacketInheritMalloc(const Packet *pkt, uint16_t headSize, uint16_t tailSize);

/**
 * @brief Release Packet.
 *
 * @param pkt Packet pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void PacketFree(Packet *pkt);

/**
 * @brief Get Packet head buffer.
 *
 * @param pkt Packet pointer.
 * @return Packet head buffer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *PacketHead(const Packet *pkt);

/**
 * @brief Get Packet head buffer.
 *
 * @param pkt Packet pointer.
 * @return Packet head buffer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *PacketTail(const Packet *pkt);

/**
 * @brief Get Packet payload buffer.
 *        Payload might be discontinuous, connect payload and return a continuous buffer.
 *
 * @param pkt Packet pointer.
 * @return Buffer pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API Buffer *PacketContinuousPayload(Packet *pkt);

/**
 * @brief Get Packet payload size.
 *
 * @param pkt Packet pointer.
 * @return Packet payload size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t PacketPayloadSize(const Packet *pkt);

/**
 * @brief Read data from Packet payload.
 *
 * @param pkt Packet pointer.
 * @param dst Data destination.
 * @param offset Offset is relative to Packet's payload.
 * @param size Read size.
 * @return Success read size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t PacketPayloadRead(const Packet *pkt, uint8_t *dst, uint32_t offset, uint32_t size);

/**
 * @brief Write data to Packet payload.
 *
 * @param pkt Packet pointer.
 * @param src Data source.
 * @param offset Offset is relative to Packet's payload.
 * @param size Write size.
 * @return Success write size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t PacketPayloadWrite(const Packet *pkt, const uint8_t *src, uint32_t offset, uint32_t size);

/**
 * @brief Add one buffer block to Packet's payload last. (Before packet's tail)
 *
 * @param pkt Packet pointer.
 * @param buf Add buffer pointer.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void PacketPayloadAddLast(const Packet *pkt, const Buffer *buf);

/**
 * @brief Get Packet size.
 *
 * @param pkt Packet pointer.
 * @return Packet size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t PacketSize(const Packet *pkt);

/**
 * @brief Read data from whole Packet.
 *
 * @param pkt Packet pointer.
 * @param dst Data destination.
 * @param offset Offset is relative to whole Packet.
 * @param size Read size.
 * @return Success read size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t PacketRead(const Packet *pkt, uint8_t *dst, uint32_t offset, uint32_t size);

/**
 * @brief Extract Packet head from payload.
 *        Used in data upstream.
 *
 * @param pkt Packet pointer.
 * @param data Data destination.
 * @param size Extract size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void PacketExtractHead(Packet *pkt, uint8_t *data, uint32_t size);

/**
 * @brief Extract Packet tail from payload.
 *        Used in data upstream.
 *
 * @param pkt Packet pointer.
 * @param data Data destination.
 * @param size Extract size.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void PacketExtractTail(const Packet *pkt, uint8_t *data, uint32_t size);

/**
 * @brief Fragment packet by fragLen.
 *        This operation will change the metadata in uplayer packet.
 *        If necessary, execute PacketRefMalloc before.
 *        Return uplayer packet remain length.
 *        Used in data downstream.
 *
 * @param uplayer Uplayer packet(Packet be fragmented).
 * @param downlayer Downlayer packet(Fragmented results packet).
 * @param fragLen Fragment length.
 * @return Uplayer packet remain length.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint32_t PacketFragment(Packet *uplayer, const Packet *downlayer, uint32_t fragLen);

/**
 * @brief Reassemble data packet.
 *        Used in data upstream.
 *
 * @param uplayer Uplayer packet(Packet assemble destination).
 * @param downlayer Downlayer packet(Packet assemble source).
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API void PacketAssemble(const Packet *uplayer, const Packet *downlayer);

/**
 * @brief Packet calculate crc16.
 *
 * @param calCrc16 CalCrc16 algorithm.
 * @return Computing result.
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API uint16_t PacketCalCrc16(const Packet *pkt, CalCrc16 calCrc16);

/**
 * @brief Packet verification crc16.
 *
 * @param calCrc16 CalCrc16 algorithm.
 * @param crcVal Verification data.
 * @return 0:success -1:failed
 * @since 1.0
 * @version 1.0
 */
BTSTACK_API int32_t PacketVerCrc16(const Packet *pkt, CalCrc16 calCrc16, uint16_t crcVal);

#ifdef __cplusplus
}
#endif

#endif  // PACKET_H