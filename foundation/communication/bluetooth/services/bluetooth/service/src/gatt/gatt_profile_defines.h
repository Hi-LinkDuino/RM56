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

#ifndef GATT_PROFLIE_DEFINES_H
#define GATT_PROFLIE_DEFINES_H

#include "bt_uuid.h"
#include "gatt_defines.h"
#include "packet.h"

namespace OHOS {
namespace bluetooth {
enum ResponesType {
    NONE,
    DISCOVER_ALL_PRIMARY_SERVICE,
    DISCOVER_SERVICE_BY_UUID,
    FIND_INCLUDE_SERVICE,
    DISCOVER_ALL_CHARACTERISTIC,
    DISCOVER_CHARACTERISTIC_BY_UUID,
    DISCOVER_ALL_CHARACTERISTIC_DESCRIPTOR,
    READ_CHARACTERISTIC_VALUE,
    READ_LONG_CHARACTERISTIC_VALUE,
    READ_USING_CHARACTERISTIC_UUID,
    READ_MULTIPLE_CHARACTERISTIC,
    READ_CHARACTERISTIC_DESCRIPTOR,
    READ_LONG_CHARACTERISTIC_DESCRIPTOR,
    WRITE_WITHOUT_RESPONSE,
    SIGNED_WRITE_WITHOUT_RESPONSE,
    WRITE_CHARACTERISTIC_VALUE,
    WRITE_CHARACTERISTIC_DESCRIPTOR,
    WRITE_LONG_CHARACTERISTIC_VALUE,
    RELIABLE_WRITE_VALUE,
    EXECUTE_WRITE_VALUE,
    EXCHANGE_MTU,
    SEND_INDICATION
};

enum ReadByTypeResponseLen {
    READ_USING_CHARACTERISTIC_BY_UUID_LENGTH = 0x04,
    DISCOVER_CHARACTERISTIC_LENGTH_16BIT = 0x07,
    DISCOVER_CHARACTERISTIC_LENGTH_128BIT = 0x15,
    FIND_INCLUDE_SERVICE_LENGTH_16BIT = 0x08,
    FIND_INCLUDE_SERVICE_LENGTH_128BIT = 0x06,
};

enum UuidLength {
    UUID_16BIT_LEN = 0x02,
    UUID_32BIT_LEN = 0x04,
    UUID_128BIT_LEN = 0x10,
};

enum UuidFormat {
    UUID_16BIT_FORMAT = 0x01,
    UUID_128BIT_FORMAT = 0x02,
};

enum RetVal {
    RET_NORMAL,
    RET_BREAK,
    RET_RETURN,
    RET_CONTINUE,
};

struct MtuInfo {
    bool isExchanged_ = false;
    uint16_t mtu_ = GATT_DEFAULT_MTU;

    MtuInfo(bool isExchanged, uint16_t mtu) : isExchanged_(isExchanged), mtu_(mtu)
    {}
};

struct CccdInfo {
    uint16_t valHandle_ = 0;
    uint16_t value_ = 0;
};

struct DeviceInfo {
    GattDevice device_;
    CccdInfo cccd_[GATT_CCCD_NUM_MAX] = {};

    explicit DeviceInfo(GattDevice dev) : device_(dev)
    {}
};

struct GattResponesInfor {
    ResponesType respType_ = NONE;
    uint16_t value_ = 0;
    GattValue data_ = nullptr;

    GattResponesInfor(ResponesType respType, uint16_t value) : respType_(respType), value_(value)
    {}
    GattResponesInfor(ResponesType respType, uint16_t value, GattValue data)
        : respType_(respType), value_(value), data_(data)
    {}
};
struct GattRequestInfo {
    uint16_t startHandle_ = 0;
    uint16_t endHandle_ = 0;
    uint16_t valHandle_ = 0;
    GattValue data_ = nullptr;
    ResponesType reqType_ = NONE;
    Uuid uuid_ = {};
    int reqId_ = 0;

    GattRequestInfo(ResponesType reqType, uint16_t starthandle, uint16_t endHandle, const Uuid &uuid, int reqId)
        : startHandle_(starthandle), endHandle_(endHandle), reqType_(reqType), uuid_(uuid), reqId_(reqId)
    {}
    GattRequestInfo(
        ResponesType reqType, uint16_t starthandle, uint16_t endHandle, uint16_t valHandle, const Uuid &uuid, int reqId)
        : startHandle_(starthandle),
          endHandle_(endHandle),
          valHandle_(valHandle),
          reqType_(reqType),
          uuid_(uuid),
          reqId_(reqId)
    {}
    GattRequestInfo(ResponesType reqType, uint16_t starthandle, uint16_t endHandle, int reqId)
        : startHandle_(starthandle), endHandle_(endHandle), reqType_(reqType), reqId_(reqId)
    {}
    GattRequestInfo(ResponesType reqType, uint16_t starthandle, uint16_t endHandle, uint16_t valHandle, int reqId)
        : startHandle_(starthandle), endHandle_(endHandle), valHandle_(valHandle), reqType_(reqType), reqId_(reqId)
    {}
    GattRequestInfo(ResponesType reqType, int reqId) : reqType_(reqType), reqId_(reqId)
    {}
    GattRequestInfo(const Uuid &uuid, ResponesType reqType, int reqId) : reqType_(reqType), uuid_(uuid), reqId_(reqId)
    {}
    GattRequestInfo(ResponesType reqType, uint16_t handle, int reqId)
        : startHandle_(handle), reqType_(reqType), reqId_(reqId)
    {}
    GattRequestInfo(ResponesType reqType, uint16_t handle, uint16_t offset, uint16_t len, GattValue data, int reqId)
        : startHandle_(handle), endHandle_(len), valHandle_(offset), data_(data), reqType_(reqType), reqId_(reqId)
    {}
};
struct ReadValCache {
    uint16_t handle_ = 0;
    uint16_t offset_ = 0;
    Packet *data_ = nullptr;

    ReadValCache(uint16_t handle, uint16_t offset, Packet *data) : handle_(handle), offset_(offset), data_(data)
    {
        data_ = PacketMalloc(0, 0, 0);
        PacketPayloadAddLast(data_, PacketContinuousPayload(data));
    }
};
struct PrepareWriteParam {
    uint16_t connectHandle_ = 0;
    uint16_t handle_ = 0;
    uint16_t offset_ = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // GATT_PROFLIE_DEFINES_H