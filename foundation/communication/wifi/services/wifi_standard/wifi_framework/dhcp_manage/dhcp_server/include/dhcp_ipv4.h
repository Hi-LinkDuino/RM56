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

#ifndef OHOS_DHCP_IPV4_H
#define OHOS_DHCP_IPV4_H

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
    CLIENT_IDENTIFIER_OPTION = 61
};

#endif