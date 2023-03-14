/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup WLAN
 * @{
 *
 * @brief Provides cross-OS migration, component adaptation, and modular assembly and compilation.
 *
 * Based on the unified APIs provided by the WLAN module, developers of the Hardware Driver Interface
 * (HDI) are capable of creating, disabling, scanning for, and connecting to WLAN hotspots, managing WLAN chips,
 * network devices, and power, and applying for, releasing, and moving network data buffers.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file wifi_mac80211_ops.h
 *
 * @brief Declares Media Access Control (MAC) APIs and functions related to control flows.
 *
 * This file declares APIs for scanning, connection, disconnection, and mode conversion, as well as data
 * structures of the parameters of these APIs.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_MAC80211_OPS_H
#define WIFI_MAC80211_OPS_H

#include "net_device.h"
#include "hdf_wifi_cmd.h"

/**
 * @brief Indicates the length of the MAC address of the device.
 *
 * The value is <b>6</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define IEEE80211_MAC_ADDR_LEN 6
/**
 * @brief Indicates the maximum length of the SSID of the device.
 *
 * The value is <b>32</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define IEEE80211_MAX_SSID_LEN 32
/**
 * @brief Indicates the length of a reserved field for the WLAN connection parameters.
 *
 * The value is <b>3</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define WIFI_CONNECT_PARM_RESV_SIZE 3
/**
 * @brief Indicates that the frequency is not specified.
 *
 * The value is <b>0</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define WLAN_FREQ_NOT_SPECFIED 0
/**
 * @brief Indicates the maximum length of the hotspot SSID array.
 *
 * The value is <b>32</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define OAL_IEEE80211_MAX_SSID_LEN 32
/**
 * @brief Indicates the maximum number of channels to scan for.
 *
 * The value is <b>14</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define MAX_SCAN_CHANNELS 14
/**
 * @brief Indicates a reserved field for a scan request.
 *
 * The value is <b>2</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define SCAN_REQUEST_RESV_SIZE 2
/**
 * @brief Indicates the maximum number of frequency filters.
 *
 * The value is <b>200</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define MAX_FREQ_FTILTER_COUNT 200
/**
 * @brief Indicates the maximum number of cipher suites.
 *
 * The value is <b>5</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define NL80211_MAX_NR_CIPHER_SUITES 5
/**
 * @brief Indicates the maximum number of AKM suites.
 *
 * The value is <b>2</b>.
 *
 * @since 1.0
 * @version 1.0
 */
#define NL80211_MAX_NR_AKM_SUITES 2

/**
 * @brief Enumerates frequency bands.
 *
 * @since 1.0
 * @version 1.0
 */
enum Ieee80211Band {
    IEEE80211_BAND_2GHZ = 0, /**< 2.4 GHz */
    IEEE80211_BAND_5GHZ,     /**< 5 GHz */
    IEEE80211_NUM_BANDS      /**< Number of frequency bands */
};

/**
 * @brief Enumerates channel widths.
 *
 * @since 1.0
 * @version 1.0
 */
enum Ieee80211ChannelWidth {
    WLAN_CHANNEL_WIDTH_10   = BIT(0),   /**< 10 MHz */
    WLAN_CHANNEL_WIDTH_20   = BIT(1),   /**< 20 MHz */
    WLAN_CHANNEL_WIDTH_40P  = BIT(2),   /**< 40 MHz (two 20 MHz channels, with the primary channel
                                         * having a higher frequency
                                         */
    WLAN_CHANNEL_WIDTH_40M  = BIT(3),   /**< 40 MHz (two 20 MHz channels, with the primary channel
                                         * having a lower frequency
                                         */
    WLAN_CHANNEL_WIDTH_80   = BIT(4),   /**< 80 MHz */
    WLAN_CHANNEL_WIDTH_160  = BIT(5),   /**< 160 MHz */
};

/**
 * @brief Enumerates WLAN modes.
 *
 * @since 1.0
 * @version 1.0
 */
enum WlanWorkMode {
    WLAN_WORKMODE_STA = 2,  /**< STA mode */
    WLAN_WORKMODE_AP = 3,    /**< AP mode */
    WLAN_WORKMODE_AP_VLAN,
    WLAN_WORKMODE_WDS,
    WLAN_WORKMODE_MONITOR,
    WLAN_WORKMODE_MESH_POINT,
    WLAN_WORKMODE_P2P_CLIENT,
    WLAN_WORKMODE_P2P_GO,
    WLAN_WORKMODE_P2P_DEVICE,
};

/**
 * @brief Describes a key.
 *
 * @since 1.0
 * @version 1.0
 */
struct KeyParams {
    uint8_t *key;    /**< Key content */
    uint8_t *seq;    /**< Content of a Temporal Key Integrity Protocol (TKIP) or Counter Mode Cipher Block Chaining
                      * Message Authentication Code Protocol (CCMP) key
                      */
    int32_t keyLen;  /**< Key length */
    int32_t seqLen;  /**< Length of a TKIP or CCMP key */
    uint32_t cipher; /**< Cipher suite */
};

#define UINT32_BIT(BIT) (((uint32_t)1) << (BIT))

#define WLAN_CHANNEL_FLAG_DFS_UNKNOWN 0x00000000
#define WLAN_CHANNEL_FLAG_DFS_MASK 0x00000300

/**
 * @brief Enumerates WLAN channel capability flags.
 *
 * @since 1.0
 * @version 1.0
 */
enum WlanChannelCapabilityFlags {
    WLAN_CHANNEL_FLAG_DISABLED = UINT32_BIT(0), /**< Channel disabled */
    WLAN_CHANNEL_FLAG_NO_IR = UINT32_BIT(1),    /**< Infrared light (IR) not supported */
    WLAN_CHANNEL_FLAG_RADAR = UINT32_BIT(3),    /**< Radar supported */
    WLAN_CHANNEL_FLAG_HT40PLUS = UINT32_BIT(4), /**< A 40 MHz channel that consists of two 20 MHz channels,
                                                 * with the primary channel having a higher frequency
                                                 */
    WLAN_CHANNEL_FLAG_HT40MINUS = UINT32_BIT(5), /**< A 40 MHz channel that consists of two 20 MHz channels,
                                                  * with the primary channel having a lower frequency
                                                  */
    WLAN_CHANNEL_FLAG_HT40 = UINT32_BIT(6),      /**< 40 MHz channel */
    WLAN_CHANNEL_FLAG_SURVEY_LIST_INITIALIZED = UINT32_BIT(7), /**< Survey list initialized */
    WLAN_CHANNEL_FLAG_DFS_USABLE = UINT32_BIT(8),       /**< Dynamic frequency selection (DFS) supported */
    WLAN_CHANNEL_FLAG_DFS_UNAVAILABLE = UINT32_BIT(9),  /**< DFS disabled */
    WLAN_CHANNEL_FLAG_DFS_AVAILABLE = WLAN_CHANNEL_FLAG_DFS_USABLE | WLAN_CHANNEL_FLAG_DFS_UNAVAILABLE,
    WLAN_CHANNEL_FLAG_VHT_10_70 =
        UINT32_BIT(11), /**< A 80 MHz channel: Frequency
                         * band = Channel's center frequency - 10 MHz to the channel's center frequency + 70 MHz
                         */
    WLAN_CHANNEL_FLAG_VHT_30_50 =
        UINT32_BIT(12), /**< A 80 MHz channel: Frequency
                         * band = Channel's center frequency - 30 MHz to the channel's center frequency + 50 MHz
                         */
    WLAN_CHANNEL_FLAG_VHT_50_30 =
        UINT32_BIT(13), /**< A 80 MHz channel: Frequency
                         * band = Channel's center frequency - 50 MHz to the channel's center frequency + 30 MHz
                         */
    WLAN_CHANNEL_FLAG_VHT_70_10 =
        UINT32_BIT(14), /**< A 80 MHz channel: Frequency
                         * band = Channel's center frequency - 70 MHz to the channel's center frequency + 10 MHz
                         */
    WLAN_CHANNEL_FLAG_INDOOR_ONLY = UINT32_BIT(16),   /**< Used only indoors */
    WLAN_CHANNEL_FLAG_GO_CONCURRENT = UINT32_BIT(17), /**< Go Concurrent supported */
    WLAN_CHANNEL_FLAG_VHT_10_150 = UINT32_BIT(20),  /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 10 MHz to the channel's center frequency + 150 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_30_130 = UINT32_BIT(21),  /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 30 MHz to the channel's center frequency + 130 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_50_110 = UINT32_BIT(22),  /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 50 MHz to the channel's center frequency + 110 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_70_90 = UINT32_BIT(23),   /**< A 160 MHz channel: Frequency band = Channel's center
                                                     frequency - 70 MHz to the channel's center frequency + 90 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_90_70 = UINT32_BIT(24),   /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 90 MHz to the channel's center frequency + 70 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_110_50 = UINT32_BIT(25),  /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 110 MHz to the channel's center frequency + 50 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_130_30 = UINT32_BIT(26),  /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 130 MHz to the channel's center frequency + 30 MHz
                                                     */
    WLAN_CHANNEL_FLAG_VHT_150_10 = UINT32_BIT(27),  /**< A 160 MHz channel: Frequency band = Channel's center
                                                     * frequency - 150 MHz to the channel's center frequency + 10 MHz
                                                     */
};

/**
 * @brief Describes a communication channel.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanChannel {
    uint8_t channelId;   /**< Channel ID */
    uint32_t centerFreq; /**< Center frequency */
    uint32_t flags;      /**< Flags for the channel, as enumerated in <b>WlanChannelCapabilityFlags</b> */
};

