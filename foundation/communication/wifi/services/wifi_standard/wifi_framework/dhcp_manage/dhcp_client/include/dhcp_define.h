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
#ifndef OHOS_DHCP_DEFINE_H
#define OHOS_DHCP_DEFINE_H


#include <stdint.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#include "wifi_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUMBER_ONE              1
#define NUMBER_TWO              2
#define NUMBER_THREE            3
#define NUMBER_FOUR             4
#define NUMBER_FIVE             5

#define RENEWAL_SEC_MULTIPLE    0.5
#define REBIND_SEC_MULTIPLE     0.875
#define TIME_INTERVAL_MAX       1
#define TIMEOUT_TIMES_MAX       6
#define TIMEOUT_WAIT_SEC        1
#define TIMEOUT_MORE_WAIT_SEC   60
#define ONE_HOURS_SEC           3600
#define LEASETIME_DEFAULT       1
#define SLEEP_TIME_200_MS       (200 * 1000)
#define SLEEP_TIME_500_MS       (500 * 1000)

#define MAC_ADDR_LEN            6
#define MAC_ADDR_CHAR_NUM       3
#define ETHERNET_TYPE           0x01
#define ETHERNET_LEN            6
#define VENDOR_MAX_LEN          64
#define MAGIC_COOKIE            0x63825363
#define BROADCAST_FLAG          0x8000
#define MAC_BCAST_ADDR          "\xff\xff\xff\xff\xff\xff"
#define SIGNED_INTEGER_MAX      0x7FFFFFFF
#define PID_MAX_LEN             16
#define DEFAULT_UMASK           027
#define DIR_MAX_LEN             256
#define INFNAME_SIZE            16    /* Length of interface name */
#define RWMODE                  0600
#define MAX_MSG_SIZE            1500
#define DHCP_FILE_MAX_BYTES     128

#define STRING_MAX_LEN          1024
#define EVENT_GET_IPV4          "usual.event.wifi.dhcp.GET_IPV4"

/* UDP port numbers for BOOTP */
#define BOOTP_SERVER            67
#define BOOTP_CLIENT            68

/* BOOTP message type */
#define BOOT_REQUEST            1
#define BOOT_REPLY              2

/* DHCP packet type */
#define DHCP_DISCOVER           1
#define DHCP_OFFER              2
#define DHCP_REQUEST            3
#define DHCP_DECLINE            4
#define DHCP_ACK                5
#define DHCP_NAK                6
#define DHCP_RELEASE            7
#define DHCP_INFORM             8
#define DHCP_FORCERENEW         9

/* dhcp state code */
enum EnumDhcpStateCode {
    DHCP_STATE_INIT = 0,
    DHCP_STATE_SELECTING,
    DHCP_STATE_REQUESTING,
    DHCP_STATE_BOUND,
    DHCP_STATE_RENEWING,
    DHCP_STATE_REBINDING,
    DHCP_STATE_INITREBOOT,
    DHCP_STATE_RELEASED,
    DHCP_STATE_RENEWED
};

/* dhcp return code */
enum EnumErrCode {
    /* success */
    DHCP_OPT_SUCCESS = 0,
    /* failed */
    DHCP_OPT_FAILED,
    /* null pointer */
    DHCP_OPT_NULL,
    /* timeout */
    DHCP_OPT_TIMEOUT,
    /* error */
    DHCP_OPT_ERROR,
    /* none */
    DHCP_OPT_NONE,
};

/* socket return code */
enum EnumSocketErrCode {
    /* success */
    SOCKET_OPT_SUCCESS = 0,
    /* failed */
    SOCKET_OPT_FAILED = -1,
    /* error */
    SOCKET_OPT_ERROR = -2
};

/* socket mode */
enum EnumSocketMode {
    SOCKET_MODE_INVALID        = 0,
    SOCKET_MODE_RAW            = 1,
    SOCKET_MODE_KERNEL         = 2
};

// See: RFC 2132        DHCP Options and BOOTP Vendor Extensions      March 1997
enum DhcpOptions {
    /*
     * 3. RFC 1497 Vendor Extensions
     * This section lists the vendor extensions as defined in RFC 1497.
     * They are defined here for completeness.
     */
    PAD_OPTION = 0,
    END_OPTION = 255,

    SUBNET_MASK_OPTION = 1,
    TIME_OFFSET_OPTION = 2,
    ROUTER_OPTION = 3,
    TIME_SERVER_OPTION = 4,
    NAME_SERVER_OPTION = 5,
    DOMAIN_NAME_SERVER_OPTION = 6,
    LOG_SERVER_OPTION = 7,
    COOKIE_SERVER_OPTION = 8,
    LPR_SERVER_OPTION = 9,
    IMPRESS_SERVER_OPTION = 10,
    RESOURCE_LOCATION_SERVER_OPTION = 11,
    HOST_NAME_OPTION = 12,
    BOOT_FILE_SIZE_OPTION = 13,
    MERIT_DUMP_FILE_OPTION = 14,
    DOMAIN_NAME_OPTION = 15,
    SWAP_SERVER_OPTION = 16,
    ROOT_PATH_OPTION = 17,
    EXTENSIONS_PATH_OPTION = 18,

