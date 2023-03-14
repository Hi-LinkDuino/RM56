/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef WIFI_DRIVER_CLIENT_H
#define WIFI_DRIVER_CLIENT_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002500

#define IFNAMSIZ 16
#define ETH_ADDR_LEN 6
#define WIFI_24G_CHANNEL_NUM 14
#define WIFI_MAX_CHANNEL_NUM 24
#define IEEE80211G_RATES_NUM 12
#define IEEE80211B_RATES_NUM 4
#define IEEE80211A_RATES_NUM 8

/* common related interface */
enum WifiDriverClientResultCode {
    RET_CODE_SUCCESS = 0,
    RET_CODE_FAILURE = -1,
    RET_CODE_NOT_SUPPORT = -2,
    RET_CODE_INVALID_PARAM = -3,
    RET_CODE_MISUSE = -4, /* uncorrectly API used */
    RET_CODE_NOT_AVAILABLE = -5,
    RET_CODE_NOMEM = -6,
};

enum WifiIfType {
    WIFI_IFTYPE_UNSPECIFIED,
    WIFI_IFTYPE_ADHOC,
    WIFI_IFTYPE_STATION,
    WIFI_IFTYPE_AP,
    WIFI_IFTYPE_AP_VLAN,
    WIFI_IFTYPE_WDS,
    WIFI_IFTYPE_MONITOR,
    WIFI_IFTYPE_MESH_POINT,
    WIFI_IFTYPE_P2P_CLIENT,
    WIFI_IFTYPE_P2P_GO,
    WIFI_IFTYPE_P2P_DEVICE,
    WIFI_IFTYPE_MAX,
};

typedef enum {
    WIFI_EVENT_NEW_STA = 0,
    WIFI_EVENT_DEL_STA,
    WIFI_EVENT_RX_MGMT,
    WIFI_EVENT_TX_STATUS,
    WIFI_EVENT_SCAN_DONE,
    WIFI_EVENT_SCAN_RESULT = 5,
    WIFI_EVENT_CONNECT_RESULT,
    WIFI_EVENT_DISCONNECT,
    WIFI_EVENT_MESH_CLOSE,
    WIFI_EVENT_NEW_PEER_CANDIDATE,
    WIFI_EVENT_REMAIN_ON_CHANNEL = 10,
    WIFI_EVENT_CANCEL_REMAIN_ON_CHANNEL,
    WIFI_EVENT_CHANNEL_SWITCH,
    WIFI_EVENT_EAPOL_RECV,
    WIFI_EVENT_TIMEOUT_DISCONN,
    WIFI_EVENT_RESET_DRIVER = 15,
    WIFI_EVENT_BUTT
} WifiEventType;

typedef struct {
    int32_t reassoc;
    uint32_t ieLen;
    uint8_t *ie;
    uint8_t *macAddr;
    uint8_t resv[2];
} WifiNewStaInfo;

typedef struct {
    uint8_t *buf;
    uint32_t len;
    int32_t sigMbm;
    int32_t freq;
} WifiRxMgmt;

typedef struct {
    uint8_t *buf;
    uint32_t len;
    uint8_t ack;
    uint8_t resv[3];
} WifiTxStatus;

typedef struct {
    int32_t flags;
    uint8_t *bssid;
    int16_t caps;
    int32_t freq;
    int16_t beaconInt;
    int32_t qual;
    uint32_t beaconIeLen;
    int32_t level;
    uint32_t age;
    uint32_t ieLen;
    uint8_t *variable;
    uint8_t *ie;
    uint8_t *beaconIe;
} WifiScanResult;

typedef struct {
    uint8_t *reqIe;
    uint32_t reqIeLen;
    uint8_t *respIe;
    uint32_t respIeLen;
    uint8_t *bssid;
    uint8_t rsv[2];
    uint16_t status;
    uint16_t freq;
} WifiConnectResult;

typedef struct {
    uint8_t *ie;
    uint16_t reason;
    uint8_t rsv[2];
    uint32_t ieLen;
} WifiDisconnect;

enum WifiClientType {
    /* 1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<10 | 1<<11 | 1<<13 */
    WIFI_KERNEL_TO_WPA_CLIENT = 11519,
    /* 1<<15 | 1<<5 | 1<<4 */
    WIFI_KERNEL_TO_HAL_CLIENT = 32816,
    WIFI_CLIENT_BUTT
};

typedef int32_t (*OnReceiveFunc)(uint32_t event, void *data, const char *ifName);

int32_t WifiRegisterEventCallback(OnReceiveFunc onRecFunc, uint32_t eventType, const char *ifName);
void WifiUnregisterEventCallback(OnReceiveFunc onRecFunc, uint32_t eventType, const char *ifName);

/* hal related interface */

enum Ieee80211Band {
    IEEE80211_BAND_2GHZ,  /**< 2.4 GHz */
    IEEE80211_BAND_5GHZ,  /**< 5 GHz */
    IEEE80211_NUM_BANDS   /**< Reserved */
};

struct NetworkInfo {
    char name[IFNAMSIZ];
    uint8_t supportMode[WIFI_IFTYPE_MAX];
};

