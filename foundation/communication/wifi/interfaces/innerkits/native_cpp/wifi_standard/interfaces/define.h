/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_DEFINE_H
#define OHOS_DEFINE_H

/* ------------sta module message define--------- */
#define WIFI_SVR_CMD_ENABLE_WIFI 0x1001               /* open wifi */
#define WIFI_SVR_CMD_DISABLE_WIFI 0x1002              /* close wifi */
#define WIFI_SVR_CMD_SET_POWER_MODE 0x1003            /* wifi lock, not used */
#define WIFI_SVR_CMD_FULL_SCAN 0x1004                 /* scan request */
#define WIFI_SVR_CMD_SPECIFIED_PARAMS_SCAN 0X1005     /* scan with params request */
#define WIFI_SVR_CMD_ADD_DEVICE_CONFIG 0X1006         /* add a network config */
#define WIFI_SVR_CMD_REMOVE_DEVICE_CONFIG 0X1007      /* remove a network config */
#define WIFI_SVR_CMD_GET_DEVICE_CONFIGS 0x1008        /* get current saved network configs */
#define WIFI_SVR_CMD_ENABLE_DEVICE 0X1009             /* set a network enabled */
#define WIFI_SVR_CMD_DISABLE_DEVICE 0X100A            /* disabled a network */
#define WIFI_SVR_CMD_CONNECT_TO 0X100B                /* connect to a network with networkid */
#define WIFI_SVR_CMD_CONNECT2_TO 0x100C               /* connect to a network with network config */
#define WIFI_SVR_CMD_RECONNECT 0X100D                 /* reconnect */
#define WIFI_SVR_CMD_REASSOCIATE 0x100E               /* reassociate */
#define WIFI_SVR_CMD_DISCONNECT 0x1010                /* disconnect */
#define WIFI_SVR_CMD_START_WPS 0x1011                 /* start wps */
#define WIFI_SVR_CMD_CANCEL_WPS 0x1012                /* stop wps */
#define WIFI_SVR_CMD_IS_WIFI_ACTIVE 0x1013            /* whether current wifi active */
#define WIFI_SVR_CMD_GET_WIFI_STATE 0x1014            /* get current wifi state */
#define WIFI_SVR_CMD_IS_SCAN_ALWAYS_ACTIVE 0x1015     /* whether set scan always */
#define WIFI_SVR_CMD_GET_SCAN_INFO_LIST 0x1016        /* get scan results */
#define WIFI_SVR_CMD_GET_LINKED_INFO 0x1017           /* get current link info */
#define WIFI_SVR_CMD_GET_DHCP_INFO 0x1018             /* get dhcp info */
#define WIFI_SVR_CMD_SET_COUNTRY_CODE 0X1019          /* set country code */
#define WIFI_SVR_CMD_GET_COUNTRY_CODE 0x101A          /* get country code */
#define WIFI_SVR_CMD_REGISTER_CALLBACK_CLIENT 0x101B  /* api register callback event */
#define WIFI_SVR_CMD_GET_SIGNAL_LEVEL 0x101C          /* get signal level */
#define WIFI_SVR_CMD_SET_SCAN_CONTROL_INFO 0x101D     /* set scan control policy */
#define WIFI_SVR_CMD_REGISTER_SCAN_CALLBACK 0x101E    /* register scan callback */
#define WIFI_SVR_CMD_UPDATE_DEVICE_CONFIG 0x101F      /* update a network config */
#define WIFI_SVR_CMD_REMOVE_ALL_DEVICE_CONFIG 0x1020  /* remove all network configs */
#define WIFI_SVR_CMD_GET_SUPPORTED_FEATURES 0x1021    /* get supported features */
#define WIFI_SVR_CMD_GET_DERVICE_MAC_ADD 0x1022       /* get mac address */
#define WIFI_SVR_CMD_INIT_WIFI_PROTECT 0x1023        /* init the Wi-Fi protect. */
#define WIFI_SVR_CMD_GET_WIFI_PROTECT 0x1024         /* get the Wi-Fi protect. */
#define WIFI_SVR_CMD_PUT_WIFI_PROTECT 0x1025         /* put the Wi-Fi protect. */
#define WIFI_SVR_CMD_IS_WIFI_CONNECTED 0x1026         /* is Wi-Fi connected */
#define WIFI_SVR_CMD_SET_LOW_LATENCY_MODE 0x1027    /* set low latency mode */

