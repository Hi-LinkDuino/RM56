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
 * @file net_device.h
 *
 * @brief Defines WLAN network device interfaces.
 *
 * This module provides network device operations for driver development.
 * The operations are as follows:
 * <li> Initializes, adds, obtains, and deletes a network device. </li>
 * <li> Reports network data, informs the network layer of the data link layer status, sets IP addresses,
 * and starts or stops Dynamic Host Configuration Protocol (DHCP). </li>
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDF_NET_DEVICE_MODULE_H
#define HDF_NET_DEVICE_MODULE_H
#include "hdf_base.h"
#include "hdf_netbuf.h"
#include "hdf_log.h"
#include "net_device_types_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Indicates a 6-byte MAC address.
 *
 * @since 1.0
 * @version 1.0
 */
#define MAC_ADDR_SIZE 6
/**
 * @brief Indicates that the network port is working.
 *
 * @since 1.0
 * @version 1.0
 */
#define NET_DEVICE_IFF_RUNNING IFF_RUNNING
/**
 * @brief Indicates Reverse Address Resolution Protocol (RARP).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_RARP 0x8035
/**
 * @brief Indicates Port Access Entity (PAE).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_PAE 0x888e
/**
 * @brief Indicates Internet Protocol (IP).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_IP 0x0800
/**
 * @brief Indicates AppleTalk Address Resolution Protocol (AARP).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_AARP 0x80f3
/**
 * @brief Indicates Internetwork Packet Exchange (IPX).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_IPX 0x8137
/**
 * @brief Indicates Address Resolution Protocol (ARP).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_ARP 0x0806
/**
 * @brief Indicates Internet Protocol version 6 (IPv6).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_IPV6 0x86dd
/**
 * @brief Indicates Tunneled Direct Link Setup (TDLS).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_TDLS 0x890d
/**
 * @brief Indicates Virtual Local Area Network (VLAN).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_VLAN 0x8100
/**
 * @brief Indicates WLAN Authentication and Privacy Infrastructure (WAPI).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_WAI 0x88b4
/**
 * @brief Indicates Link Layer Topology Discovery (LLTD).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_LLTD_TYPE 0x88D9
/**
 * @brief Indicates 802.1x network port authentication.
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_ONE_X_TYPE 0x888E
/**
 * @brief Indicates a tunnel protocol.
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TUNNEL_TYPE 0x88bd
/**
 * @brief Indicates the point-to-point discovery type.
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_PPP_DISC 0x8863
/**
 * @brief Indicates the point-to-point session discovery type.
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_PPP_SES 0x8864
/**
 * @brief Indicates IPv6 over Low Power Wireless Personal Area Networks (6LoWPANs).
 *
 * @since 1.0
 * @version 1.0
 */
#define ETHER_TYPE_6LO 0xa0ed

/**
 * @brief Indicates the Transmission Control Protocol (TCP).
 *
 * @since 1.0
 * @version 1.0
 */
#define TCP_PROTOCOL       6
/**
 * @brief Indicates the User Datagram Protocol (UDP).
 *
 * @since 1.0
 * @version 1.0
 */
#define UDP_PROTOCOL       17
/**
 * @brief Indicates the shift in the priority for an IP address.
 *
 * @since 1.0
 * @version 1.0
 */
#define IP_PRI_SHIFT        5

/**
 * @brief Indicates the source port number of DHCP.
 *
 * @since 1.0
 * @version 1.0
 */
#define DHCP_UDP_SRC_PORT   68
/**
 * @brief Indicates the destination port number of DHCP.
 *
 * @since 1.0
 * @version 1.0
 */
#define DHCP_UDP_DES_PORT   67

/**
 * @brief Indicates the default MTU value of the WLAN.
 *
 * @since 1.0
 * @version 1.0
 */
#define DEFAULT_MTU 1500

/**
 * @brief Indicates the maximum MTU value of the WLAN.
 *
 * @since 1.0
 * @version 1.0
 */
#define WLAN_MAX_MTU 1500

/**
 * @brief Indicates the minimum MTU value of the WLAN.
 *
 * @since 1.0
 * @version 1.0
 */
#define WLAN_MIN_MTU 576

/**
 * @brief Enumerates network interface categories, including lightweight OS and full OS.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    LITE_OS,   /**< Lightweight OS */
    FULL_OS   /**< Full OS */
} NetIfCategory;

/**
 * @brief Enumerates network interface states, including online and offline.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    NETIF_DOWN,  /**< Network interface offline */
    NETIF_UP     /**< Network interface online */
} NetIfStatus;

