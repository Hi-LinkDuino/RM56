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
 * @brief Defines map client service request types object.
 *
 */

/**
 * @file map_mce_types.h
 *
 * @brief map client service types header file .
 *
 */

#ifndef MAP_MCE_TYPES_H
#define MAP_MCE_TYPES_H

#include <cstdint>

namespace OHOS {
namespace bluetooth {
enum MceEnumType {
    MCE_MAX_OF_CONNECTED_DEVICES = 4,
    MCE_MAX_OF_CONNECTED_INSTANCES = 12,
    // max of message for get unread message
    MAP_MAX_LIST_COUNT_FOR_GET_UNREAD_MESSAGE = 100,
    MCE_INSTANCE_CLIENT_OBEX_MTU = 1024,
    MAX_OF_MASCLIENT_OBEX_UUID_TBL = 16,
    MCE_8BIT = 8,
    MCE_16BIT = 16,
    MCE_16BIT_MASK_LEFT_HIGH = 0xff00,
    MCE_16BIT_MASK_LEFT_LOW = 0x00ff,
    MCE_MAX_LENGTH_OF_RESPONSE_BUFFER = 2001
};
enum MceRequestType {
    MCE_REQUEST_TYPE_IDLE = 0,
    MCE_REQUEST_TYPE_GET_FOLDERLISTING,
    MCE_REQUEST_TYPE_SET_NOTIFICATION_FILTER,
    MCE_REQUEST_TYPE_SET_NOTIFICATION_REGIST,
    MCE_REQUEST_TYPE_SET_PATH,
    MCE_REQUEST_TYPE_GET_MESSAGE,
    MCE_REQUEST_TYPE_GET_MESSAGELISTING,
    MCE_REQUEST_TYPE_SEND_MESSAGE,
    MCE_REQUEST_TYPE_SET_MESSAGE_STATUS,
    MCE_REQUEST_TYPE_GET_UNREAD_MESSAGES,
    // MCE_REQUEST_TYPE_GET_SURPORTFEATURES,
    MCE_REQUEST_TYPE_GET_MASINSTANCE,
    MCE_REQUEST_TYPE_UPDATE_INBOX,
    MCE_REQUEST_TYPE_SET_OWNER_STATUS,
    MCE_REQUEST_TYPE_GET_OWNER_STATUS,
    MCE_REQUEST_TYPE_GET_CONVERSATION_LISTING,
    MCE_REQUEST_TYPE_ALL,
};
/**
 * @brief config data
 */
struct MasInstanceConfig {
    // max of connected mce device list
    uint32_t maxOfDevice = MCE_MAX_OF_CONNECTED_DEVICES;
    // max of unread message list number
    uint32_t maxOfGetUnread = MAP_MAX_LIST_COUNT_FOR_GET_UNREAD_MESSAGE;
    // max of mtu
    uint32_t l2capMtu = MCE_INSTANCE_CLIENT_OBEX_MTU;
    uint32_t rfcommMtu = MCE_INSTANCE_CLIENT_OBEX_MTU;
    // mns connected id
    uint32_t deviceId = 0;
    // if support srm
    bool isSupportSrm = true;
    // instance mode
    bool singleInstMode = false;
    // instance save
    int singleInstanceId = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // MAP_MCE_TYPES_H