/* -------------ap module message define----------------- */
#define WIFI_SVR_CMD_ENABLE_WIFI_AP 0x1100            /* open ap */
#define WIFI_SVR_CMD_DISABLE_WIFI_AP 0x1101           /* close ap */
#define WIFI_SVR_CMD_GETAPSTATE_WIFI 0x1102           /* get current ap state */
#define WIFI_SVR_CMD_SETAPCONFIG_WIFI 0x1103          /* set ap config */
#define WIFI_SVR_CMD_GET_HOTSPOT_CONFIG 0x1104        /* get ap config */
#define WIFI_SVR_CMD_IS_HOTSPOT_ACTIVE 0x1105         /* whether current ap active */
#define WIFI_SVR_CMD_GET_STATION_LIST 0x1106          /* get ap's connected sta infos */
#define WIFI_SVR_CMD_SETBAND_AP 0X1108                /* set band */
#define WIFI_SVR_CMD_GETBAND_AP 0X1109                /* get band */
#define WIFI_SVR_CMD_ADD_BLOCK_LIST 0X110A            /* add a block */
#define WIFI_SVR_CMD_DEL_BLOCK_LIST 0X110B            /* remove a block */
#define WIFI_SVR_CMD_GET_BLOCK_LISTS 0X110C           /* get total block list */
#define WIFI_SVR_CMD_DISCONNECT_STA 0X110D            /* disconnect a sta connection */
#define WIFI_SVR_CMD_GET_VALID_BANDS 0X110E           /* get current valid frequency according band */
#define WIFI_SVR_CMD_GET_VALID_CHANNELS 0X110F        /* get current valid channels associated with the band */
#define WIFI_SVR_CMD_REGISTER_HOTSPOT_CALLBACK 0X1110 /* register scan callback */

/* -------------p2p module message define----------------- */
#define WIFI_SVR_CMD_P2P_ENABLE 0x2000                 /* open p2p */
#define WIFI_SVR_CMD_P2P_DISABLE 0x2001                /* close p2p */
#define WIFI_SVR_CMD_P2P_DISCOVER_DEVICES 0x2002       /* start Wi-Fi P2P device search */
#define WIFI_SVR_CMD_P2P_STOP_DISCOVER_DEVICES 0x2003  /* stop Wi-Fi P2P device search */
#define WIFI_SVR_CMD_P2P_DISCOVER_SERVICES 0x2004      /* start Wi-Fi P2P service search */
#define WIFI_SVR_CMD_P2P_STOP_DISCOVER_SERVICES 0x2005 /* stop Wi-Fi P2P service search */
#define WIFI_SVR_CMD_P2P_REQUEST_SERVICES 0x2006       /* request the P2P service */
#define WIFI_SVR_CMD_P2P_PUT_LOCAL_SERVICES 0x2007     /* add local P2P service */
#define WIFI_SVR_CMD_P2P_DELETE_LOCAL_SERVICES 0x2008  /* remove local P2P service */
#define WIFI_SVR_CMD_P2P_START_LISTEN 0x2009           /* enable Wi-Fi P2P listening */
#define WIFI_SVR_CMD_P2P_STOP_LISTEN 0x200A            /* disable Wi-Fi P2P listening */
#define WIFI_SVR_CMD_P2P_CREATE_GROUP 0x200B             /* creating a P2P Group */
#define WIFI_SVR_CMD_P2P_REMOVE_GROUP 0x200C           /* remove a P2P Group */
#define WIFI_SVR_CMD_P2P_DELETE_GROUP 0x200D           /* delete a P2P Group */
#define WIFI_SVR_CMD_P2P_CONNECT 0x200E                /* p2p connect */
#define WIFI_SVR_CMD_P2P_CANCEL_CONNECT 0x200F             /* p2p cancel connect */
#define WIFI_SVR_CMD_P2P_QUERY_INFO 0x2010             /* querying Wi-Fi P2P Connection Information */
#define WIFI_SVR_CMD_P2P_GET_CURRENT_GROUP 0x2011      /* get the P2P current group */
#define WIFI_SVR_CMD_P2P_GET_ENABLE_STATUS 0x2012      /* obtains the P2P switch status */
#define WIFI_SVR_CMD_P2P_GET_DISCOVER_STATUS 0x2013    /* obtains the P2P discovery status */
#define WIFI_SVR_CMD_P2P_GET_CONNECTED_STATUS 0x2014   /* obtains the P2P connected status */
#define WIFI_SVR_CMD_P2P_QUERY_DEVICES 0x2015          /* query the information about the found devices */
#define WIFI_SVR_CMD_P2P_QUERY_GROUPS 0x2016           /* query the information about the found groups */
#define WIFI_SVR_CMD_P2P_QUERY_SERVICES 0x2017         /* query the information about the found services */
#define WIFI_SVR_CMD_P2P_REGISTER_CALLBACK 0x2018
#define WIFI_SVR_CMD_P2P_SET_DEVICE_NAME 0x2019        /* set device name */
#define WIFI_SVR_CMD_P2P_SET_WFD_INFO 0x201A           /* set p2p wifi display info */
#define WIFI_SVR_CMD_P2P_HID2D_APPLY_IP 0x201B    /* hid2d apply ip */
#define WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_INCREASE 0x201C    /* hid2d shared link increase */
#define WIFI_SVR_CMD_P2P_HID2D_SHARED_LINK_DECREASE 0x201D    /* hid2d shared link decrease */
#define WIFI_SVR_CMD_P2P_HID2D_CREATE_GROUP 0x201E    /* hid2d create group */
#define WIFI_SVR_CMD_P2P_HID2D_REMOVE_GC_GROUP 0x201F    /* hid2d remove GC group */
#define WIFI_SVR_CMD_P2P_HID2D_CONNECT 0x2020    /* hid2d connect to group */
#define WIFI_SVR_CMD_P2P_HID2D_CONFIG_IP 0x2021    /* hid2d configure IP address */
#define WIFI_SVR_CMD_P2P_HID2D_RELEASE_IP 0x2022    /* hid2d release IP address */
#define WIFI_SVR_CMD_GET_P2P_RECOMMENDED_CHANNEL 0x2023    /* get recommended channel */
#define WIFI_SVR_CMD_GET_5G_CHANNEL_LIST 0x2024    /* get recommended channel */
#define WIFI_SVR_CMD_GET_SELF_WIFI_CFG 0x2025    /* get self wifi configuration */
#define WIFI_SVR_CMD_SET_PEER_WIFI_CFG 0x2026    /* set peer wifi configuration */