/**
 * @brief Describes a band.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanBand {
    uint16_t channelCount;          /**< Number of channels */
    struct WlanChannel channels[0]; /**< WLAN channel structures */
};

/**
 * @brief Describes an SSID.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanSSID {
    uint8_t ssid[IEEE80211_MAX_SSID_LEN];   /**< SSID array */
    uint8_t ssidLen;                        /**< SSID length */
};

/**
 * @brief Describes scan request parameters.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanScanRequest {
    uint8_t ssidCount;      /**< Number of SSIDs */
    uint8_t freqsCount;     /**< Number of frequencies */
    uint8_t *bssid;         /**< BSSID to scan for */
    struct WlanSSID *ssids; /**< SSIDs to scan for */
    uint32_t *freqs;        /**< An array of frequencies */
    uint32_t extraIEsLen;   /**< Length of an extended information element (IE) */
    uint8_t *extraIEs;      /**< Extended IEs */
};

/**
 * @brief Describes cryptography settings.
 *
 * @since 1.0
 * @version 1.0
 */
struct CryptoSettings {
    uint32_t wpaVersions;      /**< WPA version */
    uint32_t cipherGroup;      /**< Cipher group */
    int32_t n_ciphersPairwise; /**< Number of unicast ciphers supported by the AP */
    uint32_t ciphersPairwise[NL80211_MAX_NR_CIPHER_SUITES]; /**< Unicast cipher suites. The maximum number of
                                                             * unicast cipher suites is specified by
                                                             * {@link NL80211_MAX_NR_CIPHER_SUITES}.
                                                             */
    int32_t n_akmSuites;        /**< Number of authentication and key management (AKM) suites */
    uint32_t akmSuites[NL80211_MAX_NR_AKM_SUITES];  /**< AKM suites. The maximum number of AKM suites is specified
                                                     * by {@link NL80211_MAX_NR_AKM_SUITES}.
                                                     */
    uint16_t controlPortEthertype;  /**< Data can be transmitted over an unauthenticated port. */
    int8_t controlPort;             /**< Whether the user space control port is authorized. The value <b>true</b>
                                     * indicates that the user space control port is unauthorized.
                                     */
    int8_t controlPortNoEncrypt;   /**< Whether to encrypt frames transmitted over the control port.
                                    * The value <b>1</b> indicates that the frames are not encrypted.
                                    */
};

