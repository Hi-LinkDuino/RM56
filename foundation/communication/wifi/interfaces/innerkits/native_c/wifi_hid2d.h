/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_C_HID2D_H
#define OHOS_C_HID2D_H

#include "wifi_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAC_LEN
#define MAC_LEN 6
#endif

#define IPV4_ARRAY_LEN 4

#ifndef IF_NAME_LEN
#define IF_NAME_LEN 32
#endif

#define MAX_SSID_LEN 33 // max length: 32 + \0
#define MAX_KEY_LEN 65 // max length: 64 + \0

#define CFG_DATA_MAX_BYTES 255

typedef enum DhcpMode {
    CONNECT_GO_NODHCP = 0,
    CONNECT_AP_DHCP = 1,
    CONNECT_AP_NODHCP = 2
} DhcpMode;

typedef enum FreqType {
    FREQUENCY_DEFAULT = 0,
    FREQUENCY_160M = 1,
} FreqType;

typedef enum SelfCfgType {
    TYPE_OF_GET_SELF_CONFIG = 1,
    TYPE_OF_GET_SELF_CONFIG_WITH_PASSWORD = 2
} SelfCfgType;

typedef enum PeerCfgType {
    TYPE_OF_SET_PEER_CONFIG = 1,
    TYPE_OF_SET_PEER_STATE_CHANGE = 2
} PeerCfgType;

typedef enum PreferBandwidth {
    /** default */
    BW_DEFAULT,
    /** indicates the ultimate bandwidth, corresponding to 160 Mbit/s or 320 Mbit/s in the future. */
    BW_EXTRAM,
    /** high throughput. The default value is 80 Mbit/s. */
    BW_HIGH_PERF,
    /** low-latency service type, 40 Mbit/s/80 Mbit/s,
     * which needs to be determined based on the current channel status. */
    BW_LOW_LATENCY
} PreferBandwidth;

typedef enum RecommendStatus {
    RS_SUCCESS,
    RS_LOCAL_ADJUST,
    RS_REMOTE_ADJUST,
    RS_FAILURE
} RecommendStatus;

typedef struct Hid2dConnectConfig {
    /** Service set ID (SSID). */
    char ssid[MAX_SSID_LEN];
    /** Basic service set ID (BSSID). */
    unsigned char bssid[MAC_LEN];
    /** Key. */
    char preSharedKey[MAX_KEY_LEN];
    /** group frequency. */
    int frequency;
    /** connection mode. */
    DhcpMode dhcpMode;
} Hid2dConnectConfig;

/**
 * @Description Ip address info structure, the element format is a 4-bit int array.
 * example: 127.0.0.1 -> int[ 127, 0, 0, 1 ]
 */
typedef struct IpAddrInfo {
    unsigned int ip[IPV4_ARRAY_LEN];
    unsigned int gateway[IPV4_ARRAY_LEN];
    unsigned int netmask[IPV4_ARRAY_LEN];
} IpAddrInfo;

typedef struct RecommendChannelRequest {
    /** the interface name of the remote device */
    char remoteIfName[IF_NAME_LEN];
    /**  the mode of the interface on the remote device */
    int remoteIfMode;
    /** interface name of the local device */
    char localIfName[IF_NAME_LEN];
    /** the mode of the interface on the local device */
    int localIfMode;
    /** preferred frequency band */
    int prefBand;
    /** preferred bandwidth type (enumerated) */
    PreferBandwidth prefBandwidth;
} RecommendChannelRequest;

typedef struct RecommendChannelResponse {
    /** 0: success; 1: local adjustment; 2: remote adjustment; –1: failure */
    RecommendStatus status;
    /* 1 fails. 0-N corresponds to the input array subscript (that is, the interface to be connected) */
    int index;
     /* The primary 20 MHz frequency of the channel */
    int centerFreq;
    /**
     * Do not used if the access point bandwidth is 20 MHz
     * If the AP use 40, 80 or 160 MHz, this is the center frequency, if the AP use 80 + 80 MHz,
     * this is the center frequency of the first segment
     */
    int centerFreq1;
    /**
     * Only used if the AP bandwidth is 80 + 80 MHz
     * if the AP use 80 + 80 MHz, this is the center frequency of the second segment
     */
    int centerFreq2;
    /* band width */
    int bandwidth;
} RecommendChannelResponse;

/**
 * @Description Request an IP address to the Gc from the IP address pool, used on the GO side.
 *
 * @param gcMac - gc mac address
 * @param ipAddr - Indicates the applied IP address, which is a 4-bit int array.
 *    example: 127.0.0.1 -> ipAddr[ 127, 0, 0, 1 ]
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dRequestGcIp(const unsigned char gcMac[MAC_LEN], unsigned int ipAddr[IPV4_ARRAY_LEN]);

/**
 * @Description Increase(+1) shared link reference counting
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dSharedlinkIncrease();

/**
 * @Description Decrease(-1) shared link reference counting
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dSharedlinkDecrease();

/**
 * @Description Create hid2d group, used on the GO side.
 *
 * @param frequency - frequency
 * @param type - frequency type
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dCreateGroup(const int frequency, FreqType type);

/**
 * @Description The GC side actively disconnects from the GO, used on the GC side.
 *
 * @param gcIfName - network interface name
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dRemoveGcGroup(const char gcIfName[IF_NAME_LEN]);

/**
 * @Description Connect to a specified group using hid2d, used on the GC side.
 *
 * @param config - connection parameters
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dConnect(const Hid2dConnectConfig *config);

/**
 * @Description Configuring IP addresses for P2P network interfaces, used on the GC side.
 *
 * @param ifName - network interface name
 * @param IpInfo - IP infos
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dConfigIPAddr(const char ifName[IF_NAME_LEN], const IpAddrInfo *ipInfo);

/**
 * @Description Clear IP address when the P2P connection is disconnected, used on the GC side.
 *
 * @param ifName - network interface name
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dReleaseIPAddr(const char ifName[IF_NAME_LEN]);

/**
 * @Description Obtain the recommended channel and bandwidth for link setup
 *
 * @param request - request data
 * @param response - response result
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dGetRecommendChannel(const RecommendChannelRequest *request, RecommendChannelResponse *response);

/**
 * @Description Get 5G channel list
 *
 * @param chanList - An array of pre-allocated memory for storing channel list results,
 * Use the '0' to indicates the end of valid data in the "chanList" array.
 *
 * @param len - the length of the pre-alloc "chanList"
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dGetChannelListFor5G(int *chanList, int len);

/**
 * @Description get the self wifi configuration information
 *
 * @param cfgType - configuration type
 * @param cfgData - the queried data of wifi configuration
 * @param getDatValidLen - the valid data length in the array `cfgData`
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType, char cfgData[CFG_DATA_MAX_BYTES], int *getDatValidLen);

/**
 * @Description set the peer wifi configuration information
 *
 * @param cfgType - configuration type
 * @param cfgData - the wifi configuration data to be set
 * @param setDataValidLen - the valid data length in the array `cfgData`
 * @return WifiErrorCode - operate result
 */
WifiErrorCode Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType, char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen);

/**
 * @Description Querying the support capability of wide bandwidth
 *
 * @return int - 0: not supported, 1: supported
 */
int Hid2dIsWideBandwidthSupported();
#ifdef __cplusplus
}
#endif

#endif