/**
 * @brief Enumerates network link layer states, including online and offline.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    NETIF_LINK_DOWN,  /**< Data link offline */
    NETIF_LINK_UP     /**< Data link online */
} NetIfLinkStatus;

/**
 * @brief Enumerates data link types, including Ethernet and WLAN.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    ETHERNET_LINK = 1,  /**< Ethernet */
    WIFI_LINK = 801     /**< WLAN */
} NetLinkType;

/**
 * @brief Enumerates data processing results, including continuing processing, processing completed, and error.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    PROCESSING_CONTINUE,  /**< Continuing processing */
    PROCESSING_COMPLETE,  /**< Processing completed */
    PROCESSING_ERROR      /**< Error */
} ProcessingResult;

/**
 * @brief Enumerates 802.11 network port types, including AP, STA, and P2P.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    PROTOCOL_80211_IFTYPE_UNSPECIFIED,                         /**< Unspecified */
    PROTOCOL_80211_IFTYPE_ADHOC,                               /**< Ad hoc network */
    PROTOCOL_80211_IFTYPE_STATION,                             /**< Workstation */
    PROTOCOL_80211_IFTYPE_AP,                                  /**< Access point */
    PROTOCOL_80211_IFTYPE_AP_VLAN,                             /**< Virtual access point */
    PROTOCOL_80211_IFTYPE_WDS,                                 /**< Wireless distributed system */
    PROTOCOL_80211_IFTYPE_MONITOR,                             /**< Listening */
    PROTOCOL_80211_IFTYPE_MESH_POINT,                          /**< Mesh network */
    PROTOCOL_80211_IFTYPE_P2P_CLIENT,                          /**< P2P client */
    PROTOCOL_80211_IFTYPE_P2P_GO,                              /**< P2P group owner */
    PROTOCOL_80211_IFTYPE_P2P_DEVICE,                          /**< P2P device */
    PROTOCOL_80211_IFTYPE_NUM,                                 /**< Number of network ports */
    PROTOCOL_80211_IFTYPE_MAX = PROTOCOL_80211_IFTYPE_NUM - 1  /**< Maximum number of 802.11 network port types */
} Protocol80211IfType;

/**
 * @brief Enumerates network device errors, including common errors and errors in adding network devices to LwIP.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    COMMON_ERROR = 1,  /**< Common errors */
    ADD_LWIP_ERROR     /**< Errors in adding network devices to LwIP */
} NetdeviceError;

/**
 * @brief Defines the Ethernet header information of a data frame, including the destination address, source address,
 * and Ethernet type.
 *
 * @since 1.0
 * @version 1.0
 */
struct EtherHeader {
    uint8_t etherDhost[MAC_ADDR_SIZE];  /**< Destination address {@link MAC_ADDR_SIZE} */
    uint8_t etherShost[MAC_ADDR_SIZE];  /**< Source address {@link MAC_ADDR_SIZE} */
    uint16_t etherType;                 /**< Ethernet type, such as 0x8035 (RARP), 0x888e (EAPOL), PAE/802.1x,
                                         * 0x0800 (IP), 0x86dd (IPV6), and 0x0806 (ARP)
                                         */
};

/**
 * @brief Defines the IP header information of a data frame, including the version number, service type,
 * and total length.
 *
 * @since 1.0
 * @version 1.0
 */
struct IpHeader {
    uint8_t    versionAndHl;   /**< Version and header length */
    uint8_t    tos;            /**< Service type
                                * bit7~bit5 | bit4  |  bit3 |  bit2 |   bit1  | bit0
                                * ----------|-------|-------|-------|---------|-----
                                * Priority | Delay | Throughput | Reliability | Transmission Cost | Reservation
                                */
    uint16_t   totLen;         /**< Total length of an IP data packet */
    uint16_t   id;             /**< Each data packet sent by the host */
    uint16_t   fragInfo;       /**< Fragmentation information */
    uint8_t    ttl;            /**< Generation time */
    uint8_t    protocol;       /**< Protocol, such as 1 (ICMP), 2 (IGMP), 6 (TCP), 17 (UDP), and 89 (OSPF) */
    uint16_t   check;          /**< Header check */
    uint32_t   sAddr;          /**< Source address */
    uint32_t   dAddr;          /**< Destination address */
};

/**
 * @brief Defines the UDP header information of a data frame, including the source port number and destination
 * port number.
 *
 * @since 1.0
 * @version 1.0
 */