    /*
     * IP Layer Parameters per Host
     * This section details the options that affect the operation of the IP layer on a per-host basis.
     */
    IP_FORWARDING_OPTION = 19,
    NON_LOCAL_SOURCE_ROUTING_OPTION = 20,
    POLICY_FILTER_OPTION = 21,
    MAXIMUM_DATAGRAM_REASSEMBLY_SIZE_OPTION = 22,
    DEFAULT_IP_TIME_TO_LIVE_OPTION = 23,
    PATH_MTU_AGING_TIMEOUT_OPTION = 24,
    PATH_MTU_PLATEAU_TABLE_OPTION = 25,

    /*
     * 5. IP Layer Parameters per Interface
     * This section details the options that affect the operation of the IP layer on a per-interface basis.  It is
     * expected that a client can issue multiple requests, one per interface, in order to configure interfaces with
     * their specific parameters.
     */
    INTERFACE_MTU_OPTION = 26,
    ALL_SUBNETS_ARE_LOCAL_OPTION = 27,
    BROADCAST_ADDRESS_OPTION = 28,
    PERFORM_MASK_DISCOVERY_OPTION = 29,
    MASK_SUPPLIER_OPTION = 30,
    PERFORM_ROUTER_DISCOVERY_OPTION = 31,
    ROUTER_SOLICITATION_ADDRESS_OPTION = 32,
    STATIC_ROUTE_OPTION = 33,

    /*
     * 6. Link Layer Parameters per Interface
     * This section lists the options that affect the operation of the data link layer on a per-interface basis.
     */
    TRAILER_ENCAPSULATION_OPTION = 34,   // 6.1. Trailer Encapsulation Option
    ARP_CACHE_TIMEOUT_OPTION = 35,       // 6.2. ARP Cache Timeout Option
    ETHERNET_ENCAPSULATION_OPTION = 36,  // 6.3. Ethernet Encapsulation Option

    /*
     * 7. TCP Parameters
     * This section lists the options that affect the operation of the TCP layer on a per-interface basis.
     */
    TCP_DEFAULT_TTL_OPTION = 37,         // 7.1. TCP Default TTL Option
    TCP_KEEPALIVE_INTERVAL_OPTION = 38,  // 7.2. TCP Keepalive Interval Option
    TCP_KEEPALIVE_GARBAGE_OPTION = 39,   // 7.3. TCP Keepalive Garbage Option

    /*
     * 8. Application and Service Parameters
     * This section details some miscellaneous options used to configure miscellaneous applications and services.
     */
    NETWORK_INFO_SERVICE_DOMAIN_OPTION = 40,           // 8.1. Network Information Service Domain Option
    NETWORK_INFO_SERVERS_OPTION = 41,                  // 8.2. Network Information Servers Option
    NETWORK_TIME_PROTOCOL_SERVERS_OPTION = 42,         // 8.3. Network Time Protocol Servers Option
    VENDOR_SPECIFIC_INFO_OPTION = 43,                  // 8.4. Vendor Specific Information
    NETBIOS_OVER_IP_NAME_SERVER_OPTION = 44,           // 8.5. NetBIOS over TCP/IP Name Server Option
    NETBIOS_OVER_IP_DATAGRAM_DIST_SERVER_OPTION = 45,  // 8.6. NetBIOS over TCP/IP Datagram Distribution Server Option
    NETBIOS_OVER_IP_NODE_TYPE_OPTION = 46,             // 8.7. NetBIOS over TCP/IP Node Type Option
    NETBIOS_OVER_IP_SCOPE_OPTION = 47,                 // 8.8. NetBIOS over TCP/IP Scope Option
    X_WINDOW_SYSTEM_FONT_SERVER_OPTION = 48,           // 8.9. X Window System Font Server Option
    X_WINDOW_SYSTEM_DISPLAY_MANAGER_OPTION = 49,       // 8.10. X Window System Display Manager Option
    NETWORK_INFO_SERVICE_PLUS_DOMAIN_OPTION = 64,      // 8.11. Network Information Service+ Domain Option
    NETWORK_INFO_SERVICE_PLUS_SERVERS_OPTION = 65,     // 8.12. Network Information Service+ Servers Option
    MOBILE_IP_HOME_AGENT_OPTION = 68,                  // 8.13. Mobile IP Home Agent option
    SMTP_SERVER_OPTION = 69,                           // 8.14. Simple Mail Transport Protocol (SMTP) Server Option
    POP3_SERVER_OPTION = 70,                           // 8.15. Post Office Protocol (POP3) Server Option
    NNTP_SERVER_OPTION = 71,                           // 8.16. Network News Transport Protocol (NNTP) Server Option
    DEFAULT_WEB_SERVER_OPTION = 72,                    // 8.17. Default World Wide Web (WWW) Server Option
    DEFAULT_FINGER_SERVER_OPTION = 73,                 // 8.18. Default Finger Server Option
    DEFAULT_IRC_SERVER_OPTION = 74,                    // 8.19. Default Internet Relay Chat (IRC) Server Option
    STREETTALK_SERVER_OPTION = 75,                     // 8.20. StreetTalk Server Option
    STDA_SERVER_OPTION = 76,                           // 8.21. StreetTalk Directory Assistance (STDA) Server Option

