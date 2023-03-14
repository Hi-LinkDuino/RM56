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

#ifndef OHOS_WIFI_IDL_DEFINE_H
#define OHOS_WIFI_IDL_DEFINE_H

typedef enum WifiIdlEvent {
    /* IWifiEventCallback */
    WIFI_IDL_CBK_CMD_FAILURE = 100, /* Driver loading/unloading failure */
    WIFI_IDL_CBK_CMD_STARTED, /* The driver has been loaded. */
    WIFI_IDL_CBK_CMD_STOPED,  /* The Wi-Fi driver has been uninstalled. */
    /* IWifiChipEventCallback */
    WIFI_IDL_CBK_CMD_ADD_IFACE,    /* The network device interface has been added. */
    WIFI_IDL_CBK_CMD_REMOVE_IFACE, /* The network device interface has been deleted. */
    /* AP AsscociatedEvent */
    WIFI_IDL_CBK_CMD_STA_JOIN,  /* STA connection notification in AP mode */
    WIFI_IDL_CBK_CMD_STA_LEAVE, /* STA leaving notification in AP mode */
    /* SupplicantEventCallback */
    WIFI_IDL_CBK_CMD_SCAN_INFO_NOTIFY, /* SCAN Scan Result Notification */
    WIFI_IDL_CBK_CMD_CONNECT_CHANGED,    /* Connection status change notification */
    WIFI_IDL_CBK_CMD_AP_ENABLE,          /* AP enabling notification */
    WIFI_IDL_CBK_CMD_AP_DISABLE,         /* AP closure notification */
    WIFI_IDL_CBK_CMD_WPA_STATE_CHANGEM,  /* WPA status change notification */
    WIFI_IDL_CBK_CMD_SSID_WRONG_KEY,     /* Password error status notification */
    WIFI_IDL_CBK_CMD_WPS_OVERLAP,        /* wps PBC overlap */
    WIFI_IDL_CBK_CMD_WPS_TIME_OUT,       /* wps connect time out */
    WIFI_IDL_CBK_CMD_WPS_CONNECTION_FULL,   /* network connection full */
    WIFI_IDL_CBK_CMD_WPS_CONNECTION_REJECT, /* network connection reject */
    /* P2p callback */
    WIFI_IDL_CBK_CMD_P2P_SUPPLICANT_CONNECT,            /* p2p connect supplicant */
    WIFI_IDL_CBK_CMD_SUP_CONN_FAILED_EVENT,             /* Wpa_supplicant client connection failure event */
    WIFI_IDL_CBK_CMD_P2P_DEVICE_FOUND_EVENT,            /* Device discovery event */
    WIFI_IDL_CBK_CMD_P2P_DEVICE_LOST_EVENT,             /* Device loss event */
    WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_REQUEST_EVENT,  /* Event of receiving a GO negotiation request */
    WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_SUCCESS_EVENT,  /* The GO negotiation is successful */
    WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_FAILURE_EVENT,  /* The GO negotiation fails */
    WIFI_IDL_CBK_CMD_P2P_INVITATION_RECEIVED_EVENT,     /* P2P invitation request event */
    WIFI_IDL_CBK_CMD_P2P_INVITATION_RESULT_EVENT,       /* P2P invitation result */
    WIFI_IDL_CBK_CMD_P2P_GROUP_FORMATION_SUCCESS_EVENT, /* The group is created successfully */
    WIFI_IDL_CBK_CMD_P2P_GROUP_FORMATION_FAILURE_EVENT, /* The group is created failure */
    WIFI_IDL_CBK_CMD_P2P_GROUP_STARTED_EVENT,           /* Group Start Event */
    WIFI_IDL_CBK_CMD_P2P_GROUP_REMOVED_EVENT,           /* Group removed event */
    WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_REQ_EVENT,       /* Provision Discovery request event */
    WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_RSP_EVENT,       /* Provision Discovery Response Event */
    WIFI_IDL_CBK_CMD_P2P_PROV_DISC_ENTER_PIN_EVENT,     /* Provision Discovery PIN input event */
    WIFI_IDL_CBK_CMD_P2P_PROV_DISC_SHOW_PIN_EVENT,      /* Provision Discovery Display PIN Event */
    WIFI_IDL_CBK_CMD_P2P_FIND_STOPPED_EVENT,            /* Device search stop event */
    WIFI_IDL_CBK_CMD_P2P_SERV_DISC_RESP_EVENT,          /* Service response event */
    WIFI_IDL_CBK_CMD_P2P_PROV_DISC_FAILURE_EVENT,       /* Provision Discovery failure event */
    WIFI_IDL_CBK_CMD_AP_STA_DISCONNECTED_EVENT,         /* STA Disconnected from AP */
    WIFI_IDL_CBK_CMD_AP_STA_CONNECTED_EVENT,            /* STA and AP connected event */
    WIFI_IDL_CBK_CMD_P2P_SERV_DISC_REQ_EVENT,           /* Service discovery request event */
} WifiIdlEvent;

#define SINGLE_SCAN_FAILED 1  /* Scan failure notification */
#define SINGLE_SCAN_OVER_OK 2 /* Scan success notification */
#define PNO_SCAN_OVER_OK 3    /* PNO Scan success notification */
#define WPA_CB_CONNECTED 1    /* The connection is successfully. */
#define WPA_CB_DISCONNECTED 2 /* Disconnect */
#define WIFI_IDL_COUNTRY_CODE_LENGTH 2
#define WIFI_IDL_GET_MAX_SCAN_INFO 256 /* Maximum number of scan infos obtained at a time */
#define WIFI_IDL_GET_MAX_NETWORK_LIST 128
#define WIFI_IDL_GET_MAX_BANDS 32                   /* Obtains the number of bands. */
#define WIFI_IDL_INTERFACE_SUPPORT_COMBINATIONS 32 /* chip support valid interface combinations */
#define WIFI_IDL_PSK_MIN_LENGTH 8
#define WIFI_IDL_PSK_MAX_LENGTH 64
#define WIFI_IDL_BSSID_LENGTH 17 /* bssid - mac address length */
#define WIFI_IDL_PIN_CODE_LENGTH 8
#define WIFI_IDL_P2P_DEV_ADDRESS_LEN 32
#define WIFI_IDL_P2P_LISTEN_MIN_TIME 1
#define WIFI_IDL_P2P_LISTEN_MAX_TIME 65535
#define WIFI_IDL_P2P_MIN_GO_INTENT 0
#define WIFI_IDL_P2P_MAX_GO_INTENT 15
#define WIFI_IDL_P2P_DEFAULT_GO_INTENT 6
#define WIFI_IDL_P2P_TMP_BUFFER_SIZE_128 128
#define WIFI_IDL_P2P_SERVICE_TYPE_MIN_SIZE 3
#define WIFI_IDL_P2P_SERVICE_TYPE_2_POS 2

#endif