struct UdpHeader {
    uint16_t source;  /**< Source port number */
    uint16_t dest;    /**< Destination port number */
    uint16_t len;     /**< Length of a data packet */
    uint16_t check;   /**< Header check */
};

/**
 * @brief Defines the TCP header information of a data frame, including the source port number and destination
 * port number.
 *
 * @since 1.0
 * @version 1.0
 */
struct TcpHeader {
    uint16_t  sPort;   /**< Source port number */
    uint16_t  dPort;   /**< Destination port number */
    uint32_t  seqNum;  /**< Sequence number */
    uint32_t  ackNum;  /**< Acknowledgment number */
    uint8_t   offset;  /**< Header length */
    uint8_t   flags;   /**< Flags */
    uint16_t  window;  /**< Window size */
    uint16_t  check;   /**< Checksum */
    uint16_t  urgent;  /**< Urgent pointer */
};

/**
 * @brief Defines an IPv4 address.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    uint32_t addr;  /**< IPv4 address */
} IpV4Addr;

/**
 * @brief Defines a network device notification, including an IP address and the notification type.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    uint32_t ipAddr;      /**< IP address */
    uint32_t notifyType;  /**< Notification type (reserved) */
} NetDevNotify;

/**
 * @brief Defines the network port type, for example, the WLAN network port.
 *
 * @since 1.0
 * @version 1.0
 */
typedef union {
    Protocol80211IfType wlanType;  /**< WLAN network port type: AP or STA */
} IfType;

/**
 * @brief Describes the status of a network device.
 *
 * @since 1.0
 * @version 1.0
 */
struct NetDevStats {
    uint32_t rxPackets; /**< Total number of received packets */
    uint32_t txPackets; /**< Total number of transmitted packets */
    uint32_t rxBytes;   /**< Total number of received bits */
    uint32_t txBytes;   /**< Total number of transmitted bits */
    uint32_t rxErrors;  /**< Number of received error packets */
    uint32_t txErrors;  /**< Transmitted error packets */
    uint32_t rxDropped; /**< Packets that are dropped after being received */
    uint32_t txDropped; /**< Packets dropped before transmission */
};

/**
 * @brief Defines ioctrl data.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct {
    uint32_t fake;     /**< magic field */
    uint8_t *ifrData;  /**< Data pointer */
} IfReq;

/**
 * @brief Defines a network device, including the network interface category and name, and network port type.
 *
 * @since 1.0
 * @version 1.0
 */
typedef struct NetDevice {
    NetIfCategory netifCateg;                 /**< Network interface category {@link NetIfCategory} */
    char name[IFNAMSIZ];                      /**< Network device name {@link IFNAMSIZ} */
    NetLinkType LinkLayerType;                /**< Data link layer type */
    IfType funType;                           /**< Network port type */
    uint8_t macAddr[MAC_ADDR_SIZE];           /**< MAC address {@link MAC_ADDR_SIZE} */
    uint32_t flags;                           /**< Network port status */
    uint32_t mtu;                             /**< Maximum transmission unit */
    int32_t watchdogTime;                     /**< Watchdog duration */
    uint32_t lastRxTime;                      /**< Last time when data is received */
    uint16_t hardHeaderLen;                   /**< Header length */
    uint16_t neededHeadRoom;                  /**< Length reserved for the header in netbuff{@link NetBuf} */
    uint16_t neededTailRoom;                  /**< Length reserved for the tail in netbuff{@link NetBuf} */
    uint8_t addrLen;                          /**< MAC address length */
    const char* classDriverName;              /**< driver name of class driver */
    void *classDriverPriv;                    /**< Private structure for the platform */
    void *mlPriv;                             /**< Private structure for the driver */
    void *ieee80211Ptr;                       /**< Pointer to a wireless device */
    void *specialProcPriv;                    /**< Private structure for data processing */
    struct NetDeviceInterFace *netDeviceIf;   /**< Network device interface */
    struct NetDevice *owner;                  /**< Network device */
    struct NetDevStats stats;                 /**< Network statistics */
} NetDevice;

/**
 * @brief Defines interfaces that need to be implemented externally by network devices, including initializing,
 * opening, and closing a network device.
 *
 * @since 1.0
 * @version 1.0
 */