/**
 * @brief Describes parameters for a connection request.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct WlanConnectParams {
    uint32_t centerFreq;    /**< Connection channel. If this parameter is not specified, the connection channel
                             * is automatically obtained from the scan result.
                             */
    uint8_t *bssid;         /**< AP BSSID. If this parameter is not specified, the AP BSSID is automatically
                             * obtained from the scan result.
                             */
    uint8_t *ssid;          /**< SSID */
    uint8_t *ie;            /**< IE information required for the connection */
    uint32_t ssidLen;       /**< SSID length */
    uint32_t ieLen;         /**< IE length */
    struct CryptoSettings crypto; /**< Cryptography information */
    const uint8_t *key;           /**< Wired Equivalent Privacy (WEP) key used for Shared Key
                                   * Authentication (SKA)
                                   */
    uint8_t authType;             /**< Authorization type */
    uint8_t privacy;              /**< Whether to use a privacy-enabled AP */
    uint8_t keyLen;               /**< Key length */
    uint8_t keyIdx;               /**< Index of the WEP key used for SKA */
    uint8_t mfp;                  /**< Whether to enable Management Frame Protection (MFP) */
    uint8_t aucResv[WIFI_CONNECT_PARM_RESV_SIZE];  /**< Reserved field */
} WlanConnectParams;

