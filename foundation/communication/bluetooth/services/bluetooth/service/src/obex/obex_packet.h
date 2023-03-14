/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OBEX_PACKET_H
#define OBEX_PACKET_H

#include <cstdint>
#include <memory>
#include "base_def.h"
#include "packet.h"

class ObexPacket {
public:
    explicit ObexPacket(Packet &obexPacket)
    {
        obexPacket_ = &obexPacket;
    }
    explicit ObexPacket(uint16_t size)
    {
        obexPacket_ = PacketMalloc(0, 0, size);
    }
    virtual ~ObexPacket()
    {
        if (obexPacket_ != nullptr) {
            PacketFree(obexPacket_);
        }
    }
    void Write(uint8_t *buf, uint16_t bufLen) const
    {
        PacketPayloadWrite(obexPacket_, buf, 0, bufLen);
    }

    Packet &GetPacket()
    {
        return *obexPacket_;
    }
    uint8_t *GetBuffer()
    {
        if (obexPacket_ == nullptr) {
            return nullptr;
        }
        Buffer *tmpBuffer = PacketContinuousPayload(obexPacket_);
        return (uint8_t *)BufferPtr(tmpBuffer);
    }
    size_t GetSize()
    {
        if (obexPacket_ == nullptr) {
            return 0;
        }
        return PacketSize(obexPacket_);
    }

private:
    Packet *obexPacket_ = nullptr;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexPacket);
};
#endif  // OBEX_PACKET_H