struct NetDeviceInterFace {
    int32_t (*init)(struct NetDevice *netDev);  /**< Initializes a network device to be added. */
    void (*deInit)(struct NetDevice *netDev);   /**< Deinitializes a network device to be delete. */
    int32_t (*open)(struct NetDevice *netDev);  /**< Opens the data link layer. */
    int32_t (*stop)(struct NetDevice *netDev);  /**< Closes the data link layer. */
    NetDevTxResult (*xmit)(struct NetDevice *netDev, NetBuf *netBuff);        /**< Sends data. */
    int32_t (*ioctl)(struct NetDevice *netDev, IfReq *req, int32_t cmd);      /**< Used for the control command word. */
    int32_t (*setMacAddr)(struct NetDevice *netDev, void *addr);              /**< Sets the MAC address. */
    struct NetDevStats *(*getStats)(struct NetDevice *netDev);                /**< Obtains the statistics. */
    void (*setNetIfStatus)(struct NetDevice *netDev, NetIfStatus status);     /**< Sets the network port status. */
    uint16_t (*selectQueue)(struct NetDevice *netDev, NetBuf *netBuff);       /**< Selects a priority queue. */
    uint32_t (*netifNotify)(struct NetDevice *netDev, NetDevNotify *notify);  /**< Notifies the network port status. */
    int32_t (*changeMtu)(struct NetDevice *netDev, int32_t newMtu);           /**< Changes the maximum number of
                                                                               * transmission units.
                                                                               */
    void (*linkStatusChanged)(struct NetDevice *netDev);    /**< Detects the change of
                                                             * the Ethernet port connection status.
                                                             */
    ProcessingResult (*specialEtherTypeProcess)(const struct NetDevice *netDev, NetBuf *buff);
                                                                              /**< Performs private processing without
                                                                               * involving network-layer data.
                                                                               */
};

/**
 * @brief Initializes a network device to obtain its instance.
 *
 * @param ifName Indicates the pointer to the network device name.
 * @param len Indicates the length of the network device name.
 * @param type Indicates the data link type.
 * @param ifCategory Indicates the network port category.
 *
 * @return Returns the structure {@link NetDevice} for the initialized network device if the operation is successful;
 * returns <b>NULL</b> if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
struct NetDevice *NetDeviceInit(const char *ifName, uint32_t len, NetLinkType type, NetIfCategory ifCategory);

/**
 * @brief Deletes a network device.
 *
 * @param netDevice Indicates the pointer to the network device structure obtained during initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value representing {@link HDF_STATUS}
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetDeviceDeInit(struct NetDevice *netDevice);

/**
 * @brief Adds a network device to a protocol stack.
 *
 * @param netDevice Indicates the pointer to the network device structure obtained during initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value representing {@link HDF_STATUS}
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetDeviceAdd(struct NetDevice *netDevice);

/**
 * @brief Deletes a network device from a protocol stack.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained during
 * initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value representing {@link HDF_STATUS}
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetDeviceDelete(struct NetDevice *netDevice);

/**
 * @brief Obtains the initialized network device instance by a specified device name.
 *
 * @param name Indicates the pointer to the network device name.
 *
 * @return Returns the network device structure {@link NetDevice} matching the network device name if the operation is
 * successful; returns <b>NULL</b> if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
struct NetDevice *NetDeviceGetInstByName(const char *name);

/**
 * @brief Checks whether there are working devices among the added network devices.
 *
 * @return Returns <b>true</b> if the added network devices are working; returns <b>false</b> if none of the added
 * network devices is working.
 *
 * @since 1.0
 * @version 1.0 */
bool NetDeviceIsAnyInstRunning(void);

/**
 * @brief Checks whether a specified network device is working.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained during
 * initialization.
 *
 * @return Returns <b>true</b> if the specified network device is working; returns <b>false</b> otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
bool NetDeviceIsInstRunning(const struct NetDevice *netDevice);

/**
 * @brief Obtains the number of added network devices.
 *
 * @return Returns the number of added network devices.
 *
 * @since 1.0
 * @version 1.0
 */
uint32_t NetDevGetRegisterCount(void);

/**
 * @brief Obtains the maximum number of network devices that can be registered with this system at the same time.
 *
 * @return Returns the maximum number of network devices.
 *
 * @since 1.0
 * @version 1.0
 */
uint32_t NetDeviceGetCap(void);