/**
 * @brief Describes parameters for canceling a connection.
 *
 * @since 1.0
 * @version 1.0
 */
struct StationDelParameters {
    const uint8_t *mac;  /**< MAC address of the station with which the connection is to be canceled */
    uint8_t subtype;     /**< Cancellation type */
    uint16_t reasonCode; /**< Reason code for the cancellation */
};

/**
 * @brief Describes MAC configuration parameters.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct MacConfigParam {
    uint8_t mac[IEEE80211_MAC_ADDR_LEN]; /**< MAC address. For details about its length, see {@link
                                          * IEEE80211_MAC_ADDR_LEN}.
                                          */
    uint8_t p2pMode;                     /**< Whether the peer-to-peer (P2P) mode is used. */
    uint8_t resv;                        /**< Reserved field */
} Mac80211SetMacParam;

/**
 * @brief Describes disconnection parameters.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    uint8_t macAddr[IEEE80211_MAC_ADDR_LEN]; /**< Device MAC address */
    uint16_t reasonCode;                     /**< Disconnection reason code */
} Mac80211DisconnectParam;

/**
 * @brief Describes the beacon configuration.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanBeaconConf {
    uint32_t interval;      /**< Beacon interval */
    uint32_t DTIMPeriod;    /**< Delivery Traffic Indication Message (DTIM) interval for sending beacons */
    uint8_t *headIEs;       /**< One or more IEs before the TIM IE */
    size_t headIEsLength;   /**< Length of the IEs before the TIM IE */
    uint8_t *tailIEs;       /**< One or more IEs after the TIM IE */
    size_t tailIEsLength;   /**< Length of the IEs after the TIM IE */
    bool hiddenSSID;        /**< Whether to hide the SSID */
};

/**
 * @brief Describes the AP configuration.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanAPConf {
    struct WlanSSID ssidConf;   /**< SSID configuration */
    uint16_t channel;           /**< Channel */
    uint16_t centerFreq1;       /**< Center frequency 1 */
    uint16_t centerFreq2;       /**< Center frequency 2 */
    uint8_t band;               /**< Band, as enumerated in <b>Ieee80211Band</b> */
    uint8_t width;              /**< Channel width, as enumerated in <b>Ieee80211ChannelWidth</b> */
};

/**
 * @brief Describes the hardware capability.
 *
 * @since 1.0
 * @version 1.0
 */
struct WlanHwCapability {
    void (*Release)(struct WlanHwCapability *self); /**< Function for releasing the hardware capability */
    struct WlanBand *bands[IEEE80211_NUM_BANDS];    /**< Frequency bands */
    uint16_t htCapability;       /**< High-throughput (HT) capability */
    uint16_t supportedRateCount; /**< Number of supported rates */
    uint16_t *supportedRates;    /**< An array of supported rates in 100 kbit/s */
};