/* -----------register event type and message define-------------- */
#define WIFI_CBK_CMD_STATE_CHANGE 0x1001         /* STA state change event */
#define WIFI_CBK_CMD_CONNECTION_CHANGE 0x1002    /* STA connection state change event */
#define WIFI_CBK_CMD_SCAN_STATE_CHANGE 0x1003    /* SCAN state change event */
#define WIFI_CBK_CMD_RSSI_CHANGE 0x1004          /* RSSI */
#define WIFI_CBK_CMD_HOTSPOT_STATE_CHANGE 0x1005 /* AP state change event */
#define WIFI_CBK_CMD_HOTSPOT_STATE_JOIN 0x1006   /* AP join a sta event */
#define WIFI_CBK_CMD_HOTSPOT_STATE_LEAVE 0x1007  /* AP leave a sta event */
#define WIFI_CBK_CMD_STREAM_DIRECTION 0x1008     /* traffic up/down state event */
#define WIFI_CBK_CMD_WPS_STATE_CHANGE 0x1009     /* wps state change event */

#define WIFI_CBK_CMD_P2P_STATE_CHANGE 0x1010         /* p2p state change event */
#define WIFI_CBK_CMD_PERSISTENT_GROUPS_CHANGE 0x1011 /* Persistent Group Updated */
#define WIFI_CBK_CMD_THIS_DEVICE_CHANGE 0x1012       /* The current device information has been updated */
#define WIFI_CBK_CMD_PEER_CHANGE 0x1013
#define WIFI_CBK_CMD_SERVICE_CHANGE 0x1014
#define WIFI_CBK_CMD_CONNECT_CHANGE 0x1015
#define WIFI_CBK_CMD_DISCOVERY_CHANGE 0x1016
#define WIFI_CBK_CMD_P2P_ACTION_RESULT 0x1017

#define WIFI_CBK_MSG_STATE_CHANGE 0x1001
#define WIFI_CBK_MSG_CONNECTION_CHANGE 0x1002
#define WIFI_CBK_MSG_RSSI_CHANGE 0x1003
#define WIFI_CBK_MSG_STREAM_DIRECTION 0x1004
#define WIFI_CBK_MSG_WPS_STATE_CHANGE 0x1005
#define WIFI_CBK_MSG_SCAN_STATE_CHANGE 0x1006
#define WIFI_CBK_MSG_HOTSPOT_STATE_CHANGE 0x1007
#define WIFI_CBK_MSG_HOTSPOT_STATE_JOIN 0x1008
#define WIFI_CBK_MSG_HOTSPOT_STATE_LEAVE 0x1009
#define WIFI_CBK_MSG_P2P_STATE_CHANGE 0x1010
#define WIFI_CBK_MSG_PERSISTENT_GROUPS_CHANGE 0x1011 /* Persistent Group Updated */
#define WIFI_CBK_MSG_THIS_DEVICE_CHANGE 0x1012
#define WIFI_CBK_MSG_PEER_CHANGE 0x1013
#define WIFI_CBK_MSG_SERVICE_CHANGE 0x1014
#define WIFI_CBK_MSG_CONNECT_CHANGE 0x1015
#define WIFI_CBK_MSG_DISCOVERY_CHANGE 0x1016
#define WIFI_CBK_MSG_P2P_ACTION_RESULT 0x1017

/* -----------Feature service name-------------- */
#define WIFI_SERVICE_STA "StaService"     /* STA */
#define WIFI_SERVICE_AP "ApService"       /* AP */
#define WIFI_SERVICE_P2P "P2pService"     /* P2P */
#define WIFI_SERVICE_SCAN "ScanService"   /* SCAN */
#define WIFI_SERVICE_AWARE "AwareService" /* AWARE */

/* ---------Feature service ability id */
#define WIFI_DEVICE_ABILITY_ID 1125
#define WIFI_SCAN_ABILITY_ID 1126
#define WIFI_HOTSPOT_ABILITY_ID 1127
#define WIFI_P2P_ABILITY_ID 1128

#define MODE_STATE_SCREEN (1)
#define MODE_STATE_AIR_PLANE (2)
#define MODE_STATE_APP_RUN (3)
#define MODE_STATE_POWER_SAVING (4)
#define MODE_STATE_FREEZE (5)
#define MODE_STATE_NO_CHARGER_PLUG (6)

#define MODE_STATE_OPEN (1)
#define MODE_STATE_CLOSE (2)

#endif