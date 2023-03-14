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

#ifndef OHOS_WIFI_HAL_DEFINE_H
#define OHOS_WIFI_HAL_DEFINE_H

/* Contains common header files. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#define WIFI_COUNTRY_CODE_MAXLEN 2
#define WIFI_IFACE_NAME_MAXLEN 32
#define WIFI_P2P_WPS_NAME_LENGTH 128
#define WIFI_P2P_WPS_METHODS_LENGTH 256
#define WIFI_P2P_WFD_DEVICE_CONF_LENGTH 256
#define WIFI_P2P_SERVE_INFO_LENGTH 256
#define WIFI_P2P_SERVE_DISCOVER_MSG_LENGTH 256
#define WIFI_P2P_SERVER_DISCOVERY_SEQUENCE_LENGTH 64
#define WIFI_P2P_GROUP_IFNAME_LENGTH 128
#define WIFI_P2P_SERVER_NAME_LENGTH 256
#define WIFI_NETWORK_PSK_MAXLEN 64

typedef enum WifiErrorNo {
    WIFI_HAL_SUCCESS = 0,                /* Success. */
    WIFI_HAL_FAILED = 1,                 /* Failed. */
    WIFI_HAL_SCAN_BUSY = 2,              /* Scan failed. Scan busy. */
    WIFI_HAL_PBC_OVERLAP = 3,            /* WPS PBC mode overlap. */
    WIFI_HAL_SUPPLICANT_NOT_INIT = 4,    /* The wpa_supplicant is not initialized or fails to be initialized. */
    WIFI_HAL_OPEN_SUPPLICANT_FAILED = 5, /* Start wpa_supplicant failed. */
    WIFI_HAL_CONN_SUPPLICANT_FAILED = 6, /* Connect wpa_supplicant failed. */
    WIFI_HAL_HOSTAPD_NOT_INIT = 7,       /* Hostapd is not initialized or initialization fails. */
    WIFI_HAL_OPEN_HOSTAPD_FAILED = 8,    /* Start hostapd failed. */
    WIFI_HAL_CONN_HOSTAPD_FAILED = 9,    /* Connect hostapd failed. */
    WIFI_HAL_NOT_SUPPORT = 10,           /* Not supported currently. */
    WIFI_HAL_GET_WIFI_COND_FAILED,       /* Initialized  wificond failed. */
    WIFI_HAL_BUFFER_TOO_LITTLE,          /* request buffer size too small */
    WIFI_HAL_INPUT_MAC_INVALID,
    WIFI_HAL_GET_VENDOR_HAL_FAILED, /* Initialized vendor hal failed. */
    WIFI_HAL_VENDOR_UNKNOWN,
    WIFI_HAL_VENDOR_UNINITIALIZED,
    WIFI_HAL_VENDOR_NOT_AVAILABLE,
    WIFI_HAL_VENDOR_INVALID_ARGS,
    WIFI_HAL_VENDOR_INVALID_REQUEST_ID,
    WIFI_HAL_VENDOR_TIMED_OUT,
    WIFI_HAL_VENDOR_TOO_MANY_REQUESTS,
    WIFI_HAL_VENDOR_OUT_OF_MEMORY,
    WIFI_HAL_VENDOR_BUSY,
    WIFI_HAL_INVALID_PARAM,
    WIFI_HAL_GET_P2P_GROUP_INFACE_FAILED,
} WifiErrorNo;