/**
 * @brief Describes the MAC-layer APIs for basic operations that need to be implemented by the driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfMac80211BaseOps {
    /**
     * @brief Sets the WLAN mode.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param mode Indicates the WLAN mode to set.
     *
     * @return Returns <b>0</b> if the WLAN mode is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetMode)(NetDevice *netDev, enum WlanWorkMode mode);

    /**
     * @brief Adds a key with specified parameters.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param keyIndex Indicates the key index.
     * @param pairwise Specifies whether the key is a key pair.
     * @param macAddr Indicates the pointer to the MAC address.
     * @param params Indicates the pointer to the key parameters.
     *
     * @return Returns <b>0</b> if the key is added; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*AddKey)(struct NetDevice *netDev, uint8_t keyIndex, bool pairwise, const uint8_t *macAddr,
        struct KeyParams *params);

    /**
     * @brief Deletes a key with a specified MAC address.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param keyIndex Indicates the key index.
     * @param pairwise Specifies whether the key is a key pair.
     * @param macAddr Indicates the pointer to the MAC address.
     *
     * @return Returns <b>0</b> if the key is deleted; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DelKey)(struct NetDevice *netDev, uint8_t keyIndex, bool pairwise, const uint8_t *macAddr);

    /**
     * @brief Sets the default key.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param keyIndex Indicates the key index.
     * @param unicast Indicates the unicast.
     * @param multicas Indicates the multicast.
     *
     * @return Returns <b>0</b> if the default key is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDefaultKey)(struct NetDevice *netDev, uint8_t keyIndex, bool unicast, bool multicas);

    /**
     * @brief Obtains the device MAC address.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param type Indicates the MAC address type of the device.
     * @param mac Indicates the pointer to the MAC address.
     * @param len Indicates the length of the MAC address.
     *
     * @return Returns <b>0</b> if the MAC address is obtained; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDeviceMacAddr)(NetDevice *netDev, int32_t type, uint8_t *mac, uint8_t len);

    /**
     * @brief Sets the MAC address.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param mac Indicates the pointer to the MAC address.
     * @param len Indicates the length of the MAC address.
     *
     * @return Returns <b>0</b> if the MAC address is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetMacAddr)(NetDevice *netDev, uint8_t *mac, uint8_t len);

    /**
     * @brief Sets the transmit power.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param power Indicates the transmit power to set.
     *
     * @return Returns <b>0</b> if the transmit power is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetTxPower)(NetDevice *netDev, int32_t power);

    /**
     * @brief Obtains the frequencies supported by the 2.4 GHz or 5 GHz band.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param band Indicates the frequency band, either 2.4 GHz or 5 GHz.
     * @param freqs Indicates the pointer to the supported frequencies.
     * @param num Indicates the pointer to the number of supported frequencies.
     *
     * @return Returns <b>0</b> if the supported frequencies are obtained; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetValidFreqsWithBand)(NetDevice *netDev, int32_t band, int32_t *freqs, uint32_t *num);

    /**
     * @brief Obtains the hardware capability supported by the device.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param capability Indicates the double pointer to the hardware capability.
     *
     * @return Returns <b>0</b> if the hardware capability is obtained; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetHwCapability)(NetDevice *netDev, struct WlanHwCapability **capability);

    int32_t (*SendAction)(struct NetDevice *netDev, WifiActionData *actionData);

    int32_t (*GetIftype)(struct NetDevice *netDev, uint8_t *iftype);
};

/**
 * @brief Describes the MAC-layer APIs for station operations that need to be implemented by the driver.
 *
 * @since 1.0
 * @version 1.0
 */