    /*
     * 9. DHCP Extensions
     * This section details the options that are specific to DHCP.
     */
    REQUESTED_IP_ADDRESS_OPTION = 50,
    IP_ADDRESS_LEASE_TIME_OPTION = 51,
    OPTION_OVERLOAD_OPTION = 52,
    TFTP_SERVER_NAME_OPTION = 66,
    BOOTFILE_NAME_OPTION = 67,
    DHCP_MESSAGE_TYPE_OPTION = 53,
    SERVER_IDENTIFIER_OPTION = 54,
    PARAMETER_REQUEST_LIST_OPTION = 55,
    MESSAGE_OPTION = 56,
    MAXIMUM_DHCP_MESSAGE_SIZE_OPTION = 57,
    RENEWAL_TIME_VALUE_OPTION = 58,
    REBINDING_TIME_VALUE_OPTION = 59,
    VENDOR_CLASS_IDENTIFIER_OPTION = 60,
    CLIENT_IDENTIFIER_OPTION = 61,

    /* RFC 6704 */
    FORCERENEW_NONCE_OPTION = 145, /* Forcerenew Nonce Authentication */
};

enum DHCP_OPTION_DATA_TYPE {
    DHCP_OPTION_DATA_INVALID = 0,
    DHCP_OPTION_DATA_U8,
    DHCP_OPTION_DATA_U16,
    DHCP_OPTION_DATA_S16,
    DHCP_OPTION_DATA_U32,
    DHCP_OPTION_DATA_S32,
    DHCP_OPTION_DATA_IP,
    DHCP_OPTION_DATA_IP_PAIR,
    DHCP_OPTION_DATA_IP_LIST,
    DHCP_OPTION_DATA_IP_STRING
};

/* publish event code */
enum EnumPublishEventCode {
    /* success */
    PUBLISH_CODE_SUCCESS = 0,
    /* failed */
    PUBLISH_CODE_FAILED = -1
};

/* Sizes for DhcpPacket Fields */
#define DHCP_HWADDR_LENGTH          16
#define DHCP_HOST_NAME_LENGTH       64
#define DHCP_BOOT_FILE_LENGTH       128
#define DHCP_OPTIONS_SIZE           308
#define DHCP_OPT_CODE_INDEX         0
#define DHCP_OPT_LEN_INDEX          1
#define DHCP_OPT_DATA_INDEX         2
#define DHCP_OPT_CODE_BYTES         1
#define DHCP_OPT_LEN_BYTES          1
#define DHCP_UINT8_BYTES            1
#define DHCP_UINT16_BYTES           2
#define DHCP_UINT32_BYTES           4
#define DHCP_UINT32_DOUBLE_BYTES    8
#define DHCP_UINT16_BITS            16
#define DHCP_REQ_CODE_NUM           10

#define OPTION_FIELD                0
#define FILE_FIELD                  1
#define SNAME_FIELD                 2

/* DhcpPacket Fields */
struct DhcpPacket {
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
    int32_t cookie;
    uint8_t options[DHCP_OPTIONS_SIZE];
};

struct UdpDhcpPacket {
    struct iphdr ip;
    struct udphdr udp;
    struct DhcpPacket data;
};

struct DhcpResult {
    char strYiaddr[INET_ADDRSTRLEN];        /* your (client) IP */
    char strOptServerId[INET_ADDRSTRLEN];   /* dhcp option SERVER_IDENTIFIER_OPTION */
    char strOptSubnet[INET_ADDRSTRLEN];     /* dhcp option SUBNET_MASK_OPTION */
    char strOptDns1[INET_ADDRSTRLEN];       /* dhcp option DOMAIN_NAME_SERVER_OPTION */
    char strOptDns2[INET_ADDRSTRLEN];       /* dhcp option DOMAIN_NAME_SERVER_OPTION */
    char strOptRouter1[INET_ADDRSTRLEN];    /* dhcp option ROUTER_OPTION */
    char strOptRouter2[INET_ADDRSTRLEN];    /* dhcp option ROUTER_OPTION */
    char strOptVendor[DHCP_FILE_MAX_BYTES]; /* dhcp option VENDOR_SPECIFIC_INFO_OPTION */
    uint32_t uOptLeasetime;                 /* dhcp option IP_ADDRESS_LEASE_TIME_OPTION */
};

#ifdef __cplusplus
}
#endif
#endif