/* ID of the callback event for registering the Hal service. */
typedef enum WifiHalEvent {
    WIFI_FAILURE_EVENT = 100,                /* Driver loading/unloading failure. */
    WIFI_START_EVENT,                  /* The driver has been loaded. */
    WIFI_STOP_EVENT,                   /* Driver uninstalled. */
    WIFI_ADD_IFACE_EVENT,              /* The network device interface has been added. */
    WIFI_REMOVE_IFACE_EVENT,           /* The network device interface has been deleted. */
    WIFI_STA_JOIN_EVENT,               /* STA connection notification in AP mode. */
    WIFI_STA_LEAVE_EVENT,              /* STA disconnection notification in AP mode. */
    WIFI_SCAN_INFO_NOTIFY_EVENT,       /* Scan info notification. */
    WIFI_CONNECT_CHANGED_NOTIFY_EVENT, /* Connection status change notification. */
    WIFI_AP_ENABLE_EVENT,              /* AP enabling notification. */
    WIFI_AP_DISABLE_EVENT,             /* AP closure notification. */
    WIFI_WPA_STATE_EVENT,              /* WPA status change. */
    WIFI_SSID_WRONG_KEY,               /* Incorrect password. */
    WIFI_CONNECTION_FULL_EVENT,        /* connection is full */
    WIFI_CONNECTION_REJECT_EVENT,      /* connection reject */
    WIFI_WPS_OVERLAP,                  /* wps pbc overlap */
    WIFI_WPS_TIME_OUT,                 /* wps connect time out */
    WIFI_P2P_SUP_CONNECTION_EVENT,     /* Connection result of the wpa_supplicant client */
    SUP_CONN_FAILED_EVENT,               /* Wpa_supplicant client connection failure event */
    P2P_DEVICE_FOUND_EVENT,              /* Device discovery event */
    P2P_DEVICE_LOST_EVENT,               /* Device loss event */
    P2P_GO_NEGOTIATION_REQUEST_EVENT,    /* Event of receiving a GO negotiation request */
    P2P_GO_NEGOTIATION_SUCCESS_EVENT,    /* The GO negotiation is successful */
    P2P_GO_NEGOTIATION_FAILURE_EVENT,    /* The GO negotiation fails */
    P2P_INVITATION_RECEIVED_EVENT,       /* P2P invitation request event */
    P2P_INVITATION_RESULT_EVENT,         /* P2P invitation result */
    P2P_GROUP_FORMATION_SUCCESS_EVENT,   /* The group is created successfully */
    P2P_GROUP_FORMATION_FAILURE_EVENT,   /* The group is created failure */
    P2P_GROUP_STARTED_EVENT,             /* Group Start Event */
    P2P_GROUP_REMOVED_EVENT,             /* Group removed event */
    P2P_PROV_DISC_PBC_REQ_EVENT,         /* Provision Discovery request event */
    P2P_PROV_DISC_PBC_RSP_EVENT,         /* Provision Discovery Response Event */
    P2P_PROV_DISC_ENTER_PIN_EVENT,       /* Provision Discovery PIN input event */
    P2P_PROV_DISC_SHOW_PIN_EVENT,        /* Provision Discovery Display PIN Event */
    P2P_FIND_STOPPED_EVENT,              /* Device search stop event */
    P2P_SERV_DISC_RESP_EVENT,            /* Service response event */
    P2P_PROV_DISC_FAILURE_EVENT,         /* Provision Discovery failure event */
    AP_STA_DISCONNECTED_EVENT,           /* STA Disconnected from AP */
    AP_STA_CONNECTED_EVENT,              /* STA and AP connected event */
    P2P_SERV_DISC_REQ_EVENT,             /* Service discovery request event */
    WIFI_HAL_MAX_EVENT,
} WifiHalEvent;

#define WIFI_BSSID_LENGTH 18
#define WIFI_NETWORK_FLAGS_LENGTH 64
#define WIFI_SSID_LENGTH 132
#define WIFI_SCAN_INFO_CAPABILITY_LENGTH 256
#define WIFI_NETWORK_CONFIG_NAME_LENGTH 64
#define WIFI_NETWORK_CONFIG_VALUE_LENGTH 256
#define WIFI_P2P_GROUP_CONFIG_VALUE_LENGTH 256
#define WIFI_MAC_LENGTH 17
#define WIFI_AP_PASSWORD_LENGTH 64
#define WIFI_PIN_CODE_LENGTH 8

