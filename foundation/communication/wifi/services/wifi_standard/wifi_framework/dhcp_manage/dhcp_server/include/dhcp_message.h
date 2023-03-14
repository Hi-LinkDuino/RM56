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

#ifndef OHOS_DHCP_MESSAGE_H
#define OHOS_DHCP_MESSAGE_H

#include "dhcp_define.h"
#include "dhcp_option.h"
#include <stdint.h>

#define BOOTREQUEST 1
#define BOOTREPLY 2

#define DHCP_MSG_HEADER_SIZE 236

enum DhcpMessageType {
    DHCPDISCOVER = 1,
    DHCPOFFER = 2,
    DHCPREQUEST = 3,
    DHCPDECLINE = 4,
    DHCPACK = 5,
    DHCPNAK = 6,
    DHCPRELEASE = 7,
    DHCPINFORM = 8
};

enum DhcpReplyType {
    REPLY_NONE = 0,
    REPLY_OFFER,
    REPLY_ACK,
    REPLY_NAK,
};

typedef struct DhcpMessage DhcpMessage;
struct DhcpMessage {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;

    uint8_t chaddr[DHCP_HWADDR_LENGTH];

    uint8_t sname[DHCP_HOST_NAME_LENGTH];
    uint8_t file[DHCP_BOOT_FILE_LENGTH];
    uint8_t options[DHCP_OPTIONS_SIZE];
};
typedef struct DhcpMessage *PDhcpMessage;

typedef struct DhcpMsgInfo DhcpMsgInfo;
struct DhcpMsgInfo {
    DhcpMessage packet;
    int length;
    DhcpOptionList options;
};
typedef struct DhcpMsgInfo *PDhcpMsgInfo;

#endif