/**
 * @brief Obtains a network device instance based on the index number.
 *
 * @param index Indicates the index number.
 *
 * @return Returns the network device structure {@link NetDevice} if the operation is successful; returns <b>NULL</b>
 * if the operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
struct NetDevice *NetDeviceGetInstByIndex(uint32_t index);

/**
 * @brief Sets an IP address, mask, and gateway.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained during
 * initialization.
 * @param ipAddr Indicates the IP address to set.
 * @param netMask Indicates the mask to set.
 * @param gw Indicates the gateway to set.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfSetAddr(const struct NetDevice *netDevice, const IpV4Addr *ipAddr, const IpV4Addr *netMask,
    const IpV4Addr *gw);

/**
 * @brief Notifies the network layer of the network port state.
 *
 * @param netDevice Indicates the pointer to the network device obtained during initialization.
 * @param status Indicates the network port state, as enumerated in {@link NetIfSatus}.
 * @param status Indicates the network port state, as enumerated in {@link NetIfSatus}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfSetStatus(const struct NetDevice *netDevice, NetIfStatus status);

/**
 * @brief Sets a specified MAC address for a given {@link NetDevice}.
 *
 * @param netDevice Indicates the pointer to the {@link NetDevice}.
 * @param macAddr Indicates the pointer to the MAC address to set.
 * @param length Indicates the length of the MAC address to set.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfSetMacAddr(struct NetDevice *netDevice, const unsigned char *macAddr, unsigned char length);

/**
 * @brief Notifies the network layer of the data link layer status.
 *
 * @param netDevice Indicates the pointer to the network device obtained during initialization.
 * @param status status Indicates the data link layer status, as enumerated in {@link NetIfLinkSatus}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfSetLinkStatus(const struct NetDevice *netDevice, NetIfLinkStatus status);

/**
 * @brief Get the netdevice data link layer status.
 *
 * @param netDevice Indicates the pointer to the network device obtained during initialization.
 * @param status save link layer status, as enumerated in {@link NetIfLinkSatus}.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfGetLinkStatus(const struct NetDevice *netDevice, NetIfLinkStatus *status);

/**
 * @brief Transfers data packets from the network side to a protocol stack in an interrupt processing thread.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained
 * during initialization.
 * @param buff Indicates the network-side data, in Ether format.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value {@link HDF_STATUS} if the
 * operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfRx(const struct NetDevice *netDevice, NetBuf *buff);

/**
 * @brief Transfers the input data packets from the network side to a protocol stack.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained
 * during initialization.
 * @param buff Indicates the network-side data, in Ether format.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value {@link HDF_STATUS} if the
 * operation fails.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfRxNi(const struct NetDevice *netDevice, NetBuf *buff);

/**
 * @brief Starts the DHCP server.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained
 * during initialization.
 * @param ip Indicates the IP address to start.
 * @param ipNum Indicates the number of IP addresses.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfDhcpsStart(const struct NetDevice *netDevice, char *ip, uint16_t ipNum);

/**
 * @brief Stops the DHCP server.
 *
 * @param netDevice Indicates the pointer to the network device structure {@link netDevice} obtained
 * during initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfDhcpsStop(const struct NetDevice *netDevice);

/**
 * @brief Starts the DHCP client of a specified network device.
 *
 * @param netDevice Indicates the pointer to the network device obtained during initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfDhcpStart(const struct NetDevice *netDevice);

/**
 * @brief Stops the DHCP client of a specified network device.
 *
 * @param netDevice Indicates the pointer to the network device obtained during initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfDhcpStop(const struct NetDevice *netDevice);

/**
 * @brief Obtains the DHCP negotiation status of a specified network device.
 *
 * @param netDevice Indicates the pointer to the network device obtained during initialization.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t NetIfDhcpIsBound(const struct NetDevice *netDevice);

#define GET_NET_DEV_FLAGS(dev) ((dev)->flags)
#define GET_NET_DEV_HEAD_ROOM(dev) ((dev)->neededHeadRoom)
#define GET_NET_DEV_TAIL_ROOM(dev) ((dev)->neededTailRoom)
#define GET_NET_DEV_MTU(dev) ((dev)->mtu)
#define GET_NET_DEV_CFG80211_WIRELESS(dev) ((struct wireless_dev*)((dev)->ieee80211Ptr))
#define GET_NET_DEV_PRIV(dev) ((dev)->mlPriv)
#define GET_NET_DEV_MAC_ADDR(dev) ((dev)->macAddr)
#define GET_NET_DEV_IF(dev) ((dev)->netDeviceIf)
#define GET_NET_DEV_LAST_RX_TIME(dev) ((dev)->lastRxTime)
#define GET_NET_DEV_OWNER(dev) ((dev)->owner)
#define GET_NET_DEV_WATCHDOG_TIME(dev) ((dev)->watchdogTime)
#define GET_NET_DEV_SPECIAL_PROC_PRIV(dev) ((dev)->specialProcPriv)
#ifdef __cplusplus
}
#endif

#endif /* HDF_NET_DEVICE_MODULE_H */
/** @} */