struct HdfMac80211STAOps {
    /**
     * @brief Starts a connection based on the specified parameters.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param param Indicates the pointer to the connection parameters.
     *
     * @return Returns <b>0</b> if the connection is started; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Connect)(NetDevice *netDev, WlanConnectParams *param);

    /**
     * @brief Cancels a connection.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param reasonCode Indicates the reason code for connection cancellation.
     *
     * @return Returns <b>0</b> if the connection is canceled; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Disconnect)(NetDevice *netDev, uint16_t reasonCode);

    /**
     * @brief Starts a scan based on the specified parameters.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param param Indicates the pointer to the scan parameters.
     *
     * @return Returns <b>0</b> if the scan is started; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*StartScan)(NetDevice *netDev, struct WlanScanRequest *param);

    /**
     * @brief Stops a scan.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     *
     * @return Returns <b>0</b> if the scan is stopped; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*AbortScan)(NetDevice *netDev);

    /**
     * @brief Sets scanning for a single MAC address.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param mac Indicates the pointer to the MAC address.
     * @param len Indicates the length of the MAC address.
     *
     * @return Returns <b>0</b> if the setting is successful; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetScanningMacAddress)(NetDevice *netDev, unsigned char *mac, uint32_t len);
};

/**
 * @brief Describes the MAC-layer APIs for AP operations that need to be implemented by the driver.
 *
 * @since 1.0
 */
struct HdfMac80211APOps {
    /**
     * @brief Sets an AP.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param apConf Indicates the pointer to the AP configuration.
     *
     * @return Returns <b>0</b> if the AP is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*ConfigAp)(NetDevice *netDev, struct WlanAPConf *apConf);

    /**
     * @brief Starts an AP.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     *
     * @return Returns <b>0</b> if the AP is started; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*StartAp)(NetDevice *netDev);

    /**
     * @brief Stops an AP.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     *
     * @return Returns <b>0</b> if the AP is stopped; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*StopAp)(NetDevice *netDev);

    /**
     * @brief Sets a beacon.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param beaconConf Indicates the pointer to the beacon configuration.
     *
     * @return Returns <b>0</b> if the beacon is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*ConfigBeacon)(NetDevice *netDev, struct WlanBeaconConf *beaconConf);

    /**
     * @brief Deletes a station with a specified MAC address.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param macAddr Indicates the pointer to the MAC address.
     *
     * @return Returns <b>0</b> if the station is deleted; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DelStation)(NetDevice *netDev, const uint8_t *macAddr);

    /**
     * @brief Sets the country/region code.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param code Indicates the pointer to the country/region code.
     * @param len Indicates the length of the country/region code.
     *
     * @return Returns <b>0</b> if the country/region code is set; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetCountryCode)(NetDevice *netDev, const char *code, uint32_t len);

    /**
     * @brief Obtains the number of associated stations.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param num Indicates the pointer to the number of associated stations.
     *
     * @return Returns <b>0</b> if the number of associated stations is obtained; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetAssociatedStasCount)(NetDevice *netDev, uint32_t *num);

    /**
     * @brief Obtains information about the associated stations.
     *
     * @param netDev Indicates the pointer to the network device structure obtained during initialization.
     * @param staInfo Indicates the pointer to the information about the associated stations.
     * @param num Indicates the number of associated stations.
     *
     * @return Returns <b>0</b> if the information is obtained; returns a non-zero value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetAssociatedStasInfo)(NetDevice *netDev, WifiStaInfo *staInfo, uint32_t num);
};

struct HdfMac80211P2POps {
    int32_t (*RemainOnChannel)(NetDevice *netDev, WifiOnChannel *onChannel);

    int32_t (*CancelRemainOnChannel)(NetDevice *netDev);

    int32_t (*ProbeReqReport)(NetDevice *netDev, int32_t report);

    int32_t (*AddIf)(NetDevice *netDev, WifiIfAdd *ifAdd);

    int32_t (*RemoveIf)(NetDevice *netDev, WifiIfRemove *ifRemove);

    int32_t (*SetApWpsP2pIe)(NetDevice *netDev, WifiAppIe *appIe);

    int32_t (*GetDriverFlag)(struct NetDevice *netDev, WifiGetDrvFlags **params);
};

#endif // WIFI_MAC80211_OPS_H
/** @} */