#define MAX_IFACE_NUM 20

struct NetworkInfoResult {
    uint32_t nums;
    struct NetworkInfo infos[MAX_IFACE_NUM];
};

struct FreqInfoResult {
    int32_t band;
    uint32_t nums;
    int32_t *freqs;
    int32_t *txPower;
};

#define MAX_ASSOC_STA_NUM 8

struct AssocStaInfo {
    uint8_t mac[ETH_ADDR_LEN];  /**< MAC address of the STA */
};

struct AssocStaInfoResult {
    uint32_t num;
    struct AssocStaInfo infos[MAX_ASSOC_STA_NUM];
};

struct NetDeviceInfo {
    uint32_t index;
    char ifName[IFNAMSIZ];
    uint8_t iftype;
    uint8_t mac[ETH_ADDR_LEN];
};

#define MAX_NETDEVICE_COUNT 20

struct NetDeviceInfoResult {
    struct NetDeviceInfo deviceInfos[MAX_NETDEVICE_COUNT];
};

int32_t WifiDriverClientInit(void);
void WifiDriverClientDeinit(void);

int32_t GetUsableNetworkInfo(struct NetworkInfoResult *result);
int32_t IsSupportCombo(uint8_t *isSupportCombo);
int32_t GetComboInfo(uint64_t *comboInfo, uint32_t size);
int32_t SetMacAddr(const char *ifName, unsigned char *mac, uint8_t len);
int32_t GetDevMacAddr(const char *ifName,
    int32_t type, uint8_t *mac, uint8_t len);
int32_t GetValidFreqByBand(const char *ifName, int32_t band,
    struct FreqInfoResult *result, uint32_t size);
int32_t SetTxPower(const char *ifName, int32_t power);
int32_t GetAssociatedStas(const char *ifName, struct AssocStaInfoResult *result);
int32_t WifiSetCountryCode(const char *ifName, const char *code, uint32_t len);
int32_t SetScanMacAddr(const char *ifName, uint8_t *scanMac, uint8_t len);

int32_t AcquireChipId(const char *ifName, uint8_t *chipId);
int32_t GetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num);
int32_t SetResetDriver(const uint8_t chipId, const char *ifName);
int32_t GetNetDeviceInfo(struct NetDeviceInfoResult *netDeviceInfoResult);

/* wpa related interface */
#define MAX_SSID_LEN 32
#define MAX_NR_CIPHER_SUITES 5
#define MAX_NR_AKM_SUITES 2

typedef struct {
    int32_t mode;
    int32_t freq;
    int32_t channel;

    /* for HT */
    int32_t htEnabled;

    /*
     * 0 = HT40 disabled, -1 = HT40 enabled,
     * secondary channel below primary, 1 = HT40
     * enabled, secondary channel above primary
     */
    int32_t secChannelOffset;

    /* for VHT */
    int32_t vhtEnabled;

    /*
     * valid for both HT and VHT, center_freq2 is non-zero
     * only for bandwidth 80 and an 80+80 channel
     */
    int32_t centerFreq1;
    int32_t centerFreq2;
    int32_t bandwidth;
    uint8_t band;
} WifiFreqParams;

typedef struct {
    uint32_t headLen;
    uint32_t tailLen;
    uint8_t *head;
    uint8_t *tail;
} WifiBeaconData;

typedef struct {
    WifiFreqParams freqParams;
    WifiBeaconData beaconData;
    uint32_t ssidLen;
    int32_t beaconInterval;
    int32_t dtimPeriod;
    uint8_t *ssid;
    uint8_t hiddenSsid;
    uint8_t authType;
    uint32_t meshSsidLen;
    uint8_t *meshSsid;
} WifiApSetting;

typedef struct {
    uint8_t *buf;
    uint32_t len;
} WifiRxEapol;

typedef struct {
    uint32_t freq;
    uint32_t dataLen;
    uint8_t *data;
    uint64_t *cookie;
} WifiMlmeData;

typedef struct {
    int32_t type;
    uint32_t keyIdx;
    uint32_t keyLen;
    uint32_t seqLen;
    uint32_t cipher;
    uint8_t *addr;
    uint8_t *key;
    uint8_t *seq;
    uint8_t def;
    uint8_t defMgmt;
    uint8_t defaultTypes;
    uint8_t resv;
} WifiKeyExt;

typedef struct {
    uint8_t bssid[ETH_ADDR_LEN];
    uint8_t iftype;
    uint8_t resv;
} WifiSetMode;

typedef struct {
    uint16_t channel;
    uint32_t freq;
    uint32_t flags;
} WifiIeee80211Channel;

typedef struct {
    int32_t channelNum;          /**< Number of channels */
    WifiIeee80211Channel iee80211Channel[WIFI_MAX_CHANNEL_NUM]; /**< WLAN channel structures */
}WlanBands;

typedef struct {
    int32_t channelNum;
    uint16_t bitrate[12];
    uint16_t htCapab;
    uint8_t resv[2];
    WlanBands bands[IEEE80211_NUM_BANDS];
} WifiHwFeatureData;