/* Wifi network configuration parameter flag. */
typedef enum DeviceConfigType {
    DEVICE_CONFIG_SSID = 0, /* Network Name. */
    DEVICE_CONFIG_PSK = 1,  /* Password. */
    /**
     * Encryption Mode: WPA-PSK - wpa/wp2; NONE - password less network; WPA-EAP, SAE, wpa3.
     */
    DEVICE_CONFIG_KEYMGMT = 2,
    DEVICE_CONFIG_PRIORITY = 3, /* WPA network priority */
    /**
     * Set this bit to 1 and deliver it when the hidden network is connected.
     * In other cases, set this bit to 0 but do not deliver it.
     */
    DEVICE_CONFIG_SCAN_SSID = 4,
    DEVICE_CONFIG_EAP = 5,             /* EPA Mode:/EAP/PEAP. */
    DEVICE_CONFIG_IDENTITY = 6,        /* Account name. */
    DEVICE_CONFIG_PASSWORD = 7,        /* Account password. */
    DEVICE_CONFIG_BSSID = 8,           /* bssid. */
    DEVICE_CONFIG_AUTH_ALGORITHMS = 9, /* auth algorithms */
    DEVICE_CONFIG_WEP_KEY_IDX = 10,    /* wep key idx */
    DEVICE_CONFIG_WEP_KEY_0 = 11,
    DEVICE_CONFIG_WEP_KEY_1 = 12,
    DEVICE_CONFIG_WEP_KEY_2 = 13,
    DEVICE_CONFIG_WEP_KEY_3 = 14,
    /**
     * Number of network configuration parameters, which is used as the last
     * parameter.
     */
    DEVICE_CONFIG_END_POS,
} DeviceConfigType;

typedef enum P2pGroupConfigType {
    GROUP_CONFIG_SSID = 0,
    GROUP_CONFIG_BSSID,
    GROUP_CONFIG_PSK,
    GROUP_CONFIG_PROTO,
    GROUP_CONFIG_KEY_MGMT,
    GROUP_CONFIG_PAIRWISE,
    GROUP_CONFIG_AUTH_ALG,
    GROUP_CONFIG_MODE,
    GROUP_CONFIG_DISABLED,
    GROUP_CONFIG_END_POS,
} P2pGroupConfigType;

/* AP Band */
typedef enum ApBand {
    AP_NONE_BAND = 0, /* Unknown Band */
    AP_2GHZ_BAND = 1, /* 2.4GHz Band */
    AP_5GHZ_BAND = 2, /* 5GHz Band */
    AP_ANY_BAND = 3,  /* Dual-mode frequency band */
    AP_DFS_BAND = 4
} ApBand;

/*  Encryption Mode */
typedef enum KeyMgmt {
    NONE = 0,    /* WPA not used. */
    WPA_PSK = 1, /* WPA pre-shared key ({@ preSharedKey} needs to be specified.) */
    /**
     * WPA with EAP authentication. It is usually used with an external
     * authentication server.
     */
    WPA_EAP = 2,
    /**
     * IEEE 802.1X with EAP authentication and optionally dynamically generated
     * WEP keys.
     */
    IEEE8021X = 3,
    /**
     * WPA2 pre-shared key, which is used for soft APs({@ preSharedKey} needs to
     * be specified).
     */
    WPA2_PSK = 4,
    OSEN = 5,
    FT_PSK = 6,
    FT_EAP = 7
} KeyMgmt;

/* chip supported interface combination mode */
typedef enum WifiInterfaceCombMode {
    STA_STA_MODE,
    STA_AP_MODE,
    STA_P2P_MODE,
    STA_NAN_MODE,
    AP_NAN_MODE,
} WifiInterfaceCombMode;

typedef enum HalWpsMethod {
    HAL_WPS_METHOD_PBC,
    HAL_WPS_METHOD_DISPLAY,
    HAL_WPS_METHOD_KEYPAD,
    HAL_WPS_METHOD_LABEL,
    HAL_WPS_METHOD_INVALID
} HalWpsMethod;

#ifdef __cplusplus
}
#endif
#endif