typedef struct {
    uint8_t ssid[MAX_SSID_LEN];
    uint32_t ssidLen;
} WifiDriverScanSsid;

typedef struct {
    WifiDriverScanSsid *ssids;
    int32_t *freqs;
    uint8_t *extraIes;
    uint8_t *bssid;
    uint8_t numSsids;
    uint8_t numFreqs;
    uint8_t prefixSsidScanFlag;
    uint8_t fastConnectFlag;
    int32_t extraIesLen;
} WifiScan;

typedef struct {
    uint32_t wpaVersions;
    uint32_t cipherGroup;
    int32_t nCiphersPairwise;
    uint32_t ciphersPairwise[MAX_NR_CIPHER_SUITES];
    int32_t nAkmSuites;
    uint32_t akmSuites[MAX_NR_AKM_SUITES];
} WifiCryptoSetting;

typedef struct {
    uint8_t *bssid;
    uint8_t *ssid;
    uint8_t *ie;
    uint8_t *key;
    uint8_t authType;
    uint8_t privacy;
    uint8_t keyLen;
    uint8_t keyIdx;
    uint8_t mfp;
    uint8_t rsv[3];
    uint32_t freq;
    uint32_t ssidLen;
    uint32_t ieLen;
    WifiCryptoSetting *crypto;
} WifiAssociateParams;

typedef enum {
    WIFI_PHY_MODE_11N = 0,
    WIFI_PHY_MODE_11G = 1,
    WIFI_PHY_MODE_11B = 2,
    WIFI_PHY_MODE_BUTT
} WifiPhyMode;

typedef struct {
    uint8_t status;
    enum WifiIfType ifType;
    WifiPhyMode mode;
} WifiSetNewDev;

typedef struct {
    uint8_t dst[ETH_ADDR_LEN];
    uint8_t src[ETH_ADDR_LEN];
    uint8_t bssid[ETH_ADDR_LEN];
    uint8_t *data;
    uint32_t dataLen;
} WifiActionData;

typedef struct {
    uint32_t freq;
    uint32_t duration;
} WifiOnChannel;


typedef struct {
    uint8_t type;
} WifiIfAdd;

typedef struct {
    uint8_t ifname[IFNAMSIZ];
} WifiIfRemove;

typedef struct {
    uint32_t ieLen;
    uint8_t appIeType;
    uint8_t rsv[3];
    uint8_t *ie;
} WifiAppIe;

typedef struct {
    uint64_t drvFlags;
} WifiGetDrvFlags;

int32_t WifiEapolPacketSend(const char *ifName, const uint8_t *srcAddr, const uint8_t *dstAddr, uint8_t *buf,
    uint32_t length);
int32_t WifiEapolPacketReceive(const char *ifName, WifiRxEapol *rxEapol);
int32_t WifiEapolEnable(const char *ifName);
int32_t WifiEapolDisable(const char *ifName);
int32_t WifiCmdSetAp(const char *ifName, WifiApSetting *apsettings);
int32_t WifiCmdChangeBeacon(const char *ifName, WifiApSetting *apsettings);
int32_t WifiCmdSendMlme(const char *ifName, WifiMlmeData *mlme);
int32_t WifiCmdDelKey(const char *ifName, WifiKeyExt *keyExt);
int32_t WifiCmdNewKey(const char *ifName, WifiKeyExt *keyExt);
int32_t WifiCmdSetKey(const char *ifName, WifiKeyExt *keyExt);
int32_t WifiCmdSetMode(const char *ifName, WifiSetMode *setMode);
int32_t WifiCmdGetOwnMac(const char *ifName, void *buf, uint32_t len);
int32_t WifiCmdGetHwFeature(const char *ifName, WifiHwFeatureData *hwFeatureData);
int32_t WifiCmdScan(const char *ifName, WifiScan *scan);
int32_t WifiCmdDisconnet(const char *ifName, int32_t reasonCode);
int32_t WifiCmdAssoc(const char *ifName, WifiAssociateParams *assocParams);
int32_t WifiCmdSetNetdev(const char *ifName, WifiSetNewDev *info);
int32_t WifiCmdStaRemove(const char *ifName, const uint8_t *addr, uint32_t addrLen);
int32_t WifiCmdSendAction(const char *ifName, WifiActionData *actionData);
int32_t WifiCmdSetClient(uint32_t clientNum);
int32_t WifiCmdProbeReqReport(const char* ifName, const int32_t *report);
int32_t WifiCmdRemainOnChannel(const char* ifName, const WifiOnChannel *onChannel);
int32_t WifiCmdCancelRemainOnChannel(const char* ifName);
int32_t WifiCmdAddIf(const char *ifname, const WifiIfAdd *ifAdd);
int32_t WifiCmdRemoveIf(const char *ifname, const WifiIfRemove *ifRemove);
int32_t WifiCmdSetApWpsP2pIe(const char *ifname, const WifiAppIe *appIe);
int32_t WifiCmdGetDrvFlags(const char *ifname, WifiGetDrvFlags *params);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wifi_driver_client.h